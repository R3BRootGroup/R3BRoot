// -----------------------------------------------------------------------------
// -----                R3BCalifaCrystalCal2Hit source file                -----
// -----                  Created 27/08/10  by H.Alvarez                   -----
// -----                Last modification: try "man git-log"               -----
// -----------------------------------------------------------------------------
#include "R3BCalifaCrystalCal2Hit.h"
#include "TMath.h"
#include "TVector3.h"
#include "TGeoMatrix.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TGeoManager.h"

#include "R3BCalifaGeometry.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaCrystalCalDataSim.h"
#include <vector>

#include "ROOT_template_hacks.h"
using roothacks::TCAHelper;
using roothacks::TypedCollection;
using std::cout;
using std::cerr;
using std::endl;

R3BCalifaCrystalCal2Hit::R3BCalifaCrystalCal2Hit() : 
  FairTask("R3B CALIFA CrystalCal to Hit Finder")
{
  this->SetConeAlg(); //stop macros from causing exceptions
}


R3BCalifaCrystalCal2Hit::~R3BCalifaCrystalCal2Hit()
{
  LOG(INFO) << "R3BCalifaCrystalCal2Hit: Delete instance";
  //delete fCrystalHitCA; // Just no. We do not own that! --pklenze
  if (fCalifaHitCA)
    delete fCalifaHitCA;
}



void R3BCalifaCrystalCal2Hit::SetParContainers()
{
  //commented-out stuff deleted. Look it up if you need it. --pklenze
}


// -----   Public method Init   --------------------------------------------
InitStatus R3BCalifaCrystalCal2Hit::Init()
{
  LOG(INFO) << "R3BCalifaCrystalCal2Hit::Init ";
  assert(!fCalifaHitCA); // in case someone calls Init() twice. 
  FairRootManager* ioManager = FairRootManager::Instance();
    if ( !ioManager ) LOG(fatal) << "Init: No FairRootManager";
  if( !ioManager->GetObject("CalifaCrystalCalDataSim") ) {
    fCrystalHitCA = dynamic_cast<TClonesArray*>(ioManager->GetObject("CalifaCrystalCalData"));
  } else {
    fCrystalHitCA = dynamic_cast<TClonesArray*>(ioManager->GetObject("CalifaCrystalCalDataSim"));
     kSimulation = true;
  }

  // Register output array either CalifaHitData or CalifaHitDataSim
  if(kSimulation) {
     fCalifaHitCA = new TClonesArray("R3BCalifaHitDataSim",50);
     ioManager->Register("CalifaHitDataSim", "CALIFA Hit Sim", fCalifaHitCA, kTRUE);
  } else {
     fCalifaHitCA = new TClonesArray("R3BCalifaHitData",50);
     if(!fOnline){
       ioManager->Register("CalifaHitData", "CALIFA Hit", fCalifaHitCA, kTRUE);
     }else{
       ioManager->Register("CalifaHitData", "CALIFA Hit", fCalifaHitCA, kFALSE);
     }
  }

  // Use new R3BCalifaGeometry class to get geometrical information
  fGeo = R3BCalifaGeometry::Instance(fGeometryVersion);

  // Parameter retrieval from par container
  // ...


  // Table for crystal parameters from Geometry


  // Initialization of variables, ...


  return kSUCCESS;
}



void R3BCalifaCrystalCal2Hit::SmearAllCrystalHits()
{
  //is cal2hit the correct place for this? --pklenze
  
  // Apply resolution smearing for simulation
  if(!kSimulation)
    return;
  for (auto h: TypedCollection<R3BCalifaCrystalCalData>::cast(fCrystalHitCA))
    {
      h.SetEnergy(ExpResSmearing(h.GetEnergy()));
      h.SetNf(CompSmearing(h.GetNf()));
      h.SetNs(CompSmearing(h.GetNs()));
    }
}


