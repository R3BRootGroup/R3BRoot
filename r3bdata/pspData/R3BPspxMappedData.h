/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

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
    Int_t fEnergy;      /**< Energy/Collected charge */

  public:
    ClassDef(R3BPspxMappedData, 3)
};

#endif
