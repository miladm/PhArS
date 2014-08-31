/*******************************************************************************
 * global.h define global parameters
 ******************************************************************************/
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <map>
#include <string.h>
#include <string>
#include "../lib/list.h"

typedef long long unsigned int ADDRS;
typedef long long unsigned int INS_ADDR;
typedef long long unsigned int MEM_ADDR;
typedef long long unsigned int INS_ID;
typedef long long unsigned int BB_ID;
typedef unsigned AR; //Archiectural Register
typedef unsigned PR; //Physical Register
typedef bool VALID; //Valid bit
typedef long int CYCLE;
typedef int GHz;
typedef int BITS;
typedef unsigned BYTES;
typedef int LENGTH;
typedef int WIDTH;
typedef int nMETER;
typedef long int SCALAR;

typedef char INS_STR;
//typedef dynInstruction INS_OBJ;

typedef enum {EMPTY_BUFF, AVAILABLE_BUFF, FULL_BUFF} BUFF_STATE;
typedef enum {NO_STAGE, FETCH, DECODE, DISPATCH, ISSUE, EXECUTE, MEM_ACCESS, COMPLETE, COMMIT, ready, chain, execute, complete, sideReady, sideBuffer} status; typedef status PIPE_STAGE; //TODO clean up
typedef enum {noType, ALU, MEM, FPU, BR, ASSIGN, NOP, bbHEAD} type; typedef type INS_TYPE; //TODO clean this up
typedef enum {CALL, RET, DIR_BR, INDIR_BR, JMP} brType; typedef brType BR_TYPE; //TODO clean this up
typedef enum {LOAD, STORE} MEM_TYPE;
typedef enum {none, READ, WRITE} memType; typedef memType AXES_TYPE; //TODO clean this up
typedef enum {noBrMode, noBr, allBr, statPredBr, scheduleBr, lowBiasBr, dynPredBr} brMode;
typedef enum {NO_CORE, IN_ORDER, OUT_OF_ORDER, ONE_LEVEL_DEEP_DYN, X_LEVEL_DEEP_DYN, ONE_LEVE_DEEP_STAT, X_LEVE_DEEP_STAT, PHRASE, FRAGMENT, FRAGMENT2, DOT, STRAND, PHRASEBLOCK, BASICBLOCK} core; typedef core CORE_TYPE; //TODO clean this up
typedef enum {PERFECT, TOTAL_ORDER, NAIVE_SPECUL, STORE_SET_SPECUL} memModel;
typedef enum {RR_ACTIVE, RR_INACTIVE} rrMode;

typedef enum {PIPE_NORMAL, PIPE_WAIT_FLUSH, PIPE_FLUSH, PIPE_DRAIN, PIPE_SQUASH_ROB} PIPE_STATE;
typedef enum {SQ_NO_STATE, SQ_ADDR_WAIT, SQ_COMPLETE, SQ_COMMIT, SQ_CACHE_DISPATCH} SQ_STATE;
typedef enum {LQ_NO_STATE, LQ_ADDR_WAIT, LQ_PENDING_CACHE_DISPATCH, LQ_FWD_FROM_SQ, LQ_MSHR_WAIT, LQ_CACHE_WAIT, LQ_COMPLETE} LQ_STATE;

typedef enum {AVAILABLE, RENAMED_INVALID, RENAMED_VALID, ARCH_REG} REG_REN_STATE;
typedef enum {NO_VAL_REG, WAIT_ON_WRITE_REG, DONE_WRITE_REG} REG_STATE;
typedef enum {NO_MISPRED, MEM_MISPRED, BP_MISPRED} PIPE_SQUASH_TYPE;
typedef enum {LD_QU, ST_QU} LSQ_ID;


typedef enum {
//  DBG_ALL = 0x0, 
//  DBG_INS = 0x1, 
//  DBG_RESTORE_MEM = 0x2, 
//  DBG_WRITE_MEM = 0x4, 
//  DBG_READ_MEM = 0x8, 
//  DBG_SPEC = 0x10, 
//  DBG_EXEC = 0x20, 
//  DBG_BP = 0x40, 
//  DBG_CC = 0x80, 
//  DBG_INSBUF = 0x100, 
    DBG_BRPRED = 0x200, 
    DBG_FETCH = 0x400,
    DBG_DECODE = 0x800,
    DBG_CORE = 0x1000, 
    DBG_SCHEDULER = 0x2000, 
    DBG_EXECUTION = 0x4000, 
    DBG_COMMIT = 0x8000, 
    DBG_MEMORY = 0x10000, 
    DBG_SQUASH = 0x10000, 
    DBG_PORT = 0x20000, 
    DBG_TEST = 0x40000,
    DBG_INIT = 0x80000,
    DBG_REG_FILES = 0x700000,
    DBG_L_REG_FILES = 0x200000,
    DBG_G_REG_FILES = 0x400000
} DBG_LEVEL;

