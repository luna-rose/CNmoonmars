#!/bin/bash

#tell grid engine to use current directory
#$ -cwd

exec > >(tee output/console-output.txt)
exec 2>&1

echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

echo Running on:
hostname

echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

echo Running \"make clean\":
make clean

echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

echo Running \"make\":
make

echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

RUNSTR="./CNmoonmars $@"
echo Running \"$RUNSTR\":
time $RUNSTR
