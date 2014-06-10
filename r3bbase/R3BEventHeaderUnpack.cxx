// ----------------------------------------------------------------------
// -----                       R3BEventHeaderUnpack                 -----
// -----                  Created 02-06-2014 by D. Kresan           -----
// ----------------------------------------------------------------------

#include "R3BEventHeaderUnpack.h"
#include "R3BEventHeader.h"

#include "FairRootManager.h"

#include "s_ve10_1.h"

R3BEventHeaderUnpack::R3BEventHeaderUnpack()
    : FairUnpack(-2, -2, -2, -2, -2)
    , fHeader(new R3BEventHeader())
{
}

R3BEventHeaderUnpack::~R3BEventHeaderUnpack()
{
    if (fHeader)
    {
        delete fHeader;
    }
}

Bool_t R3BEventHeaderUnpack::Init()
{
    Register();
    return kTRUE;
}

Bool_t R3BEventHeaderUnpack::DoUnpack(Int_t* data, Int_t size)
{
    s_ve10_1* event = (s_ve10_1*)data;
    fHeader->SetTrigger(event->i_trigger);
    return kTRUE;
}

void R3BEventHeaderUnpack::Reset()
{
    fHeader->SetTrigger(0);
}

void R3BEventHeaderUnpack::Register()
{
    FairRootManager* mgr = FairRootManager::Instance();
    mgr->Register("R3BEventHeader", "EventHeader", fHeader, kTRUE);
}

ClassImp(R3BEventHeaderUnpack)
