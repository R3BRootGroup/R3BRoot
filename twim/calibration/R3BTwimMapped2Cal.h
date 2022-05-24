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

// -----------------------------------------------------------------
// -----                 R3BTwimMapped2Cal                     -----
// -----        Created 24/11/19 by J.L. Rodriguez-Sanchez     -----
// -----------------------------------------------------------------

#ifndef R3BTwimMapped2Cal_H
#define R3BTwimMapped2Cal_H 1

#include "FairTask.h"
#include "R3BTwimCalData.h"
#include "R3BTwimMappedData.h"
#include "TH1F.h"
#include <TRandom.h>
#include <stdint.h>
#include <vector>

class TClonesArray;
class R3BTimeStitch;
class R3BTwimCalPar;

class R3BTwimMapped2Cal : public FairTask
{
  public:
    /** Default constructor **/
    R3BTwimMapped2Cal();

    /** Standard constructor **/
    R3BTwimMapped2Cal(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BTwimMapped2Cal();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option);

    /** Virtual method Reset **/
    virtual void Reset();

    virtual void SetParContainers();

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Method to set up the experiment ID **/
    void SetExpId(Int_t exp) { fExpId = exp; }

    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    void SetParameter();

    Int_t fNumSec;
    Int_t fNumAnodes;
    Int_t fNumEParams;
    Int_t fNumPosParams;
    Int_t fNumAnodesRef;
    Int_t fNumAnodesTrig;
    Int_t fMaxMult;
    Int_t fExpId;
    std::vector<TArrayF*> CalEParams;
    std::vector<TArrayF*> PosParams;

    Int_t mulanode[4][16 + 4];
    Double_t fE[4][20][16 + 4];
    Double_t fDT[4][20][16 + 4];

    Bool_t fOnline; // Don't store data for online

    R3BTimeStitch* fTimeStitch;
    R3BTwimCalPar* fCal_Par;         /**< Parameter container. >*/
    TClonesArray* fTwimMappedDataCA; /**< Array with Mapped-input data. >*/
    TClonesArray* fTwimCalDataCA;    /**< Array with Cal-output data. >*/

    /** Private method AddCalData **/
    // Adds a SofTwimCalData to the TwimCalCollection
    R3BTwimCalData* AddCalData(Int_t secID, Int_t anodeID, Double_t dtime, Double_t energy);

  public:
    // Class definition
    ClassDef(R3BTwimMapped2Cal, 1)
};

#endif
