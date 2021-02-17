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

#ifndef R3BCALIFACRYSTALCAL2HIT_H
#define R3BCALIFACRYSTALCAL2HIT_H

#include "FairTask.h"
#include "R3BCalifaGeometry.h"
#include "R3BCalifaHitData.h"

#include <TVector3.h>

class TClonesArray;
class R3BTGeoPar;

class R3BCalifaCrystalCal2Hit : public FairTask
{

  public:
    /** Default constructor
     **/
    R3BCalifaCrystalCal2Hit();

    /** Destructor **/
    ~R3BCalifaCrystalCal2Hit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Reset **/
    virtual void Reset();

    /** Public method SelectGeometryVersion
     **
     ** Defines the geometry
     *@param version  Integer parameter used to select the geometry:
     ** (see documentation /r3broot/cal/perlScripts/README))
     **/
    void SelectGeometryVersion(Int_t version);

    /** Public method SetSquareWindowAlg
     **
     ** Select the square window clustering algorithm
     *@param xDeltaPolar Polar aperture
     *@param xDeltaAzimuthal Azimuthal aperture
     **/
    virtual void SetSquareWindowAlg(double xDeltaPolar = 0.25, double xDeltaAzimuthal = 0.25)
    {
        fClusterAlgorithmSelector = RECT;
        fDeltaPolar = xDeltaPolar;
        fDeltaAzimuthal = xDeltaAzimuthal;
    }

    /** Public method SetRoundWindowAlg
     **
     ** Select the round window clustering algorithm
     *@param xDeltaAngleClust  Angular aperture
     **/
    virtual void SetRoundWindowAlg(double xDeltaAngleClust = 0.25)
    {
        fClusterAlgorithmSelector = ROUND;
        fDeltaAngleClust = xDeltaAngleClust;
    }

    /** Public method SetRoundEnergyScaledAlg
     **
     ** Select the round window clustering algorithm
     *@param xDeltaAngleClust Angular aperture
     *@param xenergyFactor Energy factor
     **/
    virtual void SetRoundEnergyScaledAlg(double xDeltaAngleClust = 0.25, double xenergyFactor = 0)
    {
        fClusterAlgorithmSelector = ROUND_SCALED;
        fDeltaAngleClust = xDeltaAngleClust;
        energyFactor = xenergyFactor;
    }

    /** Public method SetConeAlg
     **
     ** Select the angular cone clustering algorithm
     *@param xDeltaAngleClust Angular aperture
     **/
    virtual void SetConeAlg(double xDeltaAngleClust = 0.25)
    {
        fClusterAlgorithmSelector = CONE;
        fDeltaAngleClust = xDeltaAngleClust;
    }

    /** Public method SetPetalAlg
     **
     ** Select the petal clustering algorithm (2018 experiments)
     **/
    virtual void SetPetalAlg() { fClusterAlgorithmSelector = PETAL; }

    /** Public method SetCrystalThreshold
     **
     ** Defines the minimum energy requested in a crystal to be considered in a calorimeter Hit
     *@param thresholdEne  Double parameter used to set the threshold
     **/
    void SetCrystalThreshold(Double_t thresholdEne);

    /** Public method SetDRThreshold (for double reading)
     **
     ** Defines the minimum energy requested in a crystal to be considered in a calorimeter Hit
     *@param thresholdEne  Double parameter used to set the threshold
     **/
    void SetDRThreshold(Double_t DRthresholdEne);

    /** Virtual method SetParContainers **/
    virtual void SetParContainers();

    /** Virtual method Finish **/
    // virtual void Finish();

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

    static int AngleToPetalId(TVector3& vec)
    {
        // internal [double] petal number, not corresponding to anything real
        // only relevant for 2019 beam times
        auto a = vec.Phi();
        if (a < 0)
        {
            if (a < -2)
                return 0;
            else
                return 1;
        }
        else
        {
            if (a < 2)
            {
                if (a < 0.2)
                    return 2;
                else
                    return 3;
            }
            else
                return 4;
        }
    }

  protected:
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Method GetAnglesVector (calls R3BCalifaGeometry::GetAngles(id)) **/
    TVector3 GetAnglesVector(int id);
    TVector3 fTargetPos;
    TVector3 fCalifaPos;
    TVector3 fCalifatoTargetPos;

    TClonesArray* fCrystalHitCA;
    TClonesArray* fCalifaHitCA;

    R3BTGeoPar* fTargetGeoPar;
    R3BTGeoPar* fCalifaGeoPar;

    Bool_t fOnline;              // Selector for online data storage
    Int_t fGeometryVersion;      // Selecting the geometry of the CALIFA calorimeter
    Int_t fNbCrystalsGammaRange; // Define max. number of crystals with gamma range
    Double_t fDeltaPolar;        // Angular window (polar angle)
    Double_t fDeltaAzimuthal;    // Angular window (azimuthal angle)
    Double_t fDeltaAngleClust;   // Angular opening used for the cluster condition
    Double_t fParCluster1;       // Clustering parameter 1
    Double_t fThreshold;         // Minimum energy requested in a crystal to be included in a Cal
    Double_t fDRThreshold;       // Threshold for selecting gamma or proton branch in double reading channels
    Double_t energyFactor;

    // Clustering algorithm selector
    enum
    {
        INVALID = 0,
        RECT = 1,
        ROUND = 2,
        ROUND_SCALED = 3,
        CONE = 4,
        ALL = 5,
        NONE = 6,
        PETAL = 7
    } fClusterAlgorithmSelector; // Clustering algorithm selector

    // Parameter class
    // R3BCalifaHitPar* fCalifaHitPar;

    R3BCalifaGeometry* fCalifaGeo;

  private:
    /** Private method AddHit
     **
     ** Adds a CalifaHit to the HitCollection
     **/
    R3BCalifaHitData* AddHit(UInt_t Nbcrystals,
                             Double_t ene,
                             Double_t Nf,
                             Double_t Ns,
                             Double_t pAngle,
                             Double_t aAngle,
                             ULong64_t time);

    /** Private method Match
     **
     ** Decides if hit is in the cluster started by ref
     **/
    virtual bool Match(R3BCalifaCrystalCalData* ref, R3BCalifaCrystalCalData* hit);

    ClassDef(R3BCalifaCrystalCal2Hit, 2);
};

#endif
