#!/bin/bash

#tell grid engine to use current directory
#$ -cwd

CMDSTR="$@"

while true; do
	case "$1" in
		-resultsDir)				RESULTSDIR="$2"; shift 2 ;;
		-inputFile)					INPUTFILE="$2"; shift 2 ;;
		-limitsFile)				LIMITSFILE="$2"; shift 2 ;;
		-mFile)						MFILE="$2"; shift 2 ;;
		-abcdDistFile)				ABCDDISTFILE="$2"; shift 2 ;;
		-possibleHotspotsFile)		POSSIBLEHOTSPOTSFILE="$2"; shift 2 ;;
		-nonremovableHotspotsFile)	NONREMOVABLEHOTSPOTSFILE="$2"; shift 2 ;;
		-nonremovableProbFile)		NONREMOVABLEPROBFILE="$2"; shift 2 ;;
		*)						shift; if [ $# == 0 ]; then break; fi
	esac
done

CONSOLE_OUT_FILE=output/console-output.txt

exec > >(tee $CONSOLE_OUT_FILE)
exec 2>&1

echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

echo Running on:
hostname

echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

echo Copying console output to: \"$CONSOLE_OUT_FILE\"

echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

echo Running \"make clean\":
make clean

echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

echo Running \"make\":
make

echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

RUNSTR="./CNmoonmarsReassemble $CMDSTR"
echo Running \"$RUNSTR\":
time $RUNSTR
