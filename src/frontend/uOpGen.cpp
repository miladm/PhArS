/*******************************************************************************
 * uOpGen.cpp
 ******************************************************************************/

#include "uOpGen.h"

staticCodeParser* g__staticCode;

/* ******************************************************************* *
 * STAT GLOBAL VARIABLES
 * ******************************************************************* */
static ScalarStat& s_pin_missing_static_bb_cnt (g_stats.newScalarStat ("uOpGen", "pin_missing_static_bb_cnt", "Number of dynamic BB's with no static counterpart.", 0, NO_PRINT_ZERO));
static ScalarStat& s_pin_bb_cnt (g_stats.newScalarStat ("pars", "pin_bb_cnt", "Number of basicblocks instrumented in frontend", 0, NO_PRINT_ZERO));

/* ************************************* *
 * INS INSTRUMENTATIONS
 * ************************************ */
VOID pin__getBrIns (ADDRINT insAddr, BOOL hasFT, ADDRINT tgAddr, ADDRINT ftAddr, 
        BOOL isTaken, BOOL isCall, BOOL isRet, BOOL isJump, BOOL isDirBrOrCallOrJmp) {
    if (g__staticCode->hasIns (insAddr)) {
        g_var.stat.matchIns++;
        if (g_var.g_debug_level & DBG_UOP) 
            std::cout << "NEW BR: " << (g_var.g_wrong_path?"*":" ") << dec << g_var.g_seq_num 
                << " in BB " << g_var.g_bb_seq_num-1 << std::endl;
        if (g_var.g_core_type == BASICBLOCK) {
            dynBasicblock* g_bbObj = g_var.getLastCacheBB ();
            bbInstruction* g_insObj = g_var.getNewIns ();
            stInstruction* staticIns = g__staticCode->getInsObj (insAddr);
            staticIns->copyRegsTo (g_insObj);
            g_insObj->setBrAtr (tgAddr, ftAddr, hasFT, isTaken, isCall, isRet, isJump, isDirBrOrCallOrJmp);
            g_insObj->setInsType (BR);
            g_insObj->setInsAddr (insAddr);
            if (g_var.scheduling_mode == DYNAMIC_SCH)
                g_insObj->setInsID (g_var.g_seq_num++);
            g_insObj->setWrongPath (g_var.g_wrong_path);
            if (g_var.g_wrong_path) g_bbObj->setWrongPath ();
            if (g_bbObj->insertIns (g_insObj)) Assert (true == false && "to be implemented");
            g_insObj->setBB (g_bbObj);
        } else { /* INO & O3 */
            dynInstruction* g_insObj = g_var.getNewCodeCacheIns ();
            stInstruction* staticIns = g__staticCode->getInsObj (insAddr);
            staticIns->copyRegsTo (g_insObj);
            g_insObj->setBrAtr (tgAddr, ftAddr, hasFT, isTaken, isCall, isRet, isJump, isDirBrOrCallOrJmp);
            g_insObj->setInsType (BR);
            g_insObj->setInsAddr (insAddr);
            g_insObj->setInsID (g_var.g_seq_num++);
            g_insObj->setWrongPath (g_var.g_wrong_path);
        }
    } else {
        g_var.stat.noMatchIns++;
        g_var.stat.missingInsList.insert (insAddr);
    }
}

