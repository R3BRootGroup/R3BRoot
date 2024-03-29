#include "R3BFrsSciCalPar.h"

#include "FairDetParIo.h"
#include "FairParamList.h"
#include "TArrayF.h"
#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BFrsSciCalPar::R3BFrsSciCalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumDets(3)
    , fNumPmts(3)
    , fNumTofs(3)
    , fDetIdS2(1)
    , fDetIdS8(2)
    , fDetIdCaveC(3)
{
    if (fNumDets > 2)
    {
        fMinTofs = new TArrayD(fNumDets - 1);
        fMaxTofs = new TArrayD(fNumDets - 1);
        fTofCalGains = new TArrayD(fNumTofs);
        fTofCalOffsets = new TArrayD(fNumTofs);
        fBRho0 = new TArrayD(fNumTofs);
    }
    else
    {
        fMinTofs = NULL;
        fMaxTofs = NULL;
        fTofCalGains = NULL;
        fTofCalOffsets = NULL;
    }
    fMinPos = new TArrayF(fNumDets);
    fMaxPos = new TArrayF(fNumDets);
    fPosCalGains = new TArrayF(fNumDets);
    fPosCalOffsets = new TArrayF(fNumDets);
    fDisp = new TArrayD(fNumDets);
}

// ----  Destructor ------------------------------------------------------------
R3BFrsSciCalPar::~R3BFrsSciCalPar()
{
    status = kFALSE;
    resetInputVersions();
    if (fMinPos != NULL)
    {
        delete fMinPos;
    }
    if (fMaxPos != NULL)
    {
        delete fMaxPos;
    }
    if (fMinTofs != NULL)
    {
        delete fMinTofs;
    }
    if (fMaxTofs != NULL)
    {
        delete fMaxTofs;
    }
    if (fPosCalGains != NULL)
    {
        delete fPosCalGains;
    }
    if (fPosCalOffsets != NULL)
    {
        delete fPosCalOffsets;
    }
    if (fTofCalGains != NULL)
    {
        delete fTofCalGains;
    }
    if (fTofCalOffsets != NULL)
    {
        delete fTofCalOffsets;
    }
    if (fBRho0 != NULL)
    {
        delete fBRho0;
    }
    if (fDisp != NULL)
    {
        delete fDisp;
    }
}

