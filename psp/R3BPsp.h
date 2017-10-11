#ifndef R3BPSP_H
#define R3BPSP_H

#include "R3BDetector.h"
#include "TLorentzVector.h"

class TClonesArray;
class R3BPspPoint;
class R3BTGeoPar;
class FairVolume;

class R3BPsp : public R3BDetector
{
  public:
    /** Default constructor **/
    R3BPsp();

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param trans   position
     *@param rot     rotation
     *@param z1      psp1 z position
     *@param z2      psp2 z position
     *@param z2      psp2 z position
     */
    R3BPsp(const TString& geoFile,
           const TGeoTranslation& trans,
           const TGeoRotation& rot = TGeoRotation(),
           const Float_t z1 = -221.,
           const Float_t z2 = -89.,
           const Float_t z3 = 94.1);

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param combi   position + rotation
     *@param z1      psp1 z position
     *@param z2      psp2 z position
     *@param z2      psp2 z position
     */
    R3BPsp(const TString& geoFile,
           const TGeoCombiTrans& combi = TGeoCombiTrans(),
           const Float_t z1 = -221.,
           const Float_t z2 = -89.,
           const Float_t z3 = 94.1);

    /** Destructor **/
    ~R3BPsp();

    /** Virtual method ProcessHits
    **
    ** Defines the action to be taken when a step is inside the
    ** active volume. Creates a R3BPspPoint and adds it to the
    ** collection.
    *@param vol  Pointer to the active volume
    **/
    virtual Bool_t ProcessHits(FairVolume* vol = 0);

    /** Virtual method BeginEvent
     **
     ** If verbosity level is set, print hit collection at the
     ** end of the event and resets it afterwards.
     **/

    virtual void BeginEvent();

    /** Virtual method EndOfEvent
     **
     ** If verbosity level is set, print hit collection at the
     ** end of the event and resets it afterwards.
     **/

    virtual void EndOfEvent();

    /** Virtual method Register
     **
     ** Registers the hit collection in the ROOT manager.
     **/
    virtual void Register();

    /** Accessor to the hit collection **/
    virtual TClonesArray* GetCollection(Int_t iColl) const;

    /** Virtual method Print
     **
     ** Screen output of hit collection.
     **/
    virtual void Print(Option_t* option = "") const;

    /** Virtual method Reset
     **
     ** Clears the hit collection
     **/
    virtual void Reset();

    /** Virtual method CopyClones
     **
     ** Copies the hit collection with a given track index offset
     *@param cl1     Origin
     *@param cl2     Target
     *@param offset  Index offset
     **/
    virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset);

    /** Virtual method Construct geometry
     **
     ** Constructs the STS geometry
     **/
    virtual void ConstructGeometry();
    virtual Bool_t CheckIfSensitive(std::string name);
    virtual void Initialize();
    virtual void SetSpecialPhysicsCuts();

    //  void SaveGeoParams();

  private:
    /** Track information to be stored until the track leaves the
    active volume. **/
    Int_t fTrackID;                 //!  track index
    Int_t fVolumeID;                //!  volume id
    TLorentzVector fPosIn, fPosOut; //!  position
    TLorentzVector fMomIn, fMomOut; //!  momentum
    Double32_t fTime_in;            //!  time when entering active volume
    Double32_t fTime_out;           //!  time when exiting active volume
    Double32_t fTime;               //!  time
    Double32_t fLength_in;          //!  length when entering active volume
    Double32_t fLength_out;         //!  length when exiting active volume
    Double32_t fLength;             //!  length
    Double32_t fELoss;              //!  energy loss
    Int_t fPosIndex;                //!
    TClonesArray* fPspCollection;   //!  The hit collection
    Bool_t kGeoSaved;               //!
    TList* flGeoPar;                //!
    R3BTGeoPar* fTGeoPar;           //!

    Float_t fZ1; //! z position of PSP1
    Float_t fZ2; //! z position of PSP2
    Float_t fZ3; //! z position of PSP3

    /** Private method AddHit
     **
     ** Adds a PspPoint to the HitCollection
     **/
    R3BPspPoint* AddHit(Int_t trackID,
                        Int_t detID,
                        Int_t plane,
                        TVector3 posIn,
                        TVector3 pos_out,
                        TVector3 momIn,
                        TVector3 momOut,
                        Double_t time,
                        Double_t length,
                        Double_t eLoss);

    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();

    ClassDef(R3BPsp, 3);
};

inline void R3BPsp::ResetParameters()
{
    fTrackID = fVolumeID = 0;
    fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fTime = fLength = fELoss = 0;
    fPosIndex = 0;
};

#endif
