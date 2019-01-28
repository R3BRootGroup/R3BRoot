#include "R3BPhaseSpaceGenerator.h"
#include "FairLogger.h"
#include "FairPrimaryGenerator.h"
#include "FairRunSim.h"
#include "TDatabasePDG.h"
#include <numeric>
#include "FairRootManager.h"

R3BPhaseSpaceGenerator::R3BPhaseSpaceGenerator()
    : fGamma(1.)
    , fBeta(0.)
    , fBeamEnergy_AMeV(0.)
    , fErel_keV(0.)
    , fConstBeamEnergy(kTRUE)
    , fConstErel(kTRUE)
{
}

void R3BPhaseSpaceGenerator::AddParticle(const Int_t PDGCode)
{
    auto particle = TDatabasePDG::Instance()->GetParticle(PDGCode);
    if (particle == nullptr)
    {
        LOG(FATAL) << "R3BPhaseSpaceGenerator::AddParticle: No such particle: " << PDGCode << FairLogger::endl;
        return;
    }
    fMasses.push_back(particle->Mass());
    fPDGCodes.push_back(particle->PdgCode());
}

void R3BPhaseSpaceGenerator::AddHeavyIon(const FairIon& ion)
{
    auto run = FairRunSim::Instance();
    if (run == nullptr)
    {
        LOG(WARNING) << "R3BPhaseSpaceGenerator::AddIon: No FairRunSim" << FairLogger::endl;
    }
    else
    {
        run->AddNewIon((FairIon*)ion.Clone());
    }
    fMasses.push_back(ion.GetMass());
    fPDGCodes.push_back(1000000000 + 10 * 1000 * ion.GetZ() + 10 * ion.GetA());
}


// ########################## BeamEnergy ##########################

void R3BPhaseSpaceGenerator::SetBeamEnergyAMeV(const Double_t EBeam_AMeV)
{
    fBeamEnergy_AMeV = EBeam_AMeV;
    fConstBeamEnergy = kTRUE;
}

void R3BPhaseSpaceGenerator::SetBeamEnergyDistAMeV(const std::function<Double_t(Double_t)> dist, Double_t minE, const Double_t maxE, const int samples){
    if(minE < 0 || maxE < 0)
        LOG(fatal) << "R3BPhaseSpaceGenerator::SetBeamEnergyDistAMeV:  Lower or upper boundery is negative for a distribution passed via lambda expression! " << FairLogger::endl;
    fBeamEnergyLookup = SetupLookupGraph(dist, minE, maxE, samples);
    fConstBeamEnergy = kFALSE;                                
}

void R3BPhaseSpaceGenerator::SetBeamEnergyDistAMeV(const TGraph& dist, Double_t minE, const Double_t maxE, const int samples){
    Double_t xmin, xmax, ymin, ymax;
    dist.ComputeRange(xmin, ymin, xmax, ymax);
    if(minE > 0.){
        if(minE < xmin){
            LOG(fatal) << "R3BPhaseSpaceGenerator::SetBeamEnergyDistAMeV:  Lower boundery is smaller than the smallest value inside Graph!" << FairLogger::endl;
        }
        xmin = minE;
    }
    if(maxE > 0.){
        if(maxE < xmax){
            LOG(fatal) << "R3BPhaseSpaceGenerator::SetBeamEnergyDistAMeV:  Upper boundery is bigger than the biggest value inside Graph!" << FairLogger::endl;
        }
        xmax = maxE;
    }

    fBeamEnergyLookup = SetupLookupGraph([&dist](Double_t E){return dist.Eval(E);}, xmin, xmax, samples);
    fConstBeamEnergy = kFALSE;                                
}

void R3BPhaseSpaceGenerator::SetBeamEnergyDistAMeV(const TF1& dist, Double_t minE, const Double_t maxE, const int samples){
    Double_t xmin, xmax;
    dist.GetRange(xmin, xmax);
    if(minE > 0.){
        if(minE < xmin){
            LOG(fatal) << "R3BPhaseSpaceGenerator::SetBeamEnergyDistAMeV:  Lower boundery is outside the range of the TF1!" << FairLogger::endl;
        }
        xmin = minE;
    }
    if(maxE > 0.){
        if(maxE < xmax){
            LOG(fatal) << "R3BPhaseSpaceGenerator::SetBeamEnergyDistAMeV:  Upper boundery is outside the range of the TF1!" << FairLogger::endl;
        }
        xmax = maxE;
    }

    fBeamEnergyLookup = SetupLookupGraph([&dist](Double_t E){return dist.Eval(E);}, xmin, xmax, samples);
    fConstBeamEnergy = kFALSE;                                
}

// ########################## Erel ##########################

void R3BPhaseSpaceGenerator::SetErelkeV(const Double_t Erel_keV){ 
    fErel_keV = Erel_keV;
    fConstErel = kTRUE;
}

void R3BPhaseSpaceGenerator::SetErelDistkeV(const std::function<Double_t(Double_t)> dist, const Double_t minE, const Double_t maxE, const int samples){
    if(minE < 0 || maxE < 0)
       LOG(fatal) << "R3BPhaseSpaceGenerator::SetErelDistkeV:  Lower or upper boundery is negative for a distribution passed via lambda expression! " << FairLogger::endl;
    fErelLookup = SetupLookupGraph(dist, minE, maxE, samples);
    fConstErel = kFALSE;
}

