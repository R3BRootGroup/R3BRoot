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

#ifndef R3BEVENTHEADER_h
#define R3BEVENTHEADER_h 1

#include "FairEventHeader.h"
#include <stdexcept>

class R3BEventHeader : public FairEventHeader
{
  public:
    R3BEventHeader();
    virtual ~R3BEventHeader();

    void SetExpId(const Int_t expid) { fExpId = expid; }
    void SetEventno(const uint64_t eventno) { fEventno = eventno; }
    void SetTrigger(const Int_t trigger) { fTrigger = trigger; }
    void SetTimeStamp(const uint64_t timeStamp) { fTimeStamp = timeStamp; }
    void SetTpat(const Int_t tpat) { fTpat = tpat; }
    void SetTStart(const Double_t tStart) { fTStart = tStart; }

    Int_t GetExpId() const { return fExpId; }
    uint64_t GetEventno() const { return fEventno; }
    Int_t GetTrigger() const { return fTrigger; }
    uint64_t GetTimeStamp() const { return fTimeStamp; }
    Int_t GetTpat() const { return fTpat; }

    static constexpr uint32_t MakeTpatBit(uint8_t trigNo)
    {
        return (1 <= trigNo && trigNo <= 16) ? (1 << (trigNo - 1)) : throw std::runtime_error("Bad trigNo.");
    }

    bool HasTpatTrig(int trigNo) const { return fTpat & MakeTpatBit(trigNo); }

    Double_t GetTStart() const { return fTStart; }

    virtual void Register(Bool_t Persistance = kTRUE);

  private:
    Int_t fExpId;
    uint64_t fEventno;
    Int_t fTrigger;
    uint64_t fTimeStamp;
    Int_t fTpat;
    Double_t fTStart;

    ClassDef(R3BEventHeader, 8)
};

#endif /* R3BEVENTHEADER_h */
