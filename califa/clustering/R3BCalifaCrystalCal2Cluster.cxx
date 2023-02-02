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

#include "R3BCalifaCrystalCal2Cluster.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TRandom.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BLogger.h"
#include "R3BTGeoPar.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaGeometry.h"
#include <list>
#include <vector>


void R3BCalifaCrystalCal2Cluster::SetParContainers()
{
    // Load CALIFA and target positions from containers
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();

    auto califaGeoPar = dynamic_cast<R3BTGeoPar*>(rtdb->getContainer("CalifaGeoPar"));
    auto targetGeoPar = dynamic_cast<R3BTGeoPar*>(rtdb->getContainer("TargetGeoPar"));
    if (!califaGeoPar || !targetGeoPar)
    {
        R3BLOG_IF(warn, !califaGeoPar, "Could not get access to CalifaGeoPar container.");
        R3BLOG_IF(warn, !targetGeoPar, "Could not get access to TargetGeoPar container.");
        return;
    }
    R3BLOG(info, "Container CalifaGeoPar found.");
    R3BLOG(info, "Container TargetGeoPar found.");

    ROOT::Math::XYZVector targetPos {0, 0, 0};
    ROOT::Math::XYZVector califaPos {0, 0, 0};

    targetPos.SetXYZ(targetGeoPar->GetPosX(), targetGeoPar->GetPosY(), targetGeoPar->GetPosZ());
    califaPos.SetXYZ(califaGeoPar->GetPosX(), califaGeoPar->GetPosY(), califaGeoPar->GetPosZ());

    fCalifatoTargetPos = targetPos - califaPos;
    
    LLOG(info)<< "Using fCalifatoTargetPos := targetPos - califaPos = " <<  targetPos  << " - " <<  califaPos << "==" << fCalifatoTargetPos;
    return;
}

InitStatus R3BCalifaCrystalCal2Cluster::Init()
{
    R3BLOG(info, "");
    assert(!fCrystalCalMap && "Init called twice!");
    FairRootManager* frm = FairRootManager::Instance();
    R3BLOG_IF(fatal, !frm, "FairRootManager not found");

    {
      auto tmp = frm->InitObjectAs<const R3BCalifaCrystalCalData::container_t*>
	(R3BCalifaCrystalCalData::default_container_name);
      fCrystalCalMap=tmp; //const_cast<decltype(fCrystalCalMap)>(tmp); // sue me. 
      assert(fCrystalCalMap);
    }
    
    frm->RegisterAny(R3BCalifaClusterData::default_container_name, fClusterVec, !fOnline);
    
    fGeo=R3BCalifaGeometry::Instance();
    return kSUCCESS;
}

InitStatus R3BCalifaCrystalCal2Cluster::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BCalifaCrystalCal2Cluster::AddPositionInfo(const R3BCalifaCrystalCalData& hit)
{
  hit.fPos=fGeo->GetCrystalCenterPos(hit.fCrystalId) - fCalifatoTargetPos;
}

void R3BCalifaCrystalCal2Cluster::AddCandidate(candidate_t& unusedCrystalHits, const R3BCalifaCrystalCalData& hit)
{
  if (hit.GetEnergy() > fThreshold)
    unusedCrystalHits.push_back(&hit);
  else
    LLOG(debug) << " rejected hit in " << hit.GetCrystalId() << " because of low energy (E="
		<< hit.GetEnergy() << "<=" << fThreshold << "=E_threshold";
  
}
void R3BCalifaCrystalCal2Cluster::FilterCrystals(candidate_t& unusedCrystalHits)
{
  for(auto& h: *fCrystalCalMap)
    // get rid if redundant (dual range) crystals
    {
      int cryId=h.first;
      auto& hit=h.second;

      if (fGeo->IsHighGain(cryId)) // low gain
	  {
	    auto& hg_hit=hit;
	    if (auto it=fCrystalCalMap->find(fGeo->ToLowGain(cryId)); it!=fCrystalCalMap->end())
	      {
		// we have two measurements for that crystal, use DR threshold to pick the appropriate
		auto& lg_hit=it->second;
		if (lg_hit.fEnergy < fDRThreshold)
		  AddCandidate(unusedCrystalHits, hg_hit); // high gain, "gamma"
		else
		  AddCandidate(unusedCrystalHits, lg_hit); // low gain, "proton"
	      }
	    else // hit is unique
	      AddCandidate(unusedCrystalHits, hg_hit); 
	  }
        else // if the hit is part of a dual range measurement, it will be handled above
	  if (!fCrystalCalMap->count(fGeo->ToHighGain(cryId))) // unique measurement
	    AddCandidate(unusedCrystalHits, hit);
    }
  LLOG(debug) << "after uniquifying, we have " << unusedCrystalHits.size() << " crystal hits.";

}


