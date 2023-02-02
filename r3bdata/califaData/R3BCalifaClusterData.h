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
#include "R3BLogger.h"
#include "R3BCalifaCrystalCalData.h"
#include <vector>
#include <cmath>


struct R3BCalifaClusterData : public TObject
{
  public:
    using container_t=std::vector<R3BCalifaClusterData>;
    static constexpr char default_container_name[]="CalifaClusterVector";
    static constexpr char tca_name[]="CalifaClusterData";
    /** Default constructor **/
    R3BCalifaClusterData() {}

    /** Constructor with arguments
     *@param fNbOfCrystalHits		Crystal unique identifier
     *@param fPhi					Reconstructed phi
     **/
    //R3BCalifaClusterData(UInt_t Nb, Double_t ene, Double_t nf, Double_t ns, Double_t theta, Double_t phi, ULong64_t time);

   R3BCalifaClusterData(uint64_t time, const ROOT::Math::XYZVector& pos, uint32_t clusterId)
        : fWrts(time)
        , fPos(pos)
        , fClusterId(clusterId)
    {
    }

    /** += operator **/
    R3BCalifaClusterData& operator+=(const R3BCalifaCrystalCalData& cH)
    {
        static bool warned{};
        this->fEnergy += cH.GetEnergy();
        this->fNf += cH.GetNf();
        this->fNs += cH.GetNs();
        this->fMult++;
	if (!cH.fClusterId)
	  {
	    cH.fClusterId=this->fClusterId;
	  }
	else
	  {
	    if (!warned)
	      {
		LLOG(warning) << "R3BCalifaCrystalCalData was already set by a different task. It will not be overwritten.";
		warned=1;
	      }
	  }
        return *this;
    }

    // NEVER DO THIS:
    //R3BCalifaClusterData& operator=(const R3BCalifaClusterData&) { return *this; }

    /** Accessors **/
    UInt_t GetNbOfCrystalHits() const { return fMult; }
    Double_t GetEnergy() const { return fEnergy; }
    Double_t GetNf() const { return fNf; }
    Double_t GetNs() const { return fNs; }
    Double_t GetTheta() const { return fPos.Theta(); }
    Double_t GetPhi() const { return fPos.Phi(); }
    ULong64_t GetTime() const { return fWrts; }
    uint32_t GetClusterId() const { return fClusterId; }

    /** Modifiers **/

  public:
    // Basic Hit information
    // additive quantities
    UInt_t fMult{};            // number of crystals contribuying to the R3BCalifaClusterData
    Double_t fEnergy{};        // total energy deposited
    Double_t fNf{};            // total Nf deposited
    Double_t fNs{};            // total Ns deposited

    // set by cluster defining crystal:
    ROOT::Math::XYZVector fPos {NAN, NAN, NAN};
    ULong64_t fWrts{};           // WR time stamp
    uint32_t fClusterId{};

    ClassDef(R3BCalifaClusterData, 5)
};

using R3BCalifaHitData [[deprecated("R3BCalifaHitData was renamed to R3BCalifaClusterData.")]] =R3BCalifaClusterData;

#endif /* R3BCALIFACLUSTERDATA_H */
