#ifndef R3BMODULE_H
#define R3BMODULE_H

#include "FairModule.h"
#include "TGeoMatrix.h"
#include "TString.h"

class R3BModule : public FairModule
{
  public:
    R3BModule();

    R3BModule(const TString& name, const TString& title, const Bool_t active = kFALSE);

    R3BModule(const TString& name,
              const TString& title,
              const Bool_t active,
              const TString& geoFile,
              const TGeoTranslation& trans,
              const TGeoRotation& rot = TGeoRotation());

    R3BModule(const TString& name,
              const TString& title,
              const Bool_t active,
              const TString& geoFile,
              const TGeoCombiTrans& combi = TGeoCombiTrans());

    /** Construct geometry from root files */
    virtual void ConstructGeometry(); // override;
    virtual void ConstructRootGeometry(); // override;

    /** Method to setup the position of the detector-origin. */
    virtual void SetPosition(const TGeoTranslation& trans) { fCombiTrans.SetTranslation(trans); }

    /** Method to setup the rotation of the detector */
    virtual void SetRotation(const TGeoRotation& rot) { fCombiTrans.SetRotation(rot); }

  protected:
    TGeoCombiTrans fCombiTrans;

    ClassDef(R3BModule, 3)
    // ClassDefOverride(R3BModule, 3)
};

#endif // R3BMODULE_H
