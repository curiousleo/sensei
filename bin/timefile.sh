#!/bin/bash

TIMEFORMAT=%R

for infile in "$@"
do
	echo -n "${infile}: "
	TIME=$( { time ./solvefile.sh ${infile} > /dev/null; } 2>&1 )
	LENGTH=$(wc -l ${infile} | cut -f1 -d' ')
	FREQ=$(echo "scale=0; ${LENGTH} / ${TIME}" | bc -l)
	echo "${TIME} secs (${FREQ} Hz)"
done
