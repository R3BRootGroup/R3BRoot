#ifndef R3BPSPXMAPPEDDATA_H
#define R3BPSPXMAPPEDDATA_H

#include "TObject.h"

/**
 * Class containing PSPX detector data on Mapped level.
 * Originally, this class was introduced for the analysis of s438b, but it also works for later experiments.
 * @author Ralf Plag, Ina Syndikus
 * @since January 2016
 */

class R3BPspxMappedData : public TObject
{
  public:
    /** Default Constructor **/
    R3BPspxMappedData();

    /** Standard Constructor **/
    R3BPspxMappedData(UShort_t detector, UShort_t channel, Int_t energy);

    /** Destructor **/
    virtual ~R3BPspxMappedData() {}

    // Getters
    inline const UShort_t& GetDetector() const { return fDetector; }
    inline const UShort_t& GetChannel() const { return fChannel; }
    inline const Int_t& GetEnergy() const { return fEnergy; }

  private:
    UShort_t fDetector; /**< Detector number, counting from 1 */
    UShort_t fChannel;  /**< Channel number, counting from 1 */
    Int_t fEnergy;     /**< Energy/Collected charge */

  public:
    ClassDef(R3BPspxMappedData, 3)
};

#endif
