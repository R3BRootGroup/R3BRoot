#include "R3BRpcPars4Sim.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TMath.h"

#include "TString.h"

#include <iostream>

R3BRpcPars4Sim::R3BRpcPars4Sim(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumChannels(64)
{

    fChannelIDArray = new TArrayI(fNumChannels);
}

R3BRpcPars4Sim::~R3BRpcPars4Sim()
{
    clear();
    if (fChannelIDArray)
        delete fChannelIDArray;
}

void R3BRpcPars4Sim::clear()
{
    status = kFALSE;
    resetInputVersions();
}

void R3BRpcPars4Sim::putParams(FairParamList* list)
{
    LOG(info) << "R3BRpcPars4Sim::putParams() called";
    if (!list)
    {
        return;
    }

    fChannelIDArray->Set(fNumChannels);
    list->add("rpcChannelsIDPar", *fChannelIDArray);

    list->add("rpcChannelsNumberPar", fNumChannels);
}

Bool_t R3BRpcPars4Sim::getParams(FairParamList* list)
{
    LOG(info) << "R3BRpcPars4Sim::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!list->fill("rpcChannelslNumberPar", &fNumChannels))
    {
        return kFALSE;
    }

    fChannelIDArray->Set(fNumChannels);
    if (!(list->fill("rpcChannelsIDPar", fChannelIDArray)))
    {
        LOG(info) << "---Could not initialize rpcChannelsIDPar";
        return kFALSE;
    }

    return kTRUE;
}

void R3BRpcPars4Sim::printParams()
{
    LOG(info) << "R3BRpcPars4Sim: RPC Simulation Parameters: ";

    LOG(info) << "Channel ID"
              << " ";

    for (Int_t i = 0; i < fNumChannels; i++)
    {
        LOG(info) << i + 1 << " " << fChannelIDArray->GetAt(i);
    }
}

ClassImp(R3BRpcPars4Sim)
