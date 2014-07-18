#!/bin/sh
#$ -cwd
#$ -N traceSim
#$ -o output
#$ -e output
#./main
#mkdir /state/partition1/milad/traceResults
echo Args: $@
echo Arg 1: $1
echo Arg 2: $3

PARS_ROOT='/home/milad/esc_project/svn/PARS/src'
PIN_ROOT='/home/milad/esc_project/svn/pin-2.12'
$PIN_ROOT/pin -separate_memory -t $PARS_ROOT/obj-intel64/main_pars.so --  $@