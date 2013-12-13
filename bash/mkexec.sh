#!/bin/bash
if [ ! $# -eq 1 ] ;then
	echo "Usage : mkexec [file]"
	exit 1
fi
file="$1"
chmod +x "$file"
exit 0
