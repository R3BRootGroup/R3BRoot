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

// -------------------------------------------------------------------------
// -----           R3BStartrackCalibParFinder source file              -----
// -----           from R3BCaloCalibParFinder source file              -----
// -----              Created 22/07/14  by H.Alvarez                   -----
// -------------------------------------------------------------------------

#ifndef R3BSTARTRACKCALIBPARFINDER_H
#define R3BSTARTRACKCALIBPARFINDER_H

#include "FairTask.h"
#include "R3BStartrackCalibPar.h"
#include "R3BStartrackHit.h"
#include "R3BStartrackerHit.h"

class TClonesArray;

class R3BStartrackCalibParFinder : public FairTask
{

  public:
    /** Default constructor **/
    R3BStartrackCalibParFinder();

    /** Destructor **/
    ~R3BStartrackCalibParFinder();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Reset **/
    virtual void Reset();

  protected:
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Register **/
    virtual void Register();

    /** Virtual method SetParContainers **/
    virtual void SetParContainers();

    /** Virtual method Finish **/
    virtual void Finish();

    TClonesArray* fSiDetRawHitCA;

    // Parameter class
    R3BStartrackCalibPar* fStartrackCalibPar;

    ClassDef(R3BStartrackCalibParFinder, 1);
};

#endif
