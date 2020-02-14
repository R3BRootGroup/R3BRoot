#ifndef R3BCADDETECTOR_H
#define R3BCADDETECTOR_H

#include "FairDetector.h"
#include "TGeoMatrix.h"
#include "TString.h"

class R3BCADDetector : public FairDetector
{
  public:
    R3BCADDetector();

    R3BCADDetector(const TString& name, const Int_t detId);

    R3BCADDetector(const TString& name,
                const Int_t detId,
                const TString& geoFile,
                const TGeoTranslation& trans,
                const TGeoRotation& rot = TGeoRotation());

    R3BCADDetector(const TString& name,
                const Int_t detId,
                const TString& geoFile,
                const TGeoCombiTrans& combi = TGeoCombiTrans());

    /** Construct geometry from root files */
    virtual void ConstructGeometry(); // override;
    virtual void ConstructRootGeometry(); // override;

    /** Method to setup the position of the detector-origin. */
    virtual void SetPosition(const TGeoTranslation& trans) { fCombiTrans.SetTranslation(trans); }

    /** Method to setup the rotation of the detector */
    virtual void SetRotation(const TGeoRotation& rot) { fCombiTrans.SetRotation(rot); }

    /** Method to setup the translation + rotation of the detector */
    virtual void SetCombiTrans(const TGeoCombiTrans& combi) { fCombiTrans = combi; }

    virtual void SetEnergyCut(const Double_t cutE) { fCutE = cutE; }

    virtual void SetActive(const Bool_t flag) { fActive = flag; }

  protected:
    TGeoCombiTrans fCombiTrans;
    Double_t fCutE;

    ClassDef(R3BCADDetector, 3)
    // ClassDefOverride(R3BCADDetector, 3)
};

#endif // R3BDETECTOR_H
