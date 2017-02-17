# -*- coding: utf-8 -*-
"""
Created on Sun Dec 11 21:44:35 2016

@author: jason
"""
import numpy as np
import matplotlib.pyplot as plt
import math

lsPlotColor = ['r', 'g', 'b', 'c', 'm', 'y']

nMaxShutterRate = 60
nMinShutterRate = 10
arrExposure = np.linspace(1.0/nMaxShutterRate, 1.0/nMinShutterRate, 1000)
dcLightPeriods = {'L0':1.0/73, 'L1':1.0/79,}

#==============================================================================
# single light
#==============================================================================
dcResult = {}
for lid, dPeriod in dcLightPeriods.iteritems():
    lsRatios = []
    dcResult[lid] = lsRatios
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
        dRatio = dNonWhiteRemainder/(dQuotient*dPeriod+dRemainderWhite*3)
        lsRatios.append(dRatio)
        
#plot
ax = plt.figure().add_subplot(111)
for i, (lid, lsRatios) in enumerate(dcResult.iteritems()):
    ax.plot(arrExposure, lsRatios, color=lsPlotColor[i], label=lid)

ax.axvline(x=1.0/20, ls='--', color='k')
ax.axvline(x=1.0/33, ls='--', color='k')
ax.axvline(x=1.0/44, ls='--', color='k')




#==============================================================================
# multiple lights
#==============================================================================
lsMLResult = []
for dExposure in arrExposure:
    lsColor1 = []
    lsColor2 = []
    lsColor3 = []
    lsWhite = []
    for lid, dPeriod in dcLightPeriods.iteritems():
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
    dRatio = dNonWhiteRemainder/dWhite
    lsMLResult.append(dRatio)
    
#plot
ax.plot(arrExposure, lsMLResult, lw=2, color='k', label='Multi-light')

plt.xticks([1.0/i for i in range(nMaxShutterRate, nMinShutterRate-1, -10) ], 
           [r'$1/%d$' % i for i in range(nMaxShutterRate, nMinShutterRate-1, -10) ],
            rotation=90)
ax.set_xlabel('Exposure Time (seconds)')
ax.set_ylabel('Non-white Ratio')
ax.legend()
plt.tight_layout()
plt.grid('on')
plt.show()    