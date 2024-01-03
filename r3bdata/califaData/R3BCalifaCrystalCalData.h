/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/
#pragma once

#ifndef R3BCALIFACRYSTALCALDATA_H
#define R3BCALIFACRYSTALCALDATA_H 1

#include <TObject.h>

class R3BCalifaCrystalCalData : public TObject
{
  public:
    /** Default constructor **/
    R3BCalifaCrystalCalData() = default;

    /** Constructor with arguments
     *@param fCrystalId   Crystal unique identifier
     *@param fEnergy      Total energy deposited on the crystal ([GeV] in sim)
     *@param fNf  				Total Nf (fast)
     *@param fNs					Total Ns (slow)
     *@param fTime        Time since event start [ns]
     *@param fToT_Energy  Total energy deposited on the crystal from ToT ([GeV] in sim)
     **/
    R3BCalifaCrystalCalData(int ident, double energy, double Nf, double Ns, uint64_t time, double tot_energy = 0);

    /** Copy constructor **/
    R3BCalifaCrystalCalData(const R3BCalifaCrystalCalData&);

    R3BCalifaCrystalCalData& operator=(const R3BCalifaCrystalCalData&) { return *this; }

    /** Accessors **/
    inline const int& GetCrystalId() const { return fCrystalId; }
    inline const double& GetEnergy() const { return fEnergy; }
    inline const double& GetToTEnergy() const { return fToT_Energy; }
    inline const double& GetNf() const { return fNf; }
    inline const double& GetNs() const { return fNs; }
    inline const uint64_t& GetTime() const { return fTime; }

    /** Modifiers **/
    void SetCrystalId(int ident) { fCrystalId = ident; }
    void SetEnergy(double energy) { fEnergy = energy; }
    void SetToTEnergy(double energy) { fToT_Energy = energy; }
    void SetNf(double Nf) { fNf = Nf; }
    void SetNs(double Ns) { fNs = Ns; }
    void SetTime(uint64_t time) { fTime = time; }
    void AddMoreEnergy(double moreEnergy) { fEnergy += moreEnergy; }
    void AddMoreNf(double moreNf) { fNf += moreNf; }
    void AddMoreNs(double moreNs) { fNs += moreNs; }
    void AddMoreTot(double moreTot) { fToT_Energy += moreTot; }

  private:
    int fCrystalId = 0;     // crystal unique identifier
    double fEnergy = 0;     // total energy in the crystal
    double fNf = 0;         // total Nf in the crystal
    double fNs = 0;         // total Nf in the crystal
    double fToT_Energy = 0; // total energy in the crystal from ToT
    uint64_t fTime = 0;     // time of the interaction

  public:
    ClassDefOverride(R3BCalifaCrystalCalData, 2)
};

#endif
