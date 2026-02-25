/******************************************************************************
 *   Copyright (C) 2014 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2014-2026 Members of R3B Collaboration                     *
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

#include <FairEventHeader.h>
#include <Rtypes.h>
#include <TObject.h>
#include <cstdint>
#include <stdexcept>

class R3BEventHeader : public FairEventHeader
{
  public:
    R3BEventHeader() = default;

    ~R3BEventHeader() override = default;

    void SetExpId(const int expid) { fExpId = expid; }
    void SetRunIdR3B(const int runid) { fRunIdR3B = runid; }
    void SetParId(const int parid)
    {
        fParId = parid;
        fRunId = parid;
    }
    void SetEventno(const uint64_t eventno) { fEventno = eventno; }
    void SetTrigger(const int trigger) { fTrigger = trigger; }
    void SetTimeStamp(const uint64_t timeStamp) { fTimeStamp = timeStamp; }
    void SetTpat(const int tpat) { fTpat = tpat; }
    void SetTStart(const double tStart) { fTStart = tStart; }
    void SetTStartMaster(const double tStart) { fTStartMaster = tStart; }
    void SetTStartSimple(const double tStart) { fTStartSimple = tStart; }
    void SetTprev(const double tPrev) { fTprev = tPrev; }
    void SetTnext(const double tNext) { fTnext = tNext; }

    [[nodiscard]] int GetExpId() const { return fExpId; }
    [[nodiscard]] int GetRunIdR3B() const { return fRunIdR3B; }
    [[nodiscard]] int GetParId() const { return (fParId > 0 ? fParId : static_cast<int>(fRunId)); }
    [[nodiscard]] uint64_t GetEventno() const { return fEventno; }
    [[nodiscard]] int GetTrigger() const { return fTrigger; }
    [[nodiscard]] uint64_t GetTimeStamp() const { return fTimeStamp; }
    [[nodiscard]] int GetTpat() const { return fTpat; }

    static constexpr uint32_t MakeTpatBit(uint8_t trigNo)
    {
        return (1 <= trigNo && trigNo <= 16) ? (1 << (trigNo - 1)) : throw std::runtime_error("Bad trigNo.");
    }

    bool HasTpatTrig(int trigNo) const { return fTpat & MakeTpatBit(trigNo); }

    [[nodiscard]] double GetTStart() const { return fTStart; }

    [[nodiscard]] double GetTStartMaster() const { return fTStartMaster; }

    [[nodiscard]] double GetTStartSimple() const { return fTStartSimple; }

    [[nodiscard]] double GetTprev() const { return fTprev; }

    [[nodiscard]] double GetTnext() const { return fTnext; }

    void Register(bool Persistance = true) override{};

    auto operator=(const TNamed& obj) -> R3BEventHeader&
    {
        obj.Copy(*this);
        // auto& event_header = dynamic_cast<R3BEventHeader&>(obj);
        if (const auto* event_header = dynamic_cast<const FairEventHeader*>(&obj); event_header != nullptr)
        {
            SetRunId(event_header->GetRunId());
            SetEventTime(event_header->GetEventTime());
            SetInputFileId(event_header->GetInputFileId());
            SetMCEntryNumber(event_header->GetMCEntryNumber());
        }
        if (const auto* event_header = dynamic_cast<const R3BEventHeader*>(&obj); event_header != nullptr)
        {
            fExpId = event_header->fExpId;
            fRunIdR3B = event_header->fRunIdR3B;
            fParId = event_header->fParId;
            fEventno = event_header->fEventno;
            fTrigger = event_header->fTrigger;
            fTimeStamp = event_header->fTimeStamp;
            fTpat = event_header->fTpat;
            fTStart = event_header->fTStart;
            fTStartMaster = event_header->fTStartMaster;
            fTStartSimple = event_header->fTStartSimple;
            fTprev = event_header->fTprev;
            fTnext = event_header->fTnext;
        }
        return *this;
    }

    void Copy(TObject& obj) const override
    {
        TNamed::Copy(obj);
        // auto& event_header = dynamic_cast<R3BEventHeader&>(obj);
        if (auto* event_header = dynamic_cast<FairEventHeader*>(&obj); event_header != nullptr)
        {
            event_header->SetRunId(GetRunId());
            event_header->SetEventTime(GetEventTime());
            event_header->SetInputFileId(GetInputFileId());
            event_header->SetMCEntryNumber(GetMCEntryNumber());
        }
        if (auto* event_header = dynamic_cast<R3BEventHeader*>(&obj); event_header != nullptr)
        {
            event_header->fExpId = fExpId;
            event_header->fRunIdR3B = fRunIdR3B;
            event_header->fParId = fParId;
            event_header->fEventno = fEventno;
            event_header->fTrigger = fTrigger;
            event_header->fTimeStamp = fTimeStamp;
            event_header->fTpat = fTpat;
            event_header->fTStart = fTStart;
            event_header->fTStartMaster = fTStartMaster;
            event_header->fTStartSimple = fTStartSimple;
            event_header->fTprev = fTprev;
            event_header->fTnext = fTnext;
        }
    }

  private:
    int fExpId{};
    int fRunIdR3B{};
    int fParId{};
    uint64_t fEventno{};
    int fTrigger{};
    uint64_t fTimeStamp{};
    int fTpat{};
    double fTStart{};
    double fTStartMaster{};
    double fTStartSimple{};
    double fTprev{};
    double fTnext{};

  public:
    ClassDefOverride(R3BEventHeader, 12);
};
