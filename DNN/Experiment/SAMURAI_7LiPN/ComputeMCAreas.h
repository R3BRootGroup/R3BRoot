void CrossSections_7LiPN::ComputeMCAreas()
{
    // Define proper branch names:
    TString TheCorrectBranchName = GetBranchTitle();
    TString TheMonteCarloBranchName = "NeutronTracks_PerfectMethod_Signals";
    
    // Switch to MC branch:
    SetBranchTitle(TheMonteCarloBranchName);
    
    // Then, compute peak areas:
    LoadHistograms();
    FitHistograms();
    
    // Next, transfer the peak areas:
    MonteCarlo_PeakAreas.resize(PeakAreas.size());
    MonteCarlo_PeakAreaErrors.resize(PeakAreaErrors.size());
    
    for (Int_t k = 0; k<PeakAreas.size(); ++k) {MonteCarlo_PeakAreas.at(k) = PeakAreas.at(k);}
    for (Int_t k = 0; k<PeakAreaErrors.size(); ++k) {MonteCarlo_PeakAreaErrors.at(k) = PeakAreaErrors.at(k);}
    
    // Then, switch back to the original branch:
    SetBranchTitle(TheCorrectBranchName);
    
    // And empoty the vectors again:
    ResetHistogramVectors();
    
    // Done.
}
