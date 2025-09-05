/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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

// ROOT headers
#include <TArrayF.h>
#include <TCutG.h>
#include <TMath.h>

// FAIR headers
#include <FairTask.h>

// R3B headers
#include "R3BFrsData.h"

class R3BIncomingIDPar;
class R3BFrsSciCalPar;
class TClonesArray;
class R3BEventHeader;
class R3BTcutPar;
class R3BCoarseTimeStitch;

/**
 * This tasks reads all detector data items for the analysis of incoming
 * projectiles from FRS.
 */
class R3BAnalysisIncomingID : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BAnalysisIncomingID();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    explicit R3BAnalysisIncomingID(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BAnalysisIncomingID();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    InitStatus Init() override;

    InitStatus ReInit() override;
    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    void Exec(Option_t* option) override;

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    void FinishEvent() override;

    void SetParContainers() override;

    void SetBetaCorrectionForZ(Double_t p0, Double_t p1, Double_t p2, Double_t Zprimary, Double_t Zoffset)
    {
        fP0 = p0;
        fP1 = p1;
        fP2 = p2;
        fZprimary = Zprimary;
        fZoffset = Zoffset;
    }

    // Accessor to select online mode
    inline void SetOnline(bool option = true) { fOnline = option; }

    // Accessor to select the MUSIC for the incoming ID
    inline void SetMusicForPID() { fUseLOS = kFALSE, fUsePspx1 = kFALSE; }

    // Accessor to select the LOS for the incoming ID
    inline void SetLosForPID() { fUseLOS = kTRUE, fUsePspx1 = kFALSE; }

    inline void SetZLosMin(double zmin) { fMinLosCharge = zmin; }

    // Acsessor to set use of trigger corrected times
    inline void SetUseTref() { fUseTref = kTRUE; }

    inline void SetNumDet(int val) { fNumDet = val; }

  private:
    void SetParameter();
    R3BCoarseTimeStitch* fTimeStitch = nullptr;
    R3BIncomingIDPar* fIncomingID_Par = nullptr;
    R3BFrsSciCalPar* fCalPar = nullptr;
    R3BEventHeader* fHeader = nullptr;
    TClonesArray* fHitItemsMus = nullptr;
    TClonesArray* fHitItemsMusli = nullptr;
    TClonesArray* fFrsDataCA = nullptr;
    TClonesArray* fHitLos = nullptr;
    TClonesArray* fTriggerLos = nullptr;
    TClonesArray* fHitPspx1_x = nullptr;
    TClonesArray* fHitPspx1_y = nullptr;
    TClonesArray* fFrsSci_Tcal = nullptr;
    TClonesArray* fFrsHitData = nullptr;

    bool fOnline = false;                       // Don't store data for online
    Bool_t fUseLOS = kFALSE, fUsePspx1 = kTRUE; // Use LOS or PSPX1 charge (otherwise MUSIC charge)
    Double_t fP0, fP1, fP2, fZprimary, fZoffset;
    Bool_t fUseTref = kFALSE; // Use trigger corrected times

    Double_t fPos_p0;
    Double_t fPos_p1;
    double fMinLosCharge = 0.;

    Int_t fNumDet = 1;
    TArrayF* fToFoffset;
    TArrayF *fPosS2Left, *fPosS2Right;
    TArrayF *fTof2InvV_p0, *fTof2InvV_p1;
    TArrayF* fBrho0_S2toCC;
    TArrayF* fDispersionS2;
    Float_t fx0_point, fy0_point, frot_ang;
    Float_t fx0_Aq, fy0_Aq, fang_Aq;
    Float_t fBeta_max, fBeta_min;
    TCutG *fCutS2 = nullptr, *fCutCave = nullptr;

    R3BFrsData* AddData(Int_t StaId,
                        Int_t StoId,
                        Double_t z,
                        Double_t aq,
                        Double_t betaval,
                        Double_t brhoval,
                        Double_t xs2,
                        Double_t xc,
                        Double_t tof);

  public:
    ClassDefOverride(R3BAnalysisIncomingID, 1); // NOLINT
};
