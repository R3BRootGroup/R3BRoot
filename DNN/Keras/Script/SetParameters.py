# python import commands:
import os as TheOS

# import own functions:
from ReadInputs import GetInputInteger
from ReadInputs import GetInputDouble
from ReadInputs import GetInputString
from ReadInputs import GetInputBoolian

# Definition of how to compute the network structure:
def Compute_IO_Structure():
    global nInputNeurons
    global nOutputNeurons
    global nMaxNeurons
    
    if (NetworkType=='IO_Signals_ElenaHoemann'):
        nInputNeurons = 2+4*nBarsPerPlane*nDoublePlanes
        nOutputNeurons = MaxMultiplicity
    elif (NetworkType=='IO_Signals_Elena_9002_5'):
        nInputNeurons = 2+6*nBarsPerPlane*nDoublePlanes
        nOutputNeurons = MaxMultiplicity
    elif (NetworkType=='IO_Signals_Elena_9004_5'):
        nInputNeurons = 4+6*nBarsPerPlane*nDoublePlanes
        nOutputNeurons = MaxMultiplicity
    elif (NetworkType=='IO_Signals_Elena_6004_5'):
        nInputNeurons = 4+4*nBarsPerPlane*nDoublePlanes
        nOutputNeurons = MaxMultiplicity
    elif (NetworkType=='IO_Signals_Elena_12004_5'):
        nInputNeurons = 4+8*nBarsPerPlane*nDoublePlanes
        nOutputNeurons = MaxMultiplicity
    elif (NetworkType=='IO_Signals_MarkusPolleryd'):
        nInputNeurons = -1
        nOutputNeurons = -1
    elif (NetworkType=='IO_Signals_MultNetwork'):
        nInputNeurons = 6*nMaxSignals
        nOutputNeurons = MaxMultiplicity+1
    elif (NetworkType=='IO_Signals_StandardNetwork'):
        nInputNeurons = 6*nMaxSignals
        nOutputNeurons = nMaxSignals 
    elif (NetworkType=='IO_3to6_TradMed_BabyNetwork'):
        nInputNeurons = 3
        nOutputNeurons = MaxMultiplicity+1
    elif (NetworkType=='IO_2to6_TradMed_BabyNetwork'):
        nInputNeurons = 2
        nOutputNeurons = MaxMultiplicity+1
    elif (NetworkType=='ScoringPlus'):
        nInputNeurons = nMaxClusters*(nClusterScores+4)
        nOutputNeurons = nMaxClusters*nOutputFlags_PerCluster
    elif (NetworkType=='ScoringPlus_OneCluster'):
        nInputNeurons = nClusterScores+4
        nOutputNeurons = nOutputFlags_PerCluster

    if (IncludeTarget==True):
        nInputNeurons = nInputNeurons + 4
    
    nMaxNeurons = nInputNeurons
    if (nMaxNeurons<nOutputNeurons):
        nMaxNeurons = nOutputNeurons

# function that initializes all parameters:
def Initialize():
    global nEventsTotal
    nEventsTotal = GetInputInteger('R3BRoot_nEvents')
    
    global nEpochs
    nEpochs = GetInputInteger('NeuLAND_DNNTextFile_nEpochs')
    
    global nEpochs_Step2
    nEpochs_Step2 = GetInputInteger('NeuLAND_DNNTextFile_nStep2Epochs')
    
    global BatchSize
    BatchSize = GetInputInteger('NeuLAND_DNNTextFile_EventBatchSize')
    
    global nBatches
    nBatches = 1
    if (BatchSize>0):
        nBatches = round(nEventsTotal/BatchSize+0.5)
    if (BatchSize<0):
        BatchSize = 0
        
    global OutputPath
    OutputPath = GetInputString('TheOutputPath')
    
    global nDoublePlanes
    nDoublePlanes = GetInputInteger('NeuLAND_Number_of_DoublePlanes')
    
    global nBarsPerPlane
    nBarsPerPlane = GetInputInteger('NeuLAND_Number_of_Bars_per_plane')
    
    global MaxMultiplicity
    MaxMultiplicity = GetInputInteger('ParticleGun_Multiplicity')
    
    global nMaxSignals
    nMaxSignals = GetInputInteger('NeuLAND_DNNTextFile_MaxNumberOfSignals')
    
    global nMaxClusters
    nMaxClusters = GetInputInteger('NeuLAND_DNNTextFile_MaxNumberOfClusters')
    
    global IncludeTarget
    IncludeTarget = GetInputBoolian('NeuLAND_DNNTextFile_IncludeTargetInputs')
    
    global SaveBetweenEpochs
    SaveBetweenEpochs = GetInputBoolian('NeuLAND_DNNTextFile_SaveNetwork_BetweenEpochs')
    
    global CompressBatches
    CompressBatches = GetInputBoolian('NeuLAND_DNNTextFile_ForceBatchesEqualLength')
    
    global nDigiRuns
    nDigiRuns = GetInputInteger('NeuLAND_Digitizer_nDifferentRuns')
    
    global nClusterScores
    nClusterScores = 0
    if (GetInputBoolian('NeuLAND_Scoring_UseClusterScore_DepositedEnergy')==True): nClusterScores = nClusterScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseClusterScore_KineticEnergy')==True): nClusterScores = nClusterScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseClusterScore_RelativisticBeta')==True): nClusterScores = nClusterScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseClusterScore_TOF')==True): nClusterScores = nClusterScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseClusterScore_Size')==True): nClusterScores = nClusterScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseClusterScore_Rvalue')==True): nClusterScores = nClusterScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseClusterScore_LogR')==True): nClusterScores = nClusterScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseClusterScore_SpaceLength')==True): nClusterScores = nClusterScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseClusterScore_SpaceRadius')==True): nClusterScores = nClusterScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseClusterScore_TimeLength')==True): nClusterScores = nClusterScores + 1
    
    global nSignalScores
    nSignalScores = 0
    if (GetInputBoolian('NeuLAND_Scoring_UseSignalScore_DepositedEnergy')==True): nSignalScores = nSignalScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseSignalScore_KineticEnergy')==True): nSignalScores = nSignalScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseSignalScore_RelativisticBeta')==True): nSignalScores = nSignalScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseSignalScore_TOF')==True): nSignalScores = nSignalScores + 1
    if (GetInputBoolian('NeuLAND_Scoring_UseSignalScore_Rvalue')==True): nSignalScores = nSignalScores + 1

    # Extract network structure:
    global nInputNeurons
    nInputNeurons = 1
    
    global nOutputNeurons
    nOutputNeurons = 1
    
    global NetworkType
    NetworkType = GetInputString('NeuLAND_DNNTextFile_InfoStructure')
    
    global Step2_NetworkType
    Step2_NetworkType = GetInputString('NeuLAND_DNNTextFile_Step2Structure')
    
    global Step2_SelectedMultiplicity
    Step2_SelectedMultiplicity = 0
    
    global nOutputFlags_PerCluster
    nOutputFlags_PerCluster = GetInputInteger('NeuLAND_DNNTextFile_Step2_nOutputFlags_PerCluster')
    
    global TextFilePath
    TextFilePath = OutputPath
    if (not(BatchSize==0)):
        TextFilePath = TextFilePath + '/DNN_DataBase/'
        
    global nMaxNeurons
    nMaxNeurons = 1
    
    # Compute Network structure:
    Compute_IO_Structure()
