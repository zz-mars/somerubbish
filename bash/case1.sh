#!/bin/bash
echo -n "enter A B C D : "
read letter
case "$letter" in
	A|a)
		echo "input A"
		;;
	B|b)
		echo "input B"
		;;
	C|c)
		echo "input C"
		;;
	D|d)
		echo "input D"
		;;
	[A-Z])
		echo "input capital letter"
		;;
	*)
		echo "no A | B | C | D is input!"
		;;
esac
exit 0
