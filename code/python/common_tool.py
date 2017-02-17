# -*- coding: utf-8 -*-
"""
Created on Wed Feb 15 20:41:20 2017

@author: jason
"""

import re
import os

def getFileList(strParentPath, strSuffix=None, 
                strFileNamePattern=None):
    """
        get paths of files under a directory
    """
    lsFiles = []
    for (dirpath, dirnames, filenames) in os.walk(strParentPath):
        for fn in sorted(filenames):

            # check prefix
            if strSuffix is not None:
                if fn.split('.')[-1] != strSuffix:
                    continue
                
            # check substring
            if strFileNamePattern is not None:
                if re.search(strFileNamePattern, fn) is None:
                    continue

            lsFiles.append(dirpath+'/'+fn)
    
    return lsFiles