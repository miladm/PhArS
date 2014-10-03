/*******************************************************************************
 * config.h
 ******************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#include <map>
#include <string>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "lib/utility.h"
#include "global/global.h"
#include "lib/message.h"
#include "lib/yaml-cpp/yaml.h"

using namespace std;

class config {
	public:
		config ();
		config (string, string);
		~config ();
        void storeSimConfig (ofstream*);

        /*-- GET CONFIG ATTRIBUTES --*/
		char* getProgName ();
        SCH_MODE getSchMode  ();
        REG_ALLOC_MODE getRegAllocMode  ();
        CORE_TYPE getCoreType ();
        MEM_MODEL getMemModel ();
        bool isEnSquash ();
        bool isEnEuFwd ();
        bool isEnMemFwd ();
        bool isEnFwd ();
        bool isEnLogStat ();

    private:
		bool parsePinPointFiles ();
		void verifyConfig ();

    public:
		std::map<SIMP,SIMW> _simpoint;
		bool _use_simpoint;

	private:
		char param[PARSE_LEN]; 
		char program_name[PARSE_LEN];
		FILE* _f_bench_cfg;
		FILE* _f_sim_cfg;
        SCH_MODE _sch_mode;
        REG_ALLOC_MODE _reg_alloc_mode;
        string _config_path;
        string _bench_path;
        bool _enable_log_stat;

		//PinPoint Config
		char pinPoint_s_file[PARSE_LEN];
		char pinPoint_w_file[PARSE_LEN];
		char output_w_file[PARSE_LEN];
		char obj_r_file[PARSE_LEN];

		//CPU Config
		CORE_TYPE _core_type;
		brMode _branch_mode;
		MEM_MODEL _mem_model;
		rrMode _reg_ren_mode;
		int _num_mem_levels;
		int _cache_lat[MEM_HIGHERARCHY];
		int _st_lat;
		int _alu_cnt;
		int _rob_size;
		int _iwin_size;
		int _pb_win_cnt;
		int _pb_win_size;
        bool _enable_squash;
        bool _enable_eu_fwd;
        bool _enable_mem_fwd;

		//DELAYS
		int _fetch_delay;
		int _bp_delay;
		int _decode_delay;
		int _regren_delay;
		int _issue_delay;
		int _alu_delay;
		int _complete_delay;
		int _wb_delay;
		int _fwd_delay;

		//UNIT WIDTHS
		int _bp_width;
		int _decode_width;
		int _issue_width;
		int _dispatch_width;
		int _commit_width;
		int _squash_width;
		int _wb_width;
		int _pb_win_width;
		int _regren_width;
		int _fetch_width;
};

extern config* g_cfg;

#endif
