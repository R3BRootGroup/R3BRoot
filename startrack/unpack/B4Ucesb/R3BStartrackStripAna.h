/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        from R3BCaloCrystalAna                     -----
// -----                    Created 18/07/14  by H.Alvarez                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKSTRIPANA_H
#define R3BSTARTRACKSTRIPANA_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BStartrackStripAna : public FairTask
{
  public:
    R3BStartrackStripAna();
    virtual ~R3BStartrackStripAna();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();

  private:
    Int_t fnEvents;

    TClonesArray* fSiDetData;

    // TH1F *thWordType;
    // TH1F *thHitBit;
    TH1F* thModuleID;
    TH1F* thSide;
    TH1F* thAsicID;
    TH1F* thStripID;
    TH1F* thEnergy;
    TH1F* thTime;

    void CreateHistos();

    void WriteHistos();

  public:
    ClassDef(R3BStartrackStripAna, 0)
};

#endif
