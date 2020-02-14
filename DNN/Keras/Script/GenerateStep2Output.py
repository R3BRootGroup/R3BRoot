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

# Use scoipt inputs:
if __name__ == "__main__":
    # Process input:
    TheLabel = int(TheSys.argv[1])
    TheLabelStr = str(TheLabel)

    # Set parameters:
    SetParameters.Initialize()
    SetParameters.Step2_SelectedMultiplicity = TheLabel
    SetParameters.NetworkType = SetParameters.Step2_NetworkType
    SetParameters.TextFilePath = SetParameters.TextFilePath.replace('/DNN_DataBase/','/DNN_Step2_Mult'+TheLabelStr+'/')
    SetParameters.nMaxClusters = 2*TheLabel+1
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

    # Check that this is a Ste2 Network (But NOT the OneCluster!)
    if (SetParameters.NetworkType=='ScoringPlus'):

        # Load (& auto-compile) the model:
        TheModel=ks.models.load_model(TheNetworkName)

        # Declare variables:
        InitialEventCounter = 0
        FinalEventCounter = 0
        InputFileName = ''
        InputData = np.zeros((SetParameters.BatchSize,SetParameters.nInputNeurons))
        PredictedData = np.zeros((SetParameters.BatchSize,SetParameters.nOutputNeurons))
        PredictedMults = np.zeros((SetParameters.BatchSize,SetParameters.nOutputNeurons))
        nEventsPerBatch = 0
        nClusters = 0
        Maximum_Predicted = 0.0
        MaxIndex_Predicted = -1
        Maxima = np.zeros((1,1))
        MaxIndices = np.zeros((1,1), dtype=int)
        ThisMax = 0.0
        ThisMaxIndex = 0
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
                
                # Next, we need to find the indices of the largest outputs.
                # How many, is defined by the Label.
                Maxima = np.zeros((1,TheLabel))
                MaxIndices = np.zeros((1,TheLabel), dtype=int)
                
                # Obtain the batch length:
                nEventsPerBatch = InputData.shape[0]
        
                # Loop over the batch:
                for n in range(0,nEventsPerBatch):
                    
                    # Find the number of clusters:
                    nClusters = SetParameters.nMaxClusters
                    
                    # Reset maximum search:
                    for s in range(0,TheLabel):
                        Maxima[0][s] = 0.0
                        MaxIndices[0][s] = -1
                        
                    # Search multiple times:
                    for s in range(0,TheLabel):
                        
                        # Reset current search:
                        ThisMax = 0.0
                        ThisMaxIndex = -1
                        
                        # Loop over the clusters:
                        for k in range(0,nClusters):
                            
                            # Search:
                            if (ThisMax<PredictedData[n][k]):
                                ThisMax = PredictedData[n][k]
                                ThisMaxIndex = k
                                
                            # Done.
                        
                        # Next, store the found maximum:
                        Maxima[0][s] = ThisMax
                        MaxIndices[0][s] = ThisMaxIndex
                        
                        # And remove this one from the search:
                        PredictedData[n][ThisMaxIndex] = -1.0
                        
                        # Done.
                    
                    # Next, we should write the output:
                    WritingString = ''
                    
                    for s in range(0,TheLabel):
                        WritingString = WritingString + str(MaxIndices[0][s]) + ' '
                    WritingString = WritingString + '\n'
                        
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
        print('THIS OUTPUT SCRIPT IS ONLY SUITABLE FOR SCORINGPLUS NETWORKS!')
