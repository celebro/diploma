#!/bin/bash
LIMIT=2
for f in `find $1 -maxdepth 1 -name *.A* -type f | sort -n -t/ -k6`; do
	pattern=${f}
	target=${f/.A/.B}
	echo -n $pattern
	echo -n ";vf2="
	timeout $LIMIT ./vf_runner/Debug/vf_runner $pattern $target 1
	echo -n ";ull="
	timeout $LIMIT ./vf_runner/Debug/vf_runner $pattern $target 0
	echo -n ";odcull="
	timeout $LIMIT ./odc_ullmann/Debug/odc_ullmann $pattern $target
	echo
done

#rename "s/^[\w]*_[\w]*_[\w]//g" *

