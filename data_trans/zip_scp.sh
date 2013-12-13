#!/bin/bash
s243="root@192.168.0.243"
gzip -c /media/F/Planet\ Earth/Planet.Earth.EP01.2006.BDRip.a720.2Audio.X264.MiniHD-TLF.mkv | ssh $s243\
		 "gunzip -c - > /var/www/html/planeteartn01.mkv"
