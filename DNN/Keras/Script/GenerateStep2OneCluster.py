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

# contains-function definition for substrings:
def contains(s, other):
    return s.__contains__(other)

# Use script inputs:
if __name__ == "__main__":
    # Process input:
    TheLabel = int(TheSys.argv[1])
    TheLabelStr = str(TheLabel)

   # Set the parameters:
    SetParameters.Initialize()
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
    
    # Edit the output path:
    ThisOutputPath = SetParameters.OutputPath
    if "/DNN_Training/" in ThisOutputPath:
        ThisOutputPath = ThisOutputPath.replace("/DNN_Training/","")
    if "/DNN_Validation/" in ThisOutputPath:
        ThisOutputPath = ThisOutputPath.replace("/DNN_Validation/","")
    if "/DNN_Experiment/" in ThisOutputPath:
        ThisOutputPath = ThisOutputPath.replace("/DNN_Experiment/","")
    
    # Define training folder:
    TrainingFolder = ThisOutputPath + "/DNN_Training/"
    
    # Check if it exists:
    if (TheOS.path.exists(TrainingFolder)):
        ThisOutputPath = TrainingFolder
    
    # Then, Define the network name:
    TheNetworkName = ThisOutputPath + 'Step2Network_Mult' + TheLabelStr + '_Final.h5'

    # Check that this is the OneCluster-Step2 Network:
    if (SetParameters.NetworkType=='ScoringPlus_OneCluster'):

        # Load (& auto-compile) the model:
        TheModel=ks.models.load_model(TheNetworkName)

        # Declare variables:
        InitialEventCounter = 0
        FinalEventCounter = 0
        InputFileName = ''
        InputData = np.zeros((SetParameters.BatchSize,SetParameters.nInputNeurons))
        PredictedData = np.zeros((SetParameters.BatchSize,SetParameters.nOutputNeurons))
        nEventsPerBatch = 0
        TheClusterScore = 0.0;
        MaxOutput = 0.0;
        MaxIndex = -1;
        WritingString = ''
        
        # create the output file:
        f= open(SetParameters.OutputPath+"/PredictedClusters_Mult" + TheLabelStr + ".txt","w+")
    
        # Loop over all batches:
        for kBatch in range(0,SetParameters.nBatches):
        
            # Define filenames of inputs and outputs:
            InitialEventCounter = kBatch*SetParameters.BatchSize
            FinalEventCounter = InitialEventCounter + SetParameters.BatchSize - 1
            InputFileName = SetParameters.TextFilePath + 'DNN_InputFile_' + str(InitialEventCounter) + 'till' + str(FinalEventCounter) + '.txt'
        
            # Read input and output data:
            InputData = ReadArray(InputFileName)
        
            # Check the dimensions of the matrices:
            if ((InputData.shape[0]<=SetParameters.BatchSize) and 
                (InputData.shape[1]==SetParameters.nInputNeurons)):
        
                # Then, we can safely use the model for a prediction:
                PredictedData = TheModel.predict(InputData)
                
                # Next, Obtain the batch length:
                nEventsPerBatch = InputData.shape[0]
        
                # Loop over the batch:
                for n in range(0,nEventsPerBatch):
                    
                    # Then, compute the cluster score from the DNN classification:
                    if (SetParameters.nOutputFlags_PerCluster==1):
                        
                        # That is easy, just take the outcome:
                        TheClusterScore = PredictedData[n][0]
                        
                    elif (SetParameters.nOutputFlags_PerCluster==2):
                        
                        # This is a little more difficult. We will take the 
                        # difference. The first number if the secondary classification
                        # and the second one is the primary classification. Hence, for max.
                        # score for primary clusters, we need [1]-[0]:
                        TheClusterScore = PredictedData[n][1] - PredictedData[n][0]
                        
                    elif (SetParameters.nOutputFlags_PerCluster>2):
                        
                        # Then, we have a more complex situation. We must find
                        # some sort of combination. Begin by finding the max. output neuron:
                        MaxOutput = 0.0;
                        MaxIndex = -1;
                        
                        for flag in range(0,SetParameters.nOutputFlags_PerCluster):
                            if (MaxOutput<PredictedData[n][flag]):
                                MaxOutput = PredictedData[n][flag]
                                MaxIndex = flag
                        
                        if ((MaxIndex==0) or (MaxIndex==1)):
                            TheClusterScore = PredictedData[n][1] - PredictedData[n][0]
                        else:
                            TheClusterScore = float(MaxIndex) + PredictedData[n][MaxIndex]
                    else:
                        print('### FATAL ERROR: SetParameters.nOutputFlags_PerCluster is supposed to be >0!')
                    
                    # Then, the last task is to write the outcome to the textfile:
                    WritingString = str(TheClusterScore) + '\n'
                        
                    # Write this to the file:    
                    f.write(WritingString)
                
                    # Done. ------------------
            
                # give update statement:
                print('Executed Netwok prediction from batch '+str(kBatch+1)+'/'+str(SetParameters.nBatches)+'.')
        
            else:
                # Give a warning:
                print('Netwok update from batch '+str(kBatch+1)+' from '+str(SetParameters.nBatches)+ ' was skipped because the matrix dimensions were incorrect!')
                #print('Input Data nEvents = '+str(InputData.shape[0]))
                #print('Input Data #neurons = '+str(InputData.shape[1]))
                #print('Network structure #neurons IN: '+str(SetParameters.nInputNeurons))
                #print('Network structure #neurons OUT: '+str(SetParameters.nOutputNeurons))
                #print('Network structure BatchSize: '+str(SetParameters.BatchSize))
                #print(SetParameters.TextFilePath)
        
        # Done -----------------------------
    
        # Next, Close the file and be done with it.
        f.close()
    
    else:
        print('THIS OUTPUT SCRIPT IS ONLY SUITABLE FOR SCORINGPLUS ONECLUSTER-NETWORKS!')
