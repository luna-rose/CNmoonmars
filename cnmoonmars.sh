#!/bin/bash

#tell grid engine to use current directory
#$ -cwd

CMDSTR="$@"

while true; do
	case "$1" in
		-startIndex)				STARTINDEX="$2"; shift 2 ;;
		-endIndex)					ENDINDEX="$2"; shift 2 ;;
		-gridRes)					GRIDRES="$2"; shift 2 ;;
		-increment)					INCREMENT="$2"; shift 2 ;;
		-interval)					INTERVAL="$2"; shift 2 ;;
		-dataDir)					DATADIR="$2"; shift 2 ;;
		-statusDir)					STATUSDIR="$2"; shift 2 ;;
		-outputDir)					OUTPUTDIR="$2"; shift 2 ;;
		-inputFile)					INPUTFILE="$2"; shift 2 ;;
		-mFile)						MFILE="$2"; shift 2 ;;
		-limitsFile)				LIMITSFILE="$2"; shift 2 ;;
		-abcdDistFile)				ABCDDISTFILE="$2"; shift 2 ;;
		-possibleHotspotsFile)		POSSIBLEHOTSPOTSFILE="$2"; shift 2 ;;
		-nonremovableHotspotsFile)	NONREMOVABLEHOTSPOTSFILE="$2"; shift 2 ;;
		-nonremovableProbFile)		NONREMOVABLEPROBFILE="$2"; shift 2 ;;
		*)							shift; if [ $# == 0 ]; then break; fi
	esac
done

CONSOLE_OUT_FILE=output/console-output.txt
if [ ! -d "output" ]; then
	mkdir "output/"
fi

if [[ -n "$STARTINDEX" ]] && [[ -n "$ENDINDEX" ]]; then 
	MIDDIR=`printf %04d-%04d $STARTINDEX $ENDINDEX`
	CONSOLE_OUT_FILE="output/$MIDDIR/console-output.txt"
	if [ ! -d "output/$MIDDIR" ]; then
		mkdir "output/$MIDDIR"
	fi
fi


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

RUNSTR="./CNmoonmars $CMDSTR"
echo Running \"$RUNSTR\":
time $RUNSTR
