
{
    // Name of parameter file
    TString fileName = "/Users/kresan/data/s438b/data/params_run267_raw.root";
    // Name of container to edit
    TString contName = "LandTCalPar";
    // Run ID
    Int_t runId = 267;

    // Open the file and get the container
    TFile* file = new TFile(fileName, "UPDATE");
    R3BTCalPar* par = (R3BTCalPar*)file->Get(contName);

    // Set parameter values manually
    Bool_t result = kTRUE;
    result = (par->SetModuleParValue(2, 21, 1, 2000, 987654321) && result);
    result = (par->SetModuleParValue(2, 21, 2, 2000, 987654321) && result);

    // If successful - save to output
    if (result)
    {
        par->SavePar(Form("%d", runId));
    }
}
