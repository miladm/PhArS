#!/usr/bin/env python
from __future__ import division
from collections import deque
from collections import defaultdict
from time import time
import os, glob, os.path
import sys
import re
import os

# VERIFY THE NUMBER OF EXE INSTRUCTIONS IS SUFFICIENTLY LARGE
def stages (result_map):
    fetch_energy (result_map)
    decode_rr_energy (result_map)
    issue_energy (result_map)
    execute_energy (result_map)
    memory_energy (result_map)
    rf_energy (result_map)
    l2_l3_energy (result_map)
    commit_energy (result_map)

def fetch_energy (result_map):
    energy = 0

    energy += result_map['2bc_gskew.e_ram']
    energy += result_map['2bc_gskew.e_leak']
    energy += result_map['2bc_gskew.wire.e_w_bp2cache_o3']
    energy += result_map['2bc_gskew.wire.e_w_bp2cache_ino']
    energy += result_map['2bc_gskew.wire.e_w_bp2cache_bb']
    energy += result_map['BTB.e_cam']
    energy += result_map['BTB.e_leak']
    energy += result_map['fetch.e_ff']
    energy += result_map['fetch.e_leak']
    energy += result_map['branchPred.e_ff']
    energy += result_map['branchPred.e_leak']
    energy += result_map['l1_i_0.e_ram']
    energy += result_map['l1_i_0.e_leak']
    energy += result_map['itlb.e_cam']

    result_map['fetch'] = energy

def decode_rr_energy (result_map):
    energy = 0

    energy += result_map['grfManager.rr.wire.e_w_cache2rr']
    energy += result_map['registerFile.rr.wire.e_w_cache2rr']
    energy += result_map['registerRename.rd_wire.e_wire']
    energy += result_map['GlobalRegisterRename.rd_wire.e_wire']
    energy += result_map['decode.e_ff']
    energy += result_map['decode.e_leak']

    result_map['decode-rename'] = energy

def issue_energy (result_map):
    energy = 0

    energy += result_map['iWindow.wr_wire.e_w_cache2win']
    energy += result_map['schedule.e_ff']
    energy += result_map['schedule.e_leak']
    energy += result_map['iWindow.e_leak']
    energy += result_map['iWindow.e_ram']
    energy += result_map['iWindow.rd_wire.e_wire']
    energy += result_map['iWindow.wr_wire.e_w_win2eu']
    energy += result_map['ResStn_0.wr_wire.e_w_rr2rs']
    energy += result_map['ResStn_0.rd_wire.e_w_rs2eu']
    energy += result_map['ResStn_0.e_leak']
    energy += result_map['ResStn_0.e_cam']
    energy += result_map['ResStn_0.e_cam2']
    energy += result_map['ResStn_0.e_ram']
    energy += result_map['bbWindow.wr_wire.e_w_rr2iq']
    energy += result_map['bbWindow.rd_wire.e_w_iq2eu']
    energy += result_map['bbWindow.e_cam']
    energy += result_map['bbWindow.e_ram']
    energy += result_map['bbWindow.e_leak']
    energy += result_map['bbWinBuf.e_cam']
    energy += result_map['bbWinBuf.e_ram']
    energy += result_map['bbWinBuf.e_leak']

    result_map['issue'] = energy

def execute_energy (result_map):
    energy = 0

    energy += result_map['EU.e_eu']
    energy += result_map['execution.e_ff']
    energy += result_map['execution.e_leak']

    result_map['execute'] = energy

def memory_energy (result_map):
    energy = 0

    energy += result_map['stBuff.e_ram']
    energy += result_map['stBuff.e_leak']
    energy += result_map['SQ.wr_wire.e_w_eu2lsq_bb']
    energy += result_map['SQ.rd_wire.e_w_rr2lsq_bb']
    energy += result_map['SQ.rd_wire.e_w_cache2lsq_bb']
    energy += result_map['SQ.rd_wire.e_w_lsq2brob']
    energy += result_map['SQ.wr_wire.e_w_eu2lsq_o3']
    energy += result_map['SQ.rd_wire.e_w_rr2lsq_o3']
    energy += result_map['SQ.rd_wire.e_w_cache2lsq_o3']
    energy += result_map['SQ.e_cam']
    energy += result_map['SQ.e_ram']
    energy += result_map['SQ.e_leak']
    energy += result_map['LQ.wr_wire.e_w_eu2lsq_bb']
    energy += result_map['LQ.rd_wire.e_w_rr2lsq_bb']
    energy += result_map['LQ.rd_wire.e_w_cache2lsq_bb']
    energy += result_map['LQ.rd_wire.e_w_lsq2brob']
    energy += result_map['LQ.wr_wire.e_w_eu2lsq_o3']
    energy += result_map['LQ.rd_wire.e_w_rr2lsq_o3']
    energy += result_map['LQ.rd_wire.e_w_cache2lsq_o3']
    energy += result_map['LQ.rd_wire.e_w_lsq2rf']
    energy += result_map['LQ.e_cam']
    energy += result_map['LQ.e_ram']
    energy += result_map['LQ.e_leak']
    energy += result_map['dtlb.e_cam']
    energy += result_map['itlb.e_cam']
    energy += result_map['memory.e_ff']
    energy += result_map['memory.e_leak']
    energy += result_map['l1_d_0.e_ram']
    energy += result_map['l1_d_0.e_leak']

    result_map['memory'] = energy

