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

// ----------------------------------------------------------------------
// -----              R3BFiberMappedData header file                -----
// -----         Created 13/04/22 by J.L. Rodriguez-Sanchez         -----
// ----------------------------------------------------------------------

#ifndef R3BFiberMappedData_H
#define R3BFiberMappedData_H 1

#include "TObject.h"

class R3BFiberMappedData : public TObject
{
  public:
    R3BFiberMappedData();
    R3BFiberMappedData(UInt_t, UInt_t, Bool_t, Int_t, Int_t);
    virtual ~R3BFiberMappedData() {};

    inline UInt_t GetSide() const;
    inline UInt_t GetChannel() const;
    inline Int_t GetCoarse() const;
    inline Int_t GetFine() const;
    inline Bool_t IsTop() const;
    inline Bool_t IsBottom() const;
    inline Bool_t IsTrigger() const;
    inline Bool_t IsLeading() const;
    inline Bool_t IsTrailing() const;
    inline Bool_t IsMAPMT() const;
    inline Bool_t IsSPMT() const;
    inline Bool_t IsMAPMTTrigger() const;
    inline Bool_t IsSPMTTrigger() const;

  protected:
    // fSide is defined as follows:
    // 1 = bottom, 2 = top, 3 = MAPMT trigger (Fib3X)
    // 1 = MAPMT, 2 = SPMT, 3 = MAPMT trigger, 4 = SPMT trigger (Fib10, 11,...)
    UInt_t fSide;
    UInt_t fChannel;
    Bool_t fIsLeading;
    Int_t fCoarse;
    Int_t fFine;

  public:
    ClassDef(R3BFiberMappedData, 1)
};

#endif /* R3BFiberMappedData_H */
