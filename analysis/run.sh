MODE=$1
RESULT_IN_DIR=$2
RESULT_OUT_DIR=$3

echo "*** **** ***"
echo $RESULT_IN_DIR
echo $MODE
./results_gen.py $MODE $RESULT_IN_DIR $RESULT_OUT_DIR
