#ifndef R3BDETECTOR_H
#define R3BDETECTOR_H 1

#include "FairDetector.h"

class TVector3;
class TGeoRotation;

class R3BDetector : public FairDetector
{
  public:
    /** Default constructor **/
    R3BDetector();

    /** Constructor with arguments
     **/
    R3BDetector(const char* Name, Bool_t Active, Int_t detId = 0);

    /** Constructor with arguments
     **/
    R3BDetector(const char* Name,
                TString geoFile,
                Bool_t Active,
                Float_t x = 0.,
                Float_t y = 0.,
                Float_t z = 0.,
                Float_t rot_x = 0.,
                Float_t rot_y = 0.,
                Float_t rot_z = 0.,
                Int_t detId = 0);

    /** Destructor **/
    virtual ~R3BDetector();

    /**construct geometry from root files (TGeo)*/
    virtual void ConstructRootGeometry();
    
    /** Transformation **/
    
    /**
     * Method to setup the position of the detector-origin.
     * @param x The x-position of the detector origin
     * @param y The x-position of the detector origin
     * @param z The x-position of the detector origin
     */
    void SetPosition(Double_t x, Double_t y, Double_t z);
    
    /**
     * Method to setup the rotation of the detector around the x-,y- and z-axis.
     * @param x_deg The rotation around the x-Axis in degrees. This will be executed first.
     * @param y_deg The rotation around the y-Axis in degrees. This will be executed second.
     * @param z_deg The rotation around the z-Axis in degrees. This will be executed last.
     */
    void SetRotation(Double_t x_deg, Double_t y_deg, Double_t z_deg);

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
    TVector3* fPosition;
    TGeoRotation* fRotation;
    
    Double_t fCutE;

    ClassDef(R3BDetector, 2)
};

#endif
