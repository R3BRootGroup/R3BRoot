#include "R3BCADNEBULANeutron2DPar.h"
#include "FairParamList.h"
#include "TObjString.h"
#include <iostream>
#include <stdexcept>

R3BCADNEBULANeutron2DPar::R3BCADNEBULANeutron2DPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNeutronCuts(nullptr)
{
}

R3BCADNEBULANeutron2DPar::~R3BCADNEBULANeutron2DPar()
{
    // Note: Deleting stuff here or in clear() causes segfaults?
}

void R3BCADNEBULANeutron2DPar::clear() {}

void R3BCADNEBULANeutron2DPar::putParams(FairParamList* l)
{
    if (!l)
    {
        return;
    }
    l->addObject("NEBULANeutronCuts", fNeutronCuts);
}

Bool_t R3BCADNEBULANeutron2DPar::getParams(FairParamList* l)
{
    if (!l)
    {
        return kFALSE;
    }
    if (!l->fillObject("NEBULANeutronCuts", fNeutronCuts))
    {
        return kFALSE;
    }
    return kTRUE;
}

void R3BCADNEBULANeutron2DPar::printParams()
{
    std::cout << "R3BCADNEBULANeutron2DPar: NEBULA Neutron Cuts ..." << std::endl;
    for (const auto& nc : GetNeutronCuts())
    {
        std::cout << nc.first << std::endl;
        nc.second->Print();
    }
}

void R3BCADNEBULANeutron2DPar::SetNeutronCuts(const std::map<UInt_t, TCutG*>& cuts)
{
    fNeutronCuts = new TMap();
    fNeutronCuts->SetOwner(kTRUE);
    for (const auto& nc : cuts)
    {
        TObjString* key = new TObjString(TString::Itoa(nc.first, 10));
        fNeutronCuts->Add(key, nc.second->Clone());
    }
}

std::map<UInt_t, TCutG*> R3BCADNEBULANeutron2DPar::GetNeutronCuts() const
{
    if (fNeutronCuts == nullptr)
    {
        throw std::runtime_error("R3BCADNEBULANeutron2DPar: NeutronCuts not set!");
    }

    std::map<UInt_t, TCutG*> map;

    TObjString* key;
    TIterator* nextobj = fNeutronCuts->MakeIterator();
    while ((key = (TObjString*)nextobj->Next()))
    {
        UInt_t nNeutrons = key->GetString().Atoi();
        map[nNeutrons] = (TCutG*)fNeutronCuts->GetValue(key)->Clone();
    }

    return map;
}

TCutG* R3BCADNEBULANeutron2DPar::GetNeutronCut(const Int_t n) const { return GetNeutronCuts().at(n); }

UInt_t R3BCADNEBULANeutron2DPar::GetNeutronMultiplicity(const Double_t energy, const Double_t nClusters) const
{
    // Note: it might be better to implement std::map as a member and sync between the tmap for this type of usage.
    if (fNeutronCuts == nullptr)
    {
        throw std::runtime_error("R3BCADNEBULANeutron2DPar: NeutronCuts not set!");
    }

    TObjString* key;
    TIterator* nextobj = fNeutronCuts->MakeIterator();
    while ((key = (TObjString*)nextobj->Next()))
    {
        // UInt_t nNeutrons = key->GetString().Atoi();
        if (((TCutG*)fNeutronCuts->GetValue(key))->IsInside(energy, nClusters))
        {
            return (UInt_t)key->GetString().Atoi();
        }
    }
    // The list of cuts does contain a cut for multiplicity 0, so if no match is found, the neutron multiplicity must be
    // higher than the highest saved cut.
    return GetNeutronCuts().rbegin()->first + 1;
}

ClassImp(R3BCADNEBULANeutron2DPar);
