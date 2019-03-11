// -----------------------------------------------------------------------------
// -----                R3BCalifaCrystalCal2Hit source file                -----
// -----                  Created 27/08/10  by H.Alvarez                   -----
// -----                Last modification 19/12/16 by P.Cabanelas          -----
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

using std::cout;
using std::cerr;
using std::endl;


R3BCalifaCrystalCal2Hit::R3BCalifaCrystalCal2Hit() : FairTask("R3B CALIFA CrystalCal to Hit Finder")
{
  fGeometryVersion=17; //default version 8.11 BARREL
  fThreshold=0.;     //no threshold
  fDRThreshold=15000; //in keV, for real data
  fCrystalResolution=0.; //perfect crystals
  fComponentResolution=0.; //perfect crystals
  fLaBrResolution=0;
  fLaClResolution=0;
  fDeltaPolar=0.25;
  fDeltaAzimuthal=0.25;
  fDeltaAngleClust=0;
  fClusteringAlgorithmSelector=1;
  fParCluster1=0;
  kSimulation = false;
  fCrystalHitCA=0;
  fCalifaHitCA=0;
  nEvents=0;
  fGeo=0;
  //fCalifaHitFinderPar=0;
}


R3BCalifaCrystalCal2Hit::~R3BCalifaCrystalCal2Hit()
{
  LOG(INFO) << "R3BCalifaCrystalCal2Hit: Delete instance" << FairLogger::endl;
  delete fCrystalHitCA;
  delete fCalifaHitCA;
}



void R3BCalifaCrystalCal2Hit::SetParContainers()
{
  // // Get run and runtime database
  // FairRunAna* run = FairRunAna::Instance();
    // if (!run) LOG(fatal) << "R3BCalifaCrystalCal2Hit::SetParContainers: No analysis run";

  // FairRuntimeDb* rtdb = run->GetRuntimeDb();
    // if (!rtdb) LOG(fatal) << "R3BCalifaCrystalCal2Hit::SetParContainers: No runtime database";

  // fCalifaHitFinderPar = (R3BCalifaCrystalCal2HitPar*)(rtdb->getContainer("R3BCalifaCrystalCal2HitPar"));
  // if ( fVerbose && fCalifaHitFinderPar ) {
  //   LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetParContainers() "<< FairLogger::endl;
  //   LOG(INFO) << "Container R3BCalifaCrystalCal2HitPar loaded " << FairLogger::endl;
  // }

}


// -----   Public method Init   --------------------------------------------
InitStatus R3BCalifaCrystalCal2Hit::Init()
{
  LOG(INFO) << "R3BCalifaCrystalCal2Hit::Init " << FairLogger::endl;
  FairRootManager* ioManager = FairRootManager::Instance();
    if ( !ioManager ) LOG(fatal) << "Init: No FairRootManager";
  if( !ioManager->GetObject("CalifaCrystalCalDataSim") ) {
     fCrystalHitCA = (TClonesArray*) ioManager->GetObject("CalifaCrystalCalData");
  } else {
     fCrystalHitCA = (TClonesArray*) ioManager->GetObject("CalifaCrystalCalDataSim");
     kSimulation = true;
  }

  // Register output array either CalifaHitData or CalifaHitDataSim
  if(kSimulation) {
     fCalifaHitCA = new TClonesArray("R3BCalifaHitDataSim",1000);
     ioManager->Register("CalifaHitDataSim", "CALIFA Hit Sim", fCalifaHitCA, kTRUE);
  } else {
     fCalifaHitCA = new TClonesArray("R3BCalifaHitData",1000);
     ioManager->Register("CalifaHitData", "CALIFA Hit", fCalifaHitCA, kTRUE);
  }

  // Use new R3BCalifaGeometry class to get geometrical information
  fGeo = R3BCalifaGeometry::Instance(fGeometryVersion);

  // Parameter retrieval from par container
  // ...


  // Table for crystal parameters from Geometry


  // Initialization of variables, ...


  return kSUCCESS;

}



// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCalifaCrystalCal2Hit::ReInit()
{


  return kSUCCESS;

}


