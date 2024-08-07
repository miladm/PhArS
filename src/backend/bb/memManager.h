/*******************************************************************************
 * memManager.h
 * ----------------
 * Load-Store Queuee Controller Unit
 * The API for interfacing with LSQ is provided here
 * This is Naive Implementation of a OOO-LD INO-ST LSQ Management Model
 * (i.e. no store set speculation)
 ******************************************************************************/

#ifndef _BB_MEM_MANAGER_H
#define _BB_MEM_MANAGER_H

#include "../unit/bbInstruction.h"
#include "../unit/exeUnit.h"
#include "../unit/unit.h"
//#include "../cacheCtrl.h"
//#include "../cache.h"
#include "../unit/port.h"
#include "lsq.h"
#include <driver_simple.h>

class bb_memManager : public unit {
    public:
        bb_memManager (port<bbInstruction*>& memory_to_scheduler_port,
                       sysClock* clk, 
                       string lsq_name);
        ~bb_memManager ();
        void regStat ();

        /* LSQ CONTROL */
        BUFF_STATE getTableState (LSQ_ID);
        bool hasFreeWire (LSQ_ID, AXES_TYPE);
        void updateWireState (LSQ_ID, AXES_TYPE, list<string> wire_name = list<string>(), bool update_wire = false);
        void pushBack (bbInstruction*);
        void memAddrReady (bbInstruction*);
        bool issueToMem (LSQ_ID);
        bool commit (bbInstruction*);
        void squash (INS_ID);
        pair<bool, bbInstruction*> hasFinishedIns (LSQ_ID);
        CYCLE getAxesLatency (bbInstruction*);

        /* SQ CONTROL */
        bool hasCommitSt ();
        void delAfinishedSt ();
        bool hasStToAddr (ADDRS, INS_ID);
        pair<bool, bbInstruction*> isLQviolation (bbInstruction*);

        /* LQ CONTROL */
        void completeLd (bbInstruction*);
        void forward (bbInstruction*, CYCLE);

    private:
        port<bbInstruction*>* _memory_to_scheduler_port;

//        cache _L1;
//        cache _L2;
//        cache _L3;
        Driver _cache;

        bb_lsqCAM _LQ;
        bb_lsqCAM _SQ;

        /*-- TLB ENERGY (must remove from here TODO) --*/
        table_energy _e_dtlb;

        /*-- STAT OBJS --*/
        ScalarStat& s_ld_cnt;
        ScalarStat& s_cache_hit_cnt;
        ScalarStat& s_cache_miss_cnt;
        ScalarStat& s_cache_access_cnt;
        ScalarStat& s_ld_hit_cnt;
        ScalarStat& s_ld_miss_cnt;
        ScalarStat& s_st_miss_cnt;
        ScalarStat& s_st_hit_cnt;
        ScalarStat& s_cache_to_ld_fwd_cnt;
        ScalarStat& s_st_to_ld_fwd_cnt;
        RatioStat& s_st_to_ld_fwd_rat;
        RatioStat& s_ld_miss_rat;
        RatioStat& s_mem_miss_rat;
        RatioStat& s_inflight_ld_rat;
        RatioStat& s_inflight_cache_ld_rat;
};

#endif
