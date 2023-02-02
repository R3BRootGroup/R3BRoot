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

#include "TObject.h"
#include <Math/Vector3D.h>
#include <map>

struct R3BCalifaCrystalCalData : public TObject
{
    using container_t=std::map<int, R3BCalifaCrystalCalData>;
    static constexpr char default_container_name[]="CalifaCrystalCalMap";
    static constexpr char tca_name[]="CalifaCrystalCalData";
    /** Default constructor **/
    R3BCalifaCrystalCalData(int crId=0xffff)
      : fCrystalId(crId)
  {}

    /** Constructor with arguments
     *@param fCrystalId   Crystal unique identifier
     *@param fEnergy      Total energy deposited on the crystal ([GeV] in sim)
     *@param fNf  				Total Nf (fast)
     *@param fNs					Total Ns (slow)
     *@param fTime        Time since event start [ns]
     *@param fToT_Energy  Total energy deposited on the crystal from ToT ([GeV] in sim)
     **/
    [[deprecated]]
    R3BCalifaCrystalCalData(Int_t ident,
                            Double_t energy,
                            Double_t Nf,
                            Double_t Ns,
                            uint64_t time,
                            Double_t tot_energy = 0);

    // DO not add any fake assignment methods here!

    /** Accessors **/
    inline const Int_t& GetCrystalId() const { return fCrystalId; }
    inline const Double_t& GetEnergy() const { return fEnergy; }
    inline const Double_t& GetToT_Energy() const { return fToTEnergy; }
    inline const Double_t& GetNf() const { return fNf; }
    inline const Double_t& GetNs() const { return fNs; }
    inline const uint64_t& GetTime() const { return fWrts; }

    /** Modifiers **/
    void SetCrystalId(Int_t ident) { fCrystalId = ident; }
    void SetEnergy(Double32_t energy) { fEnergy = energy; }
    void SetToTEnergy(Double32_t energy) { fToTEnergy = energy; }
    void SetNf(Double32_t Nf) { fNf = Nf; }
    void SetNs(Double32_t Ns) { fNs = Ns; }
    void SetTime(uint64_t time) { fWrts = time; }
    void AddMoreEnergy(Double32_t moreEnergy) { fEnergy += moreEnergy; }
    void AddMoreNf(Double32_t moreNf) { fNf += moreNf; }
    void AddMoreNs(Double32_t moreNs) { fNs += moreNs; }

    double fEnergy;     // total energy in the crystal
    double fNf;         // total Nf in the crystal
    double fNs;         // total Nf in the crystal
    double fToTEnergy; // total energy in the crystal from ToT
    uint64_t fWrts;         // time of the interaction (WRTS)
    int fCrystalId;       // crystal unique identifier

    // written by clustering:
    mutable int fClusterId{};
    mutable ROOT::Math::XYZVector fPos{}; //! position. not stored in file. 

    // set by mapped2cal again:
    R3BCalifaCrystalCalData* otherRange{}; //! <-- comment exclamation mark means transient (not stored in file)
    ClassDef(R3BCalifaCrystalCalData, 2)
};

#endif
