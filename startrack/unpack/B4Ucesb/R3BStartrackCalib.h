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

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloCalib                            -----
// -----                Created 18/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKCALIB_H
#define R3BSTARTRACKCALIB_H

#include "FairTask.h"
#include "R3BStartrackCalibPar.h"
#include "R3BStartrackRawHit.h"
#include "R3BStartrackerDigitHit.h"

class TClonesArray;

class R3BStartrackCalib : public FairTask
{

  public:
    /** Default constructor **/
    R3BStartrackCalib();

    /** Destructor **/
    virtual ~R3BStartrackCalib();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Reset **/
    virtual void Reset();

  protected:
    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method SetParContainers **/
    virtual void SetParContainers();

    /** Virtual method Finish **/
    virtual void Finish();

    virtual void Register();

  private:
    // private functions making most of the calibration work
    Int_t MapModuleID(R3BStartrackRawHit* chit);
    Int_t MapSide(R3BStartrackRawHit* chit);
    Int_t MapAsicID(R3BStartrackRawHit* chit);
    Int_t MapStripID(R3BStartrackRawHit* chit);
    Double32_t CalibrateEnergy(R3BStartrackRawHit* chit);
    Double32_t CalibrateTime(R3BStartrackRawHit* chit);

    TClonesArray* fRawHitCA;
    TClonesArray* fSiDetHitCA;

    R3BStartrackCalibPar* fStartrackCalibPar;

  public:
    // Class definition
    ClassDef(R3BStartrackCalib, 0)
};

#endif
