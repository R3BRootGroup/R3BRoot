void R3BNeulandCADDigitizer::SetDetector(TString const ThisDetector)
{
    // Decides which detector the digitizer is going to use:
    Detector = ThisDetector;
    
    if (ThisDetector=="NeuLAND")
    {
        // Then we use Jan Mayers standard NeuLAND version:
        MCpoint_BranchName = "R3BNeulandPoint";
        ParContainer_BranchName = "R3BNeulandGeoPar";
        fInput = "NeulandPoints";
        fOutput = "NeulandDigis";
        fOutputPx = "NeulandPixels";
    }
    else if (ThisDetector=="CAD_NeuLAND")
    {
        // Then we use C. A. Douma's NeuLAND copy:
        MCpoint_BranchName = "R3BCADNeulandPoint";
        ParContainer_BranchName = "R3BCADNeulandGeoPar";
        fInput = "NeulandPoints";
        fOutput = "NeulandDigis";
        fOutputPx = "NeulandPixels";
    }
    else if (ThisDetector=="CAD_VETO")
    {
        // Then we use C. A. Douma's NeuLAND copy:
        MCpoint_BranchName = "R3BCADVETOPoint";
        ParContainer_BranchName = "R3BCADVETOGeoPar";
        fInput = "VETOPoints";
        fOutput = "VETODigis";
        fOutputPx = "VETOPixels";
    }
    else if (ThisDetector=="CAD_NEBULA")
    {
        // Then we use C. A. Douma's NeuLAND copy:
        MCpoint_BranchName = "R3BCADNEBULAPoint";
        ParContainer_BranchName = "R3BCADNEBULAGeoPar";
        fInput = "NEBULAPoints";
        fOutput = "NEBULADigis";
        fOutputPx = "NEBULAPixels";
    }
    else if (ThisDetector=="CAD_NEBULA_VETO")
    {
        // Then we use C. A. Douma's NeuLAND copy:
        MCpoint_BranchName = "R3BCADNEBULAVETOPoint";
        ParContainer_BranchName = "R3BCADNEBULAVETOGeoPar";
        fInput = "NEBULAVETOPoints";
        fOutput = "NEBULAVETODigis";
        fOutputPx = "NEBULAVETOPixels";
    }
    else
    {
        // Then, we use the normal default setting,
        // which is Jan Mayers NeuLAND:
        MCpoint_BranchName = "R3BNeulandPoint";
        ParContainer_BranchName = "R3BNeulandGeoPar";
        fInput = "NeulandPoints";
        fOutput = "NeulandDigis";
        fOutputPx = "NeulandPixels";
    }
}
        
