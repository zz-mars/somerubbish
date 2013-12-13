#!/bin/bash
rmsh=rm_exec.sh
for file in *
do
	if [ -x "$file" -a ! -d "$file" -a "$file" != "$rmsh" ] ;then
		rm "$file"
	fi
done
exit 0
