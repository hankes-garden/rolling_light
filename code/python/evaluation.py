# -*- coding: utf-8 -*-
"""
In this script, we evalute the performance of rolling light with different setup and scenarios.

Created on Tue Jan 24 17:51:35 2017

@author: jason
"""

import skimage.measure as measure
import skimage.color as color
import skimage.io as io
import numpy as np
import pandas as pd

import matlab.engine



# define constant values
REF = "reference"
COMPARE = "compare"
PSNR = 'psnr'
SSIM = 'ssim'
COLOR_DIFF = "color_diff"
FISM = "fism"
FISMC = "fism_c"

def computeColorDiff(arrRefImg_lab, arrCompImg_lab):
    """
        compute color difference btw images according to ciede 2000
    """
    arrColorDiff_pixel = color.deltaE_ciede2000(arrRefImg_lab, arrCompImg_lab)
    return np.average(arrColorDiff_pixel)
    
                   
def performExperiment(eng, strExperimentName, strRefImgPath,
                      dcCompareImages):
    """
        Run a experiment
        
        Parameters
        ----
        eng: matlab engine instance.
        strExperimentName: experiment name.
        strRefImgPath: file path of reference image.
        dcCompareImages: a dictionary of compare images.
        
        
    """
    # load image
    arrRefImg = io.imread(strRefImgPath)
    arrRefImg_bw = color.rgb2gray(arrRefImg)
    arrRefImg_lab = color.rgb2lab(arrRefImg)
    
    dcResult = {} # result of all sets 
    for strSetName, lsCompImg in dcCompareImages.iteritems():
        print("Comparing img set: %s..." % strSetName)
        dcSetResult = {} # set result
        for i, strCompImgPath in enumerate(lsCompImg):
            print("-->img: %s." % strCompImgPath)
            arrCompImg = io.imread(strCompImgPath)
            arrCompImg_bw = color.rgb2gray(arrCompImg)
            arrCompImg_lab = color.rgb2lab(arrCompImg)
            
            # PSNR
            dPSNR = measure.compare_psnr(arrRefImg, arrCompImg)
            
            # ssim
            dSSIM = measure.compare_ssim(arrRefImg_bw, arrCompImg_bw)
            
            
            # Color difference
            dCD = computeColorDiff(arrRefImg_lab, arrCompImg_lab)
    
            # Correlation of color histogram
            
            # FISMc
            dFISM, dFISMc = eng.FeatureSIM(strRefImgPath, strCompImgPath,\
                                           nargout=2)
        
            # add to result
            dcCompResult = {PSNR: dPSNR, 
                            SSIM: dSSIM,
                            COLOR_DIFF: dCD,
                            FISM: dFISM,
                            FISMC: dFISMc}
            dcSetResult[i] = dcCompResult
            print dcCompResult
        # statistics of set result
        dfSetResult = pd.DataFrame.from_dict(dcSetResult, orient='index')
        dfSetResult.to_csv('../../data/result/%s_%s.csv' % \
                           (strExperimentName, strSetName) )
        
        srSetMean = dfSetResult.mean()
        dcResult[strSetName] = srSetMean
        
    
    dfResult = pd.DataFrame.from_dict(dcResult, orient='index')
    print("Experiment is finished.")
    return dfResult


# evaluateion start
strExperimentName = "illuminance"
strRefImgPath = "../../data/sample_data_for_IQM/IMG_3555.JPG"
dcCompareImages = {\
    'test1': ["../../data/sample_data_for_IQM/IMG_3557.JPG",
              "../../data/sample_data_for_IQM/IMG_3560.JPG",],
    'test2': ["../../data/sample_data_for_IQM/IMG_3570.JPG",
              "../../data/sample_data_for_IQM/IMG_3574.JPG",]\
              }
eng = matlab.engine.start_matlab()
                   
dfResult = performExperiment(eng, strExperimentName, \
                             strRefImgPath, dcCompareImages)
                             
# plot
print dfResult

# write to file
dfResult.to_csv("../../data/result/%s.csv" % strExperimentName)
    
