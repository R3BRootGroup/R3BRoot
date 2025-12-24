/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include <Rtypes.h>
#include <TCutG.h>
#include <TNamed.h>

class FairParamList;

static constexpr UInt_t kMaxPoints = 40; // NOLINT

class R3BTcutPar : public TNamed
{
  public:
    /** Default constructor **/
    R3BTcutPar(const TString& cutname = "Cut");

    /** Destructor **/
    virtual ~R3BTcutPar() {}

    /** Method to print values of parameters **/
    void print();

    /** Method to store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Method to retrieve all parameters using FairRuntimeDB**/
    TCutG* getParams(FairParamList* list);

    /** Accessor functions **/
    TCutG* GetCut() { return fCut; }

    void SetMaxPoints(UInt_t p) { fMaxPoints = p; }
    void SetCut(TCutG* c) { fCut = c; }
    TString GetNameObj() { return GetName(); }

  private:
    UInt_t fMaxPoints = kMaxPoints;
    TCutG* fCut = nullptr;

  public:
    ClassDef(R3BTcutPar, 0); // NOLINT
};
