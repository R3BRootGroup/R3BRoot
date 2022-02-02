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
// -----                   R3BAlpideHitData                    -----
// -----          Created 28/01/2022 by J.L. Rodriguez         -----
// -----------------------------------------------------------------

#ifndef R3BAlpideHitData_H
#define R3BAlpideHitData_H

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
    R3BAlpideHitData(TVector3 track);

    // Destructor
    virtual ~R3BAlpideHitData() {}

    // Getters
    inline const Double_t& GetTheta() const { return fTheta; }
    inline const Double_t& GetPhi() const { return fPhi; }
    inline const TVector3 GetTrack() const { return fTrack; }

  protected:
    TVector3 fTrack;
    Double_t fTheta; // Reconstructed Theta
    Double_t fPhi;   // Reconstructed Phi

  public:
    ClassDef(R3BAlpideHitData, 1)
};

#endif /* R3BAlpideHitData */