bool R3BCalifaCrystalCal2Hit::Match(R3BCalifaCrystalCalData *ref, R3BCalifaCrystalCalData *hit)
{
  if (ref==hit)
    return 1;
  
  auto circleAbs=[](double dphi) {double d=fmod(fabs(dphi), 2*M_PI); return d<M_PI?d:2*M_PI-d;};
  // Clusterization: you want to put a condition on the angle between the highest
  // energy crystal and the others. This is done by using the TVector3 classes and
  // not with different DeltaAngle on theta and phi, to get a proper solid angle
  // and not a "square" one.                    Enrico Fiori
  TVector3 vref=this->GetAnglesVector(ref->GetCrystalId());
  TVector3 vhit=this->GetAnglesVector(hit->GetCrystalId());
  bool takeCrystalInCluster=false;
  
  // Check if the angle between the two vectors is less than the reference angle.
  switch (clusterAlg) {
  case RECT: {  //rectangular window
    if (TMath::Abs(vref.Theta()- vhit.Theta()) < fDeltaPolar &&
	circleAbs(vref.Phi()-vhit.Phi())  < fDeltaAzimuthal ){
      takeCrystalInCluster = true;
    }
    break;
  }
  case ALL:
    takeCrystalInCluster = true;
    break;
  case NONE:
    break;
  case ROUND:  //round window
    // The angle is scaled to a reference distance (e.g. here is
    // set to 35 cm) to take into account Califa's non-spherical
    // geometry. The reference angle will then have to be defined
    // in relation to this reference distance: for example, 10° at
    // 35 cm corresponds to ~6cm, setting a fDeltaAngleClust=10
    // means that the gamma rays will be allowed to travel 6 cm in
    // the CsI, no matter the position of the crystal they hit.
    if ( ((vref.Angle(vhit))*((vref.Mag()+vhit.Mag())/(35.*2.))) <
	 fDeltaAngleClust )  {
      takeCrystalInCluster = true;
    }
    break;
  case ROUND_SCALED:  //round window scaled with energy
    // The same as before but the angular window is scaled
    // according to the energy of the hit in the higher energy
    // crystal. It needs a parameter that should be calibrated.
    {
      Double_t fDeltaAngleClustScaled = fDeltaAngleClust *
	(ref->GetEnergy()*energyFactor);
      if ( ((vref.Angle(vhit))*((vref.Mag()+vhit.Mag())/(35.*2.))) <
	   fDeltaAngleClustScaled )  {
	takeCrystalInCluster = true;
      }}
    break;
  case CONE:
    takeCrystalInCluster = vref.Angle(vhit)<fDeltaAngleClust;
    break;
  case PETAL:
    takeCrystalInCluster=AngleToPetalId(vref)==AngleToPetalId(vhit);
  case INVALID:
  default:
    throw std::runtime_error("R3BCalifaCrystalCal2Hit: no clustering"
			     " algorithm selected.");
    break;
  }
  LOG(DEBUG)<< "returning R3BCalifaCrystalCal2Hit::Match("
	    << ref->GetCrystalId()<<", "<<hit->GetCrystalId()
	    << ")=" << takeCrystalInCluster<<" with alg "<<clusterAlg;
  
  return takeCrystalInCluster;
}


