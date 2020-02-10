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

#ifndef R3BCALIFAMAPPEDDATA_H
#define R3BCALIFAMAPPEDDATA_H
#include "TObject.h"
#include <stdint.h>

class R3BCalifaMappedData : public TObject
{

  public:
    // Default Constructor
    R3BCalifaMappedData();

    /** Standard Constructor
     *@param crystalId   Crystal unique identifier
     *@param energy      Total energy deposited in the crystal [GeV]
     *@param nf          Total fast amplitude deposited in the crystal [a.u.]
     *@param ns          Total slow amplitude deposited on the crystal [a.u.]
     *@param time        Time since event start [ns]
     *@param tot         Time over threshold
     **/
    R3BCalifaMappedData(UShort_t crystalId, Int_t energy, Int_t nf, Int_t ns, uint64_t time, UChar_t error, Int_t tot);

    // Destructor
    virtual ~R3BCalifaMappedData() {}

    // Getters
    inline const UShort_t& GetCrystalId() const { return fCrystalId; }
    inline const Int_t& GetEnergy() const { return fEnergy; }
    inline const Int_t& GetNf() const { return fNf; }
    inline const Int_t& GetNs() const { return fNs; }
    inline const uint64_t& GetTime() const { return fTime; }
    inline const UChar_t& GetError() const { return fError; }
    inline const Int_t& GetTot() const { return fTot; }

  protected:
    UShort_t fCrystalId; // crystal unique identifier
    Int_t fEnergy;       // total energy in the crystal
    Int_t fNf;           // total fast amplitude in the crystal
    Int_t fNs;           // total slow amplitude in the crystal
    uint64_t fTime;      // time-stamp (common to all the hits in the event)
    UChar_t fError;      // bit coded error flag
    Int_t fTot;          // time-over-treshold

  public:
    ClassDef(R3BCalifaMappedData, 2)
};

#endif
