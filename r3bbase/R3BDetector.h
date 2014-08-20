#ifndef R3BDETECTOR_H
#define R3BDETECTOR_H 1

#include "FairDetector.h"

#include "TObject.h"
#include "TVector3.h"
#include "TGeoMatrix.h"

class R3BDetector : public FairDetector
{
  public:
    /** Default constructor **/
    R3BDetector();

    /** Constructor with arguments
     **/
    R3BDetector(const char* Name, Bool_t Active, Int_t detId = 0);

    /** Destructor **/
    virtual ~R3BDetector();

    /** Transformation **/
    void SetRotAnglesEuler(Double_t phi, Double_t theta, Double_t psi);
    void SetRotAnglesXYZ(Double_t xx, Double_t yy, Double_t zz);
    void SetTranslation(Double_t tx, Double_t ty, Double_t tz);

    TGeoRotation* GetGlobalRot()
    {
        return fGlobalRot;
    }
    TGeoTranslation* GetGlobalTrans()
    {
        return fGlobalTrans;
    }
    TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans* fRef = NULL);

    inline void SetEnergyCut(Double_t cutE)
    {
        fCutE = cutE;
    }

  private:
    R3BDetector(const R3BDetector&);
    R3BDetector& operator=(const R3BDetector&)
    {
        return *this;
    }

  protected:
    TGeoRotation* fGlobalRot;
    TGeoTranslation* fGlobalTrans;
    TGeoCombiTrans* fRefRot;

    Double_t fThetaX;
    Double_t fThetaY;
    Double_t fThetaZ;

    Bool_t fLabTrans;
    Bool_t fLocalTrans;

    Double_t fX, fY, fZ;
    Double_t fPhi, fTheta, fPsi;

    Double_t fCutE;

    ClassDef(R3BDetector, 1)
};

#endif
