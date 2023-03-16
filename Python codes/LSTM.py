# -*- coding: utf-8 -*-
"""
Created on Tue Mar  7 15:20:52 2023

@author: hirait
"""


import numpy as np
import torch
import torch.nn as nn
import torch.optim as optimizers
import time
# import datetime
# from callbacks import EarlyStopping
# from sklearn.utils import shuffle
import matplotlib.pyplot as plt
import preprocess
import os
os.environ['KMP_DUPLICATE_LIB_OK']='True'

class BiLSTM(nn.Module):
    def __init__(self, vocab_size, embd_dim, hidden_dim, output_dim):
        super().__init__()
        self.EMBD = nn.Embedding(vocab_size,embd_dim)
        self.embd_dim = embd_dim
        self.RNN = nn.LSTM(embd_dim, hidden_dim,
                          batch_first=True, bidirectional = True)
        self.l1 = nn.Linear(hidden_dim*2, hidden_dim)
        self.relu = nn.ReLU()
        self.l2 = nn.Linear(hidden_dim, output_dim)
        # self.sigmoid = nn.Sigmoid()

        # nn.init.xavier_normal_(self.l1.weight_ih_l0)
        # nn.init.orthogonal_(self.l1.weight_hh_l0)

    def forward(self, x):
        x = self.EMBD(x)
        x = x.view([-1, x.shape[0], self.embd_dim])
        h, _ = self.RNN(x)
        h = self.l1(h)
        h = self.relu(h)
        y = self.l2(h)
        # y = self.sigmoid(y)
        return y


class LSTM(nn.Module):
    def __init__(self, vocab_size, embd_dim, hidden_dim, output_dim):
        super().__init__()
        self.EMBD = nn.Embedding(vocab_size,embd_dim)
        self.embd_dim = embd_dim
        self.RNN = nn.LSTM(embd_dim, hidden_dim,
                          batch_first=True)
        self.l1 = nn.Linear(hidden_dim, output_dim)
        # self.sigmoid = nn.Sigmoid()

        # nn.init.xavier_normal_(self.l1.weight_ih_l0)
        # nn.init.orthogonal_(self.l1.weight_hh_l0)

    def forward(self, x):
        x = self.EMBD(x)
        x = x.view([-1, x.shape[0], self.embd_dim])
        h, _ = self.RNN(x)
        y = self.l1(h)
        # y = self.sigmoid(y)
        return y


