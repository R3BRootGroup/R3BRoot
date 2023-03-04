#include "R3BCalifaCrystalPars4Sim.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TMath.h"

#include "TString.h"

#include <iostream>

R3BCalifaCrystalPars4Sim::R3BCalifaCrystalPars4Sim(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumCrystals(4864)
    , fNumParams4Sim(3) /* Crystal ID & Reso & Threshold */
{

    fCrystalIDArray = new TArrayI(fNumCrystals);
    fThresholdArray = new TArrayI(fNumCrystals);
    fResolutionArray = new TArrayF(fNumCrystals);
}

R3BCalifaCrystalPars4Sim::~R3BCalifaCrystalPars4Sim()
{
    clear();
    if (fCrystalIDArray)
        delete fCrystalIDArray;
    if (fThresholdArray)
        delete fThresholdArray;
    if (fResolutionArray)
        delete fResolutionArray;
}

void R3BCalifaCrystalPars4Sim::clear()
{
    status = kFALSE;
    resetInputVersions();
}

void R3BCalifaCrystalPars4Sim::putParams(FairParamList* list)
{
    LOG(info) << "R3BCalifaCrystalPars4Sim::putParams() called";
    if (!list)
    {
        return;
    }

    fCrystalIDArray->Set(fNumCrystals);
    list->add("califaCrystalIDPar", *fCrystalIDArray);

    fThresholdArray->Set(fNumCrystals);
    list->add("califaThresholdPar", *fThresholdArray);

    fResolutionArray->Set(fNumCrystals);
    list->add("califaResolutionPar", *fResolutionArray);

    list->add("califaCrystalNumberPar", fNumCrystals);
    list->add("califaNumPars4SimPar", fNumParams4Sim);
}

Bool_t R3BCalifaCrystalPars4Sim::getParams(FairParamList* list)
{
    LOG(info) << "R3BCalifaCrystalPars4Sim::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!list->fill("califaCrystalNumberPar", &fNumCrystals))
    {
        return kFALSE;
    }

    if (!list->fill("califaNumPars4SimPar", &fNumParams4Sim))
    {
        return kFALSE;
    }

    fCrystalIDArray->Set(fNumCrystals);
    if (!(list->fill("califaCrystalIDPar", fCrystalIDArray)))
    {
        LOG(info) << "---Could not initialize califaCrystalIDPar";
        return kFALSE;
    }

    fThresholdArray->Set(fNumCrystals);
    if (!(list->fill("califaThresholdPar", fThresholdArray)))
    {
        LOG(info) << "---Could not initialize califaThresholdPar";
        return kFALSE;
    }

    fResolutionArray->Set(fNumCrystals);
    if (!(list->fill("califaResolutionPar", fResolutionArray)))
    {
        LOG(info) << "---Could not initialize califaResolutionPar";
        return kFALSE;
    }

    return kTRUE;
}

void R3BCalifaCrystalPars4Sim::printParams()
{
    LOG(info) << "R3BCalifaCrystalPars4Sim: Califa Crystal Simulation Parameters: ";

    LOG(info) << "Crystal ID"
              << " "
              << "Threshold"
              << " "
              << "Resolution";

    for (Int_t i = 0; i < fNumCrystals; i++)

    {
        LOG(info) << i + 1 << " " << fCrystalIDArray->GetAt(i) << " " << fThresholdArray->GetAt(i) << " "
                  << fResolutionArray->GetAt(i);
    }
}
