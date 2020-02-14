void R3BTextFileGenerator::WriteEventNTimes(Int_t const nTimes)
{
    for (Int_t kTime = 0; kTime<nTimes; ++kTime)
    {
    
    // Decide on how to construct the batches:
    Int_t TheCounter;
    if (CompressBatches==kFALSE) {TheCounter = EventCounter;}
    else {TheCounter = FillCounter;}
    
    // Writes a single event to the textfile. First, start by generating EventCounter Strings:
    TString st = "";
    TString EventCounterString = st.Itoa(TheCounter,10);
    TString EventEndStr = st.Itoa(TheCounter+EventBatch-1,10);
    Int_t EventWritingIndex = 0;
    
    // Next, decide whether we should recreate a new file yes/no:
    TString EventTag;
    
    if (EventBatch==0)
    {
        // Then, do nothing:
        EventTag = "";
    }
    else if (EventBatch==1)
    {
        // Then, we need a simple EventTag:
        EventTag = EventCounterString;
    }
    else if ((EventBatch>1)&&(EventBatch<=Nevents))
    {
        // Then, we need a complex EventTag:
        EventTag = EventCounterString + "till" + EventEndStr;
    }
    else
    {
        ErrorMessage("EventBatch should be zero or positive & smaller then the total number of events!");
    }
    
    // Test that we can actually write:
    if (FileTest==kFALSE)
    {
        ErrorMessage("The TextFile could not be created! No event could be written! Event Index = "+EventCounterString);
    }
    else
    {
        // Next, we must make sure that the inputs & output are there.
        
        // NB: for markus format, we have dynamical inputs & outputs:
        if (FileOption=="MARKUS") {N_inputs = InputNeurons.size(); N_outputs = OutputNeurons.size();}
        
        // Then, check the format:
        if (!((InputNeurons.size()==N_inputs)&&(OutputNeurons.size()==N_outputs)))
        {
            // then give error message, if appropriate.
            if ((InputNeurons.size()==0)&&(OutputNeurons.size()==0))
            {
                // Then, nothing is going on. this event was skipped deliberately.
            }
            else
            {
                // Then, there is some trouble!
                if (InputNeurons.size()!=N_inputs)   {ErrorMessage(" You did not specify the correct number of inputs for Event index "+EventCounterString+" | N_inputs="+st.Itoa(N_inputs,10)+" | Vector Size = "+st.Itoa(InputNeurons.size(),10));}
                if (OutputNeurons.size()!=N_outputs) {ErrorMessage(" You did not specify the correct number of outputs for Event index "+EventCounterString+" | N_outputs="+st.Itoa(N_outputs,10)+" | Vector Size = "+st.Itoa(OutputNeurons.size(),10));}
            }
        }
        else
        {
            // Decide whether we create a new file yes/no:
            if ((EventTag!="")&&(TheCounter%EventBatch==0))
            {
                // Then, we should create a new file.
                // We start at TheCounter==0, so the first time, it will always work.
                GenerateOneFile(EventTag);
                BatchCounter = 0;
            }
            
            // now, we can write our event:
            TString OneInput = "";
            TString OneOutput = "";
            EventWritingIndex = TheCounter;
            if ((EventTag!="")&&(EventBatch>0)&&(EventBatch<Nevents)) {EventWritingIndex = BatchCounter;}
            
            if (FileOption=="FANN")
            {
                // Write our inputs:
                for (Int_t kin = 0; kin<N_inputs; ++kin)
                {
                    OneInput = RoundOff(InputNeurons.at(kin),IO_Significance);
                    TheTextFile << OneInput.Data() << " ";
                }
                TheTextFile << "\n";
                
                // Write our outputs:
                for (Int_t kout = 0; kout<N_outputs; ++kout)
                {
                    OneOutput = RoundOff(OutputNeurons.at(kout),IO_Significance);
                    TheTextFile << OneOutput.Data() << " ";
                }
                TheTextFile << "\n";
            }
            else if (FileOption=="KERAS")
            {
                // Write our inputs:
                TheTextFile << "InputData[" << EventWritingIndex << ",:] = [";
                
                for (Int_t kin = 0; kin<N_inputs; ++kin)
                {
                    TheTextFile << RoundOff(InputNeurons.at(kin),IO_Significance).Data();
                    if (kin!=(N_inputs-1)) {TheTextFile << ",";}
                }
                
                TheTextFile << "]\n";
                
                // Write our outputs:
                TheTextFile << "OutputData[" << EventWritingIndex << ",:] = [";
                
                for (Int_t kout = 0; kout<N_outputs; ++kout)
                {
                    TheTextFile << RoundOff(OutputNeurons.at(kout),IO_Significance).Data();
                    if (kout!=(N_outputs-1)) {TheTextFile << ",";}
                }
                
                TheTextFile << "]\n";
                
                // Done.
            }
            else if (FileOption=="PythonText")
            {
                // Simply print number and nothing else.
                
                // Print input numbers in Input-file:
                for (Int_t kin = 0; kin<N_inputs; ++kin)
                {
                    TheTextFile << RoundOff(InputNeurons.at(kin),IO_Significance).Data();
                    if (kin!=(N_inputs-1)) {TheTextFile << ";";}
                }
                TheTextFile << "\n";
                
                // Print output numbers in Output-file:
                for (Int_t kout = 0; kout<N_outputs; ++kout)
                {
                    OutTextFile << RoundOff(OutputNeurons.at(kout),IO_Significance).Data();
                    if (kout!=(N_outputs-1)) {OutTextFile << ";";}
                }
                OutTextFile << "\n";
                
                // Done.
            }
            else if (FileOption=="MARKUS")
            {
                // Write our inputs:
                TheTextFile << "Event " << EventWritingIndex+1 << "\n";
                
                // Extract size of vector:
                Int_t VectorSize = (Int_t) InputNeurons.size();
                Int_t nQuantities = 6;
                Int_t nHits = VectorSize/nQuantities;
                Int_t nRest = VectorSize%nQuantities;
                if (nRest!=0) {ErrorMessage("At fileFormat MARKUS, InputNeurons should have 6 quantities per hit, so size%6==0, which is not the case!");}
                
                for (Int_t kin = 0; kin<nHits; ++kin)
                {
                    // Write plane number:
                    if (ExecuteNormalization==kFALSE)
                    {
                        if (InputNeurons.at(nQuantities*kin+0)<10.0) {TheTextFile << " ";}
                        TheTextFile << DoubleIntTotString(InputNeurons.at(nQuantities*kin+0)) << "/";
                    }
                    else
                    {
                        TheTextFile << RoundOff(InputNeurons.at(nQuantities*kin+0),IO_Significance) << "/";
                    }
                    
                    // Write bar number inside plane:
                    if (ExecuteNormalization==kFALSE)
                    {
                        if (InputNeurons.at(nQuantities*kin+1)<10.0) {TheTextFile << " ";}
                        TheTextFile << DoubleIntTotString(InputNeurons.at(nQuantities*kin+1)) << "  ";
                    }
                    else
                    {
                        TheTextFile << RoundOff(InputNeurons.at(nQuantities*kin+1),IO_Significance) << "  ";
                    }
                    
                    // Write time t1:
                    TheTextFile << "t1= " << RoundOff(InputNeurons.at(nQuantities*kin+2),IO_Significance) << " ";
                    
                    // Write time t2:
                    TheTextFile << "t2= " << RoundOff(InputNeurons.at(nQuantities*kin+3),IO_Significance) << "  ";
                    
                    // Write energy e1:
                    TheTextFile << "e1= " << RoundOff(InputNeurons.at(nQuantities*kin+4),IO_Significance) << " ";
                    
                    // Write energy e2:
                    TheTextFile << "e2= " << RoundOff(InputNeurons.at(nQuantities*kin+5),IO_Significance) << "\n";
                    
                    // Done.
                }
                    
                // Next, write our neutron momenta, but this time to the output-file.
                // Begin by extracting the size of the vector:
                VectorSize = (Int_t) OutputNeurons.size();
                nQuantities = 4;
                nHits = VectorSize/nQuantities;
                nRest = VectorSize%nQuantities;
                if (nRest!=0) {ErrorMessage("At fileFormat MARKUS, OutputNeurons should have 4 quantities per hit, so size%4==0, which is not the case!");}
                
                // Write the first line:
                OutTextFile << "Event   " << EventWritingIndex << "   Guns    " << nHits << "\n";
                
                // Next, write the neutron momenta:
                for (Int_t kout = 0; kout<nHits; ++kout)
                {
                    OutTextFile << RoundOff(OutputNeurons.at(nQuantities*kout+0),IO_Significance) << " ";
                    OutTextFile << RoundOff(OutputNeurons.at(nQuantities*kout+1),IO_Significance) << " ";
                    OutTextFile << RoundOff(OutputNeurons.at(nQuantities*kout+2),IO_Significance) << " ";
                    OutTextFile << RoundOff(OutputNeurons.at(nQuantities*kout+3),IO_Significance) << "\n";
                    // Done.
                }
                 
                // Now, our full writing function has been performed.
            }
            else
            {
                // Default option:
                for (Int_t kin = 0; kin<N_inputs; ++kin) {TheTextFile << InputNeurons.at(kin) << " ";}
                TheTextFile << " | ";
                for (Int_t kout = 0; kout<N_outputs; ++kout) {TheTextFile << OutputNeurons.at(kout) << " ";}
                TheTextFile << "\n";
            }
            
            // Count this event as being filled:
            FillCounter = FillCounter + 1;
            
            // So now, we are talking about the future event. So if the future event will generate a new file,
            // it is now time to close a file:
            if ((EventTag!="")&&((TheCounter+1)%EventBatch==0))
            {
                TheTextFile.close();
                OutTextFile.close();
            }
            
            // Done.
        }
        
        // Next, we must close the files. so first update the eventcounter:
        EventCounter = EventCounter + 1;
        BatchCounter = BatchCounter + 1;
        
        // Done.
        
    }
    
    // Close for-loop of writing:
    }
    
    // Then, reset the vectors:
    InputNeurons.clear();
    OutputNeurons.clear();
    // ATTENTION: You have to refill the vectors again before you can call this function again!
}