// -----   Public method Exec   --------------------------------------------
void R3BCalifaCrystalCal2Hit::Exec(Option_t* opt)
{
  
  //if(++nEvents % 10000 == 0)
  //LOG(INFO) << nEvents << FairLogger::endl;
  
  // Reset entries in output arrays, local arrays
  Reset();
  
  
  //ALGORITHMS FOR HIT FINDING
  
  ULong64_t hitTime=0;
  Double_t energy=0.;       // caloHits energy
  Double_t Nf=0.;           // caloHits Nf
  Double_t Ns=0.;           // caloHits Ns
  Double_t polarAngle=0.;     // caloHits reconstructed polar angle
  Double_t azimuthalAngle=0.;   // caloHits reconstructed azimuthal angle
  Double_t rhoAngle=0.;   // caloHits reconstructed rho
  Double_t angle1,angle2;
  Double_t eInc=0.;       // total incident energy (only for simulation)
  
  bool* usedCrystalHits=NULL; //array to control the CrystalHits
  Int_t crystalsInHit=0;  //used crystals in each CalifaHitData
  Double_t testPolar=0 ;
  Double_t testAzimuthal=0 ;
  Double_t testRho =0 ;
  bool takeCrystalInCluster;
  
  R3BCalifaCrystalCalData**    crystalHit = NULL;
  
  Int_t crystalHits;        // Nb of CrystalHits in current event
  crystalHits = fCrystalHitCA->GetEntries();
  
  if (crystalHits == 0)
    return;
  
  crystalHit = new R3BCalifaCrystalCalData*[crystalHits];
  usedCrystalHits = new bool[crystalHits];
  for (Int_t i=0; i<crystalHits; i++) {
    crystalHit[i] = (R3BCalifaCrystalCalData*) fCrystalHitCA->At(i);
    if(kSimulation){
      // Apply resolution smearing for simulation
      crystalHit[i]->SetEnergy(ExpResSmearing(crystalHit[i]->GetEnergy()));
      crystalHit[i]->SetNf(CompSmearing(crystalHit[i]->GetNf()));
      crystalHit[i]->SetNs(CompSmearing(crystalHit[i]->GetNs()));
    }
    usedCrystalHits[i] = 0;
  }
  
  //For the moment a simple analysis... more to come
  Int_t crystalWithHigherEnergy = 0;
  Int_t unusedCrystals = crystalHits;
  
  //removing those crystals with energy below the threshold
  for (Int_t i=0; i<crystalHits; i++) {
    if (crystalHit[i]->GetEnergy()<fThreshold) {
      usedCrystalHits[i] = 1;
      unusedCrystals--;
    }
  }
  
  //S444 for crystals with double reading!!
  //taking only the proton branch in the cluster
  for (Int_t i=0; i<crystalHits; i++) {
    for (Int_t j=i; j<crystalHits; j++) {
      if (abs(crystalHit[i]->GetCrystalId()-crystalHit[j]->GetCrystalId()) == 5000) {
        if(crystalHit[i]->GetCrystalId() > crystalHit[j]->GetCrystalId()){ //i is protonbranch
          if(crystalHit[i]->GetEnergy()<fDRThreshold) { //take j, the gammabranch
            usedCrystalHits[i] = 1;
            unusedCrystals--;
          } else{ //take i, the protonbranch
            usedCrystalHits[j] = 1;
            unusedCrystals--;
          }
	} else{//j is protonbranch
	  if(crystalHit[i]->GetEnergy()<fDRThreshold) { //take i, the gammabranch
            usedCrystalHits[j] = 1;
            unusedCrystals--;
          } else{ //take j, the protonbranch
            usedCrystalHits[i] = 1;
            unusedCrystals--;
          }
	}
      }
    }
  }
  
  //  int n_clusters = 0;
  
  while (unusedCrystals>0) {
    // First, finding the crystal with higher energy from the unused crystalHits
    for (Int_t i=1; i<crystalHits; i++) {
      if (!usedCrystalHits[i] && crystalHit[i]->GetEnergy() >
	  crystalHit[crystalWithHigherEnergy]->GetEnergy())
	crystalWithHigherEnergy = i;
    }
    
    usedCrystalHits[crystalWithHigherEnergy] = 1;
    unusedCrystals--; crystalsInHit++;
    
    // Second, energy and angles come from the crystal with the higher energy
    hitTime = crystalHit[crystalWithHigherEnergy]->GetTime();
    energy = crystalHit[crystalWithHigherEnergy]->GetEnergy();
    Nf = crystalHit[crystalWithHigherEnergy]->GetNf();
    Ns = crystalHit[crystalWithHigherEnergy]->GetNs();
    GetAngles(crystalHit[crystalWithHigherEnergy]->GetCrystalId(),
	      &polarAngle,&azimuthalAngle,&rhoAngle);
    
    // Third, finding closest hits and adding their energy
    // Clusterization: you want to put a condition on the angle between the highest
    // energy crystal and the others. This is done by using the TVector3 classes and
    // not with different DeltaAngle on theta and phi, to get a proper solid angle
    // and not a "square" one.                    Enrico Fiori
    TVector3 refAngle(1,0,0);     // EF
    refAngle.SetTheta(polarAngle);
    refAngle.SetPhi(azimuthalAngle);
    for (Int_t i=0; i<crystalHits; i++) {
      if (!usedCrystalHits[i] ) {
          GetAngles(crystalHit[i]->GetCrystalId(), &testPolar,
		    &testAzimuthal, &testRho);

        //if(kSimulation) eInc += crystalHitSim[i]->GetEinc();

        takeCrystalInCluster = false;

        TVector3 testAngle(1,0,0);       //EF
        testAngle.SetTheta(testPolar);
        testAngle.SetPhi(testAzimuthal);
        // Check if the angle between the two vectors is less than the reference angle.
        switch (fClusteringAlgorithmSelector) {
        case 1: {  //square window
          //Dealing with the particular definition of azimuthal
	  //angles (discontinuity in pi and -pi)
          if (azimuthalAngle + fDeltaAzimuthal > TMath::Pi()) {
            angle1 = azimuthalAngle-TMath::Pi();
	    angle2 = testAzimuthal-TMath::Pi();
          } else if (azimuthalAngle - fDeltaAzimuthal < -TMath::Pi()) {
            angle1 = azimuthalAngle+TMath::Pi();
	    angle2 = testAzimuthal+TMath::Pi();
          } else {
            angle1 = azimuthalAngle; angle2 = testAzimuthal;
          }
          if (TMath::Abs(polarAngle - testPolar) < fDeltaPolar &&
              TMath::Abs(angle1 - angle2) < fDeltaAzimuthal ){
                 takeCrystalInCluster = true;
          }
          break;
        }
        case 2:  //round window
          // The angle is scaled to a reference distance (e.g. here is
	  // set to 35 cm) to take into account Califa's non-spherical
	  // geometry. The reference angle will then have to be defined
	  // in relation to this reference distance: for example, 10° at
	  // 35 cm corresponds to ~6cm, setting a fDeltaAngleClust=10
	  // means that the gamma rays will be allowed to travel 6 cm in
	  // the CsI, no matter the position of the crystal they hit.
          if ( ((refAngle.Angle(testAngle))*((testRho+rhoAngle)/(35.*2.))) <
	       fDeltaAngleClust )  {
                  takeCrystalInCluster = true;
          }
          break ;
        case 3: {  //round window scaled with energy
          // The same as before but the angular window is scaled
	  // according to the energy of the hit in the higher energy
	  // crystal. It needs a parameter that should be calibrated.
            Double_t fDeltaAngleClustScaled = fDeltaAngleClust *
	      (crystalHit[crystalWithHigherEnergy]->GetEnergy()*fParCluster1);
            if ( ((refAngle.Angle(testAngle))*((testRho+rhoAngle)/(35.*2.))) <
		 fDeltaAngleClustScaled )  {
                  takeCrystalInCluster = true;
            }
          break;
        }
        case 4: // round window scaled with the energy of the _two_ hits
	  //(to be tested and implemented!!)
          // More advanced: the condition on the distance between the
	  //two hits is function of the energy of both hits
          break;
        }

        if(takeCrystalInCluster)
        {
              energy += crystalHit[i]->GetEnergy();
  	      Nf += crystalHit[i]->GetNf();
	      Ns += crystalHit[i]->GetNs();
              usedCrystalHits[i] = 1; unusedCrystals--; crystalsInHit++;

              if(kSimulation)
                  eInc += dynamic_cast<R3BCalifaCrystalCalDataSim*>(crystalHit[i])->GetEinc();
        }
      }
    }


    if(kSimulation) {
      AddHitSim(crystalsInHit, energy, Nf, Ns, polarAngle, azimuthalAngle, eInc);
    } else {
      AddHit(crystalsInHit, energy, Nf, Ns, polarAngle, azimuthalAngle, hitTime);
//      n_clusters++;
    }

    crystalsInHit = 0; //reset for next CalifaHitData

    //Finally, setting crystalWithHigherEnergy to the first unused
    //crystalHit (for the next iteration)
    for (Int_t i=0; i<crystalHits; i++) {
      if (!usedCrystalHits[i]) {
        crystalWithHigherEnergy = i;
        break;
      }
    }
  }

//  std::cout << "# " << n_clusters << "\n--------\n";

  if(crystalHit)
     delete[] crystalHit;
  if(usedCrystalHits)
     delete[] usedCrystalHits;
}


