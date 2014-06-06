// -----------------------------------------------------------------------------
// -----                               R3BLandAna                          -----
// -----                     Created 22-04-2014 by D.Kresan                -----
// -----------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "R3BLandDigi.h"
#include "R3BLosHit.h"
#include "R3BLandAna.h"

R3BLandAna::R3BLandAna()
    : fnEvents(0)
    , fLandDigi(NULL)
    , fLosHit(NULL)
{
}

R3BLandAna::R3BLandAna(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fnEvents(0)
    , fLandDigi(NULL)
    , fLosHit(NULL)
{
}

R3BLandAna::~R3BLandAna()
{
}

InitStatus R3BLandAna::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    if(! fMan)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    }
    fLandDigi = (TClonesArray*)fMan->GetObject("LandDigi");
    fLosHit = (TClonesArray*)fMan->GetObject("LosHit");
    CreateHistos();

    return kSUCCESS;
}

void R3BLandAna::Exec(Option_t* option)
{
    Bool_t startSeen = kTRUE;
    Double_t startTime = 0.;
    if (fLosHit)
    {
        Int_t nLosHit = fLosHit->GetEntriesFast();
        Double_t stime[4];
        R3BLosHit* losHit;
        Int_t ch;
        Bool_t seen[] = { kFALSE, kFALSE, kFALSE, kFALSE };
        for (Int_t i = 0; i < nLosHit; i++)
        {
            losHit = (R3BLosHit*)fLosHit->At(i);
            ch = losHit->GetChannel();
            if (ch >= 4)
            {
                continue;
            }
            if (seen[ch])
            {
//                LOG(ERROR) << "R3BLandAna::Exec : LOS channel " << ch << " seen > once" << FairLogger::endl;
//                LOG(INFO) << "ch=" << ch << "  " << losHit->GetTime() << " vs " << stime[ch] << FairLogger::endl;
//                stime[ch] = TMath::Min(stime[ch], losHit->GetTime());
            }
            else
            {
                seen[ch] = kTRUE;
                stime[ch] = losHit->GetTime();
            }
        }

        for (Int_t i = 0; i < 4; i++)
        {
            if (!seen[i])
            {
                startSeen = kFALSE;
                break;
            }
        }
        if(startSeen)
        {
            startTime = (stime[0] + stime[1] + stime[2] + stime[3]) / 4.;
            fh_los_time->Fill(startTime);
        }
    }

    Int_t count = 0;
    Double_t tmean = 0.;
    
    if (fLandDigi)
    {
        Int_t nLandDigi = fLandDigi->GetEntriesFast();
        R3BLandDigi* digi;
        Int_t barId;
        Double_t time, time2;
        Double_t qdc;
        Double_t x, y, z;
        for (Int_t i = 0; i < nLandDigi; i++)
        {
            digi = (R3BLandDigi*)fLandDigi->At(i);

            barId = digi->GetPaddleNr();
            time = digi->GetTdc();
            qdc = digi->GetQdc();
            x = digi->GetXX();
            y = digi->GetYY();
            z = digi->GetZZ();
            
            tmean += time;
            count += 1;

            fh_land_qdcbarid->Fill(barId, qdc);
            fh_land_yx->Fill(x, y);
            if(barId < 51)
            {
                fh_land_yx1->Fill(x, y);
            }
            else
            {
                fh_land_yx2->Fill(x, y);
            }

            if (startSeen)
            {
                fh_los_corr->Fill(time, startTime);
                
                fh_land_barid->Fill(barId);
                fh_land_timebarid->Fill(barId, time);
                time2 = time - startTime - 1592.;
                fh_land_tof->Fill(time2);
                fh_land_qdctof->Fill(time2, qdc);
                Double_t l = TMath::Sqrt((x+43.)*(x+43.) + y*y + z*z);
                Double_t beta = l / time2;
                fh_land_tofbarid->Fill(barId, time2);
                fh_land_lbarid->Fill(barId, l);
                fh_land_ltime->Fill(time2, l);
                fh_land_betabarid->Fill(barId, beta);

                fh_land_beta->Fill(beta);
                fh_land_qdc->Fill(qdc);
                if(beta > 16 && beta < 26)
                {
                    fh_land_qdc_cut->Fill(qdc);
                }
            }
        }
    }
    
    fnEvents += 1;
    if (0 == (fnEvents % 1000))
    {
        LOG(INFO) << "\033[0;31mR3BLandAna : " << fnEvents << " events collected\033[0;30m" << ",  start time=" << startTime;
        if(count)
        {
             LOG(INFO) << ",  mean time=" << (tmean/(Double_t)count);
        }
        LOG(INFO) << FairLogger::endl;
    }
}

