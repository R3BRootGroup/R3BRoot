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

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                     R3BCalifaMappedDataAnalysis                   -----
// -----                           Version 1.0                             -----
// -----                    Created 03.02.2014 by Y.Gonzalez               -----
// -----                   Modified 20.03.2017 by P.Cabanelas              -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALIFAMAPPEDDATAANALYSIS_H
#define R3BCALIFAMAPPEDDATAANALYSIS_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BCalifaMappedDataAnalysis : public FairTask
{
  public:
    R3BCalifaMappedDataAnalysis();
    virtual ~R3BCalifaMappedDataAnalysis();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();

  private:
    Int_t fnEvents;

    TClonesArray* fRawData;

    TH1F* thc;
    TH1F* the;
    TH1F* tht;

    void CreateHistos();

    void WriteHistos();

  public:
    ClassDef(R3BCalifaMappedDataAnalysis, 1)
};

#endif
