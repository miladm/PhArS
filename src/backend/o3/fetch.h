/*******************************************************************************
 * fetch::.h
 ******************************************************************************/
#ifndef _O3_FETCH_H
#define _O3_FETCH_H

#include "../unit/stage.h"

//typedef enum {FRONT_END, BACK_END} SIM_MODE;

class o3_fetch : protected stage {
	public:
		o3_fetch (port<dynInstruction*>& bp_to_fetch_port,
			   port<dynInstruction*>& fetch_to_decode_port,
			   port<dynInstruction*>& fetch_to_bp_port,
			   WIDTH fetch_width,
			   string stage_name
              );
		~o3_fetch ();

		SIM_MODE doFETCH (sysClock& clk);
        PIPE_ACTIVITY fetchImpl (sysClock& clk);
        void squash (sysClock& clk);
        void regStat (sysClock& clk);

	private:
		port<dynInstruction*>* _bp_to_fetch_port;
		port<dynInstruction*>* _fetch_to_decode_port;
		port<dynInstruction*>* _fetch_to_bp_port;
        int _insListIndx;
        bool _switch_to_frontend;
};

#endif