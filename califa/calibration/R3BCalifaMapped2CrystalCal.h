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
// -----                     R3BCalifaMapped2CrystalCal                    -----
// -----                Created 18/07/2014 by H. Alvarez-Pol               -----
// -----                 Modified 20/03/2017 by P. Cabanelas               -----
// -----                 Modified 15/12/2017 by E. Galiana                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALIFAMAPPED2CRYSTALCAL_H
#define R3BCALIFAMAPPED2CRYSTALCAL_H

#include "FairTask.h"

#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMappedData.h"

#include <TRandom.h>

class R3BCalifaCrystalCalPar;
class R3BCalifaTotCalPar;

class R3BCalifaMapped2CrystalCal : public FairTask
{
  public:
    /** Default constructor **/
    R3BCalifaMapped2CrystalCal();

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
    void SetParameter();

    UInt_t fNumCrystals{};
    UInt_t fNumParams{};
    UInt_t fNumTotParams{};
    TArrayF* fCalParams{};
    TArrayF* fCalTotParams{};
    // Don't store data for online
    Bool_t fOnline{};

    R3BCalifaCrystalCalPar* fCal_Par{};  /**< Parameter container. >*/
    R3BCalifaTotCalPar* fTotCal_Par{};   /**< Tot Parameter container. >*/
    const R3BCalifaMappedData::container_t* fCalifaMappedData; 
    R3BCalifaCrystalCalData::container_t* fCalifaCryCalData=new R3BCalifaCrystalCalData::container_t;
  public:
    // Class definition
    ClassDef(R3BCalifaMapped2CrystalCal, 1)
};

#endif /* R3BCALIFAMAPPED2CRYSTALCAL_H */
