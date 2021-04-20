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
// -----              R3BSfibMappedData                -----
// -----         Created Nov 2020 by A. Falduto        -----
// ----------------------------------------------------------------

#ifndef R3BSFIBMAPPEDDATA_H
#define R3BSFIBMAPPEDDATA_H

#include "TObject.h"

class R3BSfibMappedData : public TObject
{
  public:
    R3BSfibMappedData();
    R3BSfibMappedData(Int_t, Int_t, Bool_t, Int_t, Int_t);
    virtual ~R3BSfibMappedData();

    Int_t GetChannel() const;
    Int_t GetCoarse() const;
    Int_t GetFine() const;
    Bool_t IsTop() const;
    Bool_t IsBottom() const;
    Bool_t IsLeading() const;
    Bool_t IsTrailing() const;

  protected:
    Int_t fIsBottom;
    Int_t fChannel;
    Bool_t fIsLeading;
    Int_t fCoarse;
    Int_t fFine;

  public:
    ClassDef(R3BSfibMappedData, 1)
};

#endif