// ---- Public method Reset   --------------------------------------------------
void R3BCalifaCrystalCal2Hit::Reset()
{
  // Clear the CA structure
  LOG(DEBUG) << "Clearing CalifaHitData Structure" << FairLogger::endl;
  if (fCalifaHitCA) fCalifaHitCA->Clear();
}




// ---- Public method Finish   --------------------------------------------------
void R3BCalifaCrystalCal2Hit::Finish()
{
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
	    << fCrystalResolution << "% @ 1 MeV." << FairLogger::endl;
}


// -----  Public method SetComponentResolution  ----------------------------------
void R3BCalifaCrystalCal2Hit::SetComponentResolution(Double_t componentRes)
{
  fComponentResolution = componentRes;
  LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetComponentResolution to "
	    << fComponentResolution << " MeV." << FairLogger::endl;
}

// -----  Public method SetExperimentalResolution  ----------------------------------
void R3BCalifaCrystalCal2Hit::SetPhoswichResolution(Double_t LaBr, Double_t LaCl)
{
  fLaBrResolution = LaBr;
  fLaClResolution = LaCl;
  LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetPhoswichResolution to "
	    << fLaBrResolution << "% @ 1 MeV (LaBr) and to " << fLaClResolution << "% @ 1 MeV (LaCl)" << FairLogger::endl;
}

