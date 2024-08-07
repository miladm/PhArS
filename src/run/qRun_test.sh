#!/bin/sh

OUT_PATH='/scratch/milad/qsub_outputs/perf_sim_test/out_test'
QSUB_OUT_PATH=$OUT_PATH'/qsub_files/'
PARS_ROOT='/home/milad/esc_project/svn/PARS/src'
PIN_ROOT='/home/milad/esc_project/svn/pin-2.12'
BENCH_EXEC='/home/milad/zsim-apps/build/speccpu2006'
BENCH_INPUT='/home/milad/zsim-apps/inputs'
RUN_DIR='/home/milad/esc_project/svn/PARS/src/run'
CFG_PATH='/home/milad/esc_project/svn/PARS/src/config'
CFG_FILE=$1

mkdir $OUT_PATH
mkdir $QSUB_OUT_PATH

echo "Copying the YAML config file to output dir"
cp $CFG_PATH/base.yaml $OUT_PATH/.

## QSUB
qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_gcc        $RUN_DIR/run.sh -b $CFG_PATH/403.gcc        -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/403.gcc/403.gcc -C $BENCH_INPUT/403.gcc/test/input/cccp.i -o $OUT_PATH/out.o
qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_mcf        $RUN_DIR/run.sh -b $CFG_PATH/429.mcf        -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/429.mcf/429.mcf $BENCH_INPUT/429.mcf/test/input/inp.in
#qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_bzip2      $RUN_DIR/run.sh -b $CFG_PATH/401.bzip2      -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/401.bzip2/401.bzip2 $BENCH_INPUT/401.bzip2/test/input/dryer.jpg 2
qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_hmmer      $RUN_DIR/run.sh -b $CFG_PATH/456.hmmer      -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/456.hmmer/456.hmmer --fixed 0 --mean 325 --num 45000 --sd 200 --seed 0  $BENCH_INPUT/456.hmmer/test/input/bombesin.hmm
qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_sjeng      $RUN_DIR/run.sh -b $CFG_PATH/458.sjeng      -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/458.sjeng/458.sjeng $BENCH_INPUT/458.sjeng/test/input/test.txt
#qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_gobmk      $RUN_DIR/run.sh -b $CFG_PATH/445.gobmk      -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/445.gobmk/445.gobmk --quiet --mode gtp < $BENCH_INPUT/445.gobmk/test/input/capture.tst
qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_libquantum $RUN_DIR/run.sh -b $CFG_PATH/462.libquantum -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/462.libquantum/462.libquantum 33 5
#qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_h264ref    $RUN_DIR/run.sh -b $CFG_PATH/464.h264ref    -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/464.h264ref/464.h264ref  -d $BENCH_INPUT/464.h264ref/test/input/foreman_test_encoder_baseline.cfg -p InputFile="$BENCH_INPUT/464.h264ref/all/input/foreman_qcif.yuv"
qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_astar      $RUN_DIR/run.sh -b $CFG_PATH/473.astar      -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/473.astar/473.astar $BENCH_INPUT/473.astar/test/input/lake.cfg
#qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_xalancbmk  $RUN_DIR/run.sh -b $CFG_PATH/483.xalancbmk  -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/483.xalancbmk/483.xalancbmk -v $BENCH_INPUT/483.xalancbmk/test/input/test.xml $BENCH_INPUT/483.xalancbmk/test/input/xalanc.xsl
qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_perlbench  $RUN_DIR/run.sh -b $CFG_PATH/400.perlbench  -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/400.perlbench/400.perlbench -I. -I $BENCH_INPUT/400.perlbench/all/input/lib/ $BENCH_INPUT/400.perlbench/test/input/attrs.pl
#
#cd $BENCH_INPUT/471.omnetpp/test/input
#qsub -e $QSUB_OUT_PATH -o $OUT_PATH -V -l p=16 -pe smp 1 -S /bin/sh -N ts_omnetpp    $RUN_DIR/run.sh -b $CFG_PATH/471.omnetpp    -c $CFG_PATH/$CFG_FILE -o $OUT_PATH -- $BENCH_EXEC/471.omnetpp/471.omnetpp -d omnetpp.ini


