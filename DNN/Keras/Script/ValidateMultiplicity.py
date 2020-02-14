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

# Use script inputs:
if __name__ == "__main__":
    TheEpoch = int(TheSys.argv[1])
    TheEpochStr = str(TheEpoch)

    # Set parameters:
    SetParameters.Initialize()
    
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
    
    # Define the network name:
    TheNetworkName = ThisOutputPath + 'TheMultNetwork_Epoch' + TheEpochStr + '.h5'

    # Check that this is a Multiplicity Network:
    if ((SetParameters.NetworkType=='IO_Signals_ElenaHoemann') or
        (SetParameters.NetworkType=='IO_Signals_Elena_9002_5') or
        (SetParameters.NetworkType=='IO_Signals_Elena_9004_5') or
        (SetParameters.NetworkType=='IO_Signals_Elena_6004_5') or
        (SetParameters.NetworkType=='IO_Signals_Elena_12004_5') or
        (SetParameters.NetworkType=='IO_Signals_MultNetwork') or
        (SetParameters.NetworkType=='IO_3to6_TradMed_BabyNetwork') or
        (SetParameters.NetworkType=='IO_2to6_TradMed_BabyNetwork')):

        # Load (& auto-compile) the model:
        TheModel=ks.models.load_model(TheNetworkName)

        # Declare variables:
        InitialEventCounter = 0
        FinalEventCounter = 0
        InputFileName = ''
        OutputFileName = ''
        InputData = np.zeros((SetParameters.BatchSize,SetParameters.nInputNeurons))
        OutputData = np.zeros((SetParameters.BatchSize,SetParameters.nOutputNeurons))
        PredictedData = np.zeros((SetParameters.BatchSize,SetParameters.nOutputNeurons))
        nEventsPerBatch = 0
        Predicted_Multiplicities = np.zeros((SetParameters.nOutputNeurons,1))
        True_Multiplicities = np.zeros((SetParameters.nOutputNeurons,1))
        Predicted_Limited_Multiplicities = np.zeros((SetParameters.nOutputNeurons,1))
        Maximum_Predicted = 0.0
        MaxIndex_Predicted = -1
        Maximum_True = 0.0
        MaxIndex_True = -1

        # Loop over all batches:
        for kBatch in range(0,SetParameters.nBatches):
        
            # Define filenames of inputs and outputs:
            InitialEventCounter = kBatch*SetParameters.BatchSize
            FinalEventCounter = InitialEventCounter + SetParameters.BatchSize - 1
            InputFileName = SetParameters.TextFilePath + 'DNN_InputFile_' + str(InitialEventCounter) + 'till' + str(FinalEventCounter) + '.txt'
            OutputFileName = SetParameters.TextFilePath + 'DNN_InputFile_Outputs_' + str(InitialEventCounter) + 'till' + str(FinalEventCounter) + '.txt'
        
            # Read input and output data:
            InputData = ReadArray(InputFileName)
            OutputData = ReadArray(OutputFileName)
        
            # Check the dimensions of the matrices:
            if ((InputData.shape[0]<=SetParameters.BatchSize) and 
                (OutputData.shape[0]<=SetParameters.BatchSize) and 
                (InputData.shape[1]==SetParameters.nInputNeurons) and 
                (OutputData.shape[1]==SetParameters.nOutputNeurons) and
                (InputData.shape[0]==OutputData.shape[0])):
        
                # Then, we can safely use the model for a prediction:
                PredictedData = TheModel.predict(InputData)
        
                # Next, process the statistics:
        
                # Update current number of events:
                nEventsPerBatch = InputData.shape[0]
        
                # Find max. output and use that to count. Loop over events in the batch:
                for n in range(0,nEventsPerBatch):
    
                    # Find out maximum & position in prediction. Reset parameters:
                    MaxIndex_Predicted = -1
                    Maximum_Predicted = -100.0
    
                    for k in range(0,SetParameters.nOutputNeurons):
                        if (Maximum_Predicted<PredictedData[n,k]):
                            Maximum_Predicted = PredictedData[n,k]
                            MaxIndex_Predicted = k
            
                    # Find out maximum in the true outputs. Reset parameters:
                    Maximum_True = -100.0
                    MaxIndex_True = -1
    
                    for k in range(0,SetParameters.nOutputNeurons):
                        if (Maximum_True<OutputData[n,k]):
                            Maximum_True = OutputData[n,k]
                            MaxIndex_True = k
            
                    # Next, update the counters:
                    Predicted_Multiplicities[MaxIndex_Predicted,0] = Predicted_Multiplicities[MaxIndex_Predicted,0] + 1
                    True_Multiplicities[MaxIndex_True,0] = True_Multiplicities[MaxIndex_True,0] + 1
    
                    if (MaxIndex_Predicted==MaxIndex_True):
                        Predicted_Limited_Multiplicities[MaxIndex_True,0] = Predicted_Limited_Multiplicities[MaxIndex_True,0] + 1
                
                    # Done. ------------------
            
                # give update statement:
                print('Executed Netwok prediction from batch '+str(kBatch+1)+'/'+str(SetParameters.nBatches)+'.')
        
            else:
          
                # Give error message:
                print('==> ERROR in batch '+str(kBatch+1)+'/'+str(SetParameters.nBatches)+': MATRIX DIMENSIONS WERE INCORRECT!')
        
        # Done -----------------------------
    
        # Next, to get the percentages, do a division:
        Percentages = np.zeros((SetParameters.nOutputNeurons,1))

        for k in range(0,SetParameters.nOutputNeurons):
            Percentages[k,0] = 100.0*Predicted_Limited_Multiplicities[k,0]/True_Multiplicities[k,0]

        # Then, print results:
        print('')
        print('Multiplicity efficiencies are [%]:')
        print(Percentages)
        print('')
    
    else:
        print('THIS VALIDATION SCRIPT IS ONLY SUITABLE FOR MULTIPLICITY NETWORKS!')
