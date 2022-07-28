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

// -------------------------------------------------------------
// -----             R3BTwimVertexReconstruction                        -----
// -----    Created 30/11/19 by J.L. Rodriguez-Sanchez     -----
// -------------------------------------------------------------

#ifndef R3BTwimVertexReconstruction_H
#define R3BTwimVertexReconstruction_H 1

#include "FairTask.h"
#include "R3BTwimHitData.h"
#include "TF1.h"

class TClonesArray;
class R3BEventHeader;

class R3BTwimVertexReconstruction : public FairTask
{

  public:
    /** Default constructor **/
    R3BTwimVertexReconstruction();

    /** Standard constructor **/
    R3BTwimVertexReconstruction(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BTwimVertexReconstruction();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option);

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    inline void SetTpat(Int_t tpat) { fTpat = tpat; }

  private:
    R3BEventHeader* header; /**< Event header. */
    Int_t fTpat;

    TClonesArray* fTwimHitDataCA; /**< Array with Twim Hit-output data. >*/

    TF1 *f1, *f2;

  public:
    // Class definition
    ClassDef(R3BTwimVertexReconstruction, 1)
};

#endif
