
#include "R3BIncomingIDPar.h"

#include "FairLogger.h"
#include "FairParamList.h"
#include "TArrayF.h"
#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BIncomingIDPar::R3BIncomingIDPar(const TString& name, const TString& title, const TString& context)
    : FairParGenericSet(name, title, context)
    , fNumDet(1)
{
    fToFoffset = new TArrayF(fNumDet);
    fPosS2Left = new TArrayF(fNumDet);
    fPosS2Right = new TArrayF(fNumDet);
    fBrho0_S2toCC = new TArrayF(fNumDet);
    fDispersionS2 = new TArrayF(fNumDet);
    fTof2InvV_p0 = new TArrayF(fNumDet);
    fTof2InvV_p1 = new TArrayF(fNumDet);
}

// ----  Destructor ------------------------------------------------------------
R3BIncomingIDPar::~R3BIncomingIDPar()
{
    clear();
    if (fToFoffset)
        delete fToFoffset;
    if (fPosS2Left)
        delete fPosS2Left;
    if (fPosS2Right)
        delete fPosS2Right;
    if (fBrho0_S2toCC)
        delete fBrho0_S2toCC;
    if (fDispersionS2)
        delete fDispersionS2;
    if (fTof2InvV_p0)
        delete fTof2InvV_p1;
    if (fDispersionS2)
        delete fTof2InvV_p0;
}

// ----  Method clear ----------------------------------------------------------
void R3BIncomingIDPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BIncomingIDPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BIncomingIDPar::putParams() called";
    if (!list)
    {
        return;
    }
    fToFoffset->Set(fNumDet);
    list->add("TofOffset", *fToFoffset);
    fPosS2Left->Set(fNumDet);
    list->add("PosS2Left", *fPosS2Left);
    fPosS2Right->Set(fNumDet);
    list->add("PosS2Right", *fPosS2Right);
    fBrho0_S2toCC->Set(fNumDet);
    list->add("Brho0_S2toCC", *fBrho0_S2toCC);
    fDispersionS2->Set(fNumDet);
    list->add("DispersionS2", *fDispersionS2);
    fTof2InvV_p0->Set(fNumDet);
    list->add("Tof2InvV_p0", *fTof2InvV_p0);
    fTof2InvV_p1->Set(fNumDet);
    list->add("DTof2InvV_p1", *fTof2InvV_p1);
    list->add("x0_point", fx0_point);
    list->add("y0_point", fy0_point);
    list->add("rot_ang", frot_ang);
    list->add("x0_Aq", fx0_Aq);
    list->add("y0_Aq", fy0_Aq);
    list->add("ang_Aq", fang_Aq);
    list->add("Beta_min", fBeta_min);
    list->add("Beta_max", fBeta_max);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BIncomingIDPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BIncomingIDPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!(list->fill("TofOffset", fToFoffset)))
    {
        LOG(INFO) << "---Could not initialize incomingIDTofOffsetPar";
        return kFALSE;
    }

    if (!(list->fill("PosS2Left", fPosS2Left)))
    {
        LOG(INFO) << "---Could not initialize incomingIDPosS2LeftPar";
        return kFALSE;
    }

    if (!(list->fill("PosS2Right", fPosS2Right)))
    {
        LOG(INFO) << "---Could not initialize incomingIDPosS2RightPar";
        return kFALSE;
    }

    if (!(list->fill("Brho0_S2toCC", fBrho0_S2toCC)))
    {
        LOG(INFO) << "---Could not initialize incomingIDBrho0_S2toCCPar";
        return kFALSE;
    }

    if (!(list->fill("DispersionS2", fDispersionS2)))
    {
        LOG(INFO) << "---Could not initialize incomingIDDispersionS2Par";
        return kFALSE;
    }

    if (!(list->fill("Tof2InvV_p0", fTof2InvV_p0)))
    {
        LOG(INFO) << "---Could not initialize incomingIDTof2InvV_p0Par";
        return kFALSE;
    }

    if (!(list->fill("Tof2InvV_p1", fTof2InvV_p1)))
    {
        LOG(INFO) << "---Could not initialize incomingIDTof2InvV_p1Par";
        return kFALSE;
    }

    if (!(list->fill("x0_point", &fx0_point)))
    {
        LOG(INFO) << "---Could not initialize incomingIDx0_pointPar";
        return kFALSE;
    }

    if (!(list->fill("y0_point", &fy0_point)))
    {
        LOG(INFO) << "---Could not initialize incomingIDy0_pointPar";
        return kFALSE;
    }

    if (!(list->fill("rot_ang", &frot_ang)))
    {
        LOG(INFO) << "---Could not initialize incomingIDrot_angPar";
        return kFALSE;
    }

    if (!(list->fill("x0_Aq", &fx0_Aq)))
    {
        LOG(INFO) << "---Could not initialize incomingIDx0_AqPar";
        return kFALSE;
    }

    if (!(list->fill("y0_Aq", &fy0_Aq)))
    {
        LOG(INFO) << "---Could not initialize incomingIDy0_AqPar";
        return kFALSE;
    }

    if (!(list->fill("ang_Aq", &fang_Aq)))
    {
        LOG(INFO) << "---Could not initialize incomingIDang_AqPar";
        return kFALSE;
    }

    if (!(list->fill("Beta_min", &fBeta_min)))
    {
        LOG(INFO) << "---Could not initialize incomingID Beta_min Par";
        return kFALSE;
    }

    if (!(list->fill("Beta_max", &fBeta_max)))
    {
        LOG(INFO) << "---Could not initialize incomingID Beta_max Par";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BIncomingIDPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BIncomingIDPar::printParams()
{
    LOG(INFO) << "R3BIncomingIDPar: Incoming ID parameters :";

    // LOG(INFO) << "Brho"
    //           << " "
    //           << "TofOffset"
    //           << " "
    //           << " PosS2Left"
    //           << " "
    //           << " PosS2Right" ;

    // LOG(INFO) <<  fBrho << "\t" <<  fToFoffset[0] << "\t" <<
    //          fPosS2Left[0]<< "\t" << fPosS2Right[0];
}

ClassImp(R3BIncomingIDPar)
