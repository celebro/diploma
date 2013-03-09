#!/bin/bash
LIMIT=60

echo
echo -n "Running tests for $1"
date

for f in `find $1 -maxdepth 1 -name *.A* -type f | sort -n -t/ -k6`; do
	pattern=${f}
	target=${f/.A/.B}
	echo -n $pattern
	echo -n ";odcull="
	timeout $LIMIT ./odc_ullmann/Release/odc_ullmann $pattern $target
	echo -n ";vf2_sub1="
	timeout $LIMIT ./vf_runner/Release/vf_runner $pattern $target 2
	#echo -n ";vf2_sub2="
	#timeout $LIMIT ./vf_runner/Release/vf_runner $pattern $target 4
	echo -n ";vf2="
	timeout $LIMIT ./vf_runner/Release/vf_runner $pattern $target 1
	#echo -n ";ull="
	#timeout $LIMIT ./vf_runner/Debug/vf_runner $pattern $target 0
	echo

done

#rename "s/^[\w]*_[\w]*_[\w]//g" *

