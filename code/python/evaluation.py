# -*- coding: utf-8 -*-
"""
In this script, we evalute the performance of rolling light with different 
setup and scenarios.

Created on Tue Jan 24 17:51:35 2017

@author: jason
"""

import common_tool as ct

import skimage.measure as measure
import skimage.color as color
import skimage.io as io
import numpy as np
import pandas as pd
from colormath.color_objects import sRGBColor, IPTColor
import colormath.color_conversions as cc

import matlab.engine



# define constant values
REF = "reference"
COMPARE = "compare"
PSNR = 'psnr'
SSIM = 'ssim'
QSSIM = "qssim"
COLOR_DIFF = "color_diff"
FISM = "fism"
FISMC = "fism_c"
MEAN = "mean"
STD = "std"
REF = "ref"
CMP = 'cmp'

def RGB2IPT(arrRGB):
    """
        convert pixel from sRGB space to IPT space.
    """
    rgb = sRGBColor(arrRGB[0],arrRGB[1],arrRGB[2],  True)
    ipt = cc.convert_color(rgb, IPTColor)
    return np.array(ipt.get_value_tuple() )

def convertRGBImg2IPT(arrImg_rgb):
    """
        convert an image in sRGB space to ipt space.
    """
    arrImg_ipt = np.apply_along_axis(RGB2IPT, 2, arrImg_rgb)
    return arrImg_ipt

def computeSSIMIPT(arrRefImg_ipt, arrCompImg_ipt):
    dSSIM_i = measure.compare_ssim(arrRefImg_ipt[:,:,0], \
                                   arrCompImg_ipt[:,:,0])
    dSSIM_p = measure.compare_ssim(arrRefImg_ipt[:,:,1], \
                                   arrCompImg_ipt[:,:,1])
    dSSIM_t = measure.compare_ssim(arrRefImg_ipt[:,:,2], \
                                   arrCompImg_ipt[:,:,2])
    dSSIM_ipt = dSSIM_i * dSSIM_p * dSSIM_t
    return dSSIM_ipt

def computeColorDiff(arrRefImg_lab, arrCompImg_lab):
    """
        compute color difference btw images according to ciede 2000
    """
    arrColorDiff_pixel = color.deltaE_ciede2000(arrRefImg_lab, arrCompImg_lab)
    return np.average(arrColorDiff_pixel)
    
                   
def performExperiment(eng, strExperimentName, dcCorpus):
    """
        Run a experiment
        
        Parameters
        ----
        eng: matlab engine instance.
        strExperimentName: experiment name.
        strRefImgPath: file path of reference image.
        dcCorpus: a dictionary of experiment data.
        
        
    """
       
    dcResult = {} # result of all sets 
    for strSetName, dcSetData in dcCorpus.iteritems():
        print("Comparing img set: %s..." % strSetName)
        
        strRefImgPath = dcSetData[REF]
        lsCompImg = dcSetData[CMP]
        
        # load ref image
        arrRefImg = io.imread(strRefImgPath)
        arrRefImg_lab = color.rgb2lab(arrRefImg)
        
        dcSetResult = {} # set result
        for i, strCompImgPath in enumerate(lsCompImg):
            try:
                print("-->img: %s." % strCompImgPath)
                arrCompImg = io.imread(strCompImgPath)
                arrCompImg_lab = color.rgb2lab(arrCompImg)
                
                # PSNR
                print("computing PSNR...")
                dPSNR = measure.compare_psnr(arrRefImg, arrCompImg)
                
                # qssim
                print("computing QSSIM...")
                dQSSIM = eng.qssim(strRefImgPath, strCompImgPath)
                
                # Color difference
                print("computing CD...")
                dCD = computeColorDiff(arrRefImg_lab, arrCompImg_lab)
        
                
                # FISMc
                print("computing FISMc...")
                dFISM, dFISMc = eng.FeatureSIM(strRefImgPath, strCompImgPath,\
                                               nargout=2)
            
                # add to result
                dcCompResult = {PSNR: dPSNR,
                                QSSIM: dQSSIM,
                                COLOR_DIFF: dCD,
                                FISMC: dFISMc}
                dcSetResult[strCompImgPath] = dcCompResult
                print dcCompResult
            except Exception as e:
                print(e.strerror)
        # statistics of set result
        dfSetResult = pd.DataFrame.from_dict(dcSetResult, orient='index')
        dfSetResult.to_csv('../../data/evaluation/temp_result/%s_%s.csv' % \
                           (strExperimentName, strSetName) )
        
        srSetMean = dfSetResult.mean()
        srSetStd = dfSetResult.std()
        srSetStd.rename(lambda x: x+"_"+STD, inplace=True)
        dcResult[strSetName] = srSetMean.append(srSetStd)
        
    
    dfResult = pd.DataFrame.from_dict(dcResult, orient='index')
    print("Experiment is finished.")
    return dfResult

def createCorpus(strDataDir, lsSetNames):
    """
        Create a dictionary of experiment data.
        ----
        Return:
        dcData = {'set_name': {'ref': ref_path, 'cmp': cmp list}, }
    """
    dcData = {}
    for strSetName in lsSetNames:
        dcSet = {}
        dcSet[REF] = ct.getFileList(strDataDir+strSetName, "JPG", "ref")[0]
        dcSet[CMP] = ct.getFileList(strDataDir+strSetName, "JPG", "IMG")
        if(dcSet[REF] is None or len(dcSet[CMP])==0):
            raise ValueError("No ref image or empty comparing list.")
            
        dcData[strSetName] = dcSet
    return dcData
    


# evaluateion start
strExperimentName = "device"
strDataDir = "../../data/evaluation/%s/" % strExperimentName
dcCorpus = createCorpus(strDataDir, 
                                    ["honor",])

try:
    eng is not None
except NameError:
    print("Initializing matlab engine...")
    eng = matlab.engine.start_matlab()
                   
dfResult = performExperiment(eng, strExperimentName, dcCorpus)
                             
# plot
print dfResult

# write to file
dfResult.to_csv("%s/%s.csv" % \
                (strDataDir, strExperimentName) )
    
