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

#ifndef R3BCALIFACLUSTERDATA_H
#define R3BCALIFACLUSTERDATA_H 1

#include "TObject.h"

#include "FairMultiLinkedData.h"
#include "R3BCalifaCrystalCalData.h"

class R3BCalifaClusterData : public FairMultiLinkedData
{
  public:
    /** Default constructor **/
    R3BCalifaClusterData();

    /** Constructor with arguments
     *@param fNbOfCrystalHits		Crystal unique identifier
     *@param fEnergy				Total energy deposited
     *@param fTheta					Reconstructed theta
     *@param fPhi					Reconstructed phi
     **/
    R3BCalifaClusterData(UInt_t Nb, Double_t ene, Double_t nf, Double_t ns, Double_t theta, Double_t phi, ULong64_t time);

    R3BCalifaClusterData(uint64_t time, double theta, double phi, uint32_t clusterId)
        : fTime(time)
        , fTheta(theta)
        , fPhi(phi)
        , fClusterId(clusterId)
        , fEnergy(0.)
        , fNf(0.)
        , fNs(0)
        , fNbOfCrystalHits(0)
    {
    }

    /** Copy constructor **/
    R3BCalifaClusterData(const R3BCalifaClusterData&);

    /** += operator **/
    R3BCalifaClusterData& operator+=(R3BCalifaCrystalCalData& cH)
    {
        this->fEnergy += cH.GetEnergy();
        this->fNf += cH.GetNf();
        this->fNs += cH.GetNs();
        this->fNbOfCrystalHits++;
        return *this;
    }

    R3BCalifaClusterData& operator=(const R3BCalifaClusterData&) { return *this; }

    /** Destructor **/
    virtual ~R3BCalifaClusterData() {}

    /** Accessors **/
    UInt_t GetNbOfCrystalHits() const { return fNbOfCrystalHits; }
    Double_t GetEnergy() const { return fEnergy; }
    Double_t GetNf() const { return fNf; }
    Double_t GetNs() const { return fNs; }
    Double_t GetTheta() const { return fTheta; }
    Double_t GetPhi() const { return fPhi; }
    ULong64_t GetTime() const { return fTime; }
    uint32_t GetClusterId() const { return fClusterId; }

    /** Modifiers **/
    void SetNbOfCrystalHits(UInt_t number) { fNbOfCrystalHits = number; }
    void SetEnergy(Double_t ene) { fEnergy = ene; }
    void SetNf(Double_t nf) { fNf = nf; }
    void SetNs(Double_t ns) { fNs = ns; }
    void SetTheta(Double_t theta) { fTheta = theta; }
    void SetPhi(Double_t phi) { fPhi = phi; }
    void SetTime(ULong64_t time) { fTime = time; }
    void SetClusterId(uint32_t id) { fClusterId = id; }

  protected:
    // Basic Hit information
    UInt_t fNbOfCrystalHits; // number of crystals contribuying to the R3BCalifaClusterData
    Double_t fEnergy;        // total energy deposited
    Double_t fNf;            // total Nf deposited
    Double_t fNs;            // total Ns deposited
    Double_t fTheta;         // reconstructed theta
    Double_t fPhi;           // reconstructed phi
    ULong64_t fTime;         // WR time stamp
    uint32_t fClusterId;

    ClassDef(R3BCalifaClusterData, 3)
};

using R3BCalifaHitData [[deprecated("R3BCalifaHitData was renamed to R3BCalifaClusterData.")]] =R3BCalifaClusterData;

#endif /* R3BCALIFACLUSTERDATA_H */
