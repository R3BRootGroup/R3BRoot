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

// -------------------------------------------------------------------
// -----                     R3BMwpc2Mapped2Cal                  -----
// -----         Created 10/10/19 by J.L. Rodriguez-Sanchez      -----
// -------------------------------------------------------------------

#pragma once

#include "R3BMwpcCalData.h"
#include <FairTask.h>
#include <TArrayF.h>
#include <string>

class TClonesArray;
class R3BMwpc2CalPar;

class R3BMwpc2Mapped2Cal : public FairTask
{
  public:
    /** Default constructor **/
    R3BMwpc2Mapped2Cal();

    /** Standard constructor **/
    R3BMwpc2Mapped2Cal(const std::string& name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BMwpc2Mapped2Cal();

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

    R3BMwpc2CalPar* fCal_Par;        /**< Parameter container. >*/
    TClonesArray* fMwpcMappedDataCA; /**< Array with Mapped- input data. >*/
    TClonesArray* fMwpcCalDataCA;    /**< Array with Cal- output data. >*/

    /** Private method AddCalData **/
    // Adds a MwpcCalData to the MwpcCalCollection
    R3BMwpcCalData* AddCalData(Int_t plane, Int_t pad, Float_t charge);

  public:
    // Class definition
    ClassDef(R3BMwpc2Mapped2Cal, 0);
};