namespace
{
  bool energy_compare(const R3BCalifaCrystalCalData* lhs,
		      const R3BCalifaCrystalCalData* rhs)
  {
    return lhs->GetEnergy() > rhs->GetEnergy();
  }
}

void R3BCalifaCrystalCal2Cluster::SortCandidates(candidate_t& unusedCrystalHits)
{
  unusedCrystalHits.sort(energy_compare);
}

void R3BCalifaCrystalCal2Cluster::DoClustering(candidate_t& unusedCrystalHits)
{
    fClusterVec->clear();
    while (!unusedCrystalHits.empty())
    {
        auto highest = unusedCrystalHits.front();
	if (highest->fEnergy<fClThreshold)
	  {
	    LLOG(debug) << "stopping processing, energy " << highest->fEnergy << "< fClThreshold == "<< fClThreshold;
	    // stop processing when below fClThreshold
	    break;
	  }
	
        LLOG(debug) << "starting cluster at "
                   << "crystal " << highest->GetCrystalId() << ", E=" << highest->GetEnergy();

        // Note: we do not remove highest, but process it like any others
	auto& clusterHit = fClusterVec->emplace_back(highest->fWrts, highest->fPos, highest->fCrystalId);

        // loop through remaining crystals, remove matches from list.
	// the syntax for deleting elements from a list while iterating over it is a bit painful. 
        auto it = unusedCrystalHits.begin();
        while (it != unusedCrystalHits.end())
	  if (fMatcher->DoMatch(clusterHit, **it))
	    {
                LLOG(debug) << " adding crystal " << (*it)->GetCrystalId() << ", E=" << (*it)->GetEnergy();

                clusterHit += **it; // Use R3BCalifaClusterData::operator+= to add hit to cluster
                it = unusedCrystalHits.erase(it);
            }
            else
	      ++it;
    }
}

// -----   Public method Exec   --------------------------------------------
void R3BCalifaCrystalCal2Cluster::Exec(Option_t* opt)
{
  candidate_t cand;
  FilterCrystals(cand);
  SortCandidates(cand);
  DoClustering(cand);
}



#if 0 // randomization init
    if (fRand)
    {

        fAngularDistributions = new TH2F*[4864];

        char name[100];
        for (Int_t i = 0; i < 4864; i++)
        {

            sprintf(name, "distributionCrystalID_%i", i + 1);
            fHistoFile->GetObject(name, fAngularDistributions[i]);
        }
    }

//randomization exec
        if (fRand)
        {

            if (highest->GetCrystalId() <= 2432)
                fAngularDistributions[highest->GetCrystalId() - 1]->GetRandom2(fRandPhi, fRandTheta);

            else
                fAngularDistributions[highest->GetCrystalId() - 1 - 2432]->GetRandom2(fRandPhi, fRandTheta);

            clusterHit = TCAHelper<R3BCalifaClusterData>::AddNew(
                *fCalifaClusterData, time, TMath::DegToRad() * fRandTheta, TMath::DegToRad() * fRandPhi, clusterId);
        }

#endif


ClassImp(R3BCalifaCrystalCal2Cluster);
