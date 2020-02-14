#!/bin/bash

# Full script for network usage in experimental data. This cannot be done
# in multi-thread-mode since the exp. data is usually divided over many files.
# Another argument can be provided to the script. If this is 'GPU', the 
# Script will use GPU TensorFlow instead of regular TensorFlow.

# First modify/recreate inputs for exp. processing:
cd ./Macros/
    root -l -q GenerateExperimentalInput.cpp
cd ../

# Next, obtain the output Path:
cd ./Macros/
root -l -q GenerateOutputPath.cpp
root -l -q GenerateMaxMultiplicity.cpp
cd ../

# And read it into bash:
read TheOutputPath < ./InputFiles/OutputPath.txt
read MaxMultiplicity < ./InputFiles/MaxMultiplicity.txt

# Add the training and validation parts:
TrainingOutputPath="${TheOutputPath}/DNN_Training/"
ValidationOutputPath="${TheOutputPath}/DNN_Validation/"
ExpOutputPath="${TheOutputPath}/DNN_Experiment/"

# Check for the Training directory:
if [ ! -d "${TrainingOutputPath}" ]
then
echo 'ERROR: You must train the network before running this script!'
return
fi

# Then, Deal with the experimental directory:
if [ ! -d "${ExpOutputPath}" ]
then
    # Then, we should make it:
    mkdir -p ${ExpOutputPath}
    
    # DNN DataBase directory:
    if [ ! -d "${ExpOutputPath}/DNN_DataBase/" ]
    then
        mkdir -p "${ExpOutputPath}/DNN_DataBase/"
    fi
    
    # Check step 2:
    for ((nMult=0; nMult<${MaxMultiplicity}; ++nMult))
    do
        ThisMult=`expr ${nMult} + 1`
        
        if [ ! -d "${ExpOutputPath}/DNN_Step2_Mult${ThisMult}/" ]
        then
            mkdir -p "${ExpOutputPath}/DNN_Step2_Mult${ThisMult}/"
        fi
    done
fi

# Then, check if the required files exist:
if [ -f "${TrainingOutputPath}/CutsFile.txt" ]
then
cp "${TrainingOutputPath}/CutsFile.txt" "${ExpOutputPath}/CutsFile.txt"
else
echo 'ERROR: You cannot do exp. processing without CutsFile.txt!'
return
fi

# Then, check if the required files exist:
if [ -f "${TrainingOutputPath}/TimeFile.txt" ]
then
cp "${TrainingOutputPath}/TimeFile.txt" "${ExpOutputPath}/TimeFile.txt"
else
echo 'ERROR: You cannot do exp. processing without TimeFile.txt!'
return
fi

# Then, check if the required files exist:
if [ ! -f "${TrainingOutputPath}/TheMultNetwork_Final.h5" ]
then
echo 'ERROR: You cannot do exp. processing without the final network file!'
return
fi

if [ -f "${TrainingOutputPath}/NeuLAND_TheScorersFile.root" ]
then
cp "${TrainingOutputPath}/NeuLAND_TheScorersFile.root" "${ExpOutputPath}/NeuLAND_TheScorersFile.root"
else
echo 'ERROR: You cannot do exp. processing without NeuLAND_TheScorersFile.root!'
return
fi

if [ -f "${TrainingOutputPath}/NormParams.txt" ]
then
cp "${TrainingOutputPath}/NormParams.txt" "${ExpOutputPath}/NormParams.txt"
else
echo 'NOTE: NormParams.txt is missing. But technically we could do without.'
# NOTE: No return command.
fi

####################################################################################

# Then, generate the geometry:
cd ./Geometry
./Geometry.sh
cd ../

# Next, process the data. Begin with a very small MC simulation, so that
# the Par-file etc. are there. it also allows usage of eventdisplay, etc.
cd ./Macros
root -l -q MCtransport.cpp

# Then, obtain the signals from the exp. data:
if [[ "$1" = "Background" ]]
then
root -l -q "NeuLAND_ExpTranslation.cpp(kTRUE)"
else
root -l -q "NeuLAND_ExpTranslation.cpp(kFALSE)"
fi

# Then, do the same processing chain:
root -l -q ClusterSignals.cpp

# Generate input for the multiplicity network:
root -l -q "TranslateClusters.cpp(1,0,kFALSE)"
   
# Then, run it through the network:
cd ../Keras/Script/
    
if [[ "$2" = "GPU" ]]
then
source /home/machinelearning/Desktop/GPUTensorFlow/bin/activate
else
source /home/machinelearning/Desktop/TensorFlow/bin/activate
fi

python ./GenerateMultOutput.py
cd ../../Macros
    
# Next, convert the output to ROOT data:
root -l -q "Read_DNN_Multiplicity.cpp(1)"
    
# And then, generate the DNN input for the second step:
root -l -q "TranslateClusters.cpp(1,0,kTRUE)"
    
# Run the second step through the network:
cd ../Keras/Script

for ((nMult=0; nMult<${MaxMultiplicity}; ++nMult))
do
    ThisMult=`expr ${nMult} + 1`
    python ./GenerateStep2OneCluster.py ${ThisMult}
done

# Then, continue with the analysis:
cd ../../Macros
./Analyze.sh 1
cd ../

# And, finally, save the inputs:
cp ./InputFiles/InputsFile.txt "${ExpOutputPath}/InputsFile.txt"

# Done.


