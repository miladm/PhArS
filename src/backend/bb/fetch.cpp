/*********************************************************************************
 * fetch.cpp
 ********************************************************************************/

#include "fetch.h"

bb_fetch::bb_fetch (port<dynInstruction*>& bp_to_fetch_port, 
	          port<dynInstruction*>& fetch_to_decode_port,
			  port<dynInstruction*>& fetch_to_bp_port,
			  WIDTH fetch_width,
              sysClock* clk,
			  string stage_name
			 )
	: stage (fetch_width, stage_name, clk)
{
    _bp_to_fetch_port = &bp_to_fetch_port;
    _fetch_to_bp_port = &fetch_to_bp_port;
    _fetch_to_decode_port = &fetch_to_decode_port;
    _insListIndx = 0;
    _switch_to_frontend = false;
    _fetch_state = FETCH_COMPLETE;
    _current_bb = NULL;
}

bb_fetch::~bb_fetch () {}

SIM_MODE bb_fetch::doFETCH () {
    /*-- STAT + DEBUG --*/
    dbg.print (DBG_FETCH, "** %s: (cyc: %d)\n", _stage_name.c_str (), _clk->now ());
    regStat ();
    PIPE_ACTIVITY pipe_stall = PIPE_STALL;

    /*-- SQUASH HANDLING --*/
    if (g_var.g_pipe_state == PIPE_FLUSH) { squash (); }
    if (g_var.g_pipe_state == PIPE_NORMAL) {
        pipe_stall = fetchImpl ();
    }

    /*-- STAT --*/
    if (pipe_stall == PIPE_STALL) s_stall_cycles++;

    if (_switch_to_frontend) {
        _switch_to_frontend = false;
        return FRONT_END;
    }
	return BACK_END;
}

/*-- READ FW INSTRUCTIONS FORM THE PIN INPUT QUE --*/
PIPE_ACTIVITY bb_fetch::fetchImpl () {
    PIPE_ACTIVITY pipe_stall = PIPE_STALL;
    if (_fetch_state == FETCH_COMPLETE) {
        if (g_var.isBBcacheEmpty ()) { _switch_to_frontend = true; return pipe_stall; }
        else { getNewBB (); }
    }

    Assert (_current_bb != NULL);
	for (WIDTH i = 0; i < _stage_width; i++) {
		/*-- CHECKS --*/
        if (_current_bb->getBBstate () == EMPTY_BUFF) break; //TODO no back-to-back BB fetch in 1 cycle
		if (_fetch_to_decode_port->getBuffState () == FULL_BUFF) break;

        /*-- FETCH INS --*/
        dynInstruction* ins = _current_bb->popFront ();
        ins->setPipeStage (FETCH);
		_fetch_to_decode_port->pushBack (ins);
        updateBBfetchState ();
        dbg.print (DBG_FETCH, "%s: %s %llu (cyc: %d)\n", _stage_name.c_str (), "Fetch ins", ins->getInsID (), _clk->now ());

        /*-- STAT --*/
        s_ins_cnt++;
        pipe_stall = PIPE_BUSY;
	}
    return pipe_stall;
}

void bb_fetch::getNewBB () {
    Assert (_current_bb->getBBstate () == EMPTY_BUFF);
    _current_bb = g_var.popBBcache ();
}

void bb_fetch::updateBBfetchState () {
    if (_current_bb->getBBstate () == EMPTY_BUFF) _fetch_state = FETCH_COMPLETE;
    else if (_current_bb->getBBstate () == FULL_BUFF) _fetch_state = FETCH_IN_PROGRESS;
    else if (_current_bb->getBBstate () == AVAILABLE_BUFF) _fetch_state = FETCH_IN_PROGRESS;
}

void bb_fetch::squash () {
    dbg.print (DBG_SQUASH, "%s: %s (cyc: %d)\n", _stage_name.c_str (), "Fetch Port Flush", _clk->now ());
    Assert (g_var.g_pipe_state == PIPE_FLUSH);
    INS_ID squashSeqNum = g_var.getSquashSN ();
    _fetch_to_decode_port->flushPort (squashSeqNum);
    _fetch_to_bp_port->flushPort (squashSeqNum);
}

void bb_fetch::regStat () {
    _bp_to_fetch_port->regStat ();
}