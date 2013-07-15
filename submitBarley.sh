#!/bin/bash

DIRECTORY=CNmoonmars100
USERNAME=chintanh
CORNSERVER=corn01.stanford.edu
NUM_HOTSPOTS=1333
OVERLAP=1

read -s -p 'Enter Password:' PASSWORD
echo 

NUM_BARLEYS=$#
echo Number of barleys requested: $NUM_BARLEYS

PREVINDEX=1
COUNT=1
for var in "$@"
do
	SERVER=`printf barley%02d.stanford.edu $var`
	let "CURRINDEX = COUNT*(NUM_HOTSPOTS-1)/NUM_BARLEYS+1"
	let "JOBSIZE = $CURRINDEX-$PREVINDEX+1"
	
    echo `printf "Submitting %04d-%04d, with job size %d to %s ..." $PREVINDEX $CURRINDEX $JOBSIZE $SERVER`
	
	./submitBarleySingle.sh $USERNAME $CORNSERVER $SERVER $PASSWORD $PREVINDEX $CURRINDEX $DIRECTORY
	
	let "PREVINDEX = $CURRINDEX + 1 - $OVERLAP"
	let "COUNT = COUNT + 1"
done