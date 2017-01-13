#include "./helper_neuland_geometry.C"

void create_neuland_testgeo()
{
    new FairGeoLoader("TGeo", "FairGeoLoader");
    gGeoManager->SetName("NEULANDgeom");

    TGeoVolume *volPaddle = BuildPaddleVolume();

    TGeoVolume *volNeuland = new TGeoVolumeAssembly("volNeuland");
    volNeuland->AddNode(volPaddle, 1, new TGeoTranslation(0, 0, 0));

    TGeoVolume *top = new TGeoVolumeAssembly("TOP");
    gGeoManager->SetTopVolume(top);
    top->AddNode(volNeuland, 1, new TGeoTranslation(0., 0., 5.));

    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();
    gGeoManager->Test();

    TString geoFileName = TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/neuland_test.geo.root";
    TFile *geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();

    gStyle->SetCanvasPreferGL(kTRUE);
    gGeoManager->GetVolume("volNeuland")->Draw("ogl");
    TGLViewer *v = (TGLViewer *)gPad->GetViewer3D();
    v->CurrentCamera().RotateRad(0, 0.1);
    v->SetStyle(TGLRnrCtx::kOutline);
    v->RequestDraw();
    v->SavePicture("neuland_test.geo.png");
}
