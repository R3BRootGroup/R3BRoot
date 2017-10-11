#ifndef R3BNEULAND_H
#define R3BNEULAND_H

#include "R3BDetector.h"
#include "TLorentzVector.h"
#include <memory>
#include <string>

class FairVolume;
class TClonesArray;
class R3BNeulandGeoPar;

/**
 * NeuLAND detector simulation class
 * @author Jan Mayer
 * @since 12.01.2016
 * For each simulated event, TClonesArrays are filled:
 * - NeulandPoints (R3BNeulandPoint), each representing energy deposition and light yield of a track in a paddle
 * - NeulandPrimaryNeutronInteractionPoints (FairMCPoint), each representing the first interaction points of
 *   primary neutrons.
 * Suitable geometry files require proper naming of the active volume (see CheckIfSensitive) and copy numbers.
 */

class R3BNeuland : public R3BDetector
{
  public:
    /** Default constructor */
    R3BNeuland();

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param trans   position
     *@param rot     rotation */
    R3BNeuland(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot = TGeoRotation());

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param combi   position + rotation */
    R3BNeuland(const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    /** Default Destructor */
    ~R3BNeuland();

    virtual void Initialize(); // override;

    virtual Bool_t ProcessHits(FairVolume* = nullptr); // override;

    virtual void EndOfEvent(); // override;

    virtual void Register(); // override;

    virtual TClonesArray* GetCollection(Int_t iColl) const; // override;

    virtual void Print(Option_t* = "") const; // override;

    virtual void Reset(); // override;

    virtual void PostTrack(); // override;

    virtual Bool_t CheckIfSensitive(std::string name); // override;

    /** May not be copy constructed */
    // R3BNeuland(const R3BNeuland &) = delete;
    // R3BNeuland &operator=(const R3BNeuland &) = delete;

  private:
    TClonesArray* fNeulandPoints;                          //!
    TClonesArray* fNeulandPrimaryNeutronInteractionPoints; //!
    TClonesArray* fNeulandPrimaryNeutronInteractionPixel;  //!

    R3BNeulandGeoPar* fNeulandGeoPar; //!

    /** Track information to be stored until the track leaves the active volume. */
    // TODO: Just fill an existing R3BLandPoint in the TClonesArray or
    //       Use an R3BLandPoint as Member Variable to move construct into TClonesarray?
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

    // ClassDefOverride(R3BNeuland, 0);
    ClassDef(R3BNeuland, 2);
};

#endif // R3BNEULAND_H
