#ifndef R3BCADNEULAND_H
#define R3BCADNEULAND_H

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
#include "R3BCADNeulandGeoPar.h"
#include "R3BNeulandCADNeutron.h"
#include "R3BCADNeulandPoint.h"

class FairVolume;
class TClonesArray;
class R3BCADNeulandGeoPar;

/**
 * NeuLAND detector simulation class
 * @author Jan Mayer
 * @since 12.01.2016
 * For each simulated event, TClonesArrays are filled:
 * - NeulandPoints (R3BCADNeulandPoint), each representing energy deposition and light yield of a track in a paddle
 * - NeulandPrimaryNeutronInteractionPoints (FairMCPoint), each representing the first interaction points of
 *   primary neutrons.
 * Suitable geometry files require proper naming of the active volume (see CheckIfSensitive) and copy numbers.
 */

class R3BCADNeuland : public R3BCADDetector
{
  public:
    /** Default constructor */
    R3BCADNeuland();

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param trans   position
     *@param rot     rotation */
    R3BCADNeuland(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot = TGeoRotation());

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param combi   position + rotation */
    explicit R3BCADNeuland(const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    /** Standard constructor.
     *@param nDP     number of double planes
     *@param trans   position
     *@param rot     rotation */
    R3BCADNeuland(Int_t nDP, const TGeoTranslation& trans, const TGeoRotation& rot = TGeoRotation());

    /** Standard constructor.
     *@param nDP     number of double planes
     *@param combi   position + rotation */
    explicit R3BCADNeuland(Int_t nDP, const TGeoCombiTrans& combi = TGeoCombiTrans());

    /** Default Destructor */
    ~R3BCADNeuland() override;

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
    R3BCADNeuland(const R3BCADNeuland&) = delete;            // copy constructor
    R3BCADNeuland(R3BCADNeuland&&) = delete;                 // move constructor
    R3BCADNeuland& operator=(const R3BCADNeuland&) = delete; // copy assignment
    R3BCADNeuland& operator=(R3BCADNeuland&&) = delete;      // move assignment

  private:
    TClonesArray* fNeulandPoints;                          //!
    TClonesArray* fNeulandPrimaryNeutronInteractionPoints; //!
    TClonesArray* fNeulandPrimaryNeutrons;                 //!

    R3BCADNeulandGeoPar* fNeulandGeoPar; //!

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

    ClassDefOverride(R3BCADNeuland, 3);
};

#endif // R3BNEULAND_H