// -----   Public method Exec   --------------------------------------------
void R3BCalifaCrystalCal2Hit::Exec(Option_t* opt)
{
  
  Reset();
  SmearAllCrystalHits();
  //ALGORITHMS FOR HIT FINDING
  
  // Nb of CrystalHits in current event
  const int numCrystalHits = fCrystalHitCA->GetEntries();
  LOG(DEBUG)<<"R3BCalifaCrystalCal2Hit::Exec(): crystal hits at start: "
	    << numCrystalHits <<"  ********************************";

  
  if (numCrystalHits)
    {
      auto h=dynamic_cast<R3BCalifaCrystalCalData*>((*fCrystalHitCA)[0]);
      // printf("id=%d\n", h->GetCrystalId());
    }
  std::list<R3BCalifaCrystalCalData*> unusedCrystalHits;
  auto addHit=[&](R3BCalifaCrystalCalData* h)
    {
      if (h->GetEnergy()>fThreshold)
	  unusedCrystalHits.push_back(h);
      else
	  LOG(DEBUG)<<"R3BCalifaCrystalCal2Hit::Exec(): rejected hit in "
		    << h->GetCrystalId()  <<" because of low energy (E="
		    <<h->GetEnergy() <<"<="<<fThreshold<<"=E_threshold";

    };


  // get rid if redundant (dual range) crystals
  { 
    std::map<uint32_t, R3BCalifaCrystalCalData*> crystalId2Pos;
    for (auto& h: TypedCollection<R3BCalifaCrystalCalData>::cast(fCrystalHitCA))
      crystalId2Pos[h.GetCrystalId()]=&h;

    LOG(DEBUG)<<"R3BCalifaCrystalCal2Hit::Exec():  crystalId2Pos.size()="
	      << crystalId2Pos.size();

    for (auto& k1: crystalId2Pos) // k1: lower id, gamma branch?
      if (crystalId2Pos.count(k1.first+5000))
	{
	  auto proton=*crystalId2Pos.find(k1.first+5000);
	  //k2: higher id, proton branch
	  if (proton.second->GetEnergy()<fDRThreshold)
	    addHit(k1.second); // gamma
	  else
	    addHit(proton.second);
	}
      else if (!crystalId2Pos.count(k1.first-5000))
	// not a hit where two ranges were hit
	addHit(k1.second);
  }
  LOG(DEBUG)<<"R3BCalifaCrystalCal2Hit::Exec(): after uniquifying, we have "
	    <<unusedCrystalHits.size()<<" crystal hits.";
  
  unusedCrystalHits.sort([](R3BCalifaCrystalCalData* lhs,
			    R3BCalifaCrystalCalData* rhs)
			 {return lhs->GetEnergy()>rhs->GetEnergy();});
  uint32_t clusterId=0;
  while (!unusedCrystalHits.empty())
    {
      auto highest=unusedCrystalHits.front();
      LOG(DEBUG)<<"R3BCalifaCrystalCal2Hit::Exec(): starting cluster at "
		<<"crystal "<<highest->GetCrystalId()<<", E="
		<<highest->GetEnergy();

      // Note: we do not remove highest, but process it like any others
      uint64_t time=highest->GetTime();
      auto vhighest=GetAnglesVector(highest->GetCrystalId());
      auto clusterHit=TCAHelper<R3BCalifaHitData>::AddNew(*fCalifaHitCA,
							  time,
							  vhighest.Theta(),
							  vhighest.Phi(),
							  clusterId);
      
      // loop through remaining crystals, remove matches from list.
      auto i=unusedCrystalHits.begin();
      while (i != unusedCrystalHits.end())
	if (this->Match(highest, *i))
	  {
	    LOG(DEBUG)<<"R3BCalifaCrystalCal2Hit::Exec(): adding  "
		      <<"crystal "<<(*i)->GetCrystalId()<<", E="
		      <<(*i)->GetEnergy();

	    *clusterHit+=**i;
	    i=unusedCrystalHits.erase(i);
	  } else ++i;
      ++clusterId;
    }
}


// ---- Public method Reset   --------------------------------------------------
void R3BCalifaCrystalCal2Hit::Reset()
{
  // Clear the CA structure
  LOG(DEBUG) << "Clearing CalifaHitData Structure";
  if (fCalifaHitCA) fCalifaHitCA->Clear();
}

// -----  Public method SelectGeometryVersion  ----------------------------------
void R3BCalifaCrystalCal2Hit::SelectGeometryVersion(Int_t version)
{
  fGeometryVersion = version;
}


// -----  Public method SetExperimentalResolution  ----------------------------------
void R3BCalifaCrystalCal2Hit::SetExperimentalResolution(Double_t crystalRes)
{
  fCrystalResolution = crystalRes;
  LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetExperimentalResolution to "
	    << fCrystalResolution << "% @ 1 MeV.";
}


// -----  Public method SetComponentResolution  ----------------------------------
void R3BCalifaCrystalCal2Hit::SetComponentResolution(Double_t componentRes)
{
  fComponentResolution = componentRes;
  LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetComponentResolution to "
	    << fComponentResolution << " MeV.";
}

