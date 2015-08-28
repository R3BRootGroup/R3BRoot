
#include <string>

#include "TH1F.h"
#include "TMath.h"

#include "FairLogger.h"

#include "R3BTCalPar.h"
#include "R3BTCalEngine.h"

R3BTCalEngine::R3BTCalEngine(R3BTCalPar* param, Int_t nModules, Int_t minStats)
    : fMinStats(minStats)
    , fNModules(nModules)
    , fhData(new TH1F*[nModules])
    , fhTime(new TH1F*[nModules])
    , fCal_Par(param)
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
        if (fhData[i])
        {
            delete fhData[i];
        }
        if (fhTime[i])
        {
            delete fhTime[i];
        }
    }
    delete fhData;
    delete fhTime;
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
        if (fhData[iModule]->GetEntries() < fMinStats)
        {
            continue;
        }

        // Define range of channels
        Int_t ic, iMin, iMax;
        FindRange(fhData[iModule], ic, iMin, iMax);
        if (iMin < 0 || iMax > 4095)
        {
            return;
        }
        LOG(INFO) << "R3BTCalEngine::CalculateParamTacquila() : Range of channels: " << iMin << " - " << iMax
                  << FairLogger::endl;
        
        Double_t total = fhData[iModule]->Integral(iMin, iMax);
        for(Int_t i = iMin; i <= iMax; i++)
        {
            fhTime[iModule]->SetBinContent(i, fhData[iModule]->Integral(iMin, i) / total * fClockFreq);
        }

        Int_t nparam = 0;

        R3BTCalModulePar* pTCal = NULL;
        pTCal = new R3BTCalModulePar();
        pTCal->SetModuleId(iModule);

        Int_t il = ic - 10 + 1;
        Int_t ih = ic;
        while (il > iMin)
        {
            Double_t slope = 0, offset = 0;
            LinearDown(fhData[iModule], iMin, iMax, il, ih, slope, offset);

            pTCal->SetBinLowAt(il, nparam);
            pTCal->SetBinUpAt(ih, nparam);
            pTCal->SetSlopeAt(slope, nparam);
            pTCal->SetOffsetAt(offset, nparam);
            pTCal->IncrementNofChannels();

            nparam += 1;

            ih = il;
            il = ih - 10 + 1;
        }

        if (ih > iMin)
        {
            Double_t slope = 0, offset = 0;
            Double_t tot = fhData[iModule]->Integral(iMin, iMax);
            Double_t t1 = 0.;
            Double_t t2 = fhData[iModule]->Integral(iMin, ih) / tot * fClockFreq;
            slope = (t2 - t1) / (Double_t)(ih - iMin);
            offset = t1;

            pTCal->SetBinLowAt(iMin, nparam);
            pTCal->SetBinUpAt(ih, nparam);
            pTCal->SetSlopeAt(slope, nparam);
            pTCal->SetOffsetAt(offset, nparam);
            pTCal->IncrementNofChannels();

            nparam += 1;
        }

        il = ic;
        ih = ic + 10 - 1;
        while (ih <= iMax)
        {
            Double_t slope = 0, offset = 0;
            LinearUp(fhData[iModule], iMin, iMax, il, ih, slope, offset);

            pTCal->SetBinLowAt(il, nparam);
            pTCal->SetBinUpAt(ih, nparam);
            pTCal->SetSlopeAt(slope, nparam);
            pTCal->SetOffsetAt(offset, nparam);
            pTCal->IncrementNofChannels();

            nparam += 1;

            il = ih;
            if ((iMax - ih) < 100)
            {
                ih = il + 5 - 1;
            }
            else
            {
                ih = il + 10 - 1;
            }
        }

        if (il < iMax)
        {
            Double_t slope = 0, offset = 0;
            Double_t tot = fhData[iModule]->Integral(iMin, iMax);
            Double_t t1 = fhData[iModule]->Integral(iMin, il) / tot * fClockFreq;
            Double_t t2 = fClockFreq;
            slope = (t2 - t1) / (Double_t)(iMax - il);
            offset = t1;

            pTCal->SetBinLowAt(il, nparam);
            pTCal->SetBinUpAt(iMax, nparam);
            pTCal->SetSlopeAt(slope, nparam);
            pTCal->SetOffsetAt(offset, nparam);
            pTCal->IncrementNofChannels();

            nparam += 1;
        }

        fCal_Par->AddModulePar(pTCal);

        LOG(INFO) << "R3BTCalEngine::CalculateParamTacquila() : Number of parameters: " << nparam << FairLogger::endl;

        fhData[iModule]->Write();
        fhTime[iModule]->Write();

        LOG(INFO) << "R3BTCalEngine::CalculateParamTacquila() : Module: " << iModule << " is calibrated."
                  << FairLogger::endl
                  << FairLogger::endl;
    }

    fCal_Par->setChanged();
}

