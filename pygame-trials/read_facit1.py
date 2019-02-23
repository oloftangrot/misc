#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

file = '1a_15.txt'

if os.path.exists( os.path.join('data', file ) ):
  input = open( os.path.join('data', file ), 'r' )
  for line in input:
#		print line 	
    print line.rstrip() 	

