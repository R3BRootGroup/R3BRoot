// -----------------------------------------------------------------------------
// -----                            R3BLandTdiffFill                       -----
// -----                     Created 02-05-2014 by D.Kresan                -----
// -----------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "R3BLandPmt.h"
#include "R3BLandTdiffFill.h"

#include <map>

R3BLandTdiffFill::R3BLandTdiffFill()
    : fnEvents(0)
    , fNofBars(0)
    , fLandPmt(NULL)
    , fh_tdiff(NULL)
{
}

R3BLandTdiffFill::R3BLandTdiffFill(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fnEvents(0)
    , fNofBars(0)
    , fLandPmt(NULL)
    , fh_tdiff(NULL)
{
}

R3BLandTdiffFill::~R3BLandTdiffFill()
{
    if (fh_tdiff)
    {
        for (Int_t i = 0; i < fNofBars; i++)
        {
            delete fh_tdiff[i];
        }
        delete fh_tdiff;
        fNofBars = 0;
    }
}

InitStatus R3BLandTdiffFill::Init()
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

    CreateHistos();

    fOutFile = new ofstream(fParName);

    return kSUCCESS;
}

void R3BLandTdiffFill::Exec(Option_t* option)
{
    Int_t nLandPmt = fLandPmt->GetEntriesFast();
    R3BLandPmt* pmt1;
    R3BLandPmt* pmt2;
    Int_t barId;
    for (Int_t i1 = 0; i1 < nLandPmt; i1++)
    {
        pmt1 = (R3BLandPmt*)fLandPmt->At(i1);
        if (1 != pmt1->GetSide())
        {
            continue;
        }
        barId = pmt1->GetBarId();
        if (barId < 1 || barId > fNofBars)
        {
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Wrong bar ID %d", barId);
        }

        for (Int_t i2 = 0; i2 < nLandPmt; i2++)
        {
            if(i1 == i2)
            {
                continue;
            }
            pmt2 = (R3BLandPmt*)fLandPmt->At(i2);
            if (pmt2->GetBarId() != barId)
            {
                continue;
            }
            if (2 != pmt2->GetSide())
            {
                continue;
            }
            fh_tdiff[barId - 1]->Fill(pmt1->GetTime() - pmt2->GetTime());
            fh_time->Fill(pmt1->GetTime(), pmt2->GetTime());
        }
    }

    fnEvents += 1;
    if (0 == (fnEvents % 1000))
    {
        LOG(INFO) << "\033[0;31mR3BLandTdiffFill : " << fnEvents << " events collected\033[0;30m" << FairLogger::endl;
    }
}

void R3BLandTdiffFill::FinishTask()
{
    Double_t tdiff;
    Double_t tmax;
    Double_t tmin;
    Double_t xmax = 270.; // cm
    Double_t veff;
    Int_t l = 0;
    for (Int_t ibar = 0; ibar < 100; ibar++)
    {
        if (fh_tdiff[ibar]->GetEntries() > 1000)
        {
            for (Int_t i = 0; i < 1000; i++)
            {
                if ((fh_tdiff[ibar]->GetBinContent(i + 1 + 1) - fh_tdiff[ibar]->GetBinContent(i + 1)) > 0.1*fh_tdiff[ibar]->GetMaximum())
                {
                    for (Int_t j = 999; j >= (i+1); j--)
                    {
                        if ((fh_tdiff[ibar]->GetBinContent(j + 1) - fh_tdiff[ibar]->GetBinContent(j + 1 + 1)) > 0.1*fh_tdiff[ibar]->GetMaximum())
                        {
                            l = j - 1 - i - 1;
                            break;
                        }
                    }
                    if (l > 50)
                    {
                        tmin = (i + 1) * 0.2 - 100.;
                        tmax = (i + l - 1) * 0.2 - 100.;
                        break;
                    }
                }
            }
            
            tdiff = (tmax + tmin) / 2.;
            veff = xmax / (tmax - tmin);
            
            fh_tdiff_res->SetBinContent(ibar + 1, tdiff);
            fh_veff_res->SetBinContent(ibar + 1, veff);
            LOG(INFO) << "R3BLandTdiffFill::FinishTask : bar=" << (ibar + 1) << " tmin=" << tmin << " tmax=" << tmax << " tdiff=" << tdiff << " veff=" << veff
                      << FairLogger::endl;

            (*fOutFile) << (ibar + 1) << " " << tdiff << " " << veff << std::endl;
        }
    }

    WriteHistos();

    fOutFile->close();
}

void R3BLandTdiffFill::CreateHistos()
{
    fh_time = new TH2F("h_time", "Time", 2000, 0., 2000., 2000, 0., 2000.);
    if (fNofBars < 1)
    {
        return;
    }
    fh_tdiff = new TH1F* [fNofBars];
    char str[100];
    for (Int_t i = 0; i < fNofBars; i++)
    {
        sprintf(str, "h_tdiff_%d", (i + 1));
        fh_tdiff[i] = new TH1F(str, "(Time2 - Time1)/2", 1000, -100., 100.);
    }

    fh_tdiff_res = new TH1F("h_tdiff_res", "Tdiff", fNofBars, 0.5, fNofBars + 0.5);
    fh_toff_res = new TH1F("h_toff_res", "Toff", fNofBars, 0.5, fNofBars + 0.5);
    fh_veff_res = new TH1F("h_veff_res", "Veff", fNofBars, 0.5, fNofBars + 0.5);
}

void R3BLandTdiffFill::WriteHistos()
{
    fh_time->Write();
    
    if (fh_tdiff)
    {
        for (Int_t i = 0; i < fNofBars; i++)
        {
            fh_tdiff[i]->Write();
        }
    }

    fh_tdiff_res->Write();
    fh_toff_res->Write();
    fh_veff_res->Write();
}

ClassImp(R3BLandTdiffFill)
