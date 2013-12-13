#!/bin/bash
set -x
for zarg
do
	echo "$zarg "
done
for arg in "$@"
do
	echo "$arg"
done
exit 0
