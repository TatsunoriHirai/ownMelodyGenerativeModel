# -*- coding: utf-8 -*-
"""
Created on Tue Mar  7 15:38:11 2023

@author: hirait
"""

import os
import numpy as np

lowestNoteNum = 60
vecDim = 578
noteVariation = 36

def prepareOneHotVec(folderPath, isFile = False):
    """
    - one-hot vector: 578 dim
     C4 (60) is the lowest note
     A#6 (94) is the highest note
     SoM (0) + ( 35 notes + rest ) * 16 rythme patterns (1 - 576) + EoM (577) -> total 578 dim
     note length pattern: 16th note, 8th note, dotted 8th, quarter ~ whole: 16 patterns
    """
    
    if isFile:
        files = [folderPath]
    else:
        files = os.listdir(folderPath)
    
    tmpData = []
    for file in files:
        curPos = 0
        # start of melody
        vec = np.zeros([1, vecDim])
        vec[0,0] = 1 # SoM: start of melody
        tmpData.append(vec)
        # extract melody data
        if isFile:
            filePath = file
        else:
            filePath = folderPath + file 
        with open(filePath) as f:
            for lines in f:
                # print(lines)
                line = lines.split('\t')
                midiNN = int(line[0])
                startTime = int(line[1])
                endTime = int(line[2])
                while(startTime-curPos>=16): # rest (whole 1 measure)
                    vec = np.zeros([1, vecDim])
                    vec[0,vecDim-2] = 1
                    tmpData.append(vec)
                    curPos += 16
                while(curPos<startTime): # rest
                    vec = np.zeros([1, vecDim])
                    restlength = startTime - curPos
                    curPos += restlength
                    vec[0,restlength*noteVariation] = 1
                    tmpData.append(vec)
                while(curPos<endTime): # note
                    vec = np.zeros([1, vecDim])
                    notelength = endTime - startTime
                    vec[0,(notelength-1)*noteVariation + midiNN-lowestNoteNum+1] = 1
                    tmpData.append(vec)
                    curPos += notelength
        # end of melody
        vec = np.zeros([1, vecDim])
        vec[0,vecDim-1] = 1 # EoM: end of melody
        tmpData.append(vec)
        
        
    data = np.array(tmpData).transpose([0,2,1]).squeeze(2)
    
    
    return data

def vecInd2noteLength(vecInd):
    if (vecInd==0)or(vecInd==vecDim-1):
        return 0
    return (vecInd - 1) // noteVariation + 1


def vecInds2text(vecInds):
    textResult = []
    curPos = 0
    for i in range(len(vecInds)):
        noteLen = (vecInds[i] - 1) // noteVariation + 1
        if (vecInds[i] - 1) % noteVariation < noteVariation - 1: # not rest
            midiNN = lowestNoteNum + (vecInds[i] - 1) % noteVariation
            textResult.append(str(midiNN) + "\t" + str(curPos) + "\t" + str(curPos+noteLen))
        curPos += noteLen            
            

    return textResult