void R3BPhaseSpaceGenerator::SetErelDistkeV(const TGraph& dist, const Double_t minE, const Double_t maxE, const int samples){
    Double_t xmin, xmax, ymin, ymax;
    dist.ComputeRange(xmin, ymin, xmax, ymax);
    if(minE > 0.){
        if(minE < xmin){
            LOG(fatal) << "R3BPhaseSpaceGenerator::SetErelDistkeV:  Lower boundery is smaller than the smallest value inside Graph!" << FairLogger::endl;
        }
        xmin = minE;
    }
    if(maxE > 0.){
        if(maxE < xmax){
            LOG(fatal) << "R3BPhaseSpaceGenerator::SetErelDistkeV:  Upper boundery is bigger than the biggest value inside Graph!" << FairLogger::endl;
        }
        xmax = maxE;
    }
    
    fErelLookup = SetupLookupGraph([&dist](Double_t E){return dist.Eval(E);}, xmin, xmax, samples);
    fConstErel = kFALSE;
}

void R3BPhaseSpaceGenerator::SetErelDistkeV(const TF1& dist, const Double_t minE, const Double_t maxE, const int samples){
    Double_t xmin, xmax;
    dist.GetRange(xmin, xmax);
    if(minE > 0.){
        if(minE < xmin){
            LOG(fatal) << "R3BPhaseSpaceGenerator::SetBeamEnergyDistAMeV:  Lower boundery is outside the range of the TF1!" << FairLogger::endl;
        }
        xmin = minE;
    }
    if(maxE > 0.){
        if(maxE < xmax){
            LOG(fatal) << "R3BPhaseSpaceGenerator::SetBeamEnergyDistAMeV:  Upper boundery is outside the range of the TF1!" << FairLogger::endl;
        }
        xmax = maxE;
    }
    
    fErelLookup = SetupLookupGraph([&dist](Double_t E){return dist.Eval(E);}, xmin, xmax, samples);
    fConstErel = kFALSE;
}

// ########################## Others ##########################

Bool_t R3BPhaseSpaceGenerator::Init()
{
    if(fConstErel){
        const Double_t TotE_GeV = fErel_keV / (1000. * 1000.) + std::accumulate(fMasses.begin(), fMasses.end(), 0.);
        TLorentzVector Init(0.0, 0.0, 0.0, TotE_GeV);
        fPhaseSpace.SetDecay(Init, fMasses.size(), fMasses.data());
    }
    if(fConstBeamEnergy){
        fGamma = 1 + (fBeamEnergy_AMeV / 1000.) / 0.931494028; // MeV/A -> GeV/A
        fBeta = std::sqrt(1 - 1 / std::pow(fGamma, 2));
    }
    return true;
}

Bool_t R3BPhaseSpaceGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{  
    if(!fConstErel){
        fErel_keV = fErelLookup.Eval(fRngGen.Rndm());   
        const Double_t TotE_GeV = fErel_keV / (1000. * 1000.) + std::accumulate(fMasses.begin(), fMasses.end(), 0.);
        TLorentzVector Init(0.0, 0.0, 0.0, TotE_GeV);
        fPhaseSpace.SetDecay(Init, fMasses.size(), fMasses.data());
    }
    fPhaseSpace.Generate();

    if(!fConstBeamEnergy){
        fBeamEnergy_AMeV = fBeamEnergyLookup.Eval(fRngGen.Rndm());
        fGamma = 1 + (fBeamEnergy_AMeV / 1000.) / 0.931494028; // MeV/A -> GeV/A
        fBeta = std::sqrt(1 - 1 / std::pow(fGamma, 2));
    }

    const size_t nParticles = fPDGCodes.size();
    for (size_t i = 0; i < nParticles; i++)
    {
        const TLorentzVector* p = fPhaseSpace.GetDecay(i);

        // Apply boost
        const Double_t pz = fBeta * fGamma * p->E() + fGamma * p->Pz();

        primGen->AddTrack(fPDGCodes.at(i), p->Px(), p->Py(), pz, 0., 0., 0.);
    }
    return true;
}

TGraph R3BPhaseSpaceGenerator::SetupLookupGraph(std::function<Double_t(Double_t)> dist, const Double_t minE, const Double_t maxE, const Int_t samples){    
    Double_t stepE = (maxE-minE)/samples, x, y;
    TGraph steppingGraph, integralGraph;
    
    for(int i = 0; i <= samples; ++i){
        x = minE + i*stepE;
        y = dist(x);
        steppingGraph.SetPoint(steppingGraph.GetN(), x, y);
    }
    steppingGraph.SetPoint(steppingGraph.GetN(), maxE, 0);
    
    Double_t invInt = 1.0/steppingGraph.Integral();
    
    integralGraph.SetPoint(integralGraph.GetN(),minE,0);
    for(int i = 1; i < steppingGraph.GetN()-1; ++i){
        
        steppingGraph.GetPoint(i+1,x,y);
        steppingGraph.SetPoint(i+1,x,0);

        integralGraph.SetPoint(integralGraph.GetN(),(x-stepE), steppingGraph.Integral(0,i+1)*invInt);
        steppingGraph.SetPoint(i+1,x,y);
    }
    integralGraph.SetPoint(integralGraph.GetN(), maxE, 1);

    TGraph targetG;
    for(int i = 0; i < integralGraph.GetN(); ++i){
        integralGraph.GetPoint(i,x,y);
        targetG.SetPoint(i,y,x);
    }
    
    return targetG;
}
