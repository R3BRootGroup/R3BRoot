void R3BTextFileGenerator::FinishTextFile()
{
    // Finbish up the TextFile:
    TheTextFile.close();
    OutTextFile.close();
    
    // Finish up the normalization stuff too:
    NormClassFinish();
    
    // Write control histograms:
    TString OutputPath = Inputs->GetInputString("TheOutputPath");
    OutputPath = OutputPath + "/" + SubFolder + "/";
    TFile* f = new TFile(OutputPath + "/ControlHistograms.root","recreate");
    f->cd();
    TotalClusterScore_All->Write("TotalClusterScore_All"+SubFolder,2);
    TotalClusterScore_Prim->Write("TotalClusterScore_Prim"+SubFolder,2);
    f->Close();
}
