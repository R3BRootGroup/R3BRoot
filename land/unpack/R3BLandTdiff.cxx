// -----------------------------------------------------------------------------
// -----                              R3BLandTdiff                         -----
// -----                     Created 07-05-2014 by D.Kresan                -----
// -----------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TMath.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "R3BLandPmt.h"
#include "R3BLandDigi.h"
#include "R3BLandTdiff.h"

Double_t wlk(Double_t x)
{
    Double_t y=0;
    
    Double_t par1=1500.; // +-0.2238
    Double_t par2=0.00075;//+-2.355e-05
    y=par1*TMath::Power(x,par2)-(par1*TMath::Power(400.,par2)); // Michael's
    
    //y=2.29083*log(x)-0.0870157*log(x)*log(x)-4.57824;  // mine
    
    return y;
    //return 0.;
}

R3BLandTdiff::R3BLandTdiff()
    : fLandPmt(NULL)
    , fLandDigi(new TClonesArray("R3BLandDigi"))
    , fNDigi(0)
{
}

R3BLandTdiff::R3BLandTdiff(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fLandPmt(NULL)
    , fLandDigi(new TClonesArray("R3BLandDigi"))
    , fNDigi(0)
{
}

R3BLandTdiff::~R3BLandTdiff()
{
}

InitStatus R3BLandTdiff::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    if (!fMan)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    }

    fLandPmt = (TClonesArray*)fMan->GetObject("LandPmt");
    if (!fLandPmt)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "LandPmt not found");
    }
    
    fMan->Register("LandDigi", "Land", fLandDigi, kTRUE);

    ReadParameters();

    return kSUCCESS;
}

void R3BLandTdiff::Exec(Option_t* option)
{
    Int_t nLandPmt = fLandPmt->GetEntriesFast();
    if(200 == nLandPmt)
    {
        return;
    }
    R3BLandPmt* pmt1;
    R3BLandPmt* pmt2;
    Int_t barId;
    Int_t plane;
    Double_t tdiff;
    Double_t veff;
    Double_t tsync;
    Double_t tdcL, tdcR, tdc;
    Double_t qdcL, qdcR, qdc;
    Double_t x, y, z;
    for (Int_t i1 = 0; i1 < nLandPmt; i1++)
    {
        pmt1 = (R3BLandPmt*)fLandPmt->At(i1);
        if (1 != pmt1->GetSide())
        {
            continue;
        }
        barId = pmt1->GetBarId();
        if(! fMapIsSet[barId])
        {
            continue;
        }

        for (Int_t i2 = 0; i2 < nLandPmt; i2++)
        {
            if (i1 == i2)
            {
                continue;
            }
            pmt2 = (R3BLandPmt*)fLandPmt->At(i2);
            if (2 != pmt2->GetSide())
            {
                continue;
            }
            if (pmt2->GetBarId() != barId)
            {
                continue;
            }
            
            qdcL = pmt1->GetQdc();
            qdcR = pmt2->GetQdc();
            tdiff = fMapTdiff[barId];
            veff = fMapVeff[barId];
            tsync = fMapTsync[barId];
            tdcL = pmt1->GetTime() - tdiff/2. - tsync + wlk(qdcL);
            tdcR = pmt2->GetTime() + tdiff/2. - tsync + wlk(qdcR);
            tdc = (tdcL + tdcR) / 2.;
            qdc = TMath::Sqrt(qdcL * qdcR);
            plane = (Int_t)((barId-1)/50) + 1;
            if(0 == plane % 2)
            {
                x = veff * (tdcL - tdcR);
                y = (barId - 0.5 - (plane-1)*50) * 5. - 125.;
            }
            else
            {
                x = (barId - 0.5 - (plane-1)*50) * 5. - 125.;
                y = veff * (tdcL - tdcR);
            }
            z = (plane - 0.5) * 5. + 870.;

            new ((*fLandDigi)[fNDigi]) R3BLandDigi(barId, tdcL, tdcR, tdc, qdcL, qdcR, qdc, x, y, z);
            fNDigi += 1;
        }
    }
}

void R3BLandTdiff::FinishEvent()
{
    fLandDigi->Clear();
    fNDigi = 0;
}

void R3BLandTdiff::ReadParameters()
{
    fInFile = new ifstream(fParName);
    Int_t barId;
    Double_t tdiff;
    Double_t veff;
    while (! fInFile->eof())
    {
        (*fInFile) >> barId >> tdiff >> veff;
        fMapIsSet[barId] = kTRUE;
        fMapTdiff[barId] = tdiff;
        fMapVeff[barId] = veff;
    }
    fInFile->close();
    
    ifstream ifile("neuland_sync_159.txt");
    char str1[20], str2[20], str3[20], str4[20];
    ifile >> str1 >> str2 >> str3 >> str4;
    Double_t tsync;
    while(!ifile.eof())
    {
        ifile >> barId >> tdiff >> tsync >> veff;
        if(barId > 0 && barId <= 100)
        {
            fMapTsync[barId] = tsync;
        }
    }
    ifile.close();
}

ClassImp(R3BLandTdiff)
