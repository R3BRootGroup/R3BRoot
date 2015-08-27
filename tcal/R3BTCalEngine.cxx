
#include <string>

#include "TH1F.h"
#include "TF1.h"

#include "FairLogger.h"

#include "R3BTCalPar.h"
#include "R3BTCalEngine.h"

R3BTCalEngine::R3BTCalEngine(R3BTCalPar* param, Int_t nModules, Int_t minStats)
    : fMinStats(minStats)
    , fNModules(nModules)
    , fhData(new TH1F*[nModules])
    , fhData100(new TH1F*[nModules])
    , fhTime(new TH1F*[nModules])
    , f1(new TF1("f1", "[0]", 1500., 2500.))
    , fCal_Par(param)
    , iMin(0)
    , iMax(0)
    , fClockFreq(0.)
{
    char strName[255];
    for (Int_t i = 0; i < fNModules; i++)
    {
        sprintf(strName, "%s_tcaldata_%d", fCal_Par->GetName(), i);
        fhData[i] = new TH1F(strName, "", 4096, 0.5, 4096.5);
        sprintf(strName, "%s_time_%d", fCal_Par->GetName(), i);
        fhTime[i] = new TH1F(strName, "", 4096, 0.5, 4096.5);
    }
}

R3BTCalEngine::~R3BTCalEngine()
{
    for (Int_t i = 0; i < fNModules; i++)
    {
        if(fhData[i])
        {
            delete fhData[i];
        }
        if(fhTime[i])
        {
            delete fhTime[i];
        }
    }
    delete fhData;
    delete fhTime;
    delete fhData100;
    delete f1;
}

void R3BTCalEngine::Fill(Int_t iModule, Int_t tdc)
{
    if (iModule < fNModules && iModule >= 0)
    {
        fhData[iModule]->Fill(tdc);
    }
}

void R3BTCalEngine::CalculateParamTacquila()
{
    fClockFreq = 1. / TACQUILA_CLOCK_MHZ * 1000.;
    
    for (Int_t iModule = 0; iModule < fNModules; iModule++)
    {
        if(fhData[iModule]->GetEntries() < fMinStats)
        {
            continue;
        }
            
        // Define range of channels
        fhData100[iModule] = (TH1F*)fhData[iModule]->Clone();
        fhData100[iModule]->Rebin(8);
        fhData100[iModule]->Fit(f1, "QNR");
        for (Int_t i = 256; i >= 0; i--)
        {
            if (fhData100[iModule]->GetBinContent(i + 1) < 0.1 * f1->GetParameter(0))
            {
                iMin = i - 1;
                break;
            }
        }
        for (Int_t j = 256; j < 512; j++)
        {
            if (fhData100[iModule]->GetBinContent(j + 1) < 0.1 * f1->GetParameter(0))
            {
                iMax = j + 1;
                break;
            }
        }
        if (iMax <= iMin)
        {
            LOG(ERROR) << "Error in definition of TCAL range" << FairLogger::endl;
            return;
        }
        iMin = Int_t(((Double_t)iMin - 0.5) * 8);
        iMax = Int_t(((Double_t)iMax + 0.5) * 8);
        if (iMin < 0 || iMax > 4095)
        {
            return;
        }
        LOG(INFO) << "R3BTCalEngine::CalculateParamTacquila() : Range of channels: " << iMin << " - " << iMax
                  << FairLogger::endl;

        Int_t nch = 0;
        Int_t ibin = iMin;
        Int_t group;
        Double_t prev_time = 0.;

        R3BTCalModulePar* pTCal = NULL;
        pTCal = new R3BTCalModulePar();
        pTCal->SetModuleId(iModule);

        Int_t incr = 0;
        while (ibin <= iMax)
        {
            // Iteratively compute parameter
            group = CalculateBinTacquila(iModule, prev_time, ibin, 1);
            // Fill time calibration parameter
            for (Int_t i1 = ibin; i1 < (ibin + group); i1++)
            {
                fhTime[iModule]->SetBinContent(i1 + 1, prev_time);
            }

            LOG(DEBUG) << " Module: " << iModule << " bin range: " << ibin << " : " << ibin + group
                       << " dbin: " << group << " time set: " << prev_time << FairLogger::endl;

            if (pTCal)
            {
                pTCal->SetBinLowAt(ibin, incr);
                pTCal->SetBinUpAt(ibin + group - 1, incr);
                pTCal->SetTimeAt(prev_time, incr);
                pTCal->IncrementNofChannels();
            }

            // Next range of channels
            ibin += group;
            nch += 1;
            incr++;
        }

        fCal_Par->AddModulePar(pTCal);

        LOG(INFO) << "R3BTCalEngine::CalculateParamTacquila() : Number of parameters: " << nch << FairLogger::endl;
        
        fhData[iModule]->Write();
        fhTime[iModule]->Write();
        
        LOG(INFO) << "R3BTCalEngine::CalculateParamTacquila() : Module: " << iModule << " is calibrated." << FairLogger::endl << FairLogger::endl;
       
    }
    
    fCal_Par->setChanged();
}

Int_t R3BTCalEngine::CalculateBinTacquila(Int_t iModule, Double_t& prev_time, Int_t ibin, Int_t ngroup)
{
    if ((ibin + ngroup) > iMax)
    {
        Double_t total = fhData[iModule]->Integral(1, 4096);
        Double_t itot = fhData[iModule]->Integral(1, (ibin + 1) + ngroup);
        if (itot > 0. && total > 0.)
        {
            Double_t time = fClockFreq * itot / total; // time of channel in [ns]
            LOG(DEBUG) << "R3BTCalEngine::CalculateBin() : bin=" << ibin << "  time=" << time << "  ngroup=" << ngroup
                       << FairLogger::endl;
            prev_time = time;
        }
        return ngroup;
    }
    Double_t total = fhData[iModule]->Integral(1, 4096);
    Double_t itot = fhData[iModule]->Integral(1, (ibin + 1) + ngroup);
    if (itot > 0. && total > 0.)
    {
        Double_t time = fClockFreq * itot / total; // time of channel in [ns]
        Double_t diff = time - prev_time;        // time difference to previous range
        if (diff * 1e3 < 70.)                    // check if below resolution
        {
            // Next iteration
            return CalculateBinTacquila(iModule, prev_time, ibin, ngroup + 1);
        }
        else
        {
            // Finalize
            LOG(DEBUG) << "R3BTCalEngine::CalculateBin() : bin=" << ibin << "  time=" << time << "  ngroup=" << ngroup
                       << FairLogger::endl;
            prev_time = time;
            return ngroup;
        }
    }
    return ngroup;
}

ClassImp(R3BTCalEngine)
