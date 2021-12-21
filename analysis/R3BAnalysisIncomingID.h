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

#ifndef R3BAnalysisIncomingID_H
#define R3BAnalysisIncomingID_H

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "R3BEventHeader.h"
#include "R3BFrsData.h"
#include "TClonesArray.h"
#include "TMath.h"
#include <TArrayF.h>
#include <cstdlib>

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"
#include "R3BMusicHitData.h"

class R3BIncomingIDPar;
class TClonesArray;
// class R3BEventHeader;

/**
 * This taks reads all detector data items and plots histograms
 * for online checks.
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
    R3BAnalysisIncomingID(const char* name, Int_t iVerbose = 1);

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
    virtual InitStatus Init();
    virtual InitStatus ReInit();
    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option);

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    virtual void FinishEvent();

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask();
    virtual void Reset();
    void SetBetaCorrectionForZ(Double_t p0, Double_t p1, Double_t p2, Double_t Zprimary, Double_t Zoffset)
    {
        fP0 = p0;
        fP1 = p1;
        fP2 = p2;
        fZprimary = Zprimary;
        fZoffset = Zoffset;
    }

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }
    virtual void SetParContainers();

  private:
    void SetParameter();
    R3BIncomingIDPar* fIncomingID_Par; // Parameter container
    TClonesArray* fHitItemsMus;
    TClonesArray* fFrsDataCA; /**< Array with FRS-output data. >*/

    TClonesArray* fHitSci2; /**< Array with Tcal items. */
    TClonesArray* fHitLos;

    R3BEventHeader* header; /**< Event header. */
    Bool_t fOnline;         // Don't store data for online
    Bool_t fUseLOS;         // Use LOS charge (otherwise MUSIC charge)
    Double_t fP0, fP1, fP2, fZprimary, fZoffset;

    Double_t fPos_p0;
    Double_t fPos_p1;

    Int_t fNumDet;
    TArrayF* fToFoffset;
    TArrayF *fPosS2Left, *fPosS2Right;
    TArrayF *fTof2InvV_p0, *fTof2InvV_p1;
    TArrayF* fBrho0_S2toCC;
    TArrayF* fDispersionS2;
    Float_t fx0_point, fy0_point, frot_ang;
    Float_t fx0_Aq, fy0_Aq, fang_Aq;
    Float_t fBeta_max, fBeta_min;

    // Parameter containers for R3BMusicPar
    UChar_t fNumMusicParams;
    TArrayF* CalZParams;
    Float_t fZ0 = 0., fZ1 = 0., fZ2 = 0.; // CalibPar for R3BMUSIC

    /** Private method FrsData **/
    //** Adds a FrsData to the analysis
    R3BFrsData* AddData(Int_t StaId,
                        Int_t StoId,
                        Double_t z,
                        Double_t aq,
                        Double_t betaval,
                        Double_t brhoval,
                        Double_t xs2,
                        Double_t xc);

  public:
    ClassDef(R3BAnalysisIncomingID, 1)
};

#endif