#define INS_STRING_SIZE 1000
#define BLOCK_OFFSET 3
#define WORD_OFFSET 3
#define NUM_NONBLOCKING_LOAD 30
#define ONE_LEVEL_DEEP_BUFF_SIZE 30
#define MEM_ACCESS_UNIT_INDX 0
#define ROB_SIZE 200
#define PB_ROB_SIZE 20
#define ASSIGN_LATENCY 1
#define ADDR_COMPUTE_LATENCY 1
#define CAM_ACCESS_LATENCY 1
#define ALU_LATENCY 1
#define FPU_LATENCY 5
#define BR_LATENCY 3 //IF 1 cycles + ID 2 cycles (should be 5 for OOO?)
#define NUM_REGISTERS 41
#define NUM_SIDE_BUFFERS 10 //Support for only 10 side buffers (more than enough)
#define NUM_PHRASEBLKS 8
#define INIT_RENAME_REG_NUM 100
#define QUEUE_MIN_SIZE 20
#define FETCH_STATE_LATENCY 3 //Means 4 cycles of latency for fetch state

#define NUM_FUNC_UNIT 4
#define NUM_ALU_UNIT 3
#define NUM_MEM_UNIT 1

#define SB_SIZE_LIMIT 15 //TODO make sure this number makes sense.
#define MAIN_STREAM_LIMIT 10

#define UNPRED_MEM_THRESHOLD 0.10

#define COEFF 1000000


//Latencies (from SandyBridge)
#define LONG_LATENCY 1000 //a latency placeholder until actual value found
#define ST_LATENCY 2 //1: addr compute 2: SQ access
#define ADDR_COMPUTE_LATENCY 1
#define L1_LATENCY    4+ADDR_COMPUTE_LATENCY+CAM_ACCESS_LATENCY
#define L2_LATENCY   20+ADDR_COMPUTE_LATENCY+CAM_ACCESS_LATENCY
#define L3_LATENCY   40+ADDR_COMPUTE_LATENCY+CAM_ACCESS_LATENCY
#define MEM_LATENCY 100+ADDR_COMPUTE_LATENCY+CAM_ACCESS_LATENCY
#define MEM_HIGHERARCHY 4

// Regsiter Renaming
#define USE_LRF true

#define LARF_SIZE 2000
#define GARF_SIZE 6000
#define GRRF_SIZE 8000
#define GPRF_SIZE GARF_SIZE+GRRF_SIZE

#define LARF_LO 100
#define LARF_HI LARF_LO+LARF_SIZE-1

#define GARF_LO LARF_HI+1
#define GARF_HI GARF_LO+GARF_SIZE-1

#define GRRF_LO GARF_HI+1
#define GRRF_HI GRRF_LO+GRRF_SIZE-1

#define GPRF_LO 1
#define GPRF_HI GPRF_LO+GARF_SIZE+GRRF_SIZE-1

//FRONT END GLOBALS

#ifndef ASSERTION
#define ASSERTION 1
#endif /*ASSERTION*/

#ifndef __do_debug
#define __do_debug 1
#endif /*__do_debug*/

#define DBG_INS 0x1
#define DBG_RESTORE_MEM 0x2
#define DBG_WRITE_MEM 0x4
#define DBG_READ_MEM 0x8
#define DBG_SPEC 0x10
#define DBG_EXEC 0x20
#define DBG_BP 0x40
#define DBG_CC 0x80
#define DBG_INSBUF 0x100
#define DBG_UOP 0x200

#define MAX_MEM_WRITE_LEN 32

#define V_FRONTEND 0x1
#define V_BACKEND 0x1

/*-- DEFINE --*/
#ifndef DEFS
#define PARSE_LEN 200
#define SIMP_WINDOW_SIZE 3000000
#define MILLION 1000000
#define BILLION 1000000000
#define START_CYCLE 0
#endif

typedef unsigned long SIMP;
typedef double SIMW;

#endif /*_GLOBAL_H*/