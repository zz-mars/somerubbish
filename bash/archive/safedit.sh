#!/bin/bash
PATH=/usr/bin : /bin
script=$(basename $0)
echo "script -- $script"
echo "what ?? -- $$"
case $# in
	0)
		vim
		exit 0
		;;
	1)
		if [ ! -f "$1" ] ;then
			vim "$1"
			exit 0
		fi
		if [ ! -r "$1" -o ! -w "$1" ] ;then
			echo "$script :check permission on $1" 1>&2
			exit 1
		else
			editfile=$1
		fi
		if [ ! -w "." ] ;then
			echo "$script : backup file cannot be created!" 1>&2
			exit 1
		fi
		;;
	2)
		echo "Usage : $script [file-to-edit]" 1>&2
		exit 1
		;;
esac
tmpfile=/tmp/$$.$script
cp $editfile $tmpfile
if vim $editfile
	then
		mv $tmpfile bak.$(basename $editfile)
		echo "$script : backup file created!"
	else
		mv $tmpfile editerr
		echo "$script : edit error!copy of original file is in the editerr" 1>&2
fi
exit 0
