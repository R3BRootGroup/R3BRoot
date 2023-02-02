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

#ifndef R3BCALIFAGEOMETRY_H
#define R3BCALIFAGEOMETRY_H

#include "Rtypes.h"
#include "TObject.h"
#include <TFile.h>
#include <TObject.h>
#include "R3BLogger.h"
#include <Math/Vector3D.h>

class TGeoNavigator;

/**
 * Geometrical queries to CALIFA
 */
class R3BCalifaGeometry : public TObject
{
  // TODO: this class should be converted into a self-contained program which
  // writes out a text file containing the position of crystals.
  // That text file should be loaded with the mapping.
  // For simulation, a temporary text file (e.g. man 3 mptemp) should be used. 
  // In general, it would be nice to have one interface which can be queried for
  // all info of CALIFA crystals, be it mapping, gain range (from EPICS?)
  // calibration, position, etc.
  // for now, I am turning this into a proper singleton (private constructor)
  // instead of the semi-singleton state I found it in.
  // The first time you call Instance(), set the geo version.
  // Later calls (e.g. in Cal2Cluster) may then skip the geo version. --Philipp
 protected:
  /** Standard constructor.
   *@param version geometry version
   */
  R3BCalifaGeometry(int version);

 public:
    /**
     * Returns singleton instance of R3BCalifaGeometry for given geometry version.
     * Warning: Switching the geometry version at run time will lead to undefined beheaviour!
     *
     * @param version Geometry version to use. If in doubt, use 2020. 
     * The default (version=0) will only fetch an allready instantiated version and fail
     *  if there is none. 
     * @return Instance of R3BCalifaGeometry
     */
    static R3BCalifaGeometry* Instance(Int_t version=0);
  
    /**
     * Gets position in polar coordinates of crystal with given ID.
     * On error, the x,y and z component of the ROOT::Math::XYZVector are set to NAN.
     * @param iD crystal ID (depending on geometry version)
     */
    const ROOT::Math::XYZVector& GetCrystalCenterPos(int cryID);

    /**
     * Legacy: Gets position in polar coordinates of crystal with given ID.
     *
     * Before this comment, the last three parameters were untouched on error.
     * Now, they should be set to NAN;
     *
     * @param iD crystal ID (depending on geometry version)
     * @param polar [out] Will be filled with polar angle (radians) of crystal center
     * @param azimuthal [out] Will be filled with azimuthal angle (radians) of crystal center
     * @param rho [out] Will be filled with distance (cm) of crystal center to target position (0,0,0)
     */
    [[deprecated("try to use the GetCrystalCenterPos")]]
    void GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal, Double_t* rho);

    /**
     * Gets volume path of crystal with given ID.
     *
     * @param iD crystal ID (depending on geometry version)
     * @return Volume path
     */
    const char* GetCrystalVolumePath(Int_t iD);

    /**
     * Gets crystal ID for given volume path.
     *
     * @param volumePath Volume path
     * @return Crystal ID
     */
    int GetCrystalId(const char* volumePath);

    /**
     * Calculate the distance of a given straight track through the active detector volume (crystal(s)). Usefull for
     * iPhos.
     *
     * @param startVertex Start position of track (e.g. (0,0,0) for center of target)
     * @param direction Direction of the track (does not need to be normalized)
     * @param hitPos [out] (optional) If given, this ROOT::Math::XYZVector will be filled with the position of the first contact
     * between the track and the first crystal along the path.
     * @param numCrystals [in,out]
     * \parblock
     *  (optional) If given together with crystalIDs,
     *  this function will fill numCrystals with the number of crystals along the straight
     *  and the array crystalIds with the IDs of the crystals along the straight.
     *  The array crystalIds MUST be allocated by the caller.
     *  The maximum size of the array has to be given with the content of numCrystals.
     *  The function will stop filling the array after reaching the specified, maximum capacity.
     * \endparblock
     * @param crystalIds [out] (optional, array) See numCrystals.
     * @return Distance (cm) through crystal(s)
     */
    double GetDistanceThroughCrystals(ROOT::Math::XYZVector& startVertex,
                                      ROOT::Math::XYZVector& direction,
                                      ROOT::Math::XYZVector* hitPos = NULL,
                                      Int_t* numCrystals = NULL,
                                      Int_t* crystalIds = NULL);

    /**
     * @return if we are running the simulation or data analysis
     */
    bool IsSimulation() const { return fIsSimulation; } 

    [[deprecated("Initialization is now done in Instance()")]]
    bool Init(Int_t fGeo)
  {
    assert(this->fGeometryVersion==fGeo);
    return 1;
  }
    
    size_t GetNumChannels() const {return fNumChannels;} 

    inline bool IsValid(int cryId) { return 1<=cryId && cryId <=fNumChannels;}
    inline bool IsHighGain(int cryId) { return cryId<=fNumChannels/2 ;}
    inline size_t ToLowGain(int cryId)
    {
      return cryId + fNumChannels/2;
    }
    inline size_t ToHighGain(int cryId)
    {
      return cryId - fNumChannels/2;
    }
      

  private:
    int fGeometryVersion {-1};
    int fNumChannels{2432*2};
    bool fIsSimulation{};
    TFile* f{}; // following the standard, this thing should be called fF :-P --pklenze

    ClassDef(R3BCalifaGeometry, 8);
};

#endif /* R3BCALIFAGEOMETRY_H */
