/*******************************************************************************
 * memManager.h
 * ----------------
 * Load-Store Queuee Controller Unit
 * The API for interfacing with LSQ is provided here
 * This is Naive Implementation of a OOO-LD INO-ST LSQ Management Model
 * (i.e. no store set speculation)
 ******************************************************************************/

#ifndef _O3_MEM_MANAGER_H
#define _O3_MEM_MANAGER_H

#include "../unit/dynInstruction.h"
#include "../unit/unit.h"
#include "../cacheCtrl.h"
#include "../cache.h"
#include "lsq.h"

class o3_memManager : public unit {
    public:
        o3_memManager (sysClock* clk, string rf_name);
        ~o3_memManager ();

        /* LSQ CONTROL */
        BUFF_STATE getTableState (LSQ_ID);
        bool hasFreeWire (LSQ_ID, AXES_TYPE);
        void updateWireState (LSQ_ID, AXES_TYPE);
        void pushBack (dynInstruction*);
        void memAddrReady (dynInstruction*);
        bool issueToMem (LSQ_ID);
        bool commit (dynInstruction*);
        void squash (INS_ID);
        pair<bool, dynInstruction*> hasFinishedIns (LSQ_ID);

        /* SQ CONTROL */
        bool hasCommitSt ();
        void delAfinishedSt ();
        pair<bool, dynInstruction*> isLQviolation (dynInstruction*);

        /* LQ CONTROL */
        void completeLd (dynInstruction*);

    private:
        cache _L1;
        cache _L2;
        cache _L3;

        o3_lsqCAM _LQ;
        o3_lsqCAM _SQ;
};

extern o3_memManager* g_LSQ_MGR;

#endif
