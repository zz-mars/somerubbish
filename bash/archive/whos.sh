#!/bin/bash
set -x
if [ $# -eq 0 ] ;then
	echo "Usage whos.sh user_name .." 1>&2
	exit 1
fi
for id
do
	awk -F : '{print $1,$5}' /etc/passwd | grep "$id"
done
exit 0
