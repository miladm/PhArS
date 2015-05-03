RESULTS_DIR=/scratch/milad/qsub_outputs/perf_sim_test

# REMOVE OLD LOG FILE
rm log.out

#./call_run.sh $RESULTS_DIR/multi_bb_fetch/backend_cfg/bb1
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/backend_cfg/bb2
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/backend_cfg/bb3
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/backend_cfg/bb3_4eu
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/backend_cfg/bb3_4eu_share2
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/backend_cfg/bb3_4eu_share4
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/backend_cfg/bb3_4eu_share8
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/backend_cfg/bb3_4eu_share3
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/backend_cfg/bb3_6eu_share3
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/backend_cfg/bb3_8eu_share3
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/btb_test/bb
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/btb_test/o3_ino
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/sgrd_test/198_pr/bb
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/sgrd_test/198_pr/o3_ino
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/sgrd_test/216_pr/bb
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/sgrd_test/216_pr/o3_ino
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/sgrd_test/252_pr/bb
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/sgrd_test/252_pr/o3_ino
./call_run.sh $RESULTS_DIR/multi_bb_fetch/width_size/width_1/bb
./call_run.sh $RESULTS_DIR/multi_bb_fetch/width_size/width_1/o3_ino
./call_run.sh $RESULTS_DIR/multi_bb_fetch/width_size/width_2/bb
./call_run.sh $RESULTS_DIR/multi_bb_fetch/width_size/width_2/o3_ino
./call_run.sh $RESULTS_DIR/multi_bb_fetch/width_size/width_4/bb
./call_run.sh $RESULTS_DIR/multi_bb_fetch/width_size/width_4/o3_ino
./call_run.sh $RESULTS_DIR/multi_bb_fetch/width_size/width_8/bb
./call_run.sh $RESULTS_DIR/multi_bb_fetch/width_size/width_8/o3_ino
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/perfect_mem_br/bb
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/perfect_mem_br/o3_ino
#./call_run.sh $RESULTS_DIR/multi_bb_fetch/perfect_mem/o3_ino_w1
./call_run.sh $RESULTS_DIR/multi_bb_fetch/bbWin_cnt/36_bb
./call_run.sh $RESULTS_DIR/multi_bb_fetch/bbWin_cnt/18_bb
./call_run.sh $RESULTS_DIR/multi_bb_fetch/bbWin_cnt/9_bb
./call_run.sh $RESULTS_DIR/multi_bb_fetch/bbWin_cnt/3_bb
./call_run.sh $RESULTS_DIR/multi_bb_fetch/spec_dyn_effect/1bw_ino_multi_issue
./call_run.sh $RESULTS_DIR/multi_bb_fetch/spec_dyn_effect/1bw_ino_single_issue
./call_run.sh $RESULTS_DIR/multi_bb_fetch/spec_dyn_effect/1bw_skipahead
./call_run.sh $RESULTS_DIR/multi_bb_fetch/spec_dyn_effect/9bw_ino_multi_issue
./call_run.sh $RESULTS_DIR/multi_bb_fetch/spec_dyn_effect/9bw_ino_single_issue
./call_run.sh $RESULTS_DIR/multi_bb_fetch/spec_dyn_effect/9bw_skipahead
./call_run.sh $RESULTS_DIR/multi_bb_fetch/multi_pass_issue/runahead_2
./call_run.sh $RESULTS_DIR/multi_bb_fetch/multi_pass_issue/runahead_3
./call_run.sh $RESULTS_DIR/multi_bb_fetch/multi_pass_issue/runahead_4
./call_run.sh $RESULTS_DIR/multi_bb_fetch/sgrf_cnt/sgrf_1
./call_run.sh $RESULTS_DIR/multi_bb_fetch/sgrf_cnt/sgrf_3
./call_run.sh $RESULTS_DIR/multi_bb_fetch/sgrf_cnt/sgrf_9
