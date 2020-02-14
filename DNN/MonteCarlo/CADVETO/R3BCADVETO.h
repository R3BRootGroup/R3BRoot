#ifndef R3BCADVETO_H
#define R3BCADVETO_H

// C++ headers:
#include <memory>
#include <string>
#include <iostream>

// ROOT headers:
#include "TClonesArray.h"
#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TObjArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TLorentzVector.h"

// FAIR headers:
#include "FairMCPoint.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

// R3BRoot headers:
#include "R3BCADDetector.h"
#include "../../../r3bdata/R3BMCStack.h"
#include "R3BCADVETOGeoPar.h"
#include "R3BCADVETOPoint.h"

class FairVolume;
class TClonesArray;
class R3BCADVETOGeoPar;

/**
 * VETO detector simulation class
 * @author Jan Mayer
 * @since 12.01.2016
 * For each simulated event, TClonesArrays are filled:
 * - VETOPoints (R3BCADVETOPoint), each representing energy deposition and light yield of a track in a paddle
 * - VETOPrimaryNeutronInteractionPoints (FairMCPoint), each representing the first interaction points of
 *   primary neutrons.
 * Suitable geometry files require proper naming of the active volume (see CheckIfSensitive) and copy numbers.
 */

class R3BCADVETO : public R3BCADDetector
{
  public:
    /** Default constructor */
    R3BCADVETO();

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param trans   position
     *@param rot     rotation */
    R3BCADVETO(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot = TGeoRotation());

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param combi   position + rotation */
    explicit R3BCADVETO(const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    /** Default Destructor */
    ~R3BCADVETO() override;

    void Initialize() override;

    Bool_t ProcessHits(FairVolume* = nullptr) override;

    void EndOfEvent() override;

    void Register() override;

    TClonesArray* GetCollection(Int_t iColl) const override;

    void Print(Option_t* = "") const override;

    void Reset() override;

    void PostTrack() override;

    Bool_t CheckIfSensitive(std::string name) override;

    // No copy and no move is allowed (Rule of three/five)
    R3BCADVETO(const R3BCADVETO&) = delete;            // copy constructor
    R3BCADVETO(R3BCADVETO&&) = delete;                 // move constructor
    R3BCADVETO& operator=(const R3BCADVETO&) = delete; // copy assignment
    R3BCADVETO& operator=(R3BCADVETO&&) = delete;      // move assignment

  private:
    TClonesArray* fVETOPoints;                          //!
    R3BCADVETOGeoPar* fVETOGeoPar; //!

    /** Track information to be stored until the track leaves the active volume. */
    Int_t fTrackID;
    Int_t fPaddleID;
    TLorentzVector fPosIn, fPosOut;
    TLorentzVector fMomIn, fMomOut;
    Double_t fTime;
    Double_t fLength;
    Double_t fELoss;
    Double_t fLightYield;
    Bool_t fLastHitDone;

    void ResetValues();

    void WriteParameterFile();

    ClassDefOverride(R3BCADVETO, 3);
};

#endif // R3BVETO_H
