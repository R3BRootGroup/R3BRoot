/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
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

#include <FairTask.h>
#include <TClonesArray.h>
#include <cstdint>
#include <list>

class TH1F;
class TH2F;
class R3BEventHeader;
class R3BCoarseTimeStitch;
class R3BFiberMAPMTCalData;
class R3BFiberMAPMTHitPar;
class R3BFiberMappingPar;
class R3BFiberMAPMTHitModulePar;

class R3BFiberMAPMTCal2Hit : public FairTask
{
  public:
    // This defines the fiber direction
    enum Direction
    {
        HORIZONTAL,
        VERTICAL
    };
    // This tells us where the first fiber is located:
    // VERTICAL direction ---------------------------------
    // STANDARD: fiber 1 wixhausen -> fiber 512 messel
    // INVERTED: fiber 1 messel -> fiber 512 wixhausen
    // HORIZONTAL direction -------------------------------
    // STANDARD: fiber 1 down -> fiber 512 up
    // INVERTED: fiber 1 up -> fiber 512 down
    enum Orientation
    {
        STANDARD,
        INVERTED
    };

    struct ToT
    {
        ToT(R3BFiberMAPMTCalData const*, R3BFiberMAPMTCalData const*, Double_t, Double_t, Double_t);
        R3BFiberMAPMTCalData const* lead;
        R3BFiberMAPMTCalData const* trail;
        Double_t lead_ns, tail_ns, tot_ns;
    };
    struct Channel
    {
        std::list<R3BFiberMAPMTCalData const*> lead_list;
        std::list<ToT> tot_list;
    };

    struct Fib_Hit
    {
        void Reset();
        Double_t time[2][2]; //[0-bottom, 1-top][0-lead, 1 -trail]
        int channel;
    };

    explicit R3BFiberMAPMTCal2Hit(const std::string&,
                                  int v = 1, // verbosity
                                  Direction dir = HORIZONTAL,
                                  uint32_t fnb = 512, // fiber number
                                  bool iscal = false  // is calib
    );

    virtual ~R3BFiberMAPMTCal2Hit();

    InitStatus Init() override;

    InitStatus ReInit() override;

    void SetParContainers() override;

    void Exec(Option_t*) override;

    void FinishEvent() override;

    void FinishTask() override;

    inline void SetTofWindow(Double_t tmin, Double_t tmax)
    {
        ftofmin = tmin;
        ftofmax = tmax;
    }

    inline void SetDTimeWindow(double dtwin) { fDTime_window = dtwin; }

    inline void SetWriteHisto() { fWrite = true; }

    inline void SetTofMin(Double_t min) { ftofmin = min; }

    inline void SetTofMax(Double_t max) { ftofmax = max; }

    inline void SetGate(Double_t g) { fGate_ns = g; }

    inline void SetOrientation(Orientation opt) { fOrientation = opt; }

    // Accessor to select online mode
    inline void SetOnline(bool option = true) { fOnline = option; }

  private:
    TString fName;
    int fDetId = 1;
    unsigned long long fnEvents = 0;
    uint32_t fNumFibers = 512;
    int multi = 0;
    double fClockFreq = 150.;
    double fGate_ns = 100.;

    double tsync = 0.;
    double gainUp = 10.;
    double gainDown = 10.;
    double offsetDT = 0.;
    bool fIsCalibrator = false;
    double ftofmin = -1000, ftofmax = 1000;
    bool fWrite = false;
    // Don't store data for online
    bool fOnline = false;
    double fDTime_window = 20.; // ns

    R3BEventHeader* fHeader = nullptr;
    R3BCoarseTimeStitch* fTimeStitch = nullptr;
    Direction fDirection;
    Orientation fOrientation = STANDARD;
    TClonesArray* fCalItems = nullptr;
    TClonesArray* fCalTriggerItems = nullptr;
    TClonesArray* fHitItems = nullptr;
    R3BFiberMappingPar* fMapPar = nullptr;
    R3BFiberMAPMTHitPar* fCalPar = nullptr; // Parameter container
    R3BFiberMAPMTHitPar* fHitPar = nullptr; // Hit parameter container
    int fNofHitPars = 0;                    // Number of modules in parameter file
    // [0=bottom,1=top][Channel].
    std::vector<Channel> fChannelArray[2];

    // histograms for gain matching
    TH2F* fh_ToT_bottom_Fib_raw;
    TH2F* fh_ToT_top_Fib_raw;
    TH2F* fh_ToT_bottom_Fib;
    TH2F* fh_ToT_top_Fib;
    TH2F* fh_time_top_Fib;
    TH2F* fh_dt_Fib;
    TH2F* fh_Fib_ToF;
    TH2F* fh_dt_Fib_raw;
    TH2F* fh_Fib_ToF_raw;
    TH2F* fh_time_check_tsync;
    TH1F* fh_multi;
    TH2F* fh_time_bottom_Fib;

  public:
    // Class definition
    ClassDefOverride(R3BFiberMAPMTCal2Hit, 3); // NOLINT
};
