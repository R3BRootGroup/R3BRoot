/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
// -----                  R3BActafGeometry                    -----
// -----       Created 14/09/2025 by J.L. Rodriguez Sanchez    -----
// -----------------------------------------------------------------

#pragma once

#include <Rtypes.h>
#include <TFile.h>
#include <TObject.h>
#include <TVector3.h>
#include <iostream>
#include <sstream>

class TVector3;
class TGeoNavigator;
class TRotation;

/**
 * Geometrical queries to ALPIDE device
 */

class R3BActafGeometry : public TObject
{
  public:
    /** Default constructor */
    R3BActafGeometry() = default;

    /** Standard constructor.
     *@param version geometry version
     */
    R3BActafGeometry(Int_t version);

    /** Destructor **/
    virtual ~R3BActafGeometry();

    // Initialization with a Root version
    bool Init(int fGeo);

    /**
     * Gets volume path of pad with given ID.
     *
     * @param iD pad ID (depending on geometry version)
     * @return Volume path
     */
    std::string GetPadVolumePath(int iD);

    /**
     * Gets position in polar coordinates of pad with given ID.
     * On error, the x,y and z component of the TVector3 are set to NAN.
     * @param iD crystal ID (depending on geometry version)
     */
    const TVector3& GetPosition(int iD);

    /**
     * Gets ring ID for given volume path.
     *
     * @param volumePath Volume path
     * @return ring ID
     */
    int GetRingId(const std::string volumePath);

    /**
     * Gets ring ID for given pad ID.
     *
     * @param iD    pad ID
     * @return ring ID
     */
    int GetRingId(const int iD);

    /**
     * Gets pad ID for given volume path.
     *
     * @param volumePath Volume path
     * @return pad ID
     */
    int GetPadId(const std::string volumePath);

    /**
     * @return if we are running the simulation or data analysis
     */
    bool IsSimulation() const { return fIsSimulation; }

    /**
     * Returns singleton instance of R3BActafGeometry for given geometry version.
     * @return Instance of R3BActafGeometry
     */
    static R3BActafGeometry* Instance();

  private:
    Int_t fGeometryVersion = 2025;
    Int_t fNbPads = 128;
    Bool_t fIsSimulation = false;
    Bool_t IsInitialize = false;
    TFile* f;

    ClassDef(R3BActafGeometry, 0);
};
