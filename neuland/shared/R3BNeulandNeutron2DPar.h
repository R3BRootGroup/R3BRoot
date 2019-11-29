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

#ifndef R3BNEULANDNEUTRON2DPAR_H
#define R3BNEULANDNEUTRON2DPAR_H

#include "FairParGenericSet.h"
#include "TCutG.h"
#include <map>
class FairParamList;
class TMap;

/**
 * NeuLAND number of clusters / energy - neutron multiplicity parameter storage
 * @author Jan Mayer
 *
 * Stores the cuts for the 2D Calibr method, can be asked about the neutron multiplicity
 */

class R3BNeulandNeutron2DPar : public FairParGenericSet
{
  public:
    // needs to be public?
    TMap* fNeutronCuts;

    R3BNeulandNeutron2DPar(const char* name = "R3BNeulandNeutron2DPar",
                           const char* title = "Neuland Neutron 2D Parameters",
                           const char* context = "TestDefaultContext");
    ~R3BNeulandNeutron2DPar() override;

    void clear() override;
    void putParams(FairParamList*) override;
    Bool_t getParams(FairParamList*) override;
    void printParams() override;

    std::map<UInt_t, TCutG*> GetNeutronCuts() const;
    TCutG* GetNeutronCut(const Int_t n) const;
    void SetNeutronCuts(const std::map<UInt_t, TCutG*>& cuts);
    UInt_t GetNeutronMultiplicity(const Double_t energy, const Double_t nClusters) const;

  private:
    R3BNeulandNeutron2DPar(const R3BNeulandNeutron2DPar&);
    R3BNeulandNeutron2DPar& operator=(const R3BNeulandNeutron2DPar&);

    ClassDefOverride(R3BNeulandNeutron2DPar, 2)
};

#endif // R3BNEULANDNEUTRON2DPAR_H
