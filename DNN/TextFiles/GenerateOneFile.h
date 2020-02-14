void R3BTextFileGenerator::GenerateOneFile(TString const EventTag)
{
    // This function actually produces fresh textfiles. CreateFile() first
    // checks whether these files can be created, but in this function, those
    // tests are skipped, so that this function can also be called during 
    // the eventloop for generating batches.
    
    // -----------------------------------------------------------------------
    
    // Properly define filenames:
    TString CurrentFileName = FileName;
    if (EventTag!="") {CurrentFileName.ReplaceAll(".txt","_"+EventTag+".txt");}
    
    TString OutputFileName = FileName;
    if (EventTag!="") {OutputFileName.ReplaceAll(".txt","_Outputs_"+EventTag+".txt");}
    else {OutputFileName.ReplaceAll(".txt","_Outputs.txt");}
    
    // Then, recreate the file, now correctly:
    TheTextFile = std::ofstream(CurrentFileName.Data(), std::ofstream::out);
    OutTextFile = std::ofstream(OutputFileName.Data(), std::ofstream::out);
    
    // Then, manage the number of events:
    Int_t NeventsInFile = Nevents;
    if ((EventTag!="")&&(EventBatch>0)&&(EventBatch<Nevents)) {NeventsInFile = EventBatch;}
        
    // Next, put in the header lines:
    if (FileOption=="FANN")
    {
        // FANN requires:
        // 1) Number of datapoints.
        // 2) number of inputs.
        // 3) number of outputs.
        TheTextFile << NeventsInFile << " " << N_inputs << " " << N_outputs << "\n";
        
        // Separate output file:
        OutTextFile << "This separate Output-file is not used in the FANN-format.\n";
    }
    else if (FileOption=="KERAS")
    {
        // This becomes a python-script that hard-codes all inputs & outputs.
        // Hence, we declare the matrices here:
        TheTextFile << "import numpy as np;\n";
        TheTextFile << "InputData = np.ones((" << NeventsInFile << "," << N_inputs << "))\n";
        TheTextFile << "OutputData = np.ones((" << NeventsInFile << "," << N_outputs << "))\n";
        TheTextFile << "\n";
            
        // Separate output file:
        OutTextFile << "This separate Output-file is not used in the KERAS-format.\n";
    }
    else if (FileOption=="MARKUS")
    {
        TheTextFile << "\n";
        TheTextFile << "Processing scripts/digidump.C++\n";
            
        // No initial lines are needed in the output-file here.
    }
    else if (FileOption=="PythonText")
    {
        // No initial lines or what so-ever in both files.
    }
    else
    {
        // default:
        TheTextFile << "DNN input/output file:\n";
        TheTextFile << "#events = " << NeventsInFile << " | #input neurons = " << N_inputs << " | #output neurons = " << N_outputs << "\n";
        TheTextFile << "\n\n";
            
        // Separate output file:
        OutTextFile << "This separate Output-file is not used in the Default-format.\n";
    }
            
    // Done. do not close anything just yet!
}
