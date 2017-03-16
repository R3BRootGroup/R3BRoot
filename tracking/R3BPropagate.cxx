
#include "R3BPropagate.h"
#include "R3BGladFieldMap.h"
#include "R3BMCTrack.h"
#include "R3BmTofPoint.h"
#include "R3BTPropagator.h"
#include "R3BTGeoPar.h"

#include "FairRootManager.h"
#include "FairLogger.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLine.h"
#include "TGeoTrack.h"
#include "TGraph.h"
#include "TStyle.h"

R3BPropagate::R3BPropagate(const char* name, Bool_t vis, Int_t verbose)
    : FairTask(name, verbose)
    , fFieldPar(NULL)
    , fPropagator(NULL)
    , fmTofGeo(NULL)
    , fArraymTofPoint(NULL)
    , fArrayMCTracks(NULL)
    , fh2(NULL)
    , fh_res_1(NULL)
    , Ni(0)
    , fNEvents(0)
    , fVis(vis)
{
}

R3BPropagate::~R3BPropagate() {}

InitStatus R3BPropagate::Init()
{
    FairRootManager* man = FairRootManager::Instance();

    fArraymTofPoint = (TClonesArray*)man->GetObject("mTOFPoint");
    if (NULL == fArraymTofPoint)
    {
        LOG(ERROR) << "No mTof Point array found in input file." << FairLogger::endl;
        return kERROR;
    }

    fArrayMCTracks = (TClonesArray*)man->GetObject("MCTrack");
    if (NULL == fArrayMCTracks)
    {
        LOG(ERROR) << "No MC Track array found in input file." << FairLogger::endl;
        return kERROR;
    }

    if (fVis)
    {
        fArrayGeoTracks = new TClonesArray("TGeoTrack");
        man->GetInTree()->SetBranchAddress("GeoTracks", &fArrayGeoTracks);
        if (NULL == fArrayGeoTracks)
        {
            LOG(WARNING) << "No Geo Track array found in input file." << FairLogger::endl;
        }
    }

    if (!InitPropagator())
    {
        return kERROR;
    }

    fh2 = new TH2F("h2", "", 40, -164.05, -160.05, 40, -2.05, 1.95);
    fh_res_1 = new TH1F("h_res_1", "", 1000, -0.5, 0.5);

    return kSUCCESS;
}

InitStatus R3BPropagate::ReInit()
{
    SetParContainers();

    if (!InitPropagator())
    {
        return kERROR;
    }

    return kSUCCESS;
}

void R3BPropagate::SetParContainers()
{
    fmTofGeo = (R3BTGeoPar*)FairRuntimeDb::instance()->getContainer("mTofGeoPar");
    fFieldPar = (R3BFieldPar*)FairRuntimeDb::instance()->getContainer("R3BFieldPar");
}

void R3BPropagate::Exec(const Option_t*)
{
    R3BMCTrack* ion;
    Int_t npoints;
    R3BmTofPoint* hit;
    Int_t trackId;
    TVector3 posin, momin;
    TVector3 pos1, mom1;
    Bool_t result;

    npoints = fArraymTofPoint->GetEntriesFast();
    for (Int_t ip = 0; ip < npoints; ip++)
    {
        // Get pointer to detector hit
        hit = (R3BmTofPoint*)fArraymTofPoint->At(ip);

        // Get pointer to track
        trackId = hit->GetTrackID();
        ion = (R3BMCTrack*)fArrayMCTracks->At(trackId);

        // Check if primary
        if (ion->GetMotherId() != -1)
        {
            continue;
        }

        fh2->Fill(hit->GetXIn(), hit->GetYIn());

        // Initial position and momentum
        posin.SetXYZ(ion->GetStartX(), ion->GetStartY(), ion->GetStartZ());
        momin.SetXYZ(ion->GetPx(), ion->GetPy(), ion->GetPz());

        // Propagate to plane of detector ==================================
        result = fPropagator->PropagateToDetector("mTOF", posin, momin, pos1, mom1);
        // =================================================================

        x1[Ni] = pos1.X();
        y1[Ni] = pos1.Y();
        Ni += 1;

        TVector3 poshit(TVector3(hit->GetXIn(), hit->GetYIn(), hit->GetZIn()));
        fh_res_1->Fill((pos1 - poshit).Perp());

//        LOG(INFO) << FairLogger::endl;
//        poshit.Print();
//        pos1.Print();
//        LOG(INFO) << FairLogger::endl;
//        TVector3(hit->GetPx(), hit->GetPy(), hit->GetPz()).Print();
//        mom1.Print();
//        LOG(INFO) << FairLogger::endl;
//        LOG(INFO) << TVector3(hit->GetPx(), hit->GetPy(), hit->GetPz()).Mag() << FairLogger::endl;
//        LOG(INFO) << mom1.Mag() << FairLogger::endl;
//        LOG(INFO) << FairLogger::endl;
    }

    if (0 == fNEvents && fVis && fArrayGeoTracks)
    {
        TGeoTrack* geotra = (TGeoTrack*)fArrayGeoTracks->At(0);
        Double_t xp1, yp1, zp1, tp1;
        Double_t xp2, yp2, zp2, tp2;
        for (Int_t i = 1; i < geotra->GetNpoints(); i++)
        {
            geotra->GetPoint(i - 1, xp1, yp1, zp1, tp1);
            geotra->GetPoint(i, xp2, yp2, zp2, tp2);
            TLine* l1 = new TLine(-xp1, zp1, -xp2, zp2);
            l1->SetLineColor(4);
            l1->Draw();
        }
    }

    if (0 == (fNEvents % 10))
    {
        LOG(INFO) << "Event: " << fNEvents << FairLogger::endl;
    }

    fNEvents += 1;
}

void R3BPropagate::Finish()
{
    if (fVis)
    {
        gStyle->SetPalette(0);
        /*TCanvas* c1 = */ new TCanvas("c1", "", 10, 10, 500, 500);
        fh2->Draw("colz");

        TGraph* gr = new TGraph(Ni, x1, y1);
        gr->SetMarkerStyle(24);
        gr->Draw("P");

        /*TCanvas* c2 = */ new TCanvas("c2", "", 500, 10, 500, 500);
        fh_res_1->Draw();
    }
}

Bool_t R3BPropagate::InitPropagator()
{
    FairField* fairField = FairRunAna::Instance()->GetField();
    R3BGladFieldMap* gladField = NULL;
    if (2 == fFieldPar->GetType())
    {
        gladField = (R3BGladFieldMap*)fairField;

        if (fPropagator)
        {
            delete fPropagator;
        }
        fPropagator = new R3BTPropagator(gladField, fVis);

        fmTofGeo->printParams();
        fPropagator->SetmTofGeo(fmTofGeo);
    }
    else
    {
        LOG(ERROR) << "Unsupported type of field." << FairLogger::endl;
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BPropagate)
