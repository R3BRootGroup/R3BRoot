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

#ifndef R3BCALIFACRYSTALCAL2CLUSTAR_H
#define R3BCALIFACRYSTALCAL2CLUSTAR_H 1

#include "FairTask.h"
#include "R3BCalifaGeometry.h"
#include "R3BCalifaClusterData.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaClusterMatcher.h"
#include "Rtypes.h"

#include "TH2F.h"
#include <Math/Vector3D.h>
#include <list>

class R3BTGeoPar;

class R3BCalifaCrystalCal2Cluster : public FairTask
{  
  public:
    /** Constructor
     ** @param: pointer to a matcher implementation. Ownership is transferred. 
     **/
    R3BCalifaCrystalCal2Cluster(R3BCalifaClusterMatcher* m= new R3BCalifaRectangle(14.3*degree, 14.3*degree))
      : FairTask("R3B CALIFA CrystalCal to Hit Finder")
      , fMatcher(m)
    {}

    /** Destructor **/
    virtual ~R3BCalifaCrystalCal2Cluster()
    {
      delete fMatcher;
    }

    void SetParContainers() override;
  
    /** Virtual method Exec **/
    void Exec(Option_t* opt) override;

    /** Public method SetCrystalThreshold
     **
     ** Any hits with an energy below this will not be considered for clustering at all. 
     **/
    void SetCrystalThreshold(double minTH)
    {
      fThreshold = minTH;
    }

    /**
     ** Any hit below this energy may not spawn new clusters
     ** (but may still be inserted into clusters.)
     **/
    void SetClusterStartThreshold(double th)
    {
      fClThreshold = th;
    }

    /** Public method SetDRThreshold (for dual readout channels)
     ** If both gain ranges contain a valid energy, then the
     ** low gain hit will be preferred if and only if the low 
     ** gain energy is above this threshold. 
     **/
    void SetDRThreshold(double x)
    {
      fDRThreshold=x;
    }

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

    /** Virtual method Init **/
    virtual InitStatus Init() override;

    /** Virtual method ReInit **/
    virtual InitStatus ReInit() override;

  protected:
    using candidate_t=std::list<const R3BCalifaCrystalCalData*>;
  
    void FilterCrystals(candidate_t& candidates);
    void AddPositionInfo(const R3BCalifaCrystalCalData&);
    void AddCandidate(candidate_t& candidates, const R3BCalifaCrystalCalData&);

    void SortCandidates(R3BCalifaCrystalCal2Cluster::candidate_t&);
  
    void DoClustering(candidate_t& candidates);
  
    ROOT::Math::XYZVector fCalifatoTargetPos {0, 0, 0};
  
    const R3BCalifaCrystalCalData::container_t* fCrystalCalMap{};
    R3BCalifaClusterData::container_t* fClusterVec=new R3BCalifaClusterData::container_t;
    R3BCalifaGeometry* fGeo{};

    R3BCalifaClusterMatcher* fMatcher{};
  
    Bool_t fOnline;              // Selector for online data storage
  
    double fThreshold {100};         // Minimum energy requested in a crystal to be included in a Cal
    double fClThreshold {0};
    double fDRThreshold{15e3};       // Threshold for selecting gamma or proton branch in double reading channels
  

#if 0 // TODO: move to separate class.
    void SetRandomization(Bool_t rand) { fRand = rand; }

    void SetRandomizationFile(TString file)
    {

        fRandFile = file;
        fHistoFile = new TFile(fRandFile);
    }

  
    Bool_t fRand;      // Flag to set randomization procedure
    TString fRandFile; // File with angular coverages for each crystal
    TFile* fHistoFile;
    TH2F** fAngularDistributions;
#endif
  
    ClassDefOverride(R3BCalifaCrystalCal2Cluster, 2);
};

#endif /* R3BCALIFACRYSTALCAL2CLUSTAR_H */