void R3BTCalEngine::CalculateParamVFTX()
{
    fClockFreq = 1. / VFTX_CLOCK_MHZ * 1000.;
    
    for (Int_t iModule = 0; iModule < fNModules; iModule++)
    {
        if (fhData[iModule]->GetEntries() < fMinStats)
        {
            continue;
        }
        
        // Define range of channels
        Int_t ic, iMin, iMax;
        FindRange(fhData[iModule], ic, iMin, iMax);
        if (iMin < 0 || iMax > 4095)
        {
            return;
        }
        LOG(INFO) << "R3BTCalEngine::CalculateParamVFTX() : Range of channels: " << iMin << " - " << iMax
        << FairLogger::endl;
        
        Double_t total = fhData[iModule]->Integral(iMin, iMax);
        for(Int_t i = iMin; i <= iMax; i++)
        {
            fhTime[iModule]->SetBinContent(i, fhData[iModule]->Integral(iMin, i) / total * fClockFreq);
        }
        
        Int_t nparam = 0;
        
        R3BTCalModulePar* pTCal = NULL;
        pTCal = new R3BTCalModulePar();
        pTCal->SetModuleId(iModule);
        
        for(Int_t ibin = iMin; ibin <= iMax; ibin++)
        {
            Double_t time = fhData[iModule]->Integral(iMin, ibin) / total;
            if(time > 1.)
            {
                FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Integration error.");
            }
            time *= fClockFreq;
            
            pTCal->SetBinLowAt(ibin, nparam);
            pTCal->SetOffsetAt(time, nparam);
            pTCal->IncrementNofChannels();
            nparam += 1;
        }
        
        fCal_Par->AddModulePar(pTCal);
        
        LOG(INFO) << "R3BTCalEngine::CalculateParamVFTX() : Number of parameters: " << nparam << FairLogger::endl;

        fhData[iModule]->Write();
        fhTime[iModule]->Write();
        
        LOG(INFO) << "R3BTCalEngine::CalculateParamVFTX() : Module: " << iModule << " is calibrated."
        << FairLogger::endl
        << FairLogger::endl;
    }

    fCal_Par->setChanged();
}

void R3BTCalEngine::FindRange(TH1F* h1, Int_t& ic, Int_t& iMin, Int_t& iMax)
{
    Double_t mean = h1->GetMean();
    ic = (Int_t)(mean + 0.5);
    Double_t top = h1->Integral(ic - 4, ic + 5) / 10.;

    for (Int_t i = ic; i >= 1; i--)
    {
        if (h1->GetBinContent(i) < 0.1 * top)
        {
            if (h1->Integral(i - 9, i) / 10. < 0.1 * top)
            {
                iMin = i - 1;
                break;
            }
        }
    }

    for (Int_t i = ic + 1; i <= 4096; i++)
    {
        if (h1->GetBinContent(i) < 0.1 * top)
        {
            if (h1->Integral(i, i + 9) / 10. < 0.1 * top)
            {
                //iMax = i - 1;
                iMax = i;
                break;
            }
        }
    }
}

void R3BTCalEngine::LinearUp(TH1F* h1, Int_t iMin, Int_t iMax, Int_t& il, Int_t& ih, Double_t& slope, Double_t& offset)
{
    Double_t tot = h1->Integral(iMin, iMax);
    Double_t t1 = h1->Integral(iMin, il) / tot; // * fClockFreq;
    Double_t t2 = h1->Integral(iMin, ih) / tot; // * fClockFreq;
    if (t1 > 1. || t2 > 1.)
    {
        Fatal("LinearUp", "Integration error");
    }
    t1 *= fClockFreq;
    t2 *= fClockFreq;
    slope = (t2 - t1) / (Double_t)(ih - il);
    offset = t1;

    Double_t prec = 3. / TMath::Sqrt(h1->GetEntries());

    Double_t slope1;

    Int_t iter = 1;
    while (ih <= iMax)
    {
        Int_t ih_next = ih + iter;
        if (ih_next > iMax)
        {
            break;
        }
        Double_t t21 = h1->Integral(iMin, ih_next) / tot * fClockFreq;
        slope1 = (t21 - t1) / (Double_t)(ih_next - il);

        Double_t dev = TMath::Abs(slope1 - slope) / TMath::Abs(slope);

        ih = ih_next;
        iter += 1;

        slope = slope1;
        if (dev > prec)
        {
            break;
        }
    }
}

void R3BTCalEngine::LinearDown(TH1F* h1,
                               Int_t iMin,
                               Int_t iMax,
                               Int_t& il,
                               Int_t& ih,
                               Double_t& slope,
                               Double_t& offset)
{
    Double_t tot = h1->Integral(iMin, iMax);
    Double_t t1 = h1->Integral(iMin, il) / tot * fClockFreq;
    Double_t t2 = h1->Integral(iMin, ih) / tot * fClockFreq;
    slope = (t2 - t1) / (Double_t)(ih - il);
    offset = t1;

    Double_t prec = 3. / TMath::Sqrt(h1->GetEntries());

    Double_t slope1;
    Double_t offset1;

    Int_t iter = 1;
    while (il >= iMin)
    {
        Int_t il_next = il - iter, ih_next = ih;
        if (il_next < iMin)
        {
            break;
        }
        Double_t t11 = h1->Integral(iMin, il_next) / tot * fClockFreq;
        Double_t t21 = h1->Integral(iMin, ih_next) / tot * fClockFreq;
        slope1 = (t21 - t11) / (Double_t)(ih_next - il_next);
        offset1 = t11;

        Double_t dev = TMath::Abs(slope1 - slope) / TMath::Abs(slope);

        il = il_next;
        ih = ih_next;
        iter += 1;

        slope = slope1;
        offset = offset1;
        if (dev > prec)
        {
            break;
        }
    }
}

ClassImp(R3BTCalEngine)
