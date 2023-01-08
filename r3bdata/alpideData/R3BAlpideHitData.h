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

// -----------------------------------------------------------------
// -----                   R3BAlpideHitData                    -----
// -----          Created 28/01/2022 by J.L. Rodriguez         -----
// -----------------------------------------------------------------

#ifndef R3BAlpideHitData_H
#define R3BAlpideHitData_H 1

#include "TObject.h"
#include "TVector3.h"
#include <stdint.h>

class R3BAlpideHitData : public TObject
{
  public:
    // Default Constructor
    R3BAlpideHitData();

    /** Standard Constructor
     *@param track    Reconstructed trajectory
     **/
    R3BAlpideHitData(UInt_t sensorId, UInt_t clustersize, Double_t, Double_t, Double_t z = 0.0);

    // Destructor
    virtual ~R3BAlpideHitData() {}

    // Getters
    inline const UInt_t& GetSensorId() const { return fSensorId; }
    inline const UInt_t& GetClusterSize() const { return fClustersize; }
    inline const Double_t& GetPosl() const { return fX; }
    inline const Double_t& GetPost() const { return fY; }
    inline const Double_t& GetTheta() const { return fTheta; }
    inline const Double_t& GetPhi() const { return fPhi; }
    inline const Double_t& GetX() const { return fX; }
    inline const Double_t& GetY() const { return fY; }
    inline const Double_t& GetZ() const { return fZ; }
    inline const TVector3 GetTrack() const { return fTrack; }

  protected:
    TVector3 fTrack;
    UInt_t fSensorId;
    UInt_t fClustersize;
    Double_t fX;
    Double_t fY;
    Double_t fZ;
    Double_t fTheta; // Reconstructed Theta
    Double_t fPhi;   // Reconstructed Phi

  public:
    ClassDef(R3BAlpideHitData, 1)
};

#endif /* R3BAlpideHitData */
