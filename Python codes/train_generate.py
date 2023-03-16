# -*- coding: utf-8 -*-
"""
Created on Tue Mar  7 13:45:49 2023

@author: hirait
"""

import preprocess
import LSTM
import sys

args = sys.argv
# first argument: "train" or "generate"
# second argument: cpu or gpu
# third argument: training folder path (train mode) / input melody path (gen mode)
# fourth argument: model file path (output model file (train) or using model file (gen) )
# fifth argument: # of epoch (train mode) / # of output note length (gen mode)
# sixth argument (only for gen mode): output melody file path

# arg example: 
# args = ["train_generate.exe","train", "GPU","C:/Users/hirait/Desktop/melodyData/", 
#  	"C:/Users/hirait/Desktop/output.model", "LSTM", "100"]
# args = ["train_generate.exe","generate", "GPU","C:/Users/hirait/Desktop/inputmelody_0.txt", 
#         "C:/Users/hirait/Desktop/output.model", "16","C:/Users/hirait/Desktop/outputmelody.txt"]


if len(args)<7:
    print("The number of arguments are not enough.")


trainOrGenerate = args[1]
cpuOrGpu = args[2]

if (trainOrGenerate=="train"):
    print("train mode")
    useGPU = False
    if(cpuOrGpu=="GPU"):
        useGPU = True
    folderPath = args[3]
    print("training foleder path: ",folderPath)
    modelFile = args[4]
    print("output model file path: ",modelFile)
    modelType = args[5]
    print("modelType: ",modelType)
    epochs = int(args[6])
    print("epoch: ",epochs)
    
    # data preparation
    train_data = preprocess.prepareOneHotVec(folderPath)

    # training
    LSTM.train(train_data, modelType, useGPU, epochs, modelFile)
    
    print("Training has been completed.")
    
elif(trainOrGenerate == "generate"):
    print("generate mode")
    useGPU = False
    if(cpuOrGpu=="GPU"):
        useGPU = True
    inputMelodyFile = args[3]
    print("input melody path: ",inputMelodyFile)
    modelFile = args[4]
    print("output model file path: ",modelFile)
    genLength = int(args[5])
    print("genLength: ",genLength)
    outputFile = args[6]
    print("output melody file path: ",outputFile)
    
    # generation
    inputData = preprocess.prepareOneHotVec(inputMelodyFile, True)
    inputData = inputData[:-1,:] # delete EoM data
    result = LSTM.generate(modelFile, inputData, useGPU, genLength)
    print("Generated result ", result)
    
    # save result as text file
    textResult = preprocess.vecInds2text(result)
    with open(outputFile, 'w') as f:
        for i in range(len(textResult)):
            f.write(textResult[i]+"\n")
        f.close()
    
    print("Generation has been completed.")
    
    



