void CrossSections_7LiPN::AnalyzeValidation()
{
    // Performs an analysis of the simulation validation that
    // is meant to obtain the efficiencies:
    SelectValidation();
    ComputeMCAreas();
    LoadHistograms();
    FitHistograms();
    ComputeEfficiencies();
    ComputeTheCrossSection();
}

void CrossSections_7LiPN::AnalyzeExperiment()
{
    // Performs an analysis of the experimental data, using
    // the efficiencies obtained from the validation 
    // NOTE: This file can be manually edited in the mean time
    // to include systematic errors.
    SelectExperiment();
    LoadHistograms();
    FitHistograms();
    ReadEfficiencies();
    ComputeTheCrossSection();
}
