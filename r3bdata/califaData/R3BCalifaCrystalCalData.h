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

#ifndef R3BCALIFACRYSTALCALDATA_H
#define R3BCALIFACRYSTALCALDATA_H

#include "FairMultiLinkedData.h"
#include "TObject.h"

class R3BCalifaCrystalCalData : public FairMultiLinkedData
{
  public:
    /** Default constructor **/
    R3BCalifaCrystalCalData();

    /** Constructor with arguments
     *@param fCrystalId   Crystal unique identifier
     *@param fEnergy      Total energy deposited on the crystal ([GeV] in sim)
     *@param fNf  				Total Nf (fast)
     *@param fNs					Total Ns (slow)
     *@param fTime        Time since event start [ns]
     *@param fToT_Energy  Total energy deposited on the crystal from ToT ([GeV] in sim)
     **/
    R3BCalifaCrystalCalData(Int_t ident,
                            Double_t energy,
                            Double_t Nf,
                            Double_t Ns,
                            uint64_t time,
                            Double_t tot_energy = 0);

    /** Copy constructor **/
    R3BCalifaCrystalCalData(const R3BCalifaCrystalCalData&);

    R3BCalifaCrystalCalData& operator=(const R3BCalifaCrystalCalData&) { return *this; }

    /** Destructor **/
    virtual ~R3BCalifaCrystalCalData() {}

    /** Accessors **/
    inline const Int_t& GetCrystalId() const { return fCrystalId; }
    inline const Double_t& GetEnergy() const { return fEnergy; }
    inline const Double_t& GetToT_Energy() const { return fToT_Energy; }
    inline const Double_t& GetNf() const { return fNf; }
    inline const Double_t& GetNs() const { return fNs; }
    inline const uint64_t& GetTime() const { return fTime; }

    /** Modifiers **/
    void SetCrystalId(Int_t ident) { fCrystalId = ident; }
    void SetEnergy(Double32_t energy) { fEnergy = energy; }
    void SetToT_Energy(Double32_t energy) { fToT_Energy = energy; }
    void SetNf(Double32_t Nf) { fNf = Nf; }
    void SetNs(Double32_t Ns) { fNs = Ns; }
    void SetTime(uint64_t time) { fTime = time; }
    void AddMoreEnergy(Double32_t moreEnergy) { fEnergy += moreEnergy; }
    void AddMoreNf(Double32_t moreNf) { fNf += moreNf; }
    void AddMoreNs(Double32_t moreNs) { fNs += moreNs; }

  protected:
    Double32_t fEnergy;     // total energy in the crystal
    Double32_t fNf;         // total Nf in the crystal
    Double32_t fNs;         // total Nf in the crystal
    Double32_t fToT_Energy; // total energy in the crystal from ToT
    uint64_t fTime;         // time of the interaction
    Int_t fCrystalId;       // crystal unique identifier

  public:
    ClassDef(R3BCalifaCrystalCalData, 1)
};

#endif