void R3BLandAna::FinishTask()
{
    WriteHistos();
}

void R3BLandAna::CreateHistos()
{
    fh_land_barid = new TH1F("h_land_barid", "Bar ID", 100, 0.5, 100.5);
    fh_land_qdcbarid = new TH2F("h_land_qdcbarid", "QDC vs Bar ID", 100, 0.5, 100.5, 2000, 0., 2000.);
    fh_land_timebarid = new TH2F("h_land_timebarid", "Time vs Bar ID", 100, 0.5, 100.5, 4000, 0., 2000.);
    fh_land_tofbarid = new TH2F("h_land_tofbarid", "ToF vs Bar ID", 100, 0.5, 100.5, 1000, 0., 100.);
    fh_land_tof = new TH1F("h_land_tof", "ToF", 1000, -500., 500.);
    fh_land_qdctof = new TH2F("h_land_qdctof", "QDC vs ToF", 1000, 0., 100., 2000, 0., 2000.);
    fh_land_betabarid = new TH2F("h_land_betabarid", "Velocity vs Bar ID", 100, 0.5, 100.5, 500, 0., 50.);
    fh_land_lbarid = new TH2F("h_land_lbarid", "Length vs Bar ID", 100, 0.5, 100.5, 1000, 700., 900.);
    fh_land_ltime = new TH2F("h_land_ltime", "Length vs Bar ID", 4000, 0., 2000., 1000, 700., 900.);

    fh_land_yx = new TH2F("h_land_yx", "Y vs X", 340, -170., 170., 340, -170., 170.);
    fh_land_yx1 = new TH2F("h_land_yx1", "Y vs X", 68, -170., 170., 340, -170., 170.);
    fh_land_yx2 = new TH2F("h_land_yx2", "Y vs X", 340, -170., 170., 68, -170., 170.);

    fh_land_beta = new TH1F("h_land_beta", "Velocity", 500, 0., 50.);
    fh_land_qdc = new TH1F("h_land_qdc", "QDC", 2000, 0., 2000.);
    fh_land_qdc_cut = new TH1F("h_land_qdc_cut", "QDC cut", 2000, 0., 2000.);

    fh_los_time = new TH1F("h_los_time", "LOS Time", 2000, -1000., -800.);
    
    fh_los_corr = new TH2F("h_los_corr", "LOS vs NeuLAND", 2000, 700., 900., 2000, -1000., -800.);
}

void R3BLandAna::WriteHistos()
{
    fh_land_barid->Write();
    fh_land_qdcbarid->Write();
    fh_land_tof->Write();
    fh_land_qdctof->Write();
    fh_land_timebarid->Write();
    fh_land_tofbarid->Write();
    fh_land_yx->Write();
    fh_land_yx1->Write();
    fh_land_yx2->Write();
    fh_land_lbarid->Write();
    fh_land_ltime->Write();
    fh_land_beta->Write();
    fh_land_qdc->Write();
    fh_land_qdc_cut->Write();
    fh_land_betabarid->Write();

    fh_los_time->Write();
    
    fh_los_corr->Write();
}

ClassImp(R3BLandAna)
