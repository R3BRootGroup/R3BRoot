void R3BTextFileGenerator::CreateFile()
{
    // This function recreates the textfile and switches to
    // the correct format.

    // Begin by testing if the file can actually be created:
    FileTest = kFALSE;
    std::ofstream TestFile (FileName.Data(), std::ofstream::out);
    TestFile.close();
    
    ifstream OpenFile;
    OpenFile.open(FileName.Data());
    if (OpenFile.is_open())
    {
        // then, we are OK:
        OpenFile.close();
        FileTest = kTRUE;
    }
    else
    {
        ErrorMessage("The DNN output file could not be created!");
    }
    
    // Next, test that we indeed have inputs & outputs:
    if (Nevents<1) {FileTest = kFALSE; ErrorMessage("You tried to create a file with less than 1 event!");}
    if (N_inputs<1) {FileTest = kFALSE; ErrorMessage("You tried to create a file with less than 1 input!");}
    if (N_outputs<1) {FileTest = kFALSE; ErrorMessage("You tried to create a file with less than 1 output!");}
    
    // Next, create the real file:
    if (FileTest = kTRUE)
    {
        // Then, it is safe to generate the file. However, we must first set the counters to their proper
        // values depending on the multithreading.
        
        // We always start with a new batch the first time, so do:
        BatchCounter = 0;
        
        // Calculate the number of events per thread. This is already Nevents.
        // It is divided by #threads in the macro & then passed along and it is
        // multiplied by #Digiruns in initialize() of this class. So it already
        // is the right number for the initialization of the eventcounters:
        
        // So now initialize:
        EventCounter = CurrentThread*Nevents;
        FillCounter = CurrentThread*Nevents;
        
        // and adapt in the event of 'step 2':
        if (Step2_IsSelected==kTRUE)
        {
            EventCounter = EventCounter*(2.0*((Int_t) Select_Determined_Multiplicity)+1.0);
            FillCounter = FillCounter*(2.0*((Int_t) Select_Determined_Multiplicity)+1.0);
            // NOTE: Otherwise, the first thread may overwrite in the end, the 
            // results of the second thread.
        }
        
        TString EventTag = "";
        GenerateOneFile(EventTag);
            
        // Done.
    }
}