VOID pin__getMemIns (ADDRINT insAddr, ADDRINT memAccessSize, ADDRINT memAddr, 
        BOOL isStackRd, BOOL isStackWr, BOOL isMemRead) {
    if (g__staticCode->hasIns (insAddr)) {
        g_var.stat.matchIns++;
        if (g_var.g_debug_level & DBG_UOP) 
            std::cout << "NEW MEM: " << (g_var.g_wrong_path?"*":" ") << dec << g_var.g_seq_num 
                << " in BB " << g_var.g_bb_seq_num-1 << std::endl;
        if (g_var.g_core_type == BASICBLOCK) {
            dynBasicblock* g_bbObj = g_var.getLastCacheBB ();
            bbInstruction* g_insObj = g_var.getNewIns ();
            stInstruction* staticIns = g__staticCode->getInsObj (insAddr);
            staticIns->copyRegsTo (g_insObj);
            MEM_TYPE mType = (isMemRead == true ? LOAD : STORE);
            g_insObj->setMemAtr (mType, memAddr, memAccessSize, isStackRd, isStackWr);
            g_insObj->setInsType (MEM);
            g_insObj->setInsAddr (insAddr);
            if (g_var.scheduling_mode == DYNAMIC_SCH)
                g_insObj->setInsID (g_var.g_seq_num++);
            g_insObj->setWrongPath (g_var.g_wrong_path);
            if (g_var.g_wrong_path) g_bbObj->setWrongPath ();
            if (g_bbObj->insertIns (g_insObj)) Assert (true == false && "to be implemented");
            g_insObj->setBB (g_bbObj);
        } else { /* INO & O3 */
            dynInstruction* g_insObj = g_var.getNewCodeCacheIns ();
            stInstruction* staticIns = g__staticCode->getInsObj (insAddr);
            staticIns->copyRegsTo (g_insObj);
            MEM_TYPE mType = (isMemRead == true ? LOAD : STORE);
            g_insObj->setMemAtr (mType, memAddr, memAccessSize, isStackRd, isStackWr);
            g_insObj->setInsType (MEM);
            g_insObj->setInsAddr (insAddr);
            g_insObj->setInsID (g_var.g_seq_num++);
            g_insObj->setWrongPath (g_var.g_wrong_path);
        }
    } else {
        g_var.stat.noMatchIns++;
        g_var.stat.missingInsList.insert (insAddr);
    }
}

VOID pin__getIns (ADDRINT insAddr) {
    if (g__staticCode->hasIns (insAddr)) {
        g_var.stat.matchIns++;
        if (g_var.g_debug_level & DBG_UOP) 
            std::cout << "NEW INS: " << (g_var.g_wrong_path?"*":" ") << dec << g_var.g_seq_num 
                << " in BB " << g_var.g_bb_seq_num-1 << std::endl;
        if (g_var.g_core_type == BASICBLOCK) {
            dynBasicblock* g_bbObj = g_var.getLastCacheBB ();
            if (g_bbObj == NULL) return;
            bbInstruction* g_insObj = g_var.getNewIns ();
            stInstruction* staticIns = g__staticCode->getInsObj (insAddr);
            staticIns->copyRegsTo (g_insObj);
            g_insObj->setInsType (ALU);
            g_insObj->setInsAddr (insAddr);
            if (g_var.scheduling_mode == DYNAMIC_SCH)
                g_insObj->setInsID (g_var.g_seq_num++);
            g_insObj->setWrongPath (g_var.g_wrong_path);
            if (g_var.g_wrong_path) g_bbObj->setWrongPath ();
            if (g_bbObj->insertIns (g_insObj)) Assert (true == false && "to be implemented");
            g_insObj->setBB (g_bbObj);
        } else { /* INO & O3 */
            dynInstruction* g_insObj = g_var.getNewCodeCacheIns ();
            stInstruction* staticIns = g__staticCode->getInsObj (insAddr);
            staticIns->copyRegsTo (g_insObj);
            g_insObj->setInsType (ALU);
            g_insObj->setInsAddr (insAddr);
            g_insObj->setInsID (g_var.g_seq_num++);
            g_insObj->setWrongPath (g_var.g_wrong_path);
        }
    } else {
        g_var.stat.noMatchIns++;
        g_var.stat.missingInsList.insert (insAddr);
    }
}