// -----  Public method SetExperimentalResolution  ----------------------------------
void R3BCalifaCrystalCal2Hit::SetPhoswichResolution(Double_t LaBr,
						    Double_t LaCl)
{
  fLaBrResolution = LaBr;
  fLaClResolution = LaCl;
  LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetPhoswichResolution to "
	    << fLaBrResolution << "% @ 1 MeV (LaBr) and to "
	    << fLaClResolution << "% @ 1 MeV (LaCl)";
}

// -----  Public method SetDetectionThreshold  ----------------------------------
void R3BCalifaCrystalCal2Hit::SetDetectionThreshold(Double_t thresholdEne)
{
  fThreshold = thresholdEne;
  if(kSimulation) {
    LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetDetectionThreshold to "
	     << fThreshold << " GeV.";
  }
  else{
    LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetDetectionThreshold to "
         << fThreshold << " keV.";
  }
}

// -----  Public method SetDRThreshold  ----------------------------------
void R3BCalifaCrystalCal2Hit::SetDRThreshold(Double_t DRthresholdEne)
{
  fDRThreshold = DRthresholdEne;
  LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetDRThreshold to "
	    << fDRThreshold << " keV.";
}



TVector3 R3BCalifaCrystalCal2Hit::GetAnglesVector(int id)
{
  return fGeo->GetAngles(id);
}

// -----   Private method ExpResSmearing  --------------------------------------------
Double_t R3BCalifaCrystalCal2Hit::ExpResSmearing(Double_t inputEnergy)
{
  // Smears the energy according to some Experimental Resolution distribution
  // Very simple preliminary scheme where the Experimental Resolution
  // is introduced as a gaus random distribution with a width given by the
  // parameter fCrystalResolution(in % @ MeV). Scales according to 1/sqrt(E)
  //
  // The formula is   TF1("name","0.058*x/sqrt(x)",0,10) for 3% at 1MeV (3.687 @ 662keV)
  //  ( % * energy ) / sqrt( energy )
  // and then the % is given at 1 MeV!!
  //
  if (fCrystalResolution == 0) return inputEnergy;
  else {
    //Energy in MeV, that is the reason for the factor 1000...
    Double_t randomIs = gRandom->Gaus(0,inputEnergy*fCrystalResolution*1000/(235*sqrt(inputEnergy*1000)));
    return inputEnergy + randomIs/1000;
  }
}


// -----   Private method CompSmearing  --------------------------------------------
Double_t R3BCalifaCrystalCal2Hit::CompSmearing(Double_t inputComponent)
{
  // Smears the components Ns and Nf according to fComponentResolution
  //
  if (fComponentResolution == 0) return inputComponent;
  else {
    Double_t randomIs = gRandom->Gaus(0,fComponentResolution);
    return inputComponent + randomIs;
  }
}

// -----   Private method PhoswichSmearing  --------------------------------------------
Double_t R3BCalifaCrystalCal2Hit::PhoswichSmearing(Double_t inputEnergy, bool isLaBr)
{
  // Smears the LaBr and LaCl according to fLaBr Resolution and fLaCl Resolution
  //
  Double_t fResolution = 0.;
  if(isLaBr)
    fResolution = fLaBrResolution;
  else
    fResolution = fLaClResolution;

  if (fResolution == 0)
    return inputEnergy;
  else {
    Double_t randomIs = gRandom->Gaus(0,inputEnergy*fResolution/(235*sqrt(inputEnergy)));
    return inputEnergy + randomIs;
  }
}

// -----   Private method isPhoswich  --------------------------------------------
bool R3BCalifaCrystalCal2Hit::IsPhoswich(Int_t crystalid)
{
  // Smears the LaBr and LaCl according to fLaBr Resolution and fLaCl Resolution
  //
  if(crystalid > 3000 && (crystalid - 3000)%24 < 8)
    return true;
  else
    return false;
}



ClassImp(R3BCalifaCrystalCal2Hit)
