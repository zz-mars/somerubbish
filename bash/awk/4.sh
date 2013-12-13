#!/bin/bash
(date;cat $1) | 
gawk '
NR == 1 {print "report for : " $0}
NR > 1 {print $1}
'
exit 0
