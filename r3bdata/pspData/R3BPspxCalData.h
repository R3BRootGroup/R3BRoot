#ifndef R3BPSPXCALDATA_H
#define R3BPSPXCALDATA_H

#include "TObject.h"

/**
 * Class containing PSPX detector data on Cal level.
 * Originally, this class was introduced for the analysis of s438b, but it should also work for later experiments.
 * @author Ralf Plag, Ina Syndikus
 * @since January 2016
 */

class R3BPspxCalData : public TObject
{
  public:
    /** Default Constructor **/
    R3BPspxCalData();

    /** Standard Constructor **/
    R3BPspxCalData(UShort_t detector, UShort_t strip, Float_t energy1, Float_t energy2);

    /** Destructor **/
    virtual ~R3BPspxCalData() {}

    // Getters
    inline const UShort_t& GetDetector() const { return fDetector; }
    inline const UShort_t& GetStrip() const { return fStrip; }
    inline const Float_t& GetEnergy1() const { return fEnergy1; }
    inline const Float_t& GetEnergy2() const { return fEnergy2; }

  private:
    UShort_t fDetector; /**< Detector number, counting from 1 */
    UShort_t fStrip;    /**< Strip number, counting from 1 */
    Float_t fEnergy1;   /**< Energy/Collected charge for odd channel corresponding to the strip. This value is gain
                         corrected (Precal2Cal Energy Calibration). */
    Float_t fEnergy2;   /**< Energy/Collected charge for even channel corresponding to the strip. This value is gain
                         corrected (Precal2Cal Energy Calibration). */

  public:
    ClassDef(R3BPspxCalData, 5)
};

#endif
