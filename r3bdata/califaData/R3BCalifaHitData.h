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

#ifndef R3BCALIFAHITDATA_H
#define R3BCALIFAHITDATA_H

#include "TObject.h"

#include "FairMultiLinkedData.h"
#include "R3BCalifaCrystalCalData.h"

class R3BCalifaHitData : public FairMultiLinkedData
{

  public:
    /** Default constructor **/
    R3BCalifaHitData();

    /** Constructor with arguments
     *@param fEnergy				Total energy deposited
     *@param fTheta					Reconstructed theta
     *@param fPhi					Reconstructed phi
     **/
    R3BCalifaHitData(std::vector<Int_t> crystalList, Double_t ene, Double_t nf, Double_t ns, Double_t theta, Double_t phi, ULong64_t time,Int_t clusterType);

    /** Copy constructor **/
    R3BCalifaHitData(const R3BCalifaHitData&);


    R3BCalifaHitData& operator=(const R3BCalifaHitData&) { return *this; }

    /** Destructor **/
    virtual ~R3BCalifaHitData() {}

    /** Accessors **/
    Double_t GetEnergy() const { return fEnergy; }
    Double_t GetNf() const { return fNf; }
    Double_t GetNs() const { return fNs; }
    Double_t GetTheta() const { return fTheta; }
    Double_t GetPhi() const { return fPhi; }
    ULong64_t GetTime() const { return fTime; }
    uint32_t GetClusterType() const { return fClusterType; }
    Int_t GetNbOfCrystalHits()  const { return fCrystalList.size(); }

    std::vector<Int_t> GetCrystalList() const {return fCrystalList; }


    /** Modifiers **/
    void SetEnergy(Double_t ene) { fEnergy = ene; }
    void SetNf(Double_t nf) { fNf = nf; }
    void SetNs(Double_t ns) { fNs = ns; }
    void SetTheta(Double_t theta) { fTheta = theta; }
    void SetPhi(Double_t phi) { fPhi = phi; }
    void SetTime(ULong64_t time) { fTime = time; }
    void SetClusterType(uint32_t id) { fClusterType = id; }
    void SetCrystalList(std::vector<Int_t> list) {fCrystalList = std::move(list);}
  protected:

    Double_t fEnergy;        // total energy deposited
    Double_t fNf;            // total Nf deposited
    Double_t fNs;            // total Ns deposited
    Double_t fTheta;         // reconstructed theta
    Double_t fPhi;           // reconstructed phi
    ULong64_t fTime;         // WR time stamp
    uint32_t fClusterType;
    std::vector<Int_t> fCrystalList; // List with crystals inside the cluster. First one is the mother crystal

    ClassDef(R3BCalifaHitData, 3)
};

#endif
