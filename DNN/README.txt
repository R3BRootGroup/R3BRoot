These are the steps to use the DNN module of R3BRoot for NeuLAND.

Step 1: Place this folder (DNN) in the R3BRoot source directory.

Step 2: Open the master CMakeLists.txt file of R3BRoot. Add the 
        command add_subdirectory(DNN) to the list of add_subdirectory-commands
        near lines 378-421 (approximately)
        Also, go to R3BDetectorList.h in r3bdata and add the following 
        detectors: kCADNEULAND, kCADVETO, kCADNEBULA and kCADNEBULAVETO.
        
Step 3: Recompile R3BRoot.

Step 4: Install the Keras neural network framework in a python virtual
        environment, along with the TensorFlow backend.
        There is good documentation available for this
        on the internet, but here is a quick set of instructions:
        a: open a terminal window
        b: go to your home directory
        c: sudo apt-get install python3-venv
        d: mkdir ~/Desktop/TensorFlow
        e: cd ~/Desktop/TensorFlow
        f: python3 -m venv ~/Desktop/TensorFlow
        g: source ~/Desktop/TensorFlow/bin/activate
        h: pip install numpy
        i: pip install scipy
        j: pip install wheel
        k: pip install tensorflow==1.6 (pip install tensorflow-gpu==1.14 for GPU support)
        l: pip install keras==2.1.6
        m: give the command 'deactivate'
        n: Open your .bashrc and add to it: alias TensorFlow=”source ~/Desktop/TensorFlow/bin/activate”
        o: Open the scripts Train.sh, Validate.sh and Experiment.sh and replace
           ALL the source-commands in there to match step g & n on your computer (important!)

Step 5: Run the script DefaultInputs.sh to generate a default list of input parameters.

Step 6: Adapt the input-parameters of the simulation to your needs. You can do this by 
        running the script AdaptInputs.sh, or go to the InputFiles-directory and 
        open the file InputsFile.txt to make changes directly. 
        
        ATTENTION: The number of events should be strictly larger then the batch size.
                   Otherwise, the program will crash. To get statistically meaningful 
                   results, we recommend using 1e6 events and 2 epochs for training.
                   1e4 events will do for validation. For both situations, a batch
                   size of 1000 events is recommended. However, when one wants proper 
                   relative energy spectra, 1e6 events are also needed for validation.
                   
Step 7: Train the network, by running the script Train.sh
        This script can run without any inputs. In that case, the simulation will 
        run in single-thread mode. If you want to run in multi-thread mode, you 
        have to supply the number of threads as an input argument to the script.
        In this case, the multithreading is done fully automatically.
        
        NOTE: nThreads*BatchSize should be an exact divided of nEvents for the best results.
        
        NOTE: You could enter the string GPU as a second argument to the script.
              If this is done, the network training is done on the GPU
              instead. For this to work, a second python virtual environment
              should be created, but this time with the GPU-version of
              TensorFlow & Keras installed instead of the regular ones. This is
              considerably more tricty and we refer to the TensorFlow & Keras
              documentations for this. Step 4n should also be repeated
              so that the scripts know where to find the GPU TensorFlow.
              
        ATTENTION: Running this script with 1e6 events, a batch size of 100, 10 digitizer runs
                   and 2 epochs will take 5 hours of simulation time on an
                   i7-8750H processor using 10 threads, followed by multiple hours
                   of network training on a NVIDIA GeForce GTX 1050 Ti 4 GB
                   GPU, using GPU TensorFlow (regular TensorFlow is 
                   considerably slower). The entire process takes about 1 TB HDD memory.
        
Step 8: adapt the number of events in InputsFile.txt for the validation.

Step 9: Validate the network, by running the script Validate.sh which works
        in a similar way as Train.sh (takes 15 min for 1e4 events).
        
Step 10: Study the output, generated in the OutputFiles-folder. The output 
         can be redirected to any directory of your choice, by adapting the 
         output path in InputsFile.txt prior to the simulation.
         
To process experimental data with the network, one can use the Experiment.sh script.
This script cannot run in Multi-Thread mode, but it can utilize the GPU like the other scripts.
However, since each experiment is slightly different, each experiment requires you to write
its own translator-class to the format used by this module. A template for this purpose
is provided in DNN/Experiment/Template. Duplicate this entire folder. Change the name
of the duplicated folder and of the class (and of .cxx & .h files) to your choice and add it to
CMakeLists.txt & DNNLinkDef.h of the DNN module. Inside the .cxx & .h files, pay special attention to the 
ATTENTION-notes in there. They guide you through the process of adapting the template to your own
needs. After you are done writing this class, recompile R3BRoot. Then, go to DNN/Macros
and open NeuLAND_ExpTranslation.cpp and add your new class to be executed inside
this macro. After doing these steps, the Experiment.sh script should work for your new
experiment. Note that the script requires a trained network, so one should always use
Train.sh for this puprose before running Experiment.sh (else, it will crash). 
It is not required to also run Validate.sh before Experiment.sh, but we do highly
recommend it, because otherwise the output of Experiment.sh is blind. It is what
it is and you have no idea about efficiencies, etc. This information is provided
by Validate.sh so the neutron tracks can be properly interpret afterwards.