def l2_l3_energy (result_map):
    energy = 0

    energy += result_map['l2_0.e_ram']
    energy += result_map['l2_0.e_leak']
    energy += result_map['l3_0.e_ram']
    energy += result_map['l3_0.e_leak']

    result_map['l2-l3'] = energy

def commit_energy (result_map):
    energy = 0

    energy += result_map['LQ.rd_wire.e_w_lsq2rob']
    energy += result_map['SQ.rd_wire.e_w_lsq2rob']
    energy += result_map['bbROB.wr_wire.e_w_eu2brob']
    energy += result_map['bbROB.wr_wire.e_w_rr2brob']
    energy += result_map['bbROB.rd_wire.e_wire']
    energy += result_map['bbROB.e_ram']
    energy += result_map['bbROB.e_leak']
    energy += result_map['iROB.rd_wire.e_w_eu2rob']
    energy += result_map['iROB.wr_wire.e_w_rr2rob']
    energy += result_map['iROB.e_ram']
    energy += result_map['iROB.e_leak']
    energy += result_map['commit.e_ff']
    energy += result_map['commit.e_leak']

    result_map['commit'] = energy

def rf_energy (result_map):
    energy = 0

    energy += result_map['lrfManager.e_leak']
    energy += result_map['lrfManager.e_ram']
    energy += result_map['registerRename.wr_wire.e_w_eu2rf']
    energy += result_map['GlobalRegisterRename.wr_wire.e_w_eu2grf']
    energy += result_map['LocalRegisterFile.wr_wire.e_w_eu2lrf']
    energy += result_map['LocalRegisterFile.rd_wire.e_wire']
    energy += result_map['LQ.rd_wire.e_w_lsq2lrf']
    energy += result_map['LQ.rd_wire.e_w_lsq2grf']
    energy += result_map['rfManager.e_ram']
    energy += result_map['registerFile.rd_wire.e_wire']
    energy += result_map['registerFile.wr_wire.e_wire']
    energy += result_map['registerFile.wr_wire.e_w_cache2rf']
    energy += result_map['registerFile.wr_wire.e_w_eu2simplerf']
    energy += result_map['grfManager.grf.e_leak']
    energy += result_map['grfManager.grf.e_ram']
    energy += result_map['grfManager.grat.e_ram']
    energy += result_map['grfManager.gapr.e_ram']
    energy += result_map['grfManager.garst.e_ram']

    result_map['rf'] = energy

def wire_energy (result_map):
    energy = 0

    energy += result_map['2bc_gskew.wire.e_w_bp2cache_o3']
    energy += result_map['2bc_gskew.wire.e_w_bp2cache_ino']
    energy += result_map['2bc_gskew.wire.e_w_bp2cache_bb']
    energy += result_map['grfManager.rr.wire.e_w_cache2rr']
    energy += result_map['registerFile.rr.wire.e_w_cache2rr']
    energy += result_map['iWindow.wr_wire.e_w_cache2win']
    energy += result_map['iWindow.wr_wire.e_w_win2eu']
    energy += result_map['ResStn_0.wr_wire.e_w_rr2rs']
    energy += result_map['ResStn_0.rd_wire.e_w_rs2eu']
    energy += result_map['bbWindow.wr_wire.e_w_rr2iq']
    energy += result_map['bbWindow.rd_wire.e_w_iq2eu']
    energy += result_map['SQ.wr_wire.e_w_eu2lsq_bb']
    energy += result_map['SQ.rd_wire.e_w_rr2lsq_bb']
    energy += result_map['SQ.rd_wire.e_w_cache2lsq_bb']
    energy += result_map['SQ.rd_wire.e_w_lsq2brob']
    energy += result_map['SQ.wr_wire.e_w_eu2lsq_o3']
    energy += result_map['SQ.rd_wire.e_w_rr2lsq_o3']
    energy += result_map['SQ.rd_wire.e_w_cache2lsq_o3']
    energy += result_map['SQ.rd_wire.e_w_lsq2rob']
    energy += result_map['LQ.wr_wire.e_w_eu2lsq_bb']
    energy += result_map['LQ.rd_wire.e_w_rr2lsq_bb']
    energy += result_map['LQ.rd_wire.e_w_cache2lsq_bb']
    energy += result_map['LQ.rd_wire.e_w_lsq2brob']
    energy += result_map['LQ.wr_wire.e_w_eu2lsq_o3']
    energy += result_map['LQ.rd_wire.e_w_rr2lsq_o3']
    energy += result_map['LQ.rd_wire.e_w_cache2lsq_o3']
    energy += result_map['LQ.rd_wire.e_w_lsq2rf']
    energy += result_map['LQ.rd_wire.e_w_lsq2rob']
    energy += result_map['LQ.rd_wire.e_w_lsq2lrf']
    energy += result_map['LQ.rd_wire.e_w_lsq2grf']
    energy += result_map['bbROB.wr_wire.e_w_eu2brob']
    energy += result_map['bbROB.wr_wire.e_w_rr2brob']
    energy += result_map['iROB.rd_wire.e_w_eu2rob']
    energy += result_map['iROB.wr_wire.e_w_rr2rob']
    energy += result_map['registerRename.wr_wire.e_w_eu2rf']
    energy += result_map['GlobalRegisterRename.wr_wire.e_w_eu2grf']
    energy += result_map['LocalRegisterFile.wr_wire.e_w_eu2lrf']
    energy += result_map['registerFile.wr_wire.e_w_cache2rf']
    energy += result_map['registerFile.wr_wire.e_w_eu2simplerf']

    result_map['wire'] = energy

