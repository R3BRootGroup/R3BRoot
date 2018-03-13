#include "./helper_neuland_geometry.C"

void create_neuland_v3_geo(const Int_t nDP, const Bool_t visualize = false)
{
    const TString geoFileName =
        TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/neuland_v3_" + TString::Itoa(nDP, 10) + "dp.geo.root";

    new FairGeoLoader("TGeo", "FairGeoLoader");

    auto volPaddle = BuildPaddleVolume(); // Will make sure gGeoManager is loaded

    auto volNeuland = new TGeoVolumeAssembly("volNeuland");

    const Int_t nPlanes = nDP * 2;
    Int_t nindex = 0, nPlane = 0;
    for (Double_t b = -nPlanes * gPaddleDistance + gPaddleDistance; b < nPlanes * gPaddleDistance;
         b += gPaddleDistance * 2)
    {
        nPlane++;
        for (Double_t a = -gPaddlesPerPlane * gPaddleDistance + gPaddleDistance; a < gPaddleDistance * gPaddlesPerPlane;
             a += gPaddleDistance * 2)
        {
            nindex++;
            if (nPlane % 2 == 1)
            {
                volNeuland->AddNode(volPaddle, nindex, new TGeoTranslation(0, a, b));
            }
            else
            {
                volNeuland->AddNode(volPaddle, nindex, new TGeoCombiTrans({ a, 0, b }, { "", 0., 0., 90. }));
            }
        }
    }

    auto top = new TGeoVolumeAssembly("TOP");
    gGeoManager->SetTopVolume(top);

    // The new TGeoTranslation is necessary because ROOT
    top->AddNode(volNeuland, 1, new TGeoTranslation());

    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();
    // gGeoManager->Test();

    TFile geoFile(geoFileName, "RECREATE");
    top->Write();
    geoFile.Close();

    cout << "Done. " << nindex << " Paddles in " << nPlane << " Planes = " << nDP << " dp -> " << geoFileName << endl;

    if (visualize)
    {
        gStyle->SetCanvasPreferGL(kTRUE);
        volNeuland->Draw("ogl");
        auto v = (TGLViewer*)gPad->GetViewer3D();
        v->CurrentCamera().RotateRad(0, 0.1);
        v->SetStyle(TGLRnrCtx::kOutline);
        v->RequestDraw();
        v->SavePicture("neuland_v3_" + TString::Itoa(nDP, 10) + "dp.png");
        delete v;
        gPad->Close();
    }
}
