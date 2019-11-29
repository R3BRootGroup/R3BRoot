/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BTCalPar.h"

#include "FairLogger.h"
#include "FairParamList.h" // for FairParamList
#include "FairRtdbRun.h"

ClassImp(R3BTCalPar);

R3BTCalPar::R3BTCalPar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fTCalParams(new TObjArray(NMODULEMAX))
    , fMapInit(kFALSE)
{
}

R3BTCalPar::~R3BTCalPar()
{
    if (fTCalParams)
    {
        delete fTCalParams;
        fTCalParams = NULL;
    }
}

void R3BTCalPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BTCalPar::putParams() called";
    if (!list)
    {
        return;
    }
    list->addObject(GetName(), fTCalParams);
}

Bool_t R3BTCalPar::getParams(FairParamList* list)
{
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fillObject(GetName(), fTCalParams))
    {
        return kFALSE;
    }
    return kTRUE;
}

void R3BTCalPar::clear() {}

void R3BTCalPar::printParams()
{

    LOG(INFO) << " -----------  " << GetName() << " Time Calib. Parameters -------------  ";

    LOG(INFO) << " Number of TCal Parameters " << fTCalParams->GetEntries();
    for (Int_t i = 0; i < fTCalParams->GetEntries(); i++)
    {
        R3BTCalModulePar* t_par = (R3BTCalModulePar*)fTCalParams->At(i);
        LOG(INFO) << "----------------------------------------------------------------------";
        if (t_par)
        {
            t_par->printParams();
        }
    }
}

R3BTCalModulePar* R3BTCalPar::GetModuleParAt(Int_t plane, Int_t paddle, Int_t side)
{
    if (!fMapInit)
    {
        fIndexMap.clear();
        R3BTCalModulePar* par;
        Int_t tplane;
        Int_t tpaddle;
        Int_t tside;
        Int_t index;
        for (Int_t i = 0; i < fTCalParams->GetEntries(); i++)
        {
            par = (R3BTCalModulePar*)fTCalParams->At(i);
            if (NULL == par)
            {
                continue;
            }
            tplane = par->GetPlane();
            tpaddle = par->GetPaddle();
            tside = par->GetSide();
            if (tplane < 1 || tplane > N_PLANE_MAX || tpaddle < 1 || tpaddle > N_PADDLE_MAX || tside < 1 ||
                tside > N_SIDE_MAX)
            {
                LOG(ERROR) << "R3BTCalPar::GetModuleParAt : error in plane/paddle/side indexing. " << tplane << " / "
                           << tpaddle << " / " << tside;
                continue;
            }
            index = (tplane - 1) * N_PADDLE_MAX * N_SIDE_MAX + (tpaddle - 1) * N_SIDE_MAX + tside - 1;
            if (fIndexMap.find(index) != fIndexMap.end())
            {
                LOG(ERROR) << "R3BTCalPar::GetModuleParAt : parameter found more than once. " << tplane << " / "
                           << tpaddle << " / " << tside;
                continue;
            }
            fIndexMap[index] = i;
        }
        fMapInit = kTRUE;
    }

    if (plane < 1 || plane > N_PLANE_MAX || paddle < 1 || paddle > N_PADDLE_MAX || side < 1 || side > N_SIDE_MAX)
    {
        LOG(ERROR) << "R3BTCalPar::GetModuleParAt : error in plane/paddle/side indexing. " << plane << " / " << paddle
                   << " / " << side;
        return NULL;
    }
    Int_t index = (plane - 1) * N_PADDLE_MAX * N_SIDE_MAX + (paddle - 1) * N_SIDE_MAX + side - 1;

    if (fIndexMap.find(index) == fIndexMap.end())
    {
        LOG(WARNING) << "R3BTCalPar::GetModuleParAt : parameter not found for: " << plane << " / " << paddle << " / "
                     << side;
        return NULL;
    }
    Int_t arind = fIndexMap[index];
    return (R3BTCalModulePar*)fTCalParams->At(arind);
}

void R3BTCalPar::AddModulePar(R3BTCalModulePar* tch)
{
    fMapInit = kFALSE;
    fTCalParams->Add(tch);
}

void R3BTCalPar::PrintModuleParams(Int_t plane, Int_t paddle, Int_t side)
{
    R3BTCalModulePar* par = GetModuleParAt(plane, paddle, side);
    if (NULL != par)
    {
        par->printParams();
    }
}

void R3BTCalPar::DrawModuleParams(Int_t plane, Int_t paddle, Int_t side)
{
    R3BTCalModulePar* par = GetModuleParAt(plane, paddle, side);
    if (NULL != par)
    {
        par->DrawParams();
    }
}

Bool_t R3BTCalPar::SetModuleParValue(Int_t plane, Int_t paddle, Int_t side, Int_t tac_channel, Double_t value)
{
    R3BTCalModulePar* par = GetModuleParAt(plane, paddle, side);
    if (NULL != par)
    {
        if (par->GetSlopeAt(0) > 0)
        {
            LOG(ERROR) << "R3BTCalPar::SetModuleParValue : this function does not support Tacquila.";
            return kFALSE;
        }
        Int_t i = 0;
        for (; i < par->GetNofChannels(); i++)
        {
            if (par->GetBinLowAt(i) == tac_channel)
            {
                break;
            }
        }
        par->SetOffsetAt(value, i);
        return kTRUE;
    }
    return kFALSE;
}

void R3BTCalPar::SavePar(TString runNumber)
{
    this->Write();
    FairRtdbRun* r1 = (FairRtdbRun*)gDirectory->Get(runNumber);
    if (NULL == r1)
    {
        LOG(ERROR) << "Run " << runNumber << " does not exist in parameter file! Aborting.";
        return;
    }
    FairParVersion* ver = r1->getParVersion(GetName());
    if (NULL == ver)
    {
        LOG(ERROR) << "Parameter container " << GetName() << " does not exist in parameter file! Aborting.";
        return;
    }
    ver->setRootVersion(ver->getRootVersion() + 1);
    r1->Write();
    LOG(INFO) << "Container " << GetName() << " is written to ROOT file. Version: " << ver->getRootVersion();
}