// ----  Method clear ----------------------------------------------------------
void R3BFrsSciCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BFrsSciCalPar::putParams(FairParamList* list)
{
    LOG(info) << "R3BFrsSciCalPar::putParams() called";
    if (!list)
    {
        return;
    }

    list->add("fNumDets_CalPar", fNumDets);
    list->add("fNumPmts_CalPar", fNumPmts);
    list->add("fNumTofs_CalPar", fNumTofs);

    list->add("fDetIdS2_CalPar", fDetIdS2);
    list->add("fDetIdS8_CalPar", fDetIdS2);
    list->add("fDetIdCaveC_CalPar", fDetIdCaveC);

    if (fNumDets > 1)
    {
        LOG(info) << "R3BFrsSciCalPar::putParams Array Size for fMinTofs and fMaxTofs: " << fNumDets - 1;
        fMinTofs->Set(fNumDets - 1);
        list->add("fMinTofs_CalPar", *fMinTofs);
        fMaxTofs->Set(fNumDets - 1);
        list->add("fMaxTofs_CalPar", *fMaxTofs);

        LOG(info) << "R3BFrsSciCalPar::putParams Array Size for Tof calibration: " << fNumTofs;
        fTofCalGains->Set(fNumTofs);
        list->add("fTofCalGains_CalPar", *fTofCalGains);
        fTofCalOffsets->Set(fNumTofs);
        list->add("fTofCalOffsets_CalPar", *fTofCalOffsets);
        fBRho0->Set(fNumTofs);
        list->add("fBRho0_CalPar", *fBRho0);
    }

    LOG(info) << "R3BFrsSciCalPar::putParams Array Size for PosCal gains and offsets: " << fNumDets;
    fMinPos->Set(fNumDets);
    list->add("fMinPos_CalPar", *fMinPos);
    fMaxPos->Set(fNumDets);
    list->add("fMaxPos_CalPar", *fMaxPos);
    fPosCalGains->Set(fNumDets);
    list->add("fPosCalGains_CalPar", *fPosCalGains);
    fPosCalOffsets->Set(fNumDets);
    list->add("fPosCalOffsets_CalPar", *fPosCalOffsets);
    fDisp->Set(fNumDets);
    list->add("fDisp_CalPar", *fDisp);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BFrsSciCalPar::getParams(FairParamList* list)
{
    LOG(info) << "R3BFrsSciCalPar::getParams() called";

    if (!list)
    {
        LOG(error) << "Could not initialize FairParamList";
        return kFALSE;
    }
    if (!list->fill("fNumDets_CalPar", &fNumDets))
    {
        LOG(error) << "Could not initialize fNumDets_CalPar";
        return kFALSE;
    }
    if (!list->fill("fNumPmts_CalPar", &fNumPmts))
    {
        LOG(error) << "Could not initialize fNumPmts_CalPar";
        return kFALSE;
    }
    if (!list->fill("fNumTofs_CalPar", &fNumTofs))
    {
        LOG(error) << "Could not initialize fNumTofs_CalPar";
        return kFALSE;
    }

    if (!list->fill("fDetIdS2_CalPar", &fDetIdS2))
    {
        LOG(error) << "Could not initialize fDetIdS2_CalPar";
        return kFALSE;
    }
    if (!list->fill("fDetIdS8_CalPar", &fDetIdS2))
    {
        LOG(error) << "Could not initialize fDetIdS8_CalPar";
        return kFALSE;
    }
    if (!list->fill("fDetIdCaveC_CalPar", &fDetIdCaveC))
    {
        LOG(error) << "Could not initialize fDetIdCaveC_CalPar";
        return kFALSE;
    }

    if (fNumDets > 1)
    {

        LOG(info) << "R3BFrsSciCalPar::getParams Array Size for fMinTofs and fMaxTofs: " << fNumDets - 1;
        fMinTofs->Set(fNumDets - 1);
        if (!(list->fill("fMinTofs_CalPar", fMinTofs)))
        {
            LOG(error) << "---R3BFrsSciCalPar::getParams Could not initialize fMinTofs";
            return kFALSE;
        }
        fMaxTofs->Set(fNumDets - 1);
        if (!(list->fill("fMaxTofs_CalPar", fMaxTofs)))
        {
            LOG(error) << "---R3BFrsSciCalPar::getParams Could not initialize fMaxTofs";
            return kFALSE;
        }

        LOG(info) << "R3BFrsSciCalPar::getParams Array Size for Tof calibration: " << fNumTofs;
        fTofCalGains->Set(fNumTofs);
        if (!(list->fill("fTofCalGains_CalPar", fTofCalGains)))
        {
            LOG(error) << "---R3BFrsSciCalPar::getParams Could not initialize fTofCalGains";
            return kFALSE;
        }
        fTofCalOffsets->Set(fNumTofs);
        if (!(list->fill("fTofCalOffsets_CalPar", fTofCalOffsets)))
        {
            LOG(error) << "---R3BFrsSciCalPar::getParams Could not initialize fTofCalOffsets";
            return kFALSE;
        }
        fBRho0->Set(fNumTofs);
        if (!(list->fill("fBRho0_CalPar", fBRho0)))
        {
            LOG(error) << "---R3BFrsSciCalPar::getParams Could not initialize fBRho0";
            return kFALSE;
        }
    }

    LOG(info) << "R3BFrsSciCalPar::getParams Array Size for Pos limits and PosCal gains and offsets: " << fNumDets;
    fMinPos->Set(fNumDets);
    if (!(list->fill("fMinPos_CalPar", fMinPos)))
    {
        LOG(error) << "---R3BFrsSciCalPar::getParams Could not initialize fMinPos";
        return kFALSE;
    }
    fMaxPos->Set(fNumDets);
    if (!(list->fill("fMaxPos_CalPar", fMaxPos)))
    {
        LOG(error) << "---R3BFrsSciCalPar::getParams Could not initialize fMaxPos";
        return kFALSE;
    }
    fPosCalGains->Set(fNumDets);
    if (!(list->fill("fPosCalGains_CalPar", fPosCalGains)))
    {
        LOG(error) << "---R3BFrsSciCalPar::getParams Could not initialize fPosCalGains";
        return kFALSE;
    }
    fPosCalOffsets->Set(fNumDets);
    if (!(list->fill("fPosCalOffsets_CalPar", fPosCalOffsets)))
    {
        LOG(error) << "---R3BFrsSciCalPar::getParams Could not initialize fPosCalOffsets";
        return kFALSE;
    }
    fDisp->Set(fNumDets);
    if (!(list->fill("fDisp_CalPar", fDisp)))
    {
        LOG(error) << "---R3BFrsSciCalPar::getParams Could not initialize fDisp";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method printParams ----------------------------------------------------
void R3BFrsSciCalPar::printParams()
{
    LOG(info) << "R3BFrsSciCalPar: FrsSciCal Parameters: ";
    LOG(info) << "--- --------------------------------------------";
    LOG(info) << "--- fNumDets = " << fNumDets;
    LOG(info) << "--- fNumPmts = " << fNumPmts;
    LOG(info) << "--- fNumTofs = " << fNumTofs;
    LOG(info) << "--- --------------------------------------------";
    LOG(info) << "--- fDetIdS2    = " << fDetIdS2;
    LOG(info) << "--- fDetIdS8    = " << fDetIdS8;
    LOG(info) << "--- fDetIdCaveC = " << fDetIdCaveC;
    LOG(info) << "--- --------------------------------------------";
    for (UShort_t d = 0; d < fNumDets; d++)
    {
        LOG(info) << " for Det " << d + 1 << ", fMinPos = " << fMinPos->GetAt(d) << ", fMaxPos = " << fMaxPos->GetAt(d);
        LOG(info) << "          "
                  << ", fPosCalGains = " << fPosCalGains->GetAt(d) << ", fPosCalOffsets = " << fPosCalOffsets->GetAt(d);
        LOG(info) << "          "
                  << ", fDisp = " << fDisp->GetAt(d);
    }
    LOG(info) << "--- --------------------------------------------";
    if (fNumDets > 1)
    {
        for (UShort_t d = 0; d < fNumDets - 2; d++)
        {
            LOG(info) << "Tof ( FrsSciDet" << d + 1 << "->FrsSciDet" << d + 2 << ") : fMinTofs = " << fMinTofs->GetAt(d)
                      << ", :fMaxTofs=" << fMaxTofs->GetAt(d);
        }
    }
}

ClassImp(R3BFrsSciCalPar);
