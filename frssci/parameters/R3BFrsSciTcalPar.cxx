#include "R3BFrsSciTcalPar.h"

#include "FairDetParIo.h"
#include "FairLogger.h"
#include "FairParamList.h"
#include "TArrayF.h"
#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BFrsSciTcalPar::R3BFrsSciTcalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumDets(2)
    , fNumPmts(3)
    , fNumPars(1000)
{
    fAllTcalParams = new TArrayF((UInt_t)(fNumDets * fNumPmts * fNumPars));
}

// ----  Destructor ------------------------------------------------------------
R3BFrsSciTcalPar::~R3BFrsSciTcalPar()
{
    status = kFALSE;
    resetInputVersions();
    if (fAllTcalParams != NULL)
    {
        delete fAllTcalParams;
    }
}

// ----  Method clear ----------------------------------------------------------
void R3BFrsSciTcalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BFrsSciTcalPar::putParams(FairParamList* list)
{
    LOG(info) << "R3BFrsSciTcalPar::putParams() called";
    if (!list)
    {
        return;
    }

    list->add("nDetsTcalPar", fNumDets);

    list->add("nPmtsTcalPar", fNumPmts);

    list->add("nTcalParsPerSignal", fNumPars);

    UInt_t array_size = (UInt_t)(fNumDets * fNumPmts * fNumPars);
    LOG(info) << "R3BFrsSciTcalPar::putParams Array Size for Vftx tcal: " << array_size;
    fAllTcalParams->Set(array_size);
    list->add("TcalPar", *fAllTcalParams);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BFrsSciTcalPar::getParams(FairParamList* list)
{
    LOG(info) << "R3BFrsSciTcalPar::getParams() called";

    if (!list)
    {
        LOG(error) << "Could not initialize FairParamList";
        return kFALSE;
    }
    if (!list->fill("nDetsTcalPar", &fNumDets))
    {
        LOG(error) << "Could not initialize nDetsTcalPar";
        return kFALSE;
    }
    if (!list->fill("nPmtsTcalPar", &fNumPmts))
    {
        LOG(error) << "Could not initialize nPmtsTcalPar";
        return kFALSE;
    }
    if (!list->fill("nTcalParsPerSignal", &fNumPars))
    {
        LOG(error) << "Could not initialize nTcalParsPerSignal";
        return kFALSE;
    }

    UInt_t array_size = (UInt_t)(fNumDets * fNumPmts * fNumPars);
    LOG(info) << "R3BFrsSciTcalPar::getParams Array Size for VFTX tcal par: " << array_size;
    fAllTcalParams->Set(array_size);

    if (!(list->fill("TcalPar", fAllTcalParams)))
    {
        LOG(error) << "---R3BFrsSciTcalPar::getParams Could not initialize fAllTcalParams";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method printParams ----------------------------------------------------
void R3BFrsSciTcalPar::printParams()
{
    LOG(info) << "R3BFrsSciTcalPar: FrsSciTcal Parameters: ";

    for (UShort_t det = 0; det < fNumDets; det++)
    {
        for (UShort_t pmt = 0; pmt < fNumPmts; pmt++)
        {
            UInt_t sig = (UInt_t)(det) * (UInt_t)fNumPmts + (UInt_t)pmt;
            LOG(info) << "--- --------------------------------------------";
            LOG(info) << "--- Vftx Tcal Param for signal number: " << det * fNumPmts + pmt;
            LOG(info) << "---       det " << det + 1;
            LOG(info) << "---       pmt " << pmt + 1;
            LOG(info) << "--- --------------------------------------------";

            for (UShort_t par = 0; par < fNumPars; par++)
            {
                LOG(debug) << "FineTime at Bin (" << par
                           << ") = " << fAllTcalParams->GetAt(sig * (UInt_t)fNumPars + (UInt_t)par);
            }
        }
    }
}

ClassImp(R3BFrsSciTcalPar);
