#!/bin/bash
trap '' 1 2 3 18
stty -echo
echo -n "KEY : "
read k1
echo
echo -n "Again : "
read k2
echo
k3=
if [ "$k1" = "$k2" ] ;then
	tput clear
	until [ "$k3" = "$k2" ]
	do
		read k3
	done
	else
		echo "locktty : keys do not match!" 1>&2
fi
stty echo
exit 0
