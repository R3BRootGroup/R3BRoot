# python import commands:
import numpy as np
import os as TheOS
import sys as TheSys

# import Keras and TensorFlow:
import tensorflow as tf
import keras as ks

# import own functions:
from NetworkDefinition import model
import SetParameters

# --------------------------------------------------------------------------

# Use script input:
if __name__ == "__main__":
    TheLabel = int(TheSys.argv[1])
    TheLabelStr = str(TheLabel)
    
    # NOTE: A label of 0 means that we do standard (multiplicity) No label also means this. 
    # But if we have a number, it means that we do a step2-network of given multiplicity.
    
    # Set the parameters:
    SetParameters.Initialize()
    
    # Modify based on label:
    if (TheLabel>0):
        SetParameters.Step2_SelectedMultiplicity = TheLabel
        SetParameters.NetworkType = SetParameters.Step2_NetworkType
        SetParameters.TextFilePath = SetParameters.TextFilePath.replace('/DNN_DataBase/','/DNN_Step2_Mult'+TheLabelStr+'/')
        SetParameters.nMaxClusters = 2*TheLabel+1
        SetParameters.nEpochs = SetParameters.nEpochs_Step2
        SetParameters.Compute_IO_Structure()

    # Define the model:
    TheModel=model()

    # Define optimizer & compile model:
    TheOptimizer = ks.optimizers.Adagrad(lr=0.01,epsilon=None,decay=0.0)

    if ( (SetParameters.NetworkType=='IO_Signals_ElenaHoemann') or
        (SetParameters.NetworkType=='IO_Signals_Elena_6004_5') or
        (SetParameters.NetworkType=='IO_Signals_Elena_9002_5') or
        (SetParameters.NetworkType=='IO_Signals_Elena_9004_5') or
        (SetParameters.NetworkType=='IO_Signals_Elena_12004_5') ):
        TheOptimizer = ks.optimizers.Adagrad(lr=0.003,epsilon=None,decay=0.0)
        TheModel.compile(optimizer=TheOptimizer,loss='categorical_crossentropy',metrics=['accuracy'])
    
    elif ( (SetParameters.NetworkType=='ScoringPlus') or
           (SetParameters.NetworkType=='ScoringPlus_OneCluster') ):
        TheOptimizer = ks.optimizers.Adagrad(lr=0.001,epsilon=None,decay=0.0)
        #TheOptimizer = ks.optimizers.SGD(lr=0.001, decay=1e-6, momentum=0.9, nesterov=True)
        #TheOptimizer = ks.optimizers.Adam(lr=0.001, beta_1=0.9, beta_2=0.999, epsilon=None, decay=0.0, amsgrad=False)
        TheModel.compile(optimizer=TheOptimizer,loss='categorical_crossentropy',metrics=['accuracy'])

    elif (SetParameters.NetworkType=='IO_Signals_MarkusPolleryd'):
        print('### ==> IO_Signals_MarkusPolleryd is not yet programmed! <== ###')
    
    elif (SetParameters.NetworkType=='IO_Signals_MultNetwork'):
        TheOptimizer = ks.optimizers.Adagrad(lr=0.01,epsilon=None,decay=0.0)
        TheModel.compile(optimizer=TheOptimizer,loss='categorical_crossentropy',metrics=['accuracy'])
    
    elif (SetParameters.NetworkType=='IO_Signals_StandardNetwork'):
        TheOptimizer = ks.optimizers.Adagrad(lr=0.01,epsilon=None,decay=0.0)
        TheModel.compile(optimizer=TheOptimizer,loss='categorical_crossentropy',metrics=['accuracy'])
    
    elif (SetParameters.NetworkType=='IO_3to6_TradMed_BabyNetwork'):
        TheOptimizer = ks.optimizers.Adagrad(lr=0.3,epsilon=None,decay=0.0)
        TheModel.compile(optimizer=TheOptimizer,loss='mean_squared_error',metrics=['accuracy'])
    
    elif (SetParameters.NetworkType=='IO_2to6_TradMed_BabyNetwork'):
        TheOptimizer = ks.optimizers.Adagrad(lr=0.3,epsilon=None,decay=0.0)
        TheModel.compile(optimizer=TheOptimizer,loss='mean_squared_error',metrics=['accuracy'])

    # Next, save the model
    NetworkName = SetParameters.OutputPath
    if (TheLabel>0):
        NetworkName = NetworkName + 'Step2Network_Mult' + TheLabelStr + '_Epoch0.h5'
    else:
        NetworkName = NetworkName + 'TheMultNetwork_Epoch0.h5'
    TheModel.save(NetworkName)
