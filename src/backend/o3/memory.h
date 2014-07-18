/*******************************************************************************
 * memory.h
 ******************************************************************************/

#ifndef _O3_MEMORY_H
#define _O3_MEMORY_H

#include "../unit/stage.h"
#include "rfManager.h"
#include "memManager.h"

class o3_memory : protected stage {
	public:
		o3_memory (port<dynInstruction*>& execution_to_memory_port, 
                port<dynInstruction*>& memory_to_scheduler_port, 
                CAMtable<dynInstruction*>* iROB,
			    WIDTH memory_width,
			    string stage_name);
		~o3_memory ();

		void doMEMORY (sysClock& clk);
        PIPE_ACTIVITY memoryImpl (sysClock& clk);
        void completeIns (sysClock& clk);
        void forward (dynInstruction*, CYCLE, sysClock&);
        void squash (sysClock& clk);
        void regStat (sysClock& clk);
        void manageSTbuffer (sysClock& clk);
        void manageMEMbuffer ();
        void manageMSHR ();

	private:
		port<dynInstruction*>* _execution_to_memory_port;
        port<dynInstruction*>* _memory_to_scheduler_port;
        CAMtable<dynInstruction*>* _iROB;
        port<dynInstruction*> _mem_buff;
        RAMtable<dynInstruction*> _mshr;
        FIFOtable<dynInstruction*> _st_buff;

        cache _L1;
        cache _L2;
        cache _L3;

        /* STAT OBJS */
        ScalarStat& s_cache_miss_cnt;
        ScalarStat& s_cache_hit_cnt;
        ScalarStat& s_ld_miss_cnt;
        ScalarStat& s_ld_hit_cnt;
        ScalarStat& s_st_miss_cnt;
        ScalarStat& s_st_hit_cnt;

};

#endif