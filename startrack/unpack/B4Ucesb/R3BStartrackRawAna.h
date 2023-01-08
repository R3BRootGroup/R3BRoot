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
// -----                        R3BStartrackRawAna                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.04.2014 by M.Labiche                -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKRAWANA_H
#define R3BSTARTRACKRAWANA_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BStartrackRawAna : public FairTask
{
  public:
    R3BStartrackRawAna();
    virtual ~R3BStartrackRawAna();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();

  private:
    Int_t fnEvents;

    TClonesArray* fRawData;

    TH1F* thw;
    TH1F* thh;
    TH1F* thm;
    TH1F* thsd;
    TH1F* tha;
    TH1F* thst;
    TH1F* the;
    TH1F* thts;
    TH1F* thtslbdiff;
    TH1F* thtsExt;
    TH1F* thtsExtlbdiff;
    TH1F* thif;
    TH1F* thic;

    void CreateHistos();

    void WriteHistos();

  public:
    ClassDef(R3BStartrackRawAna, 0)
};

#endif
