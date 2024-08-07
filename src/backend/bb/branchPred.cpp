/*******************************************************************************
 * branchPred.cpp
 *******************************************************************************/

#include "branchPred.h"

bb_branchPred::bb_branchPred (port<bbInstruction*>& fetch_to_bp_port, 
			    		port<bbInstruction*>& bp_to_fetch_port, 
	    				WIDTH bp_width,
                        sysClock* clk,
	    				string stage_name) 
	: stage (bp_width, stage_name, g_cfg->_root["cpu"]["backend"]["bb_pipe"]["bp"], clk)
{ 
    _fetch_to_bp_port = &fetch_to_bp_port;
    _bp_to_fetch_port = &bp_to_fetch_port;
}

bb_branchPred::~bb_branchPred () {}

void bb_branchPred::doBP () {
    /*-- STAT --*/
    regStat ();
}

void bb_branchPred::squash () {
    _e_stage.ffAccess ();
}

void bb_branchPred::regStat () {
    _fetch_to_bp_port->regStat ();
    _e_stage.ffAccess (); //READ
    _e_stage.ffAccess (); //WRITE
}