##@ TERMINAL
#../../../pin -t obj-intel64/wrong_path.so  -usectxt -- -- $BENCH_EXEC/403.gcc/403.gcc -C $BENCH_INPUT/403.gcc/test/input/cccp.i -o out.o

#../../../pin $PIN_PARAM -t obj-intel64/wrong_path.so  -usectxt -- /home/milad/esc_project/svn/benchmarks/spec2006/mcf-1.3_2/src/mcf /home/milad/esc_project/svn/benchmarks/spec2006/mcf-1.3_2/src/inp_small.in

#../../../pin $PIN_PARAM -pin_memory_range 0x80000000:0x90000000 -pause_tool 15 -t obj-intel64/wrong_path.so  -usectxt -- /home/milad/esc_project/svn/benchmarks/spec2006/bzip2/bzip2  -kf /home/milad/esc_project/svn/benchmarks/spec2006/bzip2/sample2.ref

#../../../pin $PIN_PARAM -t obj-intel64/wrong_path.so  -usectxt -- /home/milad/esc_project/svn/benchmarks/spec2006/bzip2/bzip2  -kf /home/milad/esc_project/svn/benchmarks/spec2006/bzip2/sample2.ref

#../../../pin -t obj-intel64/wrong_path.so  -usectxt -- -- $BENCH_EXEC/456.hmmer/456.hmmer --fixed 0 --mean 325 --num 45000 --sd 200 --seed 0  $BENCH_INPUT/456.hmmer/test/input/bombesin.hmm

#../../../pin -t obj-intel64/wrong_path.so  -usectxt -- -- $BENCH_EXEC/458.sjeng/458.sjeng $BENCH_INPUT/458.sjeng/test/input/test.txt

#../../../pin -t obj-intel64/wrong_path.so  -usectxt -- -- $BENCH_EXEC/445.gobmk/445.gobmk --quiet --mode gtp < $BENCH_INPUT/445.gobmk/test/input/capture.tst

# $PIN_ROOT/pin -t $PARS_ROOT/obj-intel64/main_pars.so -- -- $BENCH_EXEC/462.libquantum/462.libquantum 33 5

# echo "$PIN_ROOT/pin -t $PARS_ROOT/obj-intel64/main_pars.so -- -- $BENCH_EXEC/462.libquantum/462.libquantum 33 5"

#../../../pin -t obj-intel64/wrong_path.so  -usectxt -- -- $BENCH_EXEC/464.h264ref/464.h264ref  -d $BENCH_INPUT/464.h264ref/test/input/foreman_test_encoder_baseline.cfg -p InputFile="$BENCH_INPUT/464.h264ref/all/input/foreman_qcif.yuv"

#../../../pin -t obj-intel64/wrong_path.so  -usectxt -- -- $BENCH_EXEC/471.omnetpp/471.omnetpp -d $BENCH_INPUT/471.omnetpp/test/input/omnetpp.ini

#../../../pin -t obj-intel64/wrong_path.so  -usectxt -- -- $BENCH_EXEC/473.astar/473.astar $BENCH_INPUT/473.astar/test/input/lake.cfg

#../../../pin -t obj-intel64/wrong_path.so  -usectxt -- -- $BENCH_EXEC/483.xalancbmk/483.xalancbmk -v $BENCH_INPUT/483.xalancbmk/test/input/test.xml $BENCH_INPUT/483.xalancbmk/test/input/xalanc.xsl

#../../../pin -t obj-intel64/wrong_path.so  -usectxt -- -- $BENCH_EXEC/400.perlbench/400.perlbench -I $BENCH_INPUT/400.perlbench/ref/lib/test.xml $BENCH_INPUT/400.perlbench/test/input/attrs.pl
