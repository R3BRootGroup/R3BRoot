#ifndef R3BPSPXHITDATA_H
#define R3BPSPXHITDATA_H

#include "TObject.h"

/**
 * Class containing PSPX detector data on Hit level.
 * Originally, this class was introduced for the analysis of s438b, but it also works for later experiments.
 * @author Ina Syndikus
 * @since April 2016
 */

class R3BPspxHitData : public TObject
{
  public:
    /** Default Constructor **/
    R3BPspxHitData();

    /** Standard Constructor **/
    R3BPspxHitData(UShort_t detector,
                   Float_t u,
                   Float_t v,
                   Float_t x,
                   Float_t y,
                   Float_t sigma_x,
                   Float_t sigma_y,
                   Float_t energy,
                   UShort_t multiplicity,
                   UShort_t multiplicity_x,
                   UShort_t multiplicity_y);

    /** Destructor **/
    virtual ~R3BPspxHitData() {}

    // Getters
    inline const UShort_t& GetDetector() const { return fDetector; }
    inline const Float_t& GetU() const { return fU; }
    inline const Float_t& GetV() const { return fV; }
    inline const Float_t& GetX() const { return fX; }
    inline const Float_t& GetY() const { return fY; }
    inline const Float_t& GetSigmaX() const { return fSigmaX; }
    inline const Float_t& GetSigmaY() const { return fSigmaY; }
    inline const Float_t& GetEnergy() const { return fEnergy; }
    inline const UShort_t& GetMultiplicity() const { return fMultiplicity; }
    inline const UShort_t& GetXMultiplicity() const { return fMultiplicityX; }
    inline const UShort_t& GetYMultiplicity() const { return fMultiplicityY; }

  private:
    UShort_t fDetector; /**< Detector number, counting from 1 */
    Float_t fU;         /**< Position in x direction & detector specific units in the range [-1:1] */
    Float_t fV;         /**< Position in y direction & detector specific units in the range  [-1:1] */
    Float_t fX;         /**< Position in x direction & in cm (general coordinate system) in the range [-l/2:l/2] */
    Float_t fY;         /**< Position in y direction & in cm (general coordinate system) in the range [-l/2:l/2] */
    Float_t fSigmaX; /**< Sigma of position in x direction, at the moment: 0 for reconstructed position, 1 for position
                     via strip number */
    Float_t fSigmaY; /**< Sigma of position in y direction, at the moment: 0 for reconstructed position, 1 for position
                     via strip number */
    Float_t fEnergy; /**< Energy/Collected charge. Either from the back side (X1) or from a combination of anode/strip
                        channels (X5).*/
    UShort_t fMultiplicity;  /**< Multiplicity for whole detector: Number of channels on Precal level (after applying
                              threshold) with an energy entry */
    UShort_t fMultiplicityX; /**< Multiplicity for side of the detector providing x position: Number of channels on Precal
                              level (after applying threshold) with an energy entry */
    UShort_t fMultiplicityY; /**< Multiplicity for side of the detector providing y position: Number of channels on Precal
                              level (after applying threshold) with an energy entry */

  public:
    ClassDef(R3BPspxHitData, 6)
};

#endif
