#include "macros/r3b/r3ball.C"

void Step1_Simulate(
    const Int_t   nEvents,
    const TString basePath,
    const TString baseName,
    const TString inputFile,
    const TString geoTag
)
{
    // System paths
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH",   workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // Output files
    const TString outFile = basePath + "/" + baseName + ".sim.root";
    const TString parFile = basePath + "/" + baseName + ".par.root";

    // Constant configuration
    const TString  target = "LiH";
    const TString  fMC = "TGeant3";
    const TString  fGene = "ascii";
    const Bool_t   fEventDisplay = kTRUE;
    const Bool_t   fUserPList = kTRUE;
    const Bool_t   fR3BMagnet = kTRUE;
    const Double_t fMeasCurrent = 2500.;

    // Geometry
    TMap geometry;
    geometry.Add(new TObjString("TARGET"),      new TObjString("target_" + target + ".geo.root"));
    geometry.Add(new TObjString("ALADIN"),      new TObjString("aladin_v13a.geo.root"));
    geometry.Add(new TObjString("CRYSTALBALL"), new TObjString("cal_v13a.geo.root"));
    geometry.Add(new TObjString("DCH"),         new TObjString("dch_v13a.geo.root"));
    geometry.Add(new TObjString("NEULAND"),     new TObjString("neuland_" + geoTag + ".geo.root"));

    // Main Sim function call
    r3ball(nEvents, &geometry, target, fEventDisplay, fMC, fGene, fUserPList, fR3BMagnet, fMeasCurrent, outFile, parFile, inputFile);
}
