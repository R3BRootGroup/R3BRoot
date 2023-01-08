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

#ifndef R3BMusliMapped2Cal_H
#define R3BMusliMapped2Cal_H 1

#include "TArrayF.h"

#include "FairTask.h"
#include "R3BMusliCalData.h"
#include "R3BMusliMappedData.h"

#define MAX_MULT_MUSLI 100
#define MAX_NB_SIGNALS_MAP 18

class TClonesArray;
class R3BMusliCalPar;

class R3BMusliMapped2Cal : public FairTask
{

  public:
    /** Default constructor **/
    R3BMusliMapped2Cal();

    /** Standard constructor **/
    R3BMusliMapped2Cal(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BMusliMapped2Cal();

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

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    void SetParameters();

    Int_t mult_signalmap[MAX_NB_SIGNALS_MAP];
    UInt_t signal[MAX_MULT_MUSLI][MAX_NB_SIGNALS_MAP];
    Double_t energy[MAX_MULT_MUSLI][MAX_NB_SIGNALS_MAP];
    Double_t time[MAX_MULT_MUSLI][MAX_NB_SIGNALS_MAP];

    Int_t fNumSignals;
    Int_t fNumGroupsAnodes;
    Int_t fNumParamsEneFit;
    Int_t fNumParamsPosFit;
    Int_t fMaxMult;
    TArrayF* fEneCalParams;
    TArrayF* fPosCalParams;

    R3BMusliCalPar* fCal_Par;         /**< Parameter container. > */
    TClonesArray* fMusliMappedDataCA; /**< Array with Musli Mapped-input data. >*/
    TClonesArray* fMusliCalDataCA;    /**< Array with Musli Cal-output data. >*/

    Bool_t fOnline; // Don't store data for online

    /** Private method AddCalData **/
    //** Adds a MusliCalData to the anodeCalCollection
    R3BMusliCalData* AddCalData(UInt_t signal, Double_t dt, Double_t e);

  public:
    // Class definition
    ClassDef(R3BMusliMapped2Cal, 1)
};

#endif /* R3BMusliMapped2Cal_H */
