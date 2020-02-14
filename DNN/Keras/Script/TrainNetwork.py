# python import commands:
import numpy as np
import os as TheOS
import sys as TheSys

# import Keras and TensorFlow:
import tensorflow as tf
import keras as ks

# import own functions:
from NetworkDefinition import model
from ReadTheArray import ReadArray
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
        
        if (SetParameters.Step2_NetworkType=='ScoringPlus_OneCluster'):
            # Then, we must adapt the number of batches:
            NewNBatches = int(SetParameters.nMaxClusters*SetParameters.nEventsTotal/SetParameters.BatchSize)
            SetParameters.nBatches = NewNBatches
        
        SetParameters.Compute_IO_Structure()

    # Adjust parameters for multiple Digitizer runs:
    SetParameters.BatchSize = (SetParameters.BatchSize)*(SetParameters.nDigiRuns)

    # Load (& auto-compile) the model:
    NetworkName = SetParameters.OutputPath
    if (TheLabel>0):
        NetworkName = NetworkName + 'Step2Network_Mult' + TheLabelStr + '_Epoch0.h5'
    else:
        NetworkName = NetworkName + 'TheMultNetwork_Epoch0.h5'
    TheModel=ks.models.load_model(NetworkName)

    # Declare variables for the training:
    InitialEventCounter = 0
    FinalEventCounter = 0
    InputFileName = ''
    OutputFileName = ''
    InputData = np.zeros((SetParameters.BatchSize,SetParameters.nInputNeurons))
    OutputData = np.zeros((SetParameters.BatchSize,SetParameters.nOutputNeurons))
    CurrentNetworkName = ''
    BatchDimensions = False
    InputTest = False
    OutputTest = False

    # Define Epoch loop:
    for kEpoch in range(0,SetParameters.nEpochs):
    
        # Define Batch loop:
        for kBatch in range(0,SetParameters.nBatches):
        
            # Define filenames of inputs and outputs:
            InitialEventCounter = kBatch*SetParameters.BatchSize
            FinalEventCounter = InitialEventCounter + SetParameters.BatchSize - 1
            InputFileName = SetParameters.TextFilePath + 'DNN_InputFile_' + str(InitialEventCounter) + 'till' + str(FinalEventCounter) + '.txt'
            OutputFileName = SetParameters.TextFilePath + 'DNN_InputFile_Outputs_' + str(InitialEventCounter) + 'till' + str(FinalEventCounter) + '.txt'
        
            # Check if Input & Output files exist:
            InputTest = TheOS.path.isfile(InputFileName)
            OutputTest = TheOS.path.isfile(OutputFileName)
        
            # Check if the files exist:
            if ((InputTest==True) and (OutputTest==True)):
        
                # Read input and output data:
                InputData = ReadArray(InputFileName)
                OutputData = ReadArray(OutputFileName)
        
                # Check the length of the batch size:
                BatchDimensions = False
        
                if (SetParameters.CompressBatches == True):
                    if ((InputData.shape[0]==SetParameters.BatchSize) and 
                        (OutputData.shape[0]==SetParameters.BatchSize) and 
                        (InputData.shape[1]==SetParameters.nInputNeurons) and 
                        (OutputData.shape[1]==SetParameters.nOutputNeurons) and
                        (InputData.shape[0]==OutputData.shape[0])):
                
                        BatchDimensions = True
                    
                if (SetParameters.CompressBatches == False):
                    if ((InputData.shape[0]<=SetParameters.BatchSize) and 
                        (OutputData.shape[0]<=SetParameters.BatchSize) and 
                        (InputData.shape[1]==SetParameters.nInputNeurons) and 
                        (OutputData.shape[1]==SetParameters.nOutputNeurons) and
                        (InputData.shape[0]==OutputData.shape[0])):
            
                        BatchDimensions = True
                    
                if (BatchDimensions==True):
                    # Then, we can safely update the model:
                    TheModel.fit(x=InputData,y=OutputData,epochs=1,batch_size=SetParameters.BatchSize)
            
                    # give update statement:
                    print('Executed Netwok update from batch '+str(kBatch+1)+' from '+str(SetParameters.nBatches)+' in epoch '+str(kEpoch+1)+'/'+str(SetParameters.nEpochs)+'.')
                
                else:
                    # Give a warning:
                    print('Netwok update from batch '+str(kBatch+1)+' from '+str(SetParameters.nBatches)+' in epoch '+str(kEpoch+1)+'/'+str(SetParameters.nEpochs)+' was skipped because the matrix dimensions were incorrect!')
                    #print('Input Data nEvents = '+str(InputData.shape[0]))
                    #print('Input Data #neurons = '+str(InputData.shape[1]))
                    #print('Output Data nEvents = '+str(OutputData.shape[0]))
                    #print('Output Data #neurons = '+str(OutputData.shape[1]))
                    #print('Network structure #neurons IN: '+str(SetParameters.nInputNeurons))
                    #print('Network structure #neurons OUT: '+str(SetParameters.nOutputNeurons))
                    #print('Network structure BatchSize: '+str(SetParameters.BatchSize))
                    #print(SetParameters.TextFilePath)
        
            else:
                # This just means that the files weren't there. Nothing to worry about because it can be caused by the compressing of the batches.
                print('Netwok update from batch '+str(kBatch+1)+' from '+str(SetParameters.nBatches)+' in epoch '+str(kEpoch+1)+'/'+str(SetParameters.nEpochs)+' was skipped because Input/Output files are not present!')
    
        # Save the model after each epoch:
        if (SetParameters.SaveBetweenEpochs==True):
            TheEpochStr = str(kEpoch+1)
            CurrentNetworkName = SetParameters.OutputPath
            if (TheLabel>0):
                CurrentNetworkName = CurrentNetworkName + 'Step2Network_Mult' + TheLabelStr + '_Epoch' + TheEpochStr + '.h5'
            else:
                CurrentNetworkName = CurrentNetworkName + 'TheMultNetwork_Epoch' + TheEpochStr + '.h5'
            print('==> NOTE: Network is being saved! DO NOT INTERRUPT NOW!')
            TheModel.save(CurrentNetworkName)
            print('==> NOTE: Network saving is finished. You may interrupt if you like.')
    
        # give completed epoch statement:
        print('###: EPOCH '+str(kEpoch+1)+' FROM '+str(SetParameters.nEpochs)+' WAS COMPLETED!')
        print('')
    
    # Done ------------------------------

    # Next, save the model:
    FinalNetworkName = SetParameters.OutputPath
    if (TheLabel>0):
        FinalNetworkName = FinalNetworkName + 'Step2Network_Mult' + TheLabelStr + '_Final.h5'
    else:
        FinalNetworkName = FinalNetworkName + 'TheMultNetwork_Final.h5'
    TheModel.save(FinalNetworkName)
