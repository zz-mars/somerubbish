#!/bin/bash
function mycp()
{
	case $# in
		0)
			#no argument,cp from stdin to stdout
			exec 3<&0 4>&1
			;;
		1)
			#one argument,cp from file to stdout
			exec 3<$1 4>&1
			;;
		2)
			#TWO arguments,cp from src to dst
			exec 3<$1 4>$2
			;;
		*)
			echo "Usage : mycp src dst"
			return 1;
			;;
	esac
	cat <&3 >&4
	exec 3<&- 4>&-
}
mycp();
exit 0
