# -*- coding: utf-8 -*-
#==============================================================================
# This script examine the effect of multiple lights with different frequencies
# numberically.
#==============================================================================

import numpy as np
import matplotlib.pyplot as plt

lsPlotColor = ['r', 'g', 'b', 'c', 'm', 'y']
arrExposure = np.linspace(1.0/60, 1.0/10, 50)

lsPeriods = [1.0/67, 1.0/83]

plt.figure()
lsQuotients = []
lsRemainders = []
for i, dPeriod in enumerate(lsPeriods):
    # single-Light result
    arrQuotient = np.floor(arrExposure/dPeriod)
    arrRemainder = arrExposure%dPeriod
    lsQuotients.append(arrQuotient)
    lsRemainders.append(arrRemainder)
    arrRatio = arrRemainder/(dPeriod*arrQuotient)
    plt.plot(arrExposure, arrRatio, 
             ls='-', color=lsPlotColor[i],
             label='L%d'%i)
             
# combined result
arrCombinedRemainder = [sum(i) for i in zip(*lsRemainders) ]
arrCombinedWhite = None
for i, dPeriod in enumerate(lsPeriods):
    arrCombinedWhite = arrCombinedWhite + dPeriod * lsQuotients[i] \
        if arrCombinedWhite is not None \
        else dPeriod * lsQuotients[i]
    
arrCombinedRatio = arrCombinedRemainder/arrCombinedWhite
plt.plot(arrExposure, arrCombinedRatio, 
         ls='-', lw='2', color='k',
         label='Combined')
plt.xlabel("Exposure time (second)")
plt.ylabel("non-white color ratio")
plt.legend()
plt.show()



