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
// -----                          R3BEventHeader                    -----
// -----                  Created 02-06-2014 by D. Kresan           -----
// ----------------------------------------------------------------------

#ifndef R3BEVENTHEADER
#define R3BEVENTHEADER

#include "FairEventHeader.h"

class R3BEventHeader : public FairEventHeader
{
  public:
    R3BEventHeader();
    virtual ~R3BEventHeader();

    inline void SetEventno(const UInt_t& eventno) { fEventno = eventno; }
    inline void SetTrigger(const UInt_t& trigger) { fTrigger = trigger; }
    inline void SetTimeStamp(const ULong_t& timeStamp) { fTimeStamp = timeStamp; }
    inline void SetTpat(const UShort_t tpat) { fTpat = tpat; }

    inline const UInt_t& GetEventno() const { return fEventno; }
    inline const UInt_t& GetTrigger() const { return fTrigger; }
    inline const ULong_t& GetTimeStamp() const { return fTimeStamp; }
    inline const UShort_t GetTpat() const { return fTpat; }

  private:
    UInt_t fEventno;
    UInt_t fTrigger;
    ULong_t fTimeStamp;
    UShort_t fTpat;

  public:
    ClassDef(R3BEventHeader, 4)
};

#endif
