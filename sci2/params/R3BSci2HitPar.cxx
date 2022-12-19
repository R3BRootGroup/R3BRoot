#include "R3BSci2HitPar.h"
#include "FairLogger.h"
#include "FairParamList.h"
#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BSci2HitPar::R3BSci2HitPar(const TString& name, const TString& title, const TString& context)
    : FairParGenericSet(name, title, context)
{
}

// ----  Destructor ------------------------------------------------------------
R3BSci2HitPar::~R3BSci2HitPar() { clear(); }

// ----  Method clear ----------------------------------------------------------
void R3BSci2HitPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BSci2HitPar::putParams(FairParamList* list)
{
    LOG(info) << "R3BSci2HitPar::putParams() called";
    if (!list)
    {
        return;
    }
    list->add("Pos_p0", fPos_p0);
    list->add("Pos_p1", fPos_p1);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BSci2HitPar::getParams(FairParamList* list)
{
    LOG(info) << "R3BSci2HitPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!(list->fill("Pos_p0", &fPos_p0)))
    {
        LOG(info) << "---Could not initialize Sci2HitPos_p0Par";
        return kFALSE;
    }

    if (!(list->fill("Pos_p1", &fPos_p1)))
    {
        LOG(info) << "---Could not initialize Sci2HitPos_p1Par";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BSci2HitPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BSci2HitPar::printParams()
{
    LOG(info) << "R3BSci2HitPar: Incoming ID parameters :";

    LOG(info) << "Pos_p0"
              << " "
              << "Pos_p1"
              << " ";

    LOG(info) << fPos_p0 << "\t" << fPos_p1;
}

ClassImp(R3BSci2HitPar)
