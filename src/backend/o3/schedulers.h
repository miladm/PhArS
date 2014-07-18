/*******************************************************************************
 * scheduler.h
 ******************************************************************************/

#ifndef _O3_SCHEDULER_H
#define _O3_SCHEDULER_H

#include "../unit/stage.h"
#include "rfManager.h"
#include "memManager.h"

class o3_scheduler : protected stage {
	public:
		o3_scheduler (port<dynInstruction*>& decode_to_scheduler_port, 
                   port<dynInstruction*>& execution_to_scheduler_port, 
                   port<dynInstruction*>& memory_to_scheduler_port, 
			       port<dynInstruction*>& scheduler_to_execution_port, 
                   CAMtable<dynInstruction*>* iROB,
			       WIDTH scheduler_width,
			       string stage_name);
		~o3_scheduler ();
		void doSCHEDULER (sysClock& clk);
        void squash (sysClock& clk);
        PIPE_ACTIVITY schedulerImpl (sysClock& clk);
        void updateResStns (sysClock& clk);
        void manageCDB (sysClock& clk);
        void forwardFromCDB (dynInstruction* ins, sysClock& clk);
        void regStat (sysClock& clk);
        bool hasReadyInsInResStn (WIDTH resStnId, LENGTH &readyInsIndx);

	private:
		port<dynInstruction*>* _decode_to_scheduler_port;
		port<dynInstruction*>* _execution_to_scheduler_port;
		port<dynInstruction*>* _memory_to_scheduler_port;
		port<dynInstruction*>* _scheduler_to_execution_port;
        CAMtable<dynInstruction*>* _iROB;
        List<CAMtable<dynInstruction*>* > _ResStns;
        WIDTH _num_res_stns;
};

#endif