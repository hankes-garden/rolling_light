# -*- coding: utf-8 -*-
"""
Created on Tue Jan 24 16:33:59 2017

@author: jason
"""

import cv2
flags = [i for i in dir(cv2) if i.startswith('COLOR_')]
ipt = [i for i in flags if 'BGR' in i ]
print ipt