/*******************************************************************************
 * execution.cpp
 *******************************************************************************/

#include "execution.h"

o3_execution::o3_execution (port<dynInstruction*>& scheduler_to_execution_port, 
                      port<dynInstruction*>& execution_to_scheduler_port, 
                      port<dynInstruction*>& execution_to_memory_port, 
                      CAMtable<dynInstruction*>* iROB,
	    	          WIDTH execution_width,
	    	          string stage_name) 
	: stage (execution_width, stage_name)
{
    _scheduler_to_execution_port = &scheduler_to_execution_port;
    _execution_to_scheduler_port = &execution_to_scheduler_port;
    _execution_to_memory_port = &execution_to_memory_port;
    _iROB = iROB;
    _aluExeUnits = new List<exeUnit*>;
    for (WIDTH i = 0; i < _stage_width; i++) {
        exeUnit* newEU = new exeUnit (1,  _eu_lat._alu_lat, ALU_EU); //TODO make this config better with more EU types
        _aluExeUnits->Append (newEU);
    }
}

o3_execution::~o3_execution () {}

void o3_execution::doEXECUTION (sysClock& clk) {
    dbg.print (DBG_FETCH, "** %s: (cyc: %d)\n", _stage_name.c_str (), clk.now ());
    /* STAT */
    regStat (clk);
    PIPE_ACTIVITY pipe_stall = PIPE_STALL;

    /* WRITEBACK RESULT */
    COMPLETE_STATUS cmpl_status;
    cmpl_status = completeIns (clk);

    /* EXECUTE INS */
    if (cmpl_status == COMPLETE_NORMAL) {
        pipe_stall = executionImpl (clk);
    }

    /* SQUASH CONTROL */
    squashCtrl (clk);
    dbg.print (DBG_EXECUTION, "%s: %s %llu (cyc: %d)\n", _stage_name.c_str (), "PIPELINE STATE:", g_var.g_pipe_state, clk.now ());

    /* STAT */
    if (g_var.g_pipe_state != PIPE_NORMAL) s_squash_cycles++;
    if (pipe_stall == PIPE_STALL) s_stall_cycles++;
}

/* WRITE COMPLETE INS - WRITEBACK */
COMPLETE_STATUS o3_execution::completeIns (sysClock& clk) {
    g_var.setOldSquashSN ();
    for (WIDTH i = 0; i < _stage_width; i++) {
        exeUnit* EU = _aluExeUnits->Nth (i);
        dynInstruction* ins = EU->getEUins ();

        /* CHECKS */
        if (g_var.g_pipe_state == PIPE_FLUSH) break;
      //if (ins != NULL && ins->getInsType () == MEM && 
      //    _execution_to_memory_port->getBuffState (clk.now ()) == FULL_BUFF) break; //TODO uselss now?
        if (EU->getEUstate (clk.now (), true) != COMPLETE_EU) continue;

        /* COMPLETE INS */
        if (ins->getInsType () == MEM && ins->getMemType () == LOAD) {
            ins->setPipeStage (MEM_ACCESS);
            g_LSQ_MGR.memAddrReady (ins); //TODO what should exactly happen here for LD's?
            //_execution_to_memory_port->pushBack (ins, clk.now ());
            dbg.print (DBG_COMMIT, "%s: %s %llu (cyc: %d)\n", _stage_name.c_str (), "Complete mem ins addr", ins->getInsID (), clk.now ());
        } else if (ins->getInsType () == MEM && ins->getMemType () == STORE) {
            ins->setPipeStage (COMPLETE);
            g_LSQ_MGR.memAddrReady (ins); //TODO what should exactly happen here for LD's?
            g_GRF_MGR.completeRegs (ins); //TODO this sould not normally exist?
            //_execution_to_memory_port->pushBack (ins, clk.now ());
            dbg.print (DBG_COMMIT, "%s: %s %llu (cyc: %d)\n", _stage_name.c_str (), "Complete mem ins addr", ins->getInsID (), clk.now ());
        } else {
            //if (!g_GRF_MGR.hasFreeWrPort (clk.now ())) break; //TODO put this back and clean up the assert for available EU
            ins->setPipeStage (COMPLETE);
            g_GRF_MGR.completeRegs (ins);
            dbg.print (DBG_COMMIT, "%s: %s %llu (cyc: %d)\n", _stage_name.c_str (), "Complete ins", ins->getInsID (), clk.now ());
        }
        EU->resetEU ();

        /* SQUASH DETECT */
        if (ins->isOnWrongPath ()) {
            g_var.setSquashSN (ins->getInsID ());
        }
        dbg.print (DBG_EXECUTION, "%s: %s %llu (cyc: %d)\n", _stage_name.c_str (), "Complete ins", ins->getInsID (), clk.now ());
    }

    if (g_var.isOnWrongPath()) {
        dbg.print (DBG_EXECUTION, "%s: %s %llu %s (cyc: %d)\n", _stage_name.c_str (), "Ins on Wrong Path (ins: ", g_var.getSquashSN (), ")", clk.now ());
        return COMPLETE_SQUASH;
    }
    dbg.print (DBG_EXECUTION, "%s: %s (cyc: %d)\n", _stage_name.c_str (), "Ins on Right Path", clk.now ());
    return COMPLETE_NORMAL;
}

