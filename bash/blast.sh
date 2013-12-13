#!/bin/bash
function count_down
{
	typeset count
	count=$1
	while [ $count -gt 0 ]
	do
		echo "$count ..."
		let "count-=1"
		sleep 1
	done
	echo "blast off"
	return $?
}
