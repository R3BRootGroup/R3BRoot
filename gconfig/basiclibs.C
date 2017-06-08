// Macro for loading basic libraries used with both Geant3 and Geant4

void basiclibs()
{
    cout << "[basiclibs]: Loading basic libraries ..." << endl;
    gSystem->Load("libRIO");
    gSystem->Load("libGeom");
    gSystem->Load("libGeomPainter");
    gSystem->Load("libVMC");
    gSystem->Load("libEG");
    gSystem->Load("libEGPythia6");
    gSystem->Load("libPythia6");
    gSystem->Load("libPhysics");
    gSystem->Load("libNet");
    gSystem->Load("libTree");
    gSystem->Load("libMinuit");
    gSystem->Load("libProof");
    gSystem->Load("libProofPlayer");
    gSystem->Load("libFairDB");
    gSystem->Load("libFairTools");
}
