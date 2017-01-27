# -*- coding: utf-8 -*-
"""
Created on Mon Dec 12 01:35:00 2016

@author: jason
"""

import skimage
import skimage.io
import skimage.measure
import matplotlib.pyplot as plt
import numpy as np
import cv2

# load image
arrOriginal = cv2.imread("../../data/sample_data_for_IQM/original.JPG")
arrTest = cv2.imread("../../data/sample_data_for_IQM/wide_b.JPG")

# convert to grayscale
arrOriginalBW = cv2.cvtColor(arrOriginal, cv2.COLOR_BGR2GRAY)
arrTestBW = cv2.cvtColor(arrTest, cv2.COLOR_BGR2GRAY)

# compare the images
dSSIM = skimage.measure.compare_ssim(arrOriginalBW, arrTestBW)
dPSNR = skimage.measure.compare_psnr(arrOriginal, arrTest)
print("ssim=%f, psnr=%s." % (dSSIM, dPSNR) )

# compute SSIM on each channel
for i in xrange(3):
    arrOriginalChannel = arrOriginal[:,:,i]
    arrTestChannel = arrTest[:,:,i]
    dSSIM_channel = skimage.measure.compare_ssim(arrOriginalChannel,\
                                                 arrTestChannel)
    print("channel_%d: %f." % (i, dSSIM_channel) )


