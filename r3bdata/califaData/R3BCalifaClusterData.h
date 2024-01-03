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

#ifndef R3BCALIFACLUSTERDATA_H
#define R3BCALIFACLUSTERDATA_H 1

#include "TObject.h"

class R3BCalifaClusterData : public TObject
{
  public:
    /** Default constructor **/
    R3BCalifaClusterData() = default;

    /** Constructor with arguments
     *@param fEnergy				Total energy deposited
     *@param fTheta					Reconstructed theta
     *@param fPhi					Reconstructed phi
     **/
    R3BCalifaClusterData(std::vector<int> crystalList,
                         double ene,
                         double nf,
                         double ns,
                         double theta,
                         double phi,
                         ULong64_t time,
                         uint32_t clusterType);

    /** Copy constructor **/
    R3BCalifaClusterData(const R3BCalifaClusterData&);

    R3BCalifaClusterData& operator=(const R3BCalifaClusterData&) { return *this; }

    /** Destructor **/
    virtual ~R3BCalifaClusterData() {}

    /** Accessors **/
    double GetEnergy() const { return fEnergy; }
    double GetNf() const { return fNf; }
    double GetNs() const { return fNs; }
    double GetTheta() const { return fTheta; }
    double GetPhi() const { return fPhi; }
    ULong64_t GetTime() const { return fTime; }
    uint32_t GetClusterType() const { return fClusterType; }
    int GetNbOfCrystalHits() const { return fCrystalList.size(); }

    std::vector<int> GetCrystalList() const { return fCrystalList; }
    int GetMotherCrystal() const { return fCrystalList.at(0); }

    /** Modifiers **/
    void SetEnergy(double ene) { fEnergy = ene; }
    void SetNf(double nf) { fNf = nf; }
    void SetNs(double ns) { fNs = ns; }
    void SetTheta(double theta) { fTheta = theta; }
    void SetPhi(double phi) { fPhi = phi; }
    void SetTime(ULong64_t time) { fTime = time; }
    void SetClusterType(uint32_t id) { fClusterType = id; }
    void SetCrystalList(std::vector<int> list) { fCrystalList = std::move(list); }

  private:
    std::vector<int> fCrystalList; // List with crystals inside the cluster. First one is the mother crystal
    double fEnergy = 0;            // total energy deposited
    double fNf = 0;                // total Nf deposited
    double fNs = 0;                // total Ns deposited
    double fTheta = 0;             // reconstructed theta
    double fPhi = 0;               // reconstructed phi
    ULong64_t fTime = 0;           // WR time stamp
    uint32_t fClusterType = 0;

  public:
    ClassDefOverride(R3BCalifaClusterData, 4)
};

#endif
