#!/bin/bash
set +x
if [ $# -eq 0 -o $# -gt 2 ] ;then
	echo "Usage : link lks file [directory]" 1>&2
	exit 1
fi
if [ -d "$1" ] ;then
	echo "file cannot be a directory!"
	exit 1
fi
file="$1"
if [ $# -eq 1 ] ;then
		dir="."
	elif [ -d $2 ] ;then
		dir="$2"
	else
		echo "optional second argument must be a directory" 1>&2
		echo "Usage : link lks file [directory]" 1>&2
		exit 1
fi
if [ ! -f "$file" ] ;then
	echo "not a regular file"
	exit 1
fi
set -- $(ls -l "$file")
lkcount=$2
if [ "$lkcount" -eq 1 ] ;then
	echo "only one reference to this inode"
	exit 1
fi
set -- $(ls -i "$file")
inode=$1
echo "using find to search for files hard-linked to the given file"
find "$dir" -xdev -inum "$inode" -print
exit 0
	
