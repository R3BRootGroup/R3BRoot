
#ifndef R3B_TRACKING_DETECTOR
#define R3B_TRACKING_DETECTOR

#include "TObject.h"
#include "TVector3.h"
#include "FairTask.h"
#include "TString.h"

class R3BTGeoPar;
class TClonesArray;

/* Generic detector class that holds all infos needed for the track fitting
 */
class R3BTrackingDetector : public TObject
{
  public:
    R3BTrackingDetector(const char* geoParName, const char* hitArray = NULL);
    virtual ~R3BTrackingDetector();
    void SetHit(Double_t x, Double_t y, Double_t z);
    void SetHitTime(Double_t time);

    void SetParContainers();

    InitStatus Init();

    void Draw(Option_t* option = "");

    void GlobalToLocal(const TVector3& posGlobal, Double_t& x_local, Double_t& y_local);

  public:
    TString fGeoParName;
    TString fDataName;

    // coordinates of the plane
    TVector3 pos0;
    TVector3 pos1;
    TVector3 pos2;

    // which section of the setup: 0=before target 1=target-glad 2=after glad
    Int_t section;

    // material + thickness
    // ??
    R3BTGeoPar* fGeo;

    // resolutions (for the chi2)
    Double_t res_x;
    Double_t res_y;
    Double_t res_t;

    // TClonesArray of hits. Holding a detector dependent structure.
    // In the Exec() function the user has to copy the hit position
    // of interest into the hit_# structures.
    TClonesArray* hits; // not used directly by the fitter

    Double_t hit_x; // local coordinates (on the detector plane)
    Double_t hit_y;
    TVector3 hit_xyz; // same in global coordinates
    Double_t hit_time;
    // we could store a time here as well. Maybe later.

    Double_t track_x; // local coordinates (on the detector plane)
    Double_t track_y;
    TVector3 track_xyz; // same in global coordinates

  public:
    ClassDef(R3BTrackingDetector, 0)
};

#endif
