#!/bin/bash

TIMEFORMAT=%R

for infile in "$@"
do
	echo -n "${infile}: "

	exec 3>&1 4>&2
   TIME=$( { time ./solvefile.sh ${infile} 1>/dev/null 2>/dev/null; } 2>&1 )  # Captures time only.
   exec 3>&- 4>&-

	LENGTH=$(wc -l ${infile} | cut -f1 -d' ')
	FREQ=$(echo "scale=0; ${LENGTH} / ${TIME}" | bc -l)
	echo "${TIME} secs (${FREQ} Hz)"
done
