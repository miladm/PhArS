/*******************************************************************************
 * lrfManager.h
 ******************************************************************************/

#ifndef _BB_LRF_MANAGER_H
#define _BB_LRF_MANAGER_H

#include "../unit/unit.h"
#include "../unit/bbInstruction.h"
#include "../unit/registerFile.h"

class bb_lrfManager : public unit {
	public:
		bb_lrfManager (WIDTH, sysClock*, const YAML::Node&, string rf_name = "bb_lrfManager");
		~bb_lrfManager ();
        void resetRF ();
        bool isReady (bbInstruction* ins);
        bool checkReadyAgain (bbInstruction* ins);
        void reserveRF (bbInstruction* ins);
        bool canReserveRF (bbInstruction* ins);
        void writeToRF (bbInstruction* ins);
        void updateReg (PR reg);

        /* WIRES CTRL */
        bool hasFreeWire (AXES_TYPE, WIDTH);
        void updateWireState (AXES_TYPE, WIDTH, list<string> wire_name = list<string>(), bool update_wire = false);

        void getStat ();

	private:
        registerFile _RF;
        WIDTH _lrf_id;

    public:
        /*-- ENERGY --*/
        table_energy _e_table;

    private:
        /*-- STAT OBJS --*/
        ScalarStat& s_unavailable_cnt;
};

#endif