/* EXECUTE INS */
PIPE_ACTIVITY o3_execution::executionImpl (sysClock& clk) {
    PIPE_ACTIVITY pipe_stall = PIPE_STALL;

    for (WIDTH i = 0; i < _stage_width; i++) {
        /* CHECKS */
        if (g_var.g_pipe_state == PIPE_WAIT_FLUSH || g_var.g_pipe_state == PIPE_FLUSH) break;
        if (_scheduler_to_execution_port->getBuffState (clk.now ()) == EMPTY_BUFF) break;
        if (!_scheduler_to_execution_port->isReady (clk.now ())) break;
        exeUnit* EU = _aluExeUnits->Nth (i);
        if (EU->getEUstate (clk.now (), false) != AVAILABLE_EU) continue;

        /* EXE INS */
        dynInstruction* ins = _scheduler_to_execution_port->popFront (clk.now ());
        EU->_eu_timer.setNewTime (clk.now ());
        EU->setEUins (ins);
        EU->runEU ();
        ins->setPipeStage (EXECUTE);
        forward (ins, EU->_eu_timer.getLatency (), clk);
        dbg.print (DBG_EXECUTION, "%s: %s %llu (cyc: %d)\n", _stage_name.c_str (), "Execute ins", ins->getInsID (), clk.now ());

        /* STAT */
        s_ins_cnt++;
        pipe_stall = PIPE_BUSY;
    }
    return pipe_stall;
}

void o3_execution::forward (dynInstruction* ins, CYCLE eu_latency, sysClock& clk) {
    if (_execution_to_scheduler_port->getBuffState (clk.now ()) == FULL_BUFF) return;
    if (ins->getInsType () != MEM) {
        CYCLE cdb_ready_latency = eu_latency - 1;
        Assert (cdb_ready_latency >= 0);
        _execution_to_scheduler_port->pushBack (ins, clk.now (), cdb_ready_latency);
        dbg.print (DBG_EXECUTION, "%s: %s %llu (cyc: %d)\n", _stage_name.c_str (), "Forward wr ops of ins", ins->getInsID (), clk.now ());
    }
}

void o3_execution::squashCtrl (sysClock& clk) {
    string state_switch;
    if ((g_var.g_pipe_state == PIPE_NORMAL || g_var.g_pipe_state == PIPE_SQUASH_ROB) && 
        g_var.g_squash_seq_num != g_var.g_old_squash_seq_num) {
        g_var.g_pipe_state = PIPE_WAIT_FLUSH;
        state_switch =  "PIPE_NORMAL -> PIPE_WAIT_FLUSH";
    } else if (g_var.g_pipe_state == PIPE_NORMAL) {
        g_var.resetSquashSN ();
        return;
    } else if (g_var.g_pipe_state == PIPE_WAIT_FLUSH) {
        for (WIDTH i = 0; i < _stage_width; i++) {
            exeUnit* EU = _aluExeUnits->Nth (i);
            if (EU->getEUstate (clk.now (), false) != AVAILABLE_EU) return;
        }
        g_var.g_pipe_state = PIPE_FLUSH;
        state_switch =  "PIPE_WAIT_FLUSH -> PIPE_FLUSH";
        for (WIDTH i = 0; i < _stage_width; i++) {
            exeUnit* EU = _aluExeUnits->Nth (i);
            EU->resetEU ();
        }
        squash (clk);
    } else if (g_var.g_pipe_state == PIPE_FLUSH) {
        g_var.g_pipe_state = PIPE_DRAIN;
        state_switch =  "PIPE_FLUSH -> PIPE_DRAIN";
    } else if (g_var.g_pipe_state == PIPE_DRAIN && _iROB->hasFreeRdPort (clk.now ()) && 
               _iROB->getFront()->getInsID () >= g_var.g_squash_seq_num) {
        g_var.g_pipe_state = PIPE_SQUASH_ROB;
        state_switch =  "PIPE_DRAIN -> PIPE_SQUASH_ROB";
    } else if (g_var.g_pipe_state == PIPE_SQUASH_ROB && _iROB->getTableSize() == 0) {
        g_var.resetSquashSN ();
        g_var.g_pipe_state = PIPE_NORMAL;
        state_switch =  "PIPE_SQUASH_ROB -> PIPE_NORMAL";
        g_GRF_MGR.squashRenameReg ();
    } else {
        return; /* No state change */
    }
    dbg.print (DBG_EXECUTION, "%s: %s (cyc: %d)\n", _stage_name.c_str (), state_switch.c_str(), clk.now ());
}

void o3_execution::squash (sysClock& clk) {
    dbg.print (DBG_SQUASH, "%s: %s (cyc: %d)\n", _stage_name.c_str (), "o3_execution Ports Flush", clk.now ());
    Assert (g_var.g_pipe_state == PIPE_FLUSH);
    INS_ID squashSeqNum = g_var.getSquashSN ();
    _execution_to_scheduler_port->flushPort (squashSeqNum, clk.now ());
    _execution_to_memory_port->flushPort (squashSeqNum, clk.now ());
}

void o3_execution::regStat (sysClock& clk) {
    _scheduler_to_execution_port->regStat (clk.now ());
}