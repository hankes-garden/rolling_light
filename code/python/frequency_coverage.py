# -*- coding: utf-8 -*-
"""
This script computes the ratio of non-white energy over white energy under 
different light numbers.

@author: jason
"""
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import math

lsPlotColor = ['r', 'g', 'b', 'c', 'm', 'y']

nMaxShutterRate = 100
nMinShutterRate = 10
arrExposure = np.linspace(1.0/nMaxShutterRate, 1.0/nMinShutterRate, 1000)
dcLightFrequencies = {'L0':67, 'L1':73, 'L2':79, 'L3':83}

#==============================================================================
# single light
#==============================================================================
dcResult = {}
for strID, nFrequency in dcLightFrequencies.iteritems():
    dPeriod = 1.0/nFrequency
    lsRatios = []
    dcResult["%s=1/%d" % (strID, nFrequency) ] = lsRatios
    dColorDuration = dPeriod/3.0 # duration of each color
    for dExposure in arrExposure:
        dQuotient = math.floor(dExposure/dPeriod)
        dRemainder = dExposure%dPeriod
        
        # check whether some parts of remainder can be integrated to white
        dColor1 = min(dRemainder, dColorDuration)
        dColor2 = min(dColorDuration, dRemainder-dColorDuration) \
            if dRemainder>dColorDuration else 0.0
        dColor3 = max(0.0, dRemainder-dColorDuration*2)
        dRemainderWhite = min(dColor1, dColor2, dColor3)
        
        # compute the non-white ratio
        dNonWhiteRemainder = max(dColor1-dRemainderWhite, 0.0) + \
                             max(dColor2-dRemainderWhite, 0.0) + \
                             max(dColor3-dRemainderWhite, 0.0)
        dRatio = min(dNonWhiteRemainder/(dQuotient*dPeriod+dRemainderWhite*3),
                     1.0)
        lsRatios.append(dRatio)

#plot
ax = plt.figure().add_subplot(111)
for i, (strID, lsRatios) in enumerate(sorted(dcResult.iteritems() ) ):
    ax.plot(arrExposure, lsRatios, 
            color=lsPlotColor[i], label=strID)
ax.axvline(1/30.0, ls='--', color='k')
ax.axvline(1/29.0, ls='--', color='k')
    
#==============================================================================
# multiple lights
#==============================================================================
lsMLResult = []
for dExposure in arrExposure:
    lsColor1 = []
    lsColor2 = []
    lsColor3 = []
    lsWhite = []
    for strID, nFrequency in dcLightFrequencies.iteritems():
        dPeriod = 1.0/nFrequency
        dColorDuration = dPeriod/3.0
        dQuotient = math.floor(dExposure/dPeriod)
        dRemainder = dExposure%dPeriod
        
        # check whether some parts of remainder can be integrated to white
        dColor1 = min(dRemainder, dColorDuration)
        dColor2 = min(dColorDuration, dRemainder-dColorDuration) \
            if dRemainder>dColorDuration else 0.0
        dColor3 = max(0.0, dRemainder-dColorDuration*2)
        dRemainderWhite = min(dColor1, dColor2, dColor3)
        
        # save to list
        lsColor1.append(dColor1-dRemainderWhite)
        lsColor2.append(dColor2-dRemainderWhite)
        lsColor3.append(dColor3-dRemainderWhite)
        lsWhite.append(dQuotient*dPeriod+dRemainderWhite*3)
    
    # check whether some remainders of lights can be integrated to white
    dMLColor1 = sum(lsColor1)
    dMLColor2 = sum(lsColor2)
    dMLColor3 = sum(lsColor3)
    dMLRemainderWhite = min(dMLColor1, dMLColor2, dMLColor3)

    
    # compute non-white ratio
    dNonWhiteRemainder = max(dMLColor1-dMLRemainderWhite, 0.0) + \
                         max(dMLColor2-dMLRemainderWhite, 0.0) + \
                         max(dMLColor3-dMLRemainderWhite, 0.0)
    dWhite = sum(lsWhite) + dMLRemainderWhite*3
    dRatio = min(dNonWhiteRemainder/dWhite, 1.0)
    lsMLResult.append(dRatio)
    
#plot
ax.plot(arrExposure, lsMLResult, lw=2, color='k', label='Multi-light')

plt.xticks([1.0/i for i in range(nMaxShutterRate, nMinShutterRate-1, -10) ], 
   [r'$1/%d$' % i for i in range(nMaxShutterRate, nMinShutterRate-1, -10) ],
   rotation=90)
ax.legend()
plt.tight_layout()
plt.grid('on')
plt.show()

# save to file
dcResult['Multi-light'] = lsMLResult
dcResult['Exposure Time'] = arrExposure
dfResult = pd.DataFrame.from_dict(dcResult)
dfResult.set_index("Exposure Time", inplace=True)

dfResult.to_csv("../../data/evaluation/frequency_coverage/"
                "frequency_coverage.csv")