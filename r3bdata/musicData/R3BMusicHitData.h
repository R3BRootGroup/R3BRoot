// -------------------------------------------------------------------------
// -----                      R3BMusicHitData header file              -----
// -----                  Created 15/10/19 by JL Rodriguez             -----
// -------------------------------------------------------------------------

#ifndef R3BMusicHitData_H
#define R3BMusicHitData_H
#include "TObject.h"

class R3BMusicHitData : public TObject
{

  public:
    /** Default constructor **/
    R3BMusicHitData();

    /** Constructor with arguments
     *@param theta    Angle theta in [mrad]
     *@param z        Atomic number Z in charge units
     **/
    R3BMusicHitData(Double_t theta, Double_t z);

    /** Destructor **/
    virtual ~R3BMusicHitData() {}

    /** Accessors **/
    inline const Double_t& GetTheta() const { return fTheta; }
    inline const Double_t& GetZcharge() const { return fZ; }

    /** Modifiers **/
    void SetTheta(Double_t theta) { fTheta = theta; };
    void SetZcharge(Double_t z) { fZ = z; };

  protected:
    Double_t fTheta, fZ;

    ClassDef(R3BMusicHitData, 1)
};

#endif
