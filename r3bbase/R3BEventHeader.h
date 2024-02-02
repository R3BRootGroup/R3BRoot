/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
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
    void SetTprev(const Double_t tPrev) { fTprev = tPrev; }
    void SetTnext(const Double_t tNext) { fTnext = tNext; }

    [[nodiscard]] Int_t GetExpId() const { return fExpId; }
    [[nodiscard]] uint64_t GetEventno() const { return fEventno; }
    [[nodiscard]] Int_t GetTrigger() const { return fTrigger; }
    [[nodiscard]] uint64_t GetTimeStamp() const { return fTimeStamp; }
    [[nodiscard]] Int_t GetTpat() const { return fTpat; }

    static constexpr uint32_t MakeTpatBit(uint8_t trigNo)
    {
        return (1 <= trigNo && trigNo <= 16) ? (1 << (trigNo - 1)) : throw std::runtime_error("Bad trigNo.");
    }

    bool HasTpatTrig(int trigNo) const { return fTpat & MakeTpatBit(trigNo); }

    [[nodiscard]] Double_t GetTStart() const { return fTStart; }

    [[nodiscard]] Double_t GetTprev() const { return fTprev; }

    [[nodiscard]] Double_t GetTnext() const { return fTnext; }

    virtual void Register(Bool_t Persistance = kTRUE);

  private:
    Int_t fExpId;
    uint64_t fEventno;
    Int_t fTrigger;
    uint64_t fTimeStamp;
    Int_t fTpat;
    Double_t fTStart;
    Double_t fTprev;
    Double_t fTnext;

    ClassDef(R3BEventHeader, 9)
};

#endif /* R3BEVENTHEADER_h */
