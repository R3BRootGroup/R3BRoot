/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// --------------------------------------------------------------------
// -----              R3BFootCalData header file                  -----
// -----       Created 16/07/21  by J.L. Rodriguez-Sanchez        -----
// --------------------------------------------------------------------

#ifndef R3BFootCalData_H
#define R3BFootCalData_H

#include "TObject.h"

class R3BFootCalData : public TObject
{
  public:
    /** Default constructor **/
    R3BFootCalData();

    /** Constructor with arguments
     *@param fDetId    Detector unique identifier
     *@param fStripId  Strip unique identifier
     *@param fEnergy   Total energy deposited on the strip ([GeV] in sim)
     **/
    R3BFootCalData(Int_t detid, Int_t stripid, Double_t energy);

    /** Destructor **/
    virtual ~R3BFootCalData() {}

    /** Accessors **/
    inline const Int_t& GetDetId() const { return fDetId; }
    inline const Int_t& GetStripId() const { return fStripId; }
    inline const Double_t& GetEnergy() const { return fEnergy; }

    /** Modifiers **/
    void SetDetId(Int_t detid) { fDetId = detid; }
    void SetStripId(Int_t stripid) { fStripId = stripid; }
    void SetEnergy(Double32_t energy) { fEnergy = energy; }

  protected:
    Int_t fDetId;     // detector unique identifier
    Int_t fStripId;   // strip unique identifier
    Double_t fEnergy; // total energy in the strip

    ClassDef(R3BFootCalData, 1)
};

#endif /* R3BFootCalData_H */
