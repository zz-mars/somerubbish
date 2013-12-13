#!/bin/bash
if [ $# -eq 0 ]
	then
		echo "no arguments!"
		echo "Usage : out [-v] filenames ..." 1>&2
		exit 1
fi
echo "running"
if [ "$1" = "-v" ] ;then
		shift
		less -- "$1"
	else
		cat -- "$1"
fi
if [ -f "$1" ]
	then
		echo "$1 is an ordinary file in current dir"
	else
		echo "$1 is not an ordinary file in current dir"
fi
exit 0
