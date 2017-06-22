// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                      R3BLandRawAnaS438bS438b                      -----
// -----                    Created  27-02-2015 by D. Kresan               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BEventHeader.h"
#include "R3BNeulandMappedData.h"
#include "R3BCalifaMappedData.h"
#include "R3BLandRawAnaS438b.h"

R3BLandRawAnaS438b::R3BLandRawAnaS438b()
    : fnEvents(0)
    , fLandRawHitMapped(NULL)
    , fCalifaMappedData(NULL)
{
}

R3BLandRawAnaS438b::R3BLandRawAnaS438b(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fnEvents(0)
    , fLandRawHitMapped(NULL)
    , fCalifaMappedData(NULL)
{
}

R3BLandRawAnaS438b::~R3BLandRawAnaS438b()
{
}

InitStatus R3BLandRawAnaS438b::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fLandRawHitMapped = (TClonesArray*)fMan->GetObject("LandRawHitMapped");
    fCalifaMappedData = (TClonesArray*)fMan->GetObject("CalifaMappedData");
    CreateHistos();

    return kSUCCESS;
}

void R3BLandRawAnaS438b::Exec(Option_t* option)
{
    Int_t nCalo = 0;
    Int_t nCalo1 = 0;
    Double_t eCalo = 0.;
    R3BCalifaMappedData *caloHit;
    if (fCalifaMappedData)
    {
        nCalo = fCalifaMappedData->GetEntries();
        for(Int_t i = 0; i < nCalo; i++)
        {
            caloHit = (R3BCalifaMappedData*) fCalifaMappedData->At(i);
            eCalo += caloHit->GetEnergy();
            if(caloHit->GetEnergy() > 90.)
            {
                nCalo1 += 1;
            }
        }
    }
    
    Int_t nLand = 0;
    Int_t nLand1 = 0;
    Double_t eLand = 0.;
    Bool_t filled = kFALSE;
    if (fLandRawHitMapped)
    {
        Int_t nLandRawHitsMapped = fLandRawHitMapped->GetEntries();
        R3BNeulandMappedData* hitmapped;
        for (Int_t i = 0; i < nLandRawHitsMapped; i++)
        {
            hitmapped = (R3BNeulandMappedData*)fLandRawHitMapped->At(i);
            if(! hitmapped->Is17())
            {
                if(nCalo1 > 0 && eCalo > 60)
                {
                    fh_land_qdc->Fill(hitmapped->GetQdcData());
                    if(! filled)
                    {
                        fh_trigger_cut->Fill(fHeader->GetTrigger());
                        filled = kTRUE;
                    }
                }
                fh_land_qdc_ncalo->Fill(nCalo1, hitmapped->GetQdcData());
                fh_eland_ecalo->Fill(eCalo, hitmapped->GetQdcData());
                nLand += 1;
                eLand += hitmapped->GetQdcData();
                if(hitmapped->GetQdcData() > 200.)
                {
                    nLand1 += 1;
                }
            }
        }
    }
    
    fhNhits->Fill(nCalo1, nLand1);
    //fh_eland_ecalo->Fill(eCalo, eLand);

    fnEvents += 1;
    if(0 == (fnEvents%1000))
    {
        LOG(INFO) << "R3BLandRawAnaS438b : " << fnEvents << " events collected" << FairLogger::endl;
    }
}

void R3BLandRawAnaS438b::FinishTask()
{
}

void R3BLandRawAnaS438b::CreateHistos()
{
    fh_trigger_cut = new TH1F("h_trigger_cut", "Trigger", 10, -0.5, 9.5);
    fhNhits = new TH2F("hNhits2", "Number of hits : CALO vs LAND", 100, -0.5, 99.5, 100, -0.5, 99.5);
    fh_land_qdc = new TH1F("h_land_qdc_cut", "QDC data", 100, 0., 1000.);
    fh_land_qdc_ncalo = new TH2F("h_land_qdc_ncalo", "QDC data", 50, -0.5, 49.5, 100, 0., 1000.);
    fh_eland_ecalo = new TH2F("h_qdc_ecalo", "QDC data", 100, 0., 1000., 100, 0., 1000.);

    FairRunOnline* run = FairRunOnline::Instance();
    
    run->AddObject(fh_trigger_cut);
    run->AddObject(fhNhits);
    run->AddObject(fh_land_qdc);
    run->AddObject(fh_land_qdc_ncalo);
    run->AddObject(fh_eland_ecalo);
}

void R3BLandRawAnaS438b::WriteHistos()
{
}

ClassImp(R3BLandRawAnaS438b)
