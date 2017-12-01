#ifndef R3BPSPXPRECALDATA_H
#define R3BPSPXPRECALDATA_H

#include "TObject.h"

/**
 * Class containing PSPX detector data on Precal level.
 * Originally, this class was introduced for the analysis of jun16, but it should also work for later experiments.
 * @author Ina Syndikus
 * @since March 2017
 */

class R3BPspxPrecalData : public TObject
{
  public:
    /** Default Constructor **/
    R3BPspxPrecalData();

    /** Standard Constructor **/
    R3BPspxPrecalData(UShort_t detector, UShort_t strip, Float_t energy1, Float_t energy2);

    /** Destructor **/
    virtual ~R3BPspxPrecalData() {}

    // Getters
    inline const UShort_t& GetDetector() const { return fDetector; }
    inline const UShort_t& GetStrip() const { return fStrip; }
    inline const Float_t& GetEnergy1() const { return fEnergy1; }
    inline const Float_t& GetEnergy2() const { return fEnergy2; }

  private:
    UShort_t fDetector; /**< Detector number, counting from 1 */
    UShort_t fStrip;    /**< Strip number, counting from 1 */
    Float_t fEnergy1; /**< Energy/Collected charge for odd channel corresponding to the strip. This value needs no gain
                         correction (Mapped2Precal Position Calibration).*/
    Float_t fEnergy2; /**< Energy/Collected charge for even channel corresponding to the strip. This value is gain
                         corrected (Mapped2Precal Position Calibration).*/

  public:
    ClassDef(R3BPspxPrecalData, 2)
};

#endif