def train(train_data, modelType="LSTM", useGPU=False, epochs=100, modelFile=""):
    starttime = time.time()
    # np.random.seed(77)
    # torch.manual_seed(77)
    if useGPU:
        device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    else:
        device = torch.device('cpu')
    print(device)
    
    x_train = train_data[:-1,:]
    _,x_train = np.where(x_train)
    t_train = train_data[1:,:]
    # _,t_train = np.where(t_train)
    
    
    # datalen = train_data.shape[0]
    vocab_size = train_data.shape[1]
    embd_dim = 300
    hidden_dim = 300
    output_dim = vocab_size
    
    batch_size = 100
    print("Epoch: ", epochs)
    
    log = []
    
    if modelType == "BiLSTM":
        print("Train a BiLSTM model.")
        model = BiLSTM(vocab_size,embd_dim,hidden_dim,output_dim).to(device)
    elif modelType == "LSTM":
        print("Train a LSTM model.")
        model = LSTM(vocab_size,embd_dim,hidden_dim,output_dim).to(device)
    # criterion = nn.MSELoss(reduction='mean')
    criterion = nn.CrossEntropyLoss()
    # criterion = nn.BCEWithLogitsLoss()
    # optimizer = optimizers.Adam(model.parameters(),
    #                             lr=0.001,
    #                             betas=(0.9, 0.999), amsgrad=True)
    optimizer = optimizers.Adam(model.parameters(), lr=0.00001)
    # optimizer = optimizers.SGD(model.parameters(),
    #                             lr=1)
    
    print(model)
    
    # parameter count
    params = 0
    for p in model.parameters():
        if p.requires_grad:
            params += p.numel()
            
    print("num params：", params)

    def compute_loss(t, y):
        return criterion(y, t)

    def train_step(x, t):
        x = torch.LongTensor(x).to(device)
        _, t = np.where(t)
        t = torch.LongTensor(t).to(device)
        model.train()
        preds = model(x)
        preds = preds.view(preds.shape[1],preds.shape[2])
        loss = compute_loss(t, preds)
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        return loss, preds


    n_batches_train = x_train.shape[0] // batch_size + 1
    hist = {'loss': []}
    # es = EarlyStopping(patience=10, verbose=1)
    
    

    t = time.time() - starttime
    print("elapsed time: ",t)
    for epoch in range(epochs):
        print("Epoch: ", epoch)
        train_loss = 0.

        for batch in range(n_batches_train-1):
            start = batch * batch_size
            end = start + batch_size
            x = x_train[start:end]
            t = t_train[start:end,:]
            loss, _ = train_step(x,t)            
            train_loss += loss.item()


        train_loss /= n_batches_train

        hist['loss'].append(train_loss)

        print('epoch: {}, loss: {:.3}'.format(
            epoch+1,
            train_loss
        ))
        logStr = "epoch: " + str(epoch+1)
        log.append(logStr)
        logStr = "loss: " + str(train_loss)
        log.append(logStr)
    
        
        t = time.time() - starttime
        print("elapsed time: ",t)
        
        
    print("Finished training!!")

    torch.save(model.state_dict(), modelFile)
    print(modelFile)
    
    logFile = modelFile[:-6] + "_log.txt"
    with open(logFile, 'w') as f:
        for l in log:
            f.write(l+'\n')
    f.close()
    
    # plot a result
    fig = plt.figure()
    plt.plot(np.arange(len(hist['loss'])), hist['loss'], label='train', color="b")
    plt.xlabel('epoch')
    plt.ylabel('loss')
    plt.legend()
    # plt.show()
    figname = modelFile[:-6] + "_loss.png"
    fig.savefig(figname)
    
    # save model information file
    infoFile = modelFile[:-6] + "_info.txt"
    with open(infoFile, 'w') as f:
        f.write('vocab_size:\t'+str(vocab_size)+'\n')
        f.write('embd_dim:\t'+str(embd_dim)+'\n')
        f.write('hidden_dim:\t'+str(hidden_dim)+'\n')
        f.write('output_dim:\t'+str(output_dim)+'\n')
        f.write('modelType:\t'+modelType+'\n')
    f.close()
    
    
    elapsed_time = time.time() - starttime
    print("Finished processing.")
    print("elapsed_time:{0}".format(elapsed_time) + "[sec]")
    print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    
    return




def generate(modelFile, inputData, useGPU=False, genLength=16):
    
    # load parameter settings
    ind = modelFile.rfind('.')
    infoFile = modelFile[:ind] + "_info.txt"    
    with open(infoFile, 'r') as f:
        lines = f.readline()
        line = lines.split('\t')
        vocab_size = int(line[1])
        lines = f.readline()
        line = lines.split('\t')
        embd_dim = int(line[1])
        lines = f.readline()
        line = lines.split('\t')
        hidden_dim = int(line[1])
        lines = f.readline()
        line = lines.split('\t')
        output_dim = int(line[1])
        lines = f.readline()
        line = lines.split('\t')
        modelType = line[1][:-1]
    f.close()
    
    # load model 
    if useGPU:
        device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    else:
        device = torch.device('cpu')
    print(device)
    if modelType == "BiLSTM":
        print("generate with a BiLSTM model.")
        model = BiLSTM(vocab_size,embd_dim,hidden_dim,output_dim).to(device)
    elif modelType == "LSTM":
        print("generate with a LSTM model.")
        model = LSTM(vocab_size,embd_dim,hidden_dim,output_dim).to(device)
    model.load_state_dict(torch.load(modelFile))
    model.eval()
    
    # prepare input data
    _,x = np.where(inputData)
    x = torch.LongTensor(x).to(device)
    
    # prediction note by note
    outputNotes = []
    remainingNotes = genLength
    while(remainingNotes>0):
        preds = model(x)
        preds = preds.view(preds.shape[1],preds.shape[2])
        nextNote = preds[-1,:].argmax().view(-1)
        outputNotes.append(nextNote.item())
        x = torch.cat([x[1:], nextNote])
        noteLength = preprocess.vecInd2noteLength(nextNote.item())
        remainingNotes -= noteLength


    return outputNotes
