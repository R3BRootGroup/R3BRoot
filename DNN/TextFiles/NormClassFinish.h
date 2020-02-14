void R3BTextFileGenerator::NormClassFinish()
{
    // We need to write our learned parameters to a file:
    LearnAdvancedNorms->WriteNormFile(FileName);
    
    // Also write the used parameters to a file:
    TString UsedFileName = FileName;
    UsedFileName.ReplaceAll(".txt","_Used.txt");
    TheNormalization->WriteNormFile(UsedFileName);
    
    // Save control histograms:
    Int_t VectorSize = TheNormalization->GetNparameters();
    NormUnit* ThisUnit;
    TString TheOutputPath = Inputs->GetInputString("TheOutputPath");
    TString HistFileName = TheOutputPath + "/Normalization_ControlHistograms.root";
    TFile* f = new TFile(HistFileName.Data(),"recreate");
    f->cd();
    
    for (Int_t k = 0; k<VectorSize; ++k)
    {
        ThisUnit = TheNormalization->GetNormPar(k);
        if (((ThisUnit->ControlHistogram)!=0)&&((ThisUnit->ControlHistogram)!=nullptr))
        {
            (ThisUnit->ControlHistogram)->Write((ThisUnit->Name).Data(),2);
        }
    }
    
    f->Close();
}