VOID pin__getNopIns (ADDRINT insAddr) {
    if (g__staticCode->hasIns (insAddr)) {
        g_var.stat.matchIns++;
        if (g_var.g_debug_level & DBG_UOP) 
            std::cout << "NEW NOP: " << (g_var.g_wrong_path?"*":" ") << dec << g_var.g_seq_num 
                << " in BB " << g_var.g_bb_seq_num-1 << std::endl;
        if (g_var.g_core_type == BASICBLOCK) {
            dynBasicblock* g_bbObj = g_var.getLastCacheBB ();
            bbInstruction* g_insObj = g_var.getNewIns ();
            stInstruction* staticIns = g__staticCode->getInsObj (insAddr);
            staticIns->copyRegsTo (g_insObj);
            g_insObj->setInsType (NOP);
            g_insObj->setInsAddr (insAddr);
            if (g_var.scheduling_mode == DYNAMIC_SCH)
                g_insObj->setInsID (g_var.g_seq_num++);
            g_insObj->setWrongPath (g_var.g_wrong_path);
            if (g_var.g_wrong_path) g_bbObj->setWrongPath ();
            if (g_bbObj->insertIns (g_insObj)) Assert (true == false && "to be implemented");
            g_insObj->setBB (g_bbObj);
        } else { /* INO & O3 */
            dynInstruction* g_insObj = g_var.getNewCodeCacheIns ();
            stInstruction* staticIns = g__staticCode->getInsObj (insAddr);
            staticIns->copyRegsTo (g_insObj);
            g_insObj->setInsType (NOP);
            g_insObj->setInsAddr (insAddr);
            g_insObj->setInsID (g_var.g_seq_num++);
            g_insObj->setWrongPath (g_var.g_wrong_path);
        }
    } else {
        g_var.stat.noMatchIns++;
        g_var.stat.missingInsList.insert (insAddr);
    }
}

/* ************************************* *
 * BB INSTRUMENTATIONS
 * ************************************ */
void pin__getBBhead (ADDRINT bb_addr, ADDRINT bb_br_addr, BOOL is_tail_br) {
    if (g_var.g_debug_level & DBG_UOP) 
        std::cout << "NEW BB: " << (g_var.g_wrong_path?"*":" ") << dec << g_var.g_bb_seq_num << std::endl;
    if (g_var.scheduling_mode == STATIC_SCH) {
        dynBasicblock* lastBB = g_var.getLastCacheBB ();
        if (lastBB != NULL) lastBB->rescheduleInsList (&g_var.g_seq_num);
    }
    dynBasicblock* g_bbObj = g_var.getNewCacheBB ();
    g_bbObj->setBBID (g_var.g_bb_seq_num++);
    g_bbObj->setBBbrAddr (is_tail_br, bb_br_addr);
    if (g_var.scheduling_mode == STATIC_SCH) {
        if (g__staticCode->hasStaticBB (bb_addr))
            g_bbObj->setBBstaticInsList (g__staticCode->getBBinsList (bb_addr));
        else
            s_pin_missing_static_bb_cnt++;
    }
    s_pin_bb_cnt++;
}

void pin__get_bb_header (ADDRINT bb_addr, INS bb_tail_ins) {
    BOOL is_br = INS_IsBranchOrCall (bb_tail_ins) || INS_IsFarRet (bb_tail_ins) || INS_IsRet (bb_tail_ins);
    ADDRINT bb_br_addr = INS_Address (bb_tail_ins);
    if (is_br) {
        if (INS_HasFallThrough (bb_tail_ins)) {
            INS_InsertCall (bb_tail_ins, IPOINT_AFTER, (AFUNPTR) pin__getBBhead,
                    IARG_ADDRINT, bb_addr,
                    IARG_ADDRINT, bb_br_addr,
                    IARG_BOOL, is_br,
                    IARG_END);
        }
        INS_InsertCall (bb_tail_ins, IPOINT_TAKEN_BRANCH, (AFUNPTR) pin__getBBhead,
                IARG_ADDRINT, bb_addr,
                IARG_ADDRINT, bb_br_addr,
                IARG_BOOL, is_br,
                IARG_END);
    } else {
        INS_InsertCall (bb_tail_ins, IPOINT_BEFORE, (AFUNPTR) pin__getBBhead,
                IARG_ADDRINT, bb_addr,
                IARG_ADDRINT, bb_br_addr,
                IARG_BOOL, is_br,
                IARG_END);
    }
}

/* ************************************* *
 * PIN INSTRUMENTATION
 * ************************************ */
void pin__uOpGenInit (staticCodeParser &staticCode) {
    g__staticCode = &staticCode;
}

