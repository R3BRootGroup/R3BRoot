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

// ----------------------------------------------------------------
// -----              R3BBunchedFiberMappedData                -----
// -----         Created Jan 2018 by M.Heil        -----
// ----------------------------------------------------------------

//
// This class contains mapped data for one fiber electronics channel and
// logical signal edge, not for one fiber!
//

#ifndef R3BBUNCHEDFIBERMAPPEDDATA_H
#define R3BBUNCHEDFIBERMAPPEDDATA_H

#include "TObject.h"

class R3BBunchedFiberMappedData : public TObject
{
  public:
    R3BBunchedFiberMappedData();
    R3BBunchedFiberMappedData(Bool_t, Int_t, Bool_t, Int_t, Int_t);
    virtual ~R3BBunchedFiberMappedData();

    Int_t GetChannel() const;
    Int_t GetCoarse() const;
    Int_t GetFine() const;
    Bool_t IsMAPMT() const;
    Bool_t IsSPMT() const;
    Bool_t IsLeading() const;
    Bool_t IsTrailing() const;

  protected:
    Bool_t fIsMAPMT;
    Int_t fChannel;
    Bool_t fIsLeading;
    Int_t fCoarse;
    Int_t fFine;

  public:
    ClassDef(R3BBunchedFiberMappedData, 1)
};

#endif