def stage_energy (result_map):
    energy = 0

    energy += result_map['fetch.e_leak']
    energy += result_map['branchPred.e_leak']
    energy += result_map['decode.e_leak']
    energy += result_map['schedule.e_leak']
    energy += result_map['execution.e_leak']
    energy += result_map['stBuff.e_ram']
    energy += result_map['stBuff.e_leak']
    energy += result_map['memory.e_leak']
    energy += result_map['commit.e_leak']
    energy += result_map['fetch.e_ff']
    energy += result_map['branchPred.e_ff']
    energy += result_map['decode.e_ff']
    energy += result_map['schedule.e_ff']
    energy += result_map['execution.e_ff']
    energy += result_map['stBuff.e_ram']
    energy += result_map['stBuff.e_leak']
    energy += result_map['memory.e_ff']
    energy += result_map['commit.e_ff']

    result_map['stage-reg'] = energy

# ENERGY DELAY ANALYSIS
def ed (result_map):
    ed = 0.0
    cycle_time = 0.5e-9
    energy = result_map['TOTAL.Energy']
    delay = result_map['sysClock.clk_cycles'] * cycle_time
    ed = energy * delay

    result_map['ED'] = ed

def ed2 (result_map):
    ed2 = 0.0
    cycle_time = 0.5e-9
    energy = result_map['TOTAL.Energy']
    delay = result_map['sysClock.clk_cycles'] * cycle_time
    ed2 = energy * delay * delay

    result_map['ED2'] = ed2

# EU BUSY BREAKDOWN
def no_compute (result_map):
    result_map['no-compute'] = 1.0 - result_map['execution.ipc']
    
def mem_compute (result_map):
    mem_ops = result_map['commit.ins_type_cnt_2']
    all_ops = result_map['commit.ins_cnt']
    mem_rate = float(mem_ops) / all_ops
    result_map['mem-compute'] = result_map['execution.ipc'] * mem_rate
    
def alu_compute (result_map):
    alu_ops = result_map['commit.ins_type_cnt_1']
    br_ops = result_map['commit.ins_type_cnt_4']
    all_ops = result_map['commit.ins_cnt']
    alu_rate = float(alu_ops + br_ops) / all_ops
    result_map['alu-compute'] = result_map['execution.ipc'] * alu_rate

#SPECULATION ACCURACY
def br_accuracy (result_map):
    bp_mis_pred = result_map['pars.bp_misspred_cnt']
    btb_mis_pred = result_map['pars.btb_misspred_cnt']
#    bpu_lookup = result_map['pars.bpu_lookup_cnt']
    ins_cnt = result_map['commit.ins_cnt']

    accu = (bp_mis_pred + btb_mis_pred) / ins_cnt
    accu_per_1k = accu * 1000

    result_map['bpu-accuracy'] = accu_per_1k

#SPECULATION ACCURACY
def mem_accuracy (result_map):
    mem_mis_pred = result_map['execution.mem_mispred_cnt']
#    mem_lookup = result_map['memory.ins_cnt']
    ins_cnt = result_map['commit.ins_cnt']

    accu = mem_mis_pred / ins_cnt
    accu_per_1k = accu * 1000

    result_map['mem-accuracy'] = accu_per_1k

#MISC
def wasted_ins_rat (result_map):
    wasted_ins_cnt = result_map['commit.num_waste_ins']
    ins_cnt = result_map['commit.ins_cnt']
    rat = wasted_ins_cnt / ins_cnt
    rat_per_1k = rat * 1000

    result_map['wasted_ins_rat'] = rat_per_1k
