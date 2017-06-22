#ifndef R3BNEULAND_H
#define R3BNEULAND_H

#include "R3BDetector.h"
#include "TLorentzVector.h"

#include <string>
#include <memory>

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
     *@param name    detector name
     *@param active  sensitivity flag
     */
    R3BNeuland(const char* name, Bool_t active);

    /** Standard constructor.
     *@param name    detector name
     *@param geoFile name of the ROOT geometry file
     *@param active  sensitivity flag
     *@param x       position in cave
     *@param y       position in cave
     *@param z       position in cave
     *@param rot_x   rotation in cave
     *@param rot_y   rotation in cave
     *@param rot_z   rotation in cave
     */
    R3BNeuland(const char* name,
               TString geoFile,
               Bool_t active,
               Float_t x = 0.,
               Float_t y = 0.,
               Float_t z = 0.,
               Float_t rot_x = 0.,
               Float_t rot_y = 0.,
               Float_t rot_z = 0.);

    /** Default Destructor */
    // virtual ~R3BNeuland() = default;
    virtual ~R3BNeuland(){};

    virtual void Initialize(); // override;

    virtual Bool_t ProcessHits(FairVolume* = nullptr); // override;

    virtual void EndOfEvent(); // override;

    virtual void Register(); // override;

    virtual TClonesArray* GetCollection(Int_t iColl) const; // override;

    virtual void Print(Option_t* = "") const; // override;

    virtual void Reset(); // override;

    virtual void ConstructGeometry(); // override;

    virtual void PostTrack(); // override;

    virtual Bool_t CheckIfSensitive(std::string name); // override;

    /** May not be copy constructed */
    // R3BNeuland(const R3BNeuland &) = delete;
    // R3BNeuland &operator=(const R3BNeuland &) = delete;

  private:
    TClonesArray* fNeulandPoints;                            //!
    TClonesArray* fNeulandPrimaryNeutronInteractionPoints;   //!
    TClonesArray* fNeulandPrimaryNeutronInteractionPixel;    //!

    R3BNeulandGeoPar* fNeulandGeoPar;                        //!

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

    /** ROOT ClassDef
     * ClassDefOverride to avoid inconsistent-missing-override warnings
     * ClassVersionID = 0, this class should not be read/written (no IO necessary)
     */
    // ClassDefOverride(R3BNeuland, 0);
    ClassDef(R3BNeuland, 1);
};

#endif // R3BNEULAND_H
