# python import commands:
import numpy as np

# import Keras and TensorFlow:
import tensorflow as tf
import keras as ks

# import own functions:
import SetParameters

# Define the Network:
def model():
    
    # Go through all options:
    if ( (SetParameters.NetworkType=='IO_Signals_ElenaHoemann') or
         (SetParameters.NetworkType=='IO_Signals_Elena_6004_5') or
         (SetParameters.NetworkType=='IO_Signals_Elena_9002_5') or
         (SetParameters.NetworkType=='IO_Signals_Elena_9004_5') or
         (SetParameters.NetworkType=='IO_Signals_Elena_12004_5') ):
    
        # Define input layer:
        InputLayer = ks.Input((SetParameters.nInputNeurons,))
    
        # Define hidden layer:
        MiddleLayer1 = ks.layers.Dense(9000,activation='relu',use_bias=True)(InputLayer)
        MiddleLayer2 = ks.layers.Dense(1200,activation='relu',use_bias=True)(MiddleLayer1)
    
        # Define output layer:
        OutputLayer = ks.layers.Dense(SetParameters.nOutputNeurons,activation='softmax',use_bias=True)(MiddleLayer2)
    
        # Now generate network architecture:
        TheModel = ks.models.Model(inputs=InputLayer,outputs=OutputLayer,name=SetParameters.NetworkType)
    
        # Return network as function output:
        return(TheModel)
    
    elif (SetParameters.NetworkType=='ScoringPlus'):
    
        # Define input layer:
        InputLayer = ks.Input((SetParameters.nInputNeurons,))
    
        # Define hidden layer:
        MiddleLayer1 = ks.layers.Dense(200,activation='relu',use_bias=True)(InputLayer)
        MiddleLayer2 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer1)
        MiddleLayer3 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer2)
    
        # Define output layer:
        OutputLayer = ks.layers.Dense(SetParameters.nOutputNeurons,activation='softmax',use_bias=True)(MiddleLayer3)
    
        # Now generate network architecture:
        TheModel = ks.models.Model(inputs=InputLayer,outputs=OutputLayer,name='ScoringPlus')
    
        # Return network as function output:
        return(TheModel)
    
    elif (SetParameters.NetworkType=='ScoringPlus_OneCluster'):
    
        # Define input layer:
        InputLayer = ks.Input((SetParameters.nInputNeurons,))
    
        # Define hidden layer:
        MiddleLayer1 = ks.layers.Dense(200,activation='relu',use_bias=True)(InputLayer)
        MiddleLayer2 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer1)
        MiddleLayer3 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer2)
        MiddleLayer4 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer3)
        MiddleLayer5 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer4)
        MiddleLayer6 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer5)
        MiddleLayer7 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer6)
        MiddleLayer8 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer7)
        MiddleLayer9 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer8)
        MiddleLayer10 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer9)
        MiddleLayer11 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer10)
        MiddleLayer12 = ks.layers.Dense(200,activation='relu',use_bias=True)(MiddleLayer11)
    
        # Define output layer:
        OutputLayer = ks.layers.Dense(SetParameters.nOutputNeurons,activation='softmax',use_bias=True)(MiddleLayer12)
    
        # Now generate network architecture:
        TheModel = ks.models.Model(inputs=InputLayer,outputs=OutputLayer,name='ScoringPlus_OneCluster')
    
        # Return network as function output:
        return(TheModel)
    
    elif (SetParameters.NetworkType=='IO_Signals_MarkusPolleryd'):
        
        # No idea yet...
        print('### ==> IO_Signals_MarkusPolleryd is not yet programmed! <== ###')
        TheModel = ks.models.Sequential()
        return TheModel
        
    elif (SetParameters.NetworkType=='IO_Signals_MultNetwork'):
        
        # Define input layer:
        InputLayer = ks.Input((SetParameters.nInputNeurons,))
    
        # Define hidden layers:
        MiddleLayer1 = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(InputLayer)
        MiddleLayer2 = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(MiddleLayer1)
        MiddleLayer3 = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(MiddleLayer2)
        MiddleLayer4 = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(MiddleLayer3)
        MiddleLayer5 = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(MiddleLayer4)
    
        # Define output layer:
        OutputLayer = ks.layers.Dense(SetParameters.nOutputNeurons,activation='sigmoid',use_bias=True)(MiddleLayer5)
    
        # Now generate network architecture:
        TheModel = ks.models.Model(inputs=InputLayer,outputs=OutputLayer,name='IO_Signals_MultNetwork')
    
        # Return network as function output:
        return(TheModel)
    
    elif (SetParameters.NetworkType=='IO_Signals_StandardNetwork'):
        
        # Define input layer:
        InputLayer = ks.Input((SetParameters.nInputNeurons,))
    
        # Define hidden layers:
        MiddleLayer1 = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(InputLayer)
        MiddleLayer2 = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(MiddleLayer1)
        MiddleLayer3 = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(MiddleLayer2)
        MiddleLayer4 = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(MiddleLayer3)
        MiddleLayer5 = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(MiddleLayer4)
    
        # Define output layer:
        OutputLayer = ks.layers.Dense(SetParameters.nOutputNeurons,activation='sigmoid',use_bias=True)(MiddleLayer5)
    
        # Now generate network architecture:
        TheModel = ks.models.Model(inputs=InputLayer,outputs=OutputLayer,name='IO_Signals_StandardNetwork')
    
        # Return network as function output:
        return(TheModel)
        
    elif (SetParameters.NetworkType=='IO_2to6_TradMed_BabyNetwork'):
        
        # Define input layer:
        InputLayer = ks.Input((SetParameters.nInputNeurons,))
    
        # Define hidden layer:
        MiddleLayer = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(InputLayer)
    
        # Define output layer:
        OutputLayer = ks.layers.Dense(SetParameters.nOutputNeurons,activation='sigmoid',use_bias=True)(MiddleLayer)
    
        # Now generate network architecture:
        TheModel = ks.models.Model(inputs=InputLayer,outputs=OutputLayer,name='IO_2to6_TradMed_BabyNetwork')
        
        # Return network as function output:
        return(TheModel)
        
    elif (SetParameters.NetworkType=='IO_3to6_TradMed_BabyNetwork'):
        
        # Define input layer:
        InputLayer = ks.Input((SetParameters.nInputNeurons,))
    
        # Define hidden layer:
        MiddleLayer = ks.layers.Dense(SetParameters.nMaxNeurons,activation='sigmoid',use_bias=True)(InputLayer)
    
        # Define output layer:
        OutputLayer = ks.layers.Dense(SetParameters.nOutputNeurons,activation='sigmoid',use_bias=True)(MiddleLayer)
    
        # Now generate network architecture:
        TheModel = ks.models.Model(inputs=InputLayer,outputs=OutputLayer,name='IO_3to6_TradMed_BabyNetwork')
    
        # Return network as function output:
        return(TheModel)
    
    else:
        
        # Put in some default:
        print('### ==> Default model! <== ###')
        TheModel = ks.models.Sequential()
        return TheModel
        
# Done --------------------------------------------------------
