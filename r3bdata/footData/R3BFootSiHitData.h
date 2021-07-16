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

// --------------------------------------------------------------------
// -----            R3BFootSiHitData header file                  -----
// -----       Created 16/07/21  by J.L. Rodriguez-Sanchez        -----
// --------------------------------------------------------------------

#ifndef R3BFootSiHitData_H
#define R3BFootSiHitData_H

#include "FairMultiLinkedData.h"
#include "TObject.h"
#include "TVector3.h"

class R3BFootSiHitData : public FairMultiLinkedData
{
  public:
    /** Default constructor **/
    R3BFootSiHitData();

    /** Constructor with arguments
     *@param fDetId     Detector unique identifier
     *@param fNbHit     Number of hits
     *@param fMulStrip  Number of strips with signals
     *@param fX         Position X [mm] local coordinate (detector frame)
     *@param fY         Position Y [mm] local coordinate (detector frame)
     *@param fTheta     Master: Angle theta [rad] (lab frame)
     *@param fPhi       Master: Angle Phi [rad] (lab frame)
     *@param fEnergy    Total energy deposited by the hit ([GeV] in sim)
     **/
    R3BFootSiHitData(Int_t detid,
                     Int_t nbhit,
                     Double_t x,
                     Double_t y,
                     TVector3 master,
                     Double_t energy,
                     Int_t mulstrip = 0);

    /** Copy constructor **/
    R3BFootSiHitData(const R3BFootSiHitData&);

    R3BFootSiHitData& operator=(const R3BFootSiHitData&) { return *this; }

    /** Destructor **/
    virtual ~R3BFootSiHitData() {}

    /** Accessors **/
    inline const Int_t& GetDetId() const { return fDetId; }
    inline const Int_t& GetNbHit() const { return fNbHit; }
    inline const Int_t& GetMulStrip() const { return fMulStrip; }
    inline const Double_t& GetX() const { return fX; }
    inline const Double_t& GetY() const { return fY; }
    inline const Double_t& GetTheta() const { return fTheta; }
    inline const Double_t& GetPhi() const { return fPhi; }
    inline const TVector3 GetPosLab() const { return fmaster; }
    inline const Double_t& GetEnergy() const { return fEnergy; }

  protected:
    Int_t fDetId;
    Int_t fNbHit;
    Int_t fMulStrip;
    Double_t fX, fY;
    Double_t fTheta, fPhi;
    TVector3 fmaster;
    Double_t fEnergy;

    ClassDef(R3BFootSiHitData, 0)
};

#endif /* R3BFootSiHitData_H */