void pin__getOp (INS ins) {
    BOOL is_ret = INS_IsRet (ins) || INS_IsFarRet (ins);
    BOOL is_call = INS_IsCall (ins) || INS_IsFarCall (ins);
    BOOL is_dir_br_jmp = INS_IsDirectFarJump (ins) || INS_IsDirectBranchOrCall (ins);
    BOOL is_br_jmp = INS_IsDirectFarJump (ins) || INS_IsFarJump (ins) || (INS_IsBranch (ins) && INS_HasFallThrough (ins));

//    if (INS_IsBranchOrCall (ins) || INS_IsFarRet (ins) || INS_IsRet (ins)) { //TODO put is back
    if (INS_IsBranchOrCall (ins)) {
        if (INS_HasFallThrough (ins)) {
            INS_InsertCall (ins, IPOINT_AFTER, (AFUNPTR) pin__getBrIns,
                    IARG_ADDRINT, INS_Address (ins),
                    IARG_BOOL, INS_HasFallThrough (ins),
                    IARG_BRANCH_TARGET_ADDR, 
                    IARG_FALLTHROUGH_ADDR,
                    IARG_BRANCH_TAKEN,
                    IARG_BOOL, is_call,
                    IARG_BOOL, is_ret,
                    IARG_BOOL, is_br_jmp,
                    IARG_BOOL, is_dir_br_jmp,
                    IARG_END);
        }
        INS_InsertCall (ins, IPOINT_TAKEN_BRANCH, (AFUNPTR) pin__getBrIns,
                IARG_ADDRINT, INS_Address (ins),
                IARG_BOOL, INS_HasFallThrough (ins),
                IARG_BRANCH_TARGET_ADDR, 
                IARG_FALLTHROUGH_ADDR,
                IARG_BRANCH_TAKEN,
                IARG_BOOL, is_call,
                IARG_BOOL, is_ret,
                IARG_BOOL, is_br_jmp,
                IARG_BOOL, is_dir_br_jmp,
                IARG_END);
        /*
        //capture mem u-op
        if (INS_IsMemoryWrite (ins) || INS_IsMemoryRead (ins)) {
        INS_InsertCall (ins, IPOINT_BEFORE, (AFUNPTR) pin__getMemIns,
        IARG_ADDRINT, INS_Address (ins),
        IARG_MEMORYWRITE_SIZE,
        IARG_MEMORYWRITE_EA,
        IARG_END);
        }
        */
    } else if (INS_IsMemoryWrite (ins)) {
        BOOL isMemRead;
        isMemRead = false;
        INS_InsertCall (ins, IPOINT_BEFORE, (AFUNPTR) pin__getMemIns,
                IARG_ADDRINT, INS_Address (ins),
                IARG_MEMORYWRITE_SIZE,
                IARG_MEMORYWRITE_EA,
                IARG_BOOL, INS_IsStackRead (ins),
                IARG_BOOL, INS_IsStackWrite (ins),
                IARG_BOOL, isMemRead,
                IARG_END);
    } else if (INS_IsMemoryRead (ins)) {
        bool isMemRead;
        isMemRead = true;
        INS_InsertCall (ins, IPOINT_BEFORE, (AFUNPTR) pin__getMemIns,
                IARG_ADDRINT, INS_Address (ins),        
                IARG_MEMORYREAD_SIZE,
                IARG_MEMORYREAD_EA,
                IARG_BOOL, INS_IsStackRead (ins),
                IARG_BOOL, INS_IsStackWrite (ins),
                IARG_BOOL, isMemRead,
                IARG_END);
    } else if (INS_IsNop (ins)) {
        INS_InsertCall (ins, IPOINT_BEFORE, (AFUNPTR) pin__getNopIns,
                IARG_ADDRINT, INS_Address (ins),
                IARG_END);
    } else {
        INS_InsertCall (ins, IPOINT_BEFORE, (AFUNPTR) pin__getIns,
                IARG_ADDRINT, INS_Address (ins),
                IARG_END);
        /*
        //capture mem u-op
        if (INS_IsMemoryWrite (ins) || INS_IsMemoryRead (ins)) {
        INS_InsertCall (ins, IPOINT_BEFORE, (AFUNPTR) pin__getMemIns,
        IARG_ADDRINT, INS_Address (ins),
        IARG_MEMORYWRITE_SIZE,
        IARG_MEMORYWRITE_EA,
        IARG_END);
        */
    }
}