// -----  Public method SetDetectionThreshold  ----------------------------------
void R3BCalifaCrystalCal2Hit::SetDetectionThreshold(Double_t thresholdEne)
{
  fThreshold = thresholdEne;
  if(kSimulation) {
    LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetDetectionThreshold to "
	     << fThreshold << " GeV." << FairLogger::endl;
  }
  else{
    LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetDetectionThreshold to "
         << fThreshold << " keV." << FairLogger::endl;
  }
}

// -----  Public method SetDRThreshold  ----------------------------------
void R3BCalifaCrystalCal2Hit::SetDRThreshold(Double_t DRthresholdEne)
{
  fDRThreshold = DRthresholdEne;
  LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetDRThreshold to "
	    << fDRThreshold << " keV." << FairLogger::endl;
}


// ---- Public method GetAngles   --------------------------------------------------
void R3BCalifaCrystalCal2Hit::GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal)
{
	//Old GetAngles with two arguments...
	Double_t rho=0;
	GetAngles(iD, polar, azimuthal, &rho);
}


// ---- Public method GetAngles   --------------------------------------------------
void R3BCalifaCrystalCal2Hit::GetAngles(Int_t iD, Double_t* polar,
				 Double_t* azimuthal, Double_t* rho)
{
  Double_t local[3]={0,0,0};
  Double_t master[3];
  Int_t crystalType = 0;
  Int_t crystalCopy = 0;
  Int_t alveolusCopy =0;
  Int_t crystalInAlveolus=0;

  if (fGeometryVersion>=16) {
    // Use new R3BCalifaGeometry class to get geometrical information
    fGeo->GetAngles(iD, polar, azimuthal, rho);
    return;
  } else LOG(ERROR) << "R3BCalifaCrystalCal2Hit: Geometry version not available in R3BCalifa::ProcessHits(). " << FairLogger::endl;


  //TVector3 masterV(master[0],master[1],master[2]);
  //masterV.Print();
  //*polar=masterV.Theta();
  //*azimuthal=masterV.Phi();
  //*rho=masterV.Mag();
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
bool R3BCalifaCrystalCal2Hit::isPhoswich(Int_t crystalid)
{
  // Smears the LaBr and LaCl according to fLaBr Resolution and fLaCl Resolution
  //
  if(crystalid > 3000 && (crystalid - 3000)%24 < 8)
    return true;
  else
    return false;
}


// -----   Public method SetClusteringAlgorithm  --------------------------------------------
void R3BCalifaCrystalCal2Hit::SetClusteringAlgorithm(Int_t ClusteringAlgorithmSelector, Double_t ParCluster1)
{
  // Select the clustering algorithm and the parameters of some of them
  // ClusteringAlgorithmSelector = 1  ->  square window
  // ClusteringAlgorithmSelector = 2  ->  round window
  // ClusteringAlgorithmSelector = 3  ->  advanced round window with opening proportional to the
  //                                     energy of the hit, need ParCluster1
  // ClusteringAlgorithmSelector = 4  ->  advanced round window with opening proportional to the
  //                                     energy of the two hit, need ParCluster1 NOT YET IMPLEMENTED!
  fClusteringAlgorithmSelector = ClusteringAlgorithmSelector ;
  fParCluster1 = ParCluster1 ;
}

// -----   Public method SetAngularWindow  --------------------------------------------
void R3BCalifaCrystalCal2Hit::SetAngularWindow(Double_t deltaPolar, Double_t deltaAzimuthal, Double_t DeltaAngleClust)
{
  //
  // Set the angular window open around the crystal with the largest energy
  // to search for additional crystal hits and addback to the same cal hit
  // [0.25 around 14.3 degrees, 3.2 for the complete calorimeter]
  fDeltaPolar = deltaPolar;
  fDeltaAzimuthal = deltaAzimuthal;
  fDeltaAngleClust = DeltaAngleClust;

}


// -----   Private method AddHit  --------------------------------------------
R3BCalifaHitData* R3BCalifaCrystalCal2Hit::AddHit(UInt_t Nbcrystals,Double_t ene,Double_t Nf,Double_t Ns,Double_t pAngle,Double_t aAngle, ULong64_t time)
{
//std::cout << "Ncrystals = " << Nbcrystals << endl;
  // It fills the R3BCalifaHitData array
  TClonesArray& clref = *fCalifaHitCA;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BCalifaHitData(Nbcrystals, ene, Nf, Ns, pAngle, aAngle, time);
}

// -----   Private method AddHitSim  --------------------------------------------
R3BCalifaHitDataSim* R3BCalifaCrystalCal2Hit::AddHitSim(UInt_t Nbcrystals,Double_t ene,Double_t Nf,Double_t Ns,Double_t pAngle,Double_t aAngle, Double_t einc)
{
  // It fills the R3BCalifaHitDataSim array
  TClonesArray& clref = *fCalifaHitCA;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BCalifaHitDataSim(Nbcrystals, ene, Nf, Ns, pAngle, aAngle, einc);
}

/*

 Double_t GetCMEnergy(Double_t theta, Double_t energy){
 //
 // Calculating the CM energy from the lab energy and the polar angle
 //
 Double_t beta = 0.8197505718204776;  //beta is 0.8197505718204776
 Double_t gamma = 1/sqrt(1-beta*beta);
 //Lorenzt boost correction
 //E' = gamma E + beta gamma P|| = gamma E + beta gamma P cos(theta)
 //In photons E=P
 Double_t energyCorrect = gamma*energy - beta*gamma*energy*cos(theta);

 return energyCorrect;
 }

*/


ClassImp(R3BCalifaCrystalCal2Hit)
