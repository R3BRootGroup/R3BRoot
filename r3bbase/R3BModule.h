#ifndef R3BMODULE_H
#define R3BMODULE_H 1


#include "FairModule.h"

class TVector3;
class TGeoRotation;

class R3BModule : public FairModule
{
 public:
  /** Default constructor **/
  R3BModule();
  
  /** Constructor with arguments
   **/
  R3BModule(const char* Name, const char* title, Bool_t Active=kFALSE);

  /** Constructor with arguments
   **/
  R3BModule(const char* Name,
            const char* title,
            TString geoFile,
            Float_t x = 0.,
            Float_t y = 0.,
            Float_t z = 0.,
            Float_t rot_x = 0.,
            Float_t rot_y = 0.,
            Float_t rot_z = 0.,
            Bool_t Active = kFALSE);

  /** Destructor **/
  virtual ~R3BModule();
  
  /**construct geometry from root files (TGeo)*/
  void ConstructRootGeometry();
  
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
  
 private:
  R3BModule(const R3BModule&);
  R3BModule& operator=(const R3BModule&) { return *this; }
  
 protected:
  TVector3* fPosition;
  TGeoRotation* fRotation;
  
  ClassDef(R3BModule, 2)
};

#endif
