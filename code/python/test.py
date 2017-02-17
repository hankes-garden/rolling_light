# -*- coding: utf-8 -*-
"""
Created on Tue Jan 24 16:33:59 2017

@author: jason
"""
import skimage.io
from colormath.color_objects import sRGBColor, IPTColor
import colormath.color_conversions as cc
import numpy as np


def RGB2IPT(arrRGB):
    rgb = sRGBColor(arrRGB[0],arrRGB[1],arrRGB[2],  True)
    ipt = cc.convert_color(rgb, IPTColor)
    return np.array(ipt.get_value_tuple() )

def convertRGBImg2IPT(arrImg_rgb):
    arrImg_ipt = np.apply_along_axis(RGB2IPT, 2, arrImg_rgb)
    return arrImg_ipt


arrRef = skimage.io.imread("../../data/sample_data_for_IQM/IMG_3555.JPG")
arrRef_ipt = convertRGBImg2IPT(arrRef)
