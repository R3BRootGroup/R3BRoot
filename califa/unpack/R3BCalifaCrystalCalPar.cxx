#include "R3BCalifaCrystalCalPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TMath.h"
#include "TString.h"

#include <iostream>

using std::cout;
using std::endl;

R3BCalifaCrystalCalPar::R3BCalifaCrystalCalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    fCryCalParams = new TArrayF(1030); // 512crystals*2CalibrationParameters
    fNumCrystals = 515;
    fNumParamsFit = 2; // pol1
}

R3BCalifaCrystalCalPar::~R3BCalifaCrystalCalPar()
{
    clear();
    delete fCryCalParams;
}

void R3BCalifaCrystalCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

void R3BCalifaCrystalCalPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BCalifaCrystalCalPar::putParams() called";
    if (!list)
    {
        return;
    }

    Int_t array_size = fNumCrystals * fNumParamsFit;
    LOG(INFO) << "Array Size: " << array_size;

    fCryCalParams->Set(array_size);

    list->add("califaCrystalCalPar", *fCryCalParams);
    list->add("califaCrystalNumberPar", fNumCrystals);
    list->add("califaCrystalParamsFitPar", fNumParamsFit);
}

Bool_t R3BCalifaCrystalCalPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BCalifaCrystalCalPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!list->fill("califaCrystalNumberPar", &fNumCrystals))
    {
        return kFALSE;
    }

    if (!list->fill("califaCrystalParamsFitPar", &fNumParamsFit))
    {
        return kFALSE;
    }

    Int_t array_size = fNumCrystals * fNumParamsFit;
    LOG(INFO) << "Array Size: " << array_size;
    fCryCalParams->Set(array_size);

    if (!(list->fill("califaCrystalCalPar", fCryCalParams)))
    {
        LOG(INFO) << "---Could not initialize califaCrystalCalPar";
        return kFALSE;
    }

    return kTRUE;
}

void R3BCalifaCrystalCalPar::printParams()
{
    LOG(INFO) << "R3BCalifaCrystalCalPar: Califa Crystal Calibration Parameters: ";
    Int_t array_size = fNumCrystals * fNumParamsFit;

    for (Int_t i = 0; i < fNumCrystals; i++)
    {
        cout << "Califa Crystal number: " << i + 1 << endl;
        for (Int_t j = 0; j < fNumParamsFit; j++)
        {
            cout << "FitParam(" << j << ") = " << fCryCalParams->GetAt(i * fNumParamsFit + j) << endl;
        }
    }
}
