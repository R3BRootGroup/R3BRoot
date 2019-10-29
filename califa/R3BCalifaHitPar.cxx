#include "R3BCalifaHitPar.h"
#include "TMath.h"
#include <iostream>

R3BCalifaHitPar::R3BCalifaHitPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    clear();
}

void R3BCalifaHitPar::putParams(FairParamList* list)
{
    std::cout << "-I- R3BCalifaHitPar::putParams() called" << std::endl;

    if (!list)
        return;
    list->add("fThreshold", (Double_t)fThreshold);
    list->add("fDRThreshold", (Double_t)fDRThreshold);
    list->add("fDeltaPolar", (Double_t)fDeltaPolar);
    list->add("fDeltaAzimuthal", (Double_t)fDeltaAzimuthal);
    list->add("fDeltaAngleClust", (Double_t)fDeltaAngleClust);
    list->add("fClusterAlgorithmSelector", (Int_t)fClusterAlgorithmSelector);
    list->add("fParCluster1", (Double_t)fParCluster1);
}

Bool_t R3BCalifaHitPar::getParams(FairParamList* list)
{
    std::cout << "-I- R3BCalifaHitPar::getParams() called" << std::endl;
    if (!list)
        return kFALSE;
    std::cout << "-I- R3BCalifaHitPar::getParams() 1 ";

    if (!list->fill("fThreshold", &fThreshold, 1))
        return kFALSE;
    if (!list->fill("fDRThreshold", &fDRThreshold, 1))
        return kFALSE;
    if (!list->fill("fDeltaPolar", &fDeltaPolar, 1))
        return kFALSE;
    if (!list->fill("fDeltaAzimuthal", &fDeltaAzimuthal, 1))
        return kFALSE;
    if (!list->fill("fDeltaAngleClust", &fDeltaAngleClust, 1))
        return kFALSE;
    if (!list->fill("fClusterAlgorithmSelector", &fClusterAlgorithmSelector, 1))
        return kFALSE;
    if (!list->fill("fParCluster1", &fParCluster1, 1))
        return kFALSE;

    return kTRUE;
}

void R3BCalifaHitPar::Print(Option_t* option) const
{
    std::cout << "-I- CALIFA HitFinder Parameters:" << std::endl;
    std::cout << "fThreshold " << fThreshold << std::endl;
    std::cout << "fDRThreshold " << fDRThreshold << std::endl;
    std::cout << "fDeltaPolar " << fDeltaPolar << std::endl;
    std::cout << "fDeltaAzimuthal " << fDeltaAzimuthal << std::endl;
    std::cout << "fDeltaAngleClust " << fDeltaAngleClust << std::endl;
    std::cout << "fClusterAlgorithmSelector " << fClusterAlgorithmSelector << std::endl;
    std::cout << "fParCluster1 " << fParCluster1 << std::endl;
}

ClassImp(R3BCalifaHitPar);
