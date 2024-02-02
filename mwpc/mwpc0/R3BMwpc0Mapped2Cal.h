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

// ----------------------------------------------------------------
// -----                 R3BMwpc0Mapped2Cal                   -----
// -----        Created 07/10/19 by J.L. Rodriguez-Sanchez    -----
// ----------------------------------------------------------------

#ifndef R3BMwpc0Mapped2Cal_H
#define R3BMwpc0Mapped2Cal_H 1

#include "FairTask.h"
#include "R3BMwpcCalData.h"
#include <TRandom.h>

class TClonesArray;
class R3BMwpc0CalPar;

class R3BMwpc0Mapped2Cal : public FairTask
{

  public:
    /** Default constructor **/
    R3BMwpc0Mapped2Cal();

    /** Standard constructor **/
    R3BMwpc0Mapped2Cal(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BMwpc0Mapped2Cal();

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

    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    void SetParameter();

    Int_t NumPadX;
    Int_t NumPadY;
    Int_t NumParams;
    TArrayF* CalParams;

    Bool_t fOnline; // Don't store data for online

    R3BMwpc0CalPar* fCal_Par;        /**< Parameter container. >*/
    TClonesArray* fMwpcMappedDataCA; /**< Array with Mapped input data. >*/
    TClonesArray* fMwpcCalDataCA;    /**< Array with Cal output data. >*/

    /** Private method AddCalData **/
    R3BMwpcCalData* AddCalData(Int_t plane, Int_t pad, Float_t charge);

  public:
    // Class definition
    ClassDef(R3BMwpc0Mapped2Cal, 1)
};

#endif
