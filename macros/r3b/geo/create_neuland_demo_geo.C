#include "./helper_neuland_geometry.C"

void create_neuland_demo_geo(const Int_t nPlanes = 8, const Double_t distance = 1100., const TString geoTag = "demo_11m")
{
    new FairGeoLoader("TGeo", "FairGeoLoader");
    gGeoManager->SetName("NEULANDgeom");

    TGeoVolume *volPaddle = BuildPaddleVolume();
    TGeoRotation *rot90 = new TGeoRotation();
    rot90->RotateZ(90.);

    TGeoVolume *volNeuland = new TGeoVolumeAssembly("volNeuland");
    Int_t nindex = 0, nPlane = 0;
    for (Double_t b = -nPlanes * gPaddleDistance + gPaddleDistance; b < nPlanes * gPaddleDistance; b += gPaddleDistance * 2) {
        nPlane++;
        for (Double_t a = -gPaddlesPerPlane * gPaddleDistance + gPaddleDistance; a < gPaddleDistance * gPaddlesPerPlane; a += gPaddleDistance * 2) {
            nindex++;
            if (nPlane % 2 == 1) {
                volNeuland->AddNode(volPaddle, nindex, new TGeoTranslation(0, a, b));
            } else {
                volNeuland->AddNode(volPaddle, nindex, new TGeoCombiTrans(a, 0, b, rot90));
            }
        }
    }


    TGeoVolume *top = new TGeoVolumeAssembly("TOP");
    gGeoManager->SetTopVolume(top);
    top->AddNode(volNeuland, 1, new TGeoTranslation(0., 0., distance + nPlanes * gPaddleDistance));

    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();
    gGeoManager->Test();

    TString geoFileName = TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/neuland_" + geoTag + ".geo.root";
    TFile *geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();

    cout << "Done. " << nindex << " Paddles in " << nPlane << " Planes" << endl;

    gStyle->SetCanvasPreferGL(kTRUE);
    gGeoManager->GetVolume("volNeuland")->Draw("ogl");
    TGLViewer *v = (TGLViewer *)gPad->GetViewer3D();
    v->CurrentCamera().RotateRad(0, 0.1);
    v->SetStyle(TGLRnrCtx::kOutline);
    v->RequestDraw();
    v->SavePicture("neuland_" + geoTag + ".png");
}
