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
  fCrystalResolution=0.; //perfect crystals
  fDeltaPolar=0.25;
  fDeltaAzimuthal=0.25;
  fDeltaAngleClust=0;
  fClusteringAlgorithmSelector=1;
  fParCluster1=0;
  kSimulation = false;
  fCrystalHitCA=0;
  fCalifaHitCA=0;
  nEvents=0;
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
  // if (!run) Fatal("R3BCalifaCrystalCal2Hit::SetParContainers", "No analysis run");

  // FairRuntimeDb* rtdb = run->GetRuntimeDb();
  // if (!rtdb) Fatal("R3BCalifaCrystalCal2Hit::SetParContainers", "No runtime database");

  // fCalifaHitFinderPar = (R3BCalifaCrystalCal2HitPar*)(rtdb->getContainer("R3BCalifaCrystalCal2HitPar"));
  // if ( fVerbose && fCalifaHitFinderPar ) {
  //   LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetParContainers() "<< FairLogger::endl;
  //   LOG(INFO) << "Container R3BCalifaCrystalCal2HitPar loaded " << FairLogger::endl;
  // } 
  
}


// -----   Public method Init   --------------------------------------------
InitStatus R3BCalifaCrystalCal2Hit::Init()
{

  FairRootManager* ioManager = FairRootManager::Instance();
  if ( !ioManager ) Fatal("Init", "No FairRootManager");
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

  // Parameter retrieval from par container
  // ...


  // Table for crystal parameters from Geometry


  // Initialization of variables, histograms, ...


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

  if(++nEvents % 10000 == 0)
	LOG(INFO) << nEvents << FairLogger::endl;

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
    if(kSimulation)
    {
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
  LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetDetectionThreshold to " 
	    << fThreshold << " GeV." << FairLogger::endl;
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

  //  TGeoVolume *pAWorld  =  gGeoManager->GetTopVolume();
  if (fGeometryVersion==0) {
    //The present scheme here done works nicely with 5.0
    // crystalType = crystal type (from 1 to 30)
    // crystalCopy = crystal copy (from 1 to 512 for crystal types from 1 to 6 
    //          (BARREL), from 1 to 64 for crystal types from 7 to 30 (ENDCAP))
    // crystalId = (crystal type-1) *512 + crystal copy  (from 1 to 3072) 
    //          for the BARREL
    // crystalId = 3072 + (crystal type-7) *64 + crystal copy  (from 3073 to 
    //          4608) for the ENDCAP
 
    if (iD<3073)
      crystalType = (Int_t)((iD-1)/512) + 1;  //crystal type (from 1 to 30)
    else
      crystalType = (Int_t)((iD-3073)/64) + 7; //crystal type (from 1 to 30)

    if (iD<3073)
      crystalCopy = ((iD-1)%512) + 1;     //CrystalCopy (from 1 to 512)
    else
      crystalCopy = ((iD-3073)%64) + 1;     //CrystalCopy (from 1 to 160)


    Char_t nameVolume[200];
    //in the crystalLog creation description in califa/R3BCalifa.cxx for v5.0, 
    //the crystalCopy begins in 0!!!
    sprintf(nameVolume, "/cave_1/CalifaWorld_0/crystalLog%i_%i",
	    crystalType,crystalCopy-1);

    gGeoManager->cd(nameVolume);
    TGeoNode* currentNode = gGeoManager->GetCurrentNode();
    currentNode->LocalToMaster(local, master);

    sprintf(nameVolume, "/cave_1/CalifaWorld_0");
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);

  } else if (fGeometryVersion==1) {
    //The present scheme here done works nicely with 7.05
    // crystalType = alveolus type (from 1 to 24)   [Basically the alveolus number]
    // crystalCopy = (alveolus copy - 1) * 4 + crystals copy (from 1 to 160)  
    //           [Not exactly azimuthal]
    // crystalId = (alveolus type-1)*160 + (alvelous copy-1)*4 + (crystal copy)  
    //           (from 1 to 3840) crystalID is asingle identifier per crystal!
    //That is:
    // crystalId = (crystalType-1)*160 + cpAlv * 4 + cpCry;
    //
    crystalType = (Int_t)((iD-1)/160) + 1;  //Alv type (from 1 to 24)
    crystalCopy = ((iD-1)%160) + 1;     //CrystalCopy (from 1 to 160)
    alveolusCopy =(Int_t)(((iD-1)%160)/4) +1; //Alveolus copy (from 1 to 40)
    crystalInAlveolus = (iD-1)%4 + 1;   //Crystal number in alveolus (from 1 to 4)

    Int_t alveoliType[24]={1,1,2,2,2,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,6,6,6};
    Char_t nameVolume[200];
    if (crystalInAlveolus<3)
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
              crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
	      crystalInAlveolus, alveoliType[crystalType-1]);
    else
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i/Crystal_%iB_1",
              crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
	      crystalInAlveolus, alveoliType[crystalType-1]);

    gGeoManager->cd(nameVolume);
    TGeoNode* currentNode = gGeoManager->GetCurrentNode();
    currentNode->LocalToMaster(local, master);
    if (crystalInAlveolus<3)
      sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
              crystalType, alveolusCopy-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);
    else
      sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i",
              crystalType, alveolusCopy-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);

    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);

    sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	    crystalType, alveolusCopy-1);
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);

    sprintf(nameVolume, "/cave_1/CalifaWorld_0");
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
  } else if (fGeometryVersion==2) {
    //The present scheme here done works nicely with 7.07
    // crystalType = alveolus type (from 1 to 20)   [Alveolus number]
    // crystalCopy = (alveolus copy - 1) * 4 + crystals copy (from 1 to 128)  
    //            [Not exactly azimuthal]
    // crystalId = (alveolus type-1)*128 + (alvelous copy-1)*4 + (crystal copy)  
    //            (from 1 to 2560)
    //That is:
    // crystalId = (crystalType-1)*128 + cpAlv * 4 + cpCry;
    //
    crystalType = (Int_t)((iD-1)/128) + 1;  //Alv type (from 1 to 20)
    crystalCopy = ((iD-1)%128) + 1; //CrystalCopy (from 1 to 128)
    alveolusCopy =(Int_t)(((iD-1)%128)/4) +1; //Alveolus copy (from 1 to 32)
    crystalInAlveolus = (iD-1)%4 + 1; //Crystal number in alveolus (from 1 to 4)

    Int_t alveoliType[20]={1,1,2,2,2,3,3,4,4,4,5,5,6,6,6,7,7,7,8,8};

    Char_t nameVolume[200];
    if (crystalInAlveolus<3)
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
              crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
	      crystalInAlveolus, alveoliType[crystalType-1]);
    else
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i/Crystal_%iB_1",
              crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
	      crystalInAlveolus, alveoliType[crystalType-1]);

    gGeoManager->cd(nameVolume);
    TGeoNode* currentNode = gGeoManager->GetCurrentNode();
    currentNode->LocalToMaster(local, master);
    if (crystalInAlveolus<3)
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
              crystalType, alveolusCopy-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);
    else
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i",
              crystalType, alveolusCopy-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);

    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);

    sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	    crystalType, alveolusCopy-1);
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);

    sprintf(nameVolume, "/cave_1/CalifaWorld_0");
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
  } else if (fGeometryVersion==3) {
    //The present scheme here done works with 7.09
    
    Char_t nameVolume[200];
    if (iD<2049) {
      Int_t alveoliType[19]={1,1,2,2,3,3,3,3,3,3,4,4,4,5,5,5,6,6,6};

      crystalType = (Int_t)((iD-1)/128) + 1;  //Alv type (from 1 to 19)
      crystalCopy = ((iD-1)%128) + 1;     //CrystalCopy (from 1 to 128)
      alveolusCopy =(Int_t)(((iD-1)%128)/4) +1; //Alveolus copy (from 1 to 32)
      crystalInAlveolus = (iD-1)%4 + 1; //Crystal number in alveolus (from 1 to 4)

      if (crystalInAlveolus<3)
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
                crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
		crystalInAlveolus, alveoliType[crystalType-1]);
      else
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i/Crystal_%iB_1",
                crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
		crystalInAlveolus, alveoliType[crystalType-1]);

      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      if (crystalInAlveolus<3)
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus);
      else
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus);

      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	      crystalType, alveolusCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

    }
    // for the last three (type 6) alveoli, which hold a single large crystal.
    else if (iD>2048 && iD<2145) {
      Int_t alveoliType[19]={1,1,2,2,3,3,3,3,3,3,4,4,4,5,5,5,6,6,6};
      crystalType = (Int_t)((iD-2049)/32) + 17; //Alv type (from 17 to 19)
      crystalCopy = ((iD-2049)%32) + 1;     //CrystalCopy (from 1 to 32)
      alveolusCopy =(Int_t)((iD-1)%32) + 1; //Alveolus copy (from 1 to 32)
      crystalInAlveolus = 1;         //Crystal number in alveolus (just one)
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
              crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
	      crystalInAlveolus, alveoliType[crystalType-1]);

      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
              crystalType, alveolusCopy-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);

      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	      crystalType, alveolusCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

    }
  } else if (fGeometryVersion==4) {
    //The present scheme here done works nicely with 7.17
    // crystalType = crystals type (from 1 to 23)
    // crystalCopy = alveolus copy (from 1 to 32)
    // crystalId = 3000 + (alvelous copy-1)*23 + (crystal copy-1)  
    // (from 3000 to 3736)

    crystalType = ((iD-3000)%23) + 1;
    crystalCopy = (iD-3000)/23 + 1;
    
    //here the alveoliType array meaning is opposite to the other BARREL cases
    //the array shows the alveoli number where each crystal of the EndCap belong
    Int_t alveoliType[23]={1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3};
    
    Char_t nameVolume[200];
    sprintf(nameVolume, 
	    "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
	    alveoliType[crystalType-1], crystalCopy-1, crystalType, crystalType);
    
    // The definition of the crystals is different in this particular EndCap design
    // the origin for each crystal is the alveoli corner
    if (crystalType==1) {
      local[0]=-1.24654194026499; local[1]=0.648218850702041; local[2]=9.75;
    } else if (crystalType==2) {
      local[0]=-4.57345805973491; local[1]=0.64821885070204; local[2]=9.75;
    } else if (crystalType==3) {
      local[0]=-1.33137201955507; local[1]=2.59875; local[2]=9.75;
    } else if (crystalType==4) {
      local[0]=-4.48862798044483; local[1]=2.59875; local[2]=9.75;
    } else if (crystalType==5) {
      local[0]=-1.40646166097653; local[1]=4.33125; local[2]=9.75;
    } else if (crystalType==6) {
      local[0]=-4.41353833902337; local[1]=4.33125; local[2]=9.75;
    } else if (crystalType==7) {
      local[0]=-1.49184443219704; local[1]=6.29334272612279; local[2]=9.75;
    } else if (crystalType==8) {
      local[0]=-4.32815556780286; local[1]=6.29334272612279; local[2]=9.75;
    } else if (crystalType==9) {
      local[0]=-1.01201040487607; local[1]=0.739366638062432; local[2]=9.75;
    } else if (crystalType==10) {
      local[0]=-3.64595915673333; local[1]=0.739366638062431; local[2]=9.75;
    } else if (crystalType==11) {
      local[0]=-1.10703474322362; local[1]=2.8015; local[2]=9.75;
    } else if (crystalType==12) {
      local[0]=-3.55093481838578; local[1]=2.8015; local[2]=9.75;
    } else if (crystalType==13) {
      local[0]=-1.19303835409747; local[1]=4.669; local[2]=9.75;
    } else if (crystalType==14) {
      local[0]=-3.46496164590243; local[1]=4.669; local[2]=9.75;
    } else if (crystalType==15) {
      local[0]=-1.28957028029965; local[1]=6.76354164083689; local[2]=9.75;
    } else if (crystalType==16) {
      local[0]=-3.36842971970024; local[1]=6.76354164083688; local[2]=9.75;
    } else if (crystalType==17) {
      local[0]=-0.70032959626342; local[1]=0.836296001993993; local[2]=9.75;
    } else if (crystalType==18) {
      local[0]=-2.53967040373648; local[1]=0.836296001993992; local[2]=9.75;
    } else if (crystalType==19) {
      local[0]=-0.797829723064974; local[1]=2.95125; local[2]=9.75;
    } else if (crystalType==20) {
      local[0]=-2.44217027693492; local[1]=2.95125; local[2]=9.75;
    } else if (crystalType==21) {
      local[0]=-0.888582051402232; local[1]=4.91875; local[2]=9.75;
    } else if (crystalType==22) {
      local[0]=-2.35141794859767; local[1]=4.91875; local[2]=9.75;
    } else if (crystalType==23) {
      local[0]=-1.61999999999997; local[1]=7.22933631874703; local[2]=9.75;
    }
    
    gGeoManager->cd(nameVolume);
    TGeoNode* currentNode = gGeoManager->GetCurrentNode();
    currentNode->LocalToMaster(local, master);
    
    sprintf(nameVolume, 
	    "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1",
	    alveoliType[crystalType-1], crystalCopy-1, crystalType);
    
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
    
    sprintf(nameVolume, 
	    "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i",
	    alveoliType[crystalType-1], crystalCopy-1);
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
    
    sprintf(nameVolume, "/cave_1/CalifaWorld_0");
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
     
  } else if (fGeometryVersion==5) {
    //The present scheme here done works nicely with 7.07+7.17
    //see the explanation for geometries 2 and 4
    Char_t nameVolume[200];
    if (iD<3000) {
      crystalType = (Int_t)((iD-1)/128) + 1;  //Alv type (from 1 to 20)
      crystalCopy = ((iD-1)%128) + 1;     //CrystalCopy (from 1 to 128)
      alveolusCopy =(Int_t)(((iD-1)%128)/4) +1; //Alveolus copy (from 1 to 32)
      crystalInAlveolus = (iD-1)%4 + 1;//Crystal number in alveolus (from 1 to 4)

      Int_t alveoliType[20]={1,1,2,2,2,3,3,4,4,4,5,5,6,6,6,7,7,7,8,8};

      if (crystalInAlveolus<3)
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
                crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
		crystalInAlveolus, alveoliType[crystalType-1]);
      else
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i/Crystal_%iB_1",
                crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
		crystalInAlveolus, alveoliType[crystalType-1]);

      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      if (crystalInAlveolus<3)
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus);
      else
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus);

      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	      crystalType, alveolusCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    } else {
      crystalType = ((iD-3000)%23) + 1;
      crystalCopy = (iD-3000)/23 + 1;
      
      //here the alveoliType array meaning is opposite to the other BARREL cases
      //the array shows the alveoli number where each crystal of the EndCap belong
      Int_t alveoliType[23]={1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3};
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
	      alveoliType[crystalType-1], crystalCopy-1, crystalType, crystalType);
      
      // The definition of the crystals is different in this particular EndCap design
      // the origin for each crystal is the alveoli corner
      if (crystalType==1) {
	local[0]=-1.24654194026499; local[1]=0.648218850702041; local[2]=9.75;
      } else if (crystalType==2) {
	local[0]=-4.57345805973491; local[1]=0.64821885070204; local[2]=9.75;
      } else if (crystalType==3) {
	local[0]=-1.33137201955507; local[1]=2.59875; local[2]=9.75;
      } else if (crystalType==4) {
	local[0]=-4.48862798044483; local[1]=2.59875; local[2]=9.75;
      } else if (crystalType==5) {
	local[0]=-1.40646166097653; local[1]=4.33125; local[2]=9.75;
      } else if (crystalType==6) {
	local[0]=-4.41353833902337; local[1]=4.33125; local[2]=9.75;
      } else if (crystalType==7) {
	local[0]=-1.49184443219704; local[1]=6.29334272612279; local[2]=9.75;
      } else if (crystalType==8) {
	local[0]=-4.32815556780286; local[1]=6.29334272612279; local[2]=9.75;
      } else if (crystalType==9) {
	local[0]=-1.01201040487607; local[1]=0.739366638062432; local[2]=9.75;
      } else if (crystalType==10) {
	local[0]=-3.64595915673333; local[1]=0.739366638062431; local[2]=9.75;
      } else if (crystalType==11) {
	local[0]=-1.10703474322362; local[1]=2.8015; local[2]=9.75;
      } else if (crystalType==12) {
	local[0]=-3.55093481838578; local[1]=2.8015; local[2]=9.75;
      } else if (crystalType==13) {
	local[0]=-1.19303835409747; local[1]=4.669; local[2]=9.75;
      } else if (crystalType==14) {
	local[0]=-3.46496164590243; local[1]=4.669; local[2]=9.75;
      } else if (crystalType==15) {
	local[0]=-1.28957028029965; local[1]=6.76354164083689; local[2]=9.75;
      } else if (crystalType==16) {
	local[0]=-3.36842971970024; local[1]=6.76354164083688; local[2]=9.75;
      } else if (crystalType==17) {
	local[0]=-0.70032959626342; local[1]=0.836296001993993; local[2]=9.75;
      } else if (crystalType==18) {
	local[0]=-2.53967040373648; local[1]=0.836296001993992; local[2]=9.75;
      } else if (crystalType==19) {
	local[0]=-0.797829723064974; local[1]=2.95125; local[2]=9.75;
      } else if (crystalType==20) {
	local[0]=-2.44217027693492; local[1]=2.95125; local[2]=9.75;
      } else if (crystalType==21) {
	local[0]=-0.888582051402232; local[1]=4.91875; local[2]=9.75;
      } else if (crystalType==22) {
	local[0]=-2.35141794859767; local[1]=4.91875; local[2]=9.75;
      } else if (crystalType==23) {
	local[0]=-1.61999999999997; local[1]=7.22933631874703; local[2]=9.75;
      }
      
      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1",
	      alveoliType[crystalType-1], crystalCopy-1, crystalType);
      
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i",
	      alveoliType[crystalType-1], crystalCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
    } 
  } else if (fGeometryVersion==6) {
    //The present scheme here done works with 7.09+7.17
    //see the explanation for geometries 3 and 4
    
    Char_t nameVolume[200];
    if (iD<2049) {
      Int_t alveoliType[19]={1,1,2,2,3,3,3,3,3,3,4,4,4,5,5,5,6,6,6};
      crystalType = (Int_t)((iD-1)/128) + 1;  //Alv type (from 1 to 20)
      crystalCopy = ((iD-1)%128) + 1;     //CrystalCopy (from 1 to 128)
      alveolusCopy =(Int_t)(((iD-1)%128)/4) +1; //Alveolus copy (from 1 to 32)
      crystalInAlveolus = (iD-1)%4 + 1;//Crystal number in alveolus (from 1 to 4)

      if (crystalInAlveolus<3)
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
                crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
		crystalInAlveolus, alveoliType[crystalType-1]);
      else
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i/Crystal_%iB_1",
                crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
		crystalInAlveolus, alveoliType[crystalType-1]);

      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      if (crystalInAlveolus<3)
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus);
      else
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus);

      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	      crystalType, alveolusCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    }
    // For alveoli type 6, which has only 1 crystal in each of the 
    // last three alveoli that are type 6.
    else if (iD>2048&&iD<3000) {
      Int_t alveoliType[19]={1,1,2,2,3,3,3,3,3,3,4,4,4,5,5,5,6,6,6};
      crystalType = (Int_t)((iD-2049)/32) + 17; //Alv type (from 1 to 20)
      crystalCopy = ((iD-2049)%32) + 1;     //CrystalCopy (from 1 to 32)
      alveolusCopy =(Int_t)((iD-1)%32) + 1; //Alveolus copy (from 1 to 32)
      crystalInAlveolus = 1;    //Crystal number in alveolus (from 1 to 4)

      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
              crystalType, alveolusCopy-1, alveoliType[crystalType-1], 
	      crystalInAlveolus, alveoliType[crystalType-1]);

      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
              crystalType, alveolusCopy-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);

      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	      crystalType, alveolusCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    } else {
      crystalType = ((iD-3000)%23) + 1;
      crystalCopy = (iD-3000)/23 + 1;
      
      //here the alveoliType array meaning is opposite to the other BARREL cases
      //the array shows the alveoli number where each crystal of the EndCap belong
      Int_t alveoliType[23]={1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3};
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
	      alveoliType[crystalType-1], crystalCopy-1, crystalType, crystalType);
      
      // The definition of the crystals is different in this particular EndCap design
      // the origin for each crystal is the alveoli corner
      if (crystalType==1) {
	local[0]=-1.24654194026499; local[1]=0.648218850702041; local[2]=9.75;
      } else if (crystalType==2) {
	local[0]=-4.57345805973491; local[1]=0.64821885070204; local[2]=9.75;
      } else if (crystalType==3) {
	local[0]=-1.33137201955507; local[1]=2.59875; local[2]=9.75;
      } else if (crystalType==4) {
	local[0]=-4.48862798044483; local[1]=2.59875; local[2]=9.75;
      } else if (crystalType==5) {
	local[0]=-1.40646166097653; local[1]=4.33125; local[2]=9.75;
      } else if (crystalType==6) {
	local[0]=-4.41353833902337; local[1]=4.33125; local[2]=9.75;
      } else if (crystalType==7) {
	local[0]=-1.49184443219704; local[1]=6.29334272612279; local[2]=9.75;
      } else if (crystalType==8) {
	local[0]=-4.32815556780286; local[1]=6.29334272612279; local[2]=9.75;
      } else if (crystalType==9) {
	local[0]=-1.01201040487607; local[1]=0.739366638062432; local[2]=9.75;
      } else if (crystalType==10) {
	local[0]=-3.64595915673333; local[1]=0.739366638062431; local[2]=9.75;
      } else if (crystalType==11) {
	local[0]=-1.10703474322362; local[1]=2.8015; local[2]=9.75;
      } else if (crystalType==12) {
	local[0]=-3.55093481838578; local[1]=2.8015; local[2]=9.75;
      } else if (crystalType==13) {
	local[0]=-1.19303835409747; local[1]=4.669; local[2]=9.75;
      } else if (crystalType==14) {
	local[0]=-3.46496164590243; local[1]=4.669; local[2]=9.75;
      } else if (crystalType==15) {
	local[0]=-1.28957028029965; local[1]=6.76354164083689; local[2]=9.75;
      } else if (crystalType==16) {
	local[0]=-3.36842971970024; local[1]=6.76354164083688; local[2]=9.75;
      } else if (crystalType==17) {
	local[0]=-0.70032959626342; local[1]=0.836296001993993; local[2]=9.75;
      } else if (crystalType==18) {
	local[0]=-2.53967040373648; local[1]=0.836296001993992; local[2]=9.75;
      } else if (crystalType==19) {
	local[0]=-0.797829723064974; local[1]=2.95125; local[2]=9.75;
      } else if (crystalType==20) {
	local[0]=-2.44217027693492; local[1]=2.95125; local[2]=9.75;
      } else if (crystalType==21) {
	local[0]=-0.888582051402232; local[1]=4.91875; local[2]=9.75;
      } else if (crystalType==22) {
	local[0]=-2.35141794859767; local[1]=4.91875; local[2]=9.75;
      } else if (crystalType==23) {
	local[0]=-1.61999999999997; local[1]=7.22933631874703; local[2]=9.75;
      }
      
      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1",
	      alveoliType[crystalType-1], crystalCopy-1, crystalType);
      
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i",
	      alveoliType[crystalType-1], crystalCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
    } 
  }  else if (fGeometryVersion==7) {
    //RESERVED FOR CALIFA 717PHOSWICH, only phoswich ENDCAP 
    Char_t nameVolume[200];
    crystalType = ((iD-3000)%30) + 1;
    crystalCopy = (iD-3000)/30 + 1;
    Int_t alveoliType[30]={1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,
			   10,10,11,11,12,12,13,13,14,14,15,15};
    sprintf(nameVolume, 
	    "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
	    alveoliType[crystalType-1], crystalCopy-1, 
	    crystalType, crystalType);	
    gGeoManager->cd(nameVolume);
    TGeoNode* currentNode = gGeoManager->GetCurrentNode();
    currentNode->LocalToMaster(local, master);
    
    sprintf(nameVolume, 
	    "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1",
	    alveoliType[crystalType-1], crystalCopy-1, crystalType);	
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
    
    sprintf(nameVolume, 
	    "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i",
	    alveoliType[crystalType-1], crystalCopy-1);
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
    
    sprintf(nameVolume, "/cave_1/CalifaWorld_0");
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
  } else if (fGeometryVersion==8) {
    //RESERVED FOR CALIFA 7.07 BARREL + 717PHOSWICH 
    
    Char_t nameVolume[200];
    if (iD<3000) {
      crystalType = (Int_t)((iD-1)/128) + 1;  //Alv type (from 1 to 20)
      crystalCopy = ((iD-1)%128) + 1;     //CrystalCopy (from 1 to 128)
      alveolusCopy =(Int_t)(((iD-1)%128)/4) +1; //Alveolus copy (from 1 to 32)
      crystalInAlveolus = (iD-1)%4 + 1;//Crystal number in alveolus (from 1 to 4)
      
      Int_t alveoliType[20]={1,1,2,2,2,3,3,4,4,4,5,5,6,6,6,7,7,7,8,8};
      
      if (crystalInAlveolus<3)
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus, 
		alveoliType[crystalType-1]);
      else
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i/Crystal_%iB_1",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus, 
		alveoliType[crystalType-1]);
      
      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      if (crystalInAlveolus<3)
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus);
      else
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus);
      
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	      crystalType, alveolusCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    } else {
      crystalType = ((iD-3000)%30) + 1;
      crystalCopy = (iD-3000)/30 + 1; 
      Int_t alveoliType[30]={1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,
			     10,10,11,11,12,12,13,13,14,14,15,15};
    
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
              alveoliType[crystalType-1], crystalCopy-1, 
	      crystalType, crystalType);
      
      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1",
              alveoliType[crystalType-1], crystalCopy-1, crystalType);
      
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i",
	      alveoliType[crystalType-1], crystalCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    }
  } else if (fGeometryVersion==9) {
    //RESERVED FOR CALIFA 7.09 BARREL + 717PHOSWICH
    
    Char_t nameVolume[200];
    if (iD<2049) {
      Int_t alveoliType[19]={1,1,2,2,3,3,3,3,3,3,4,4,4,5,5,5,6,6,6};
      crystalType = (Int_t)((iD-1)/128) + 1;  //Alv type (from 1 to 20)
      crystalCopy = ((iD-1)%128) + 1;     //CrystalCopy (from 1 to 128)
      alveolusCopy =(Int_t)(((iD-1)%128)/4) +1; //Alveolus copy (from 1 to 32)
      crystalInAlveolus = (iD-1)%4 + 1;//Crystal number in alveolus (from 1 to 4)
      
      if (crystalInAlveolus<3)
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus, 
		alveoliType[crystalType-1]);
      else
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i/Crystal_%iB_1",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus, 
		alveoliType[crystalType-1]);
      
      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      if (crystalInAlveolus<3)
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus);
      else
        sprintf(nameVolume, 
		"/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i",
                crystalType, alveolusCopy-1, 
		alveoliType[crystalType-1], crystalInAlveolus);
      
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	      crystalType, alveolusCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    }
    // For alveoli type 6, which has only 1 crystal in each of the 
    // last three alveoli that are type 6.
    else if (iD>2048&&iD<3000) {
      Int_t alveoliType[19]={1,1,2,2,3,3,3,3,3,3,4,4,4,5,5,5,6,6,6};
      crystalType = (Int_t)((iD-2049)/32) + 17; //Alv type (from 1 to 20)
      crystalCopy = ((iD-2049)%32) + 1;     //CrystalCopy (from 1 to 32)
      alveolusCopy =(Int_t)((iD-1)%32) + 1; //Alveolus copy (from 1 to 32)
      crystalInAlveolus = 1;    //Crystal number in alveolus (from 1 to 4)
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
              crystalType, alveolusCopy-1, 
	      alveoliType[crystalType-1], crystalInAlveolus, 
	      alveoliType[crystalType-1]);
      
      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
              crystalType, alveolusCopy-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);
      
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	      crystalType, alveolusCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    }
    else {
      crystalType = ((iD-3000)%30) + 1;
      crystalCopy = (iD-3000)/30 + 1;
      
      Int_t alveoliType[30]={1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,
			     10,10,11,11,12,12,13,13,14,14,15,15};

      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
              alveoliType[crystalType-1], crystalCopy-1, 
	      crystalType, crystalType);
      
      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1",
              alveoliType[crystalType-1], crystalCopy-1, crystalType);
      
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i",
	      alveoliType[crystalType-1], crystalCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    }
  } else if (fGeometryVersion==10) {
    //The present scheme here done works with 8.11
    // crystalType = alveolus type (from 1 to 17) [Alveolus number]
    // crystalCopy = alveolus copy * 4 + crystals copy +1 (from 1 to 128)
    // crystalId = 1 to 32 for the first 32 crystals 
    //                     (single crystal in each alveoli)
    // or 32 + (alveolus type-2)*128 + (alvelous copy)*4 + (crystal copy) + 1
    //                     (from 1 to 1952)
    //
    if(iD<33) crystalType = 1;  //Alv type 1
    else crystalType = (Int_t)((iD-33)/128) + 2;  //Alv type (2 to 16)
    if(iD<33) crystalCopy = iD;     //for Alv type 1 
    else crystalCopy = ((iD-33)%128) + 1;         //CrystalCopy (1 to 128)
    if(iD<33) alveolusCopy = iD;    //Alv type 1 
    else alveolusCopy =(Int_t)(((iD-33)%128)/4) +1; //Alveolus copy (1 to 32)
    if(iD<33) crystalInAlveolus =1;          //Alv type 1
    else crystalInAlveolus = (iD-33)%4 + 1;//Crystal number in alveolus (1 to 4)
    
    Int_t alveoliType[16]={1,2,2,2,2,3,3,4,4,4,5,5,5,6,6,6};
    
    Char_t nameVolume[200];
    sprintf(nameVolume, 
	    "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1/CrystalWithWrapping_%i_%i_%i/Crystal_%i_%i_1",
	    crystalType, alveolusCopy-1, 
	    crystalType, alveoliType[crystalType-1], 
	    crystalInAlveolus, crystalInAlveolus-1, 
	    alveoliType[crystalType-1], crystalInAlveolus);
    
    // The definition of the crystals is different in this particular EndCap design:
    // the origin for each crystal is the alveoli corner
    if (crystalType==1) {
      local[0]=27.108/8; local[1]=-28.0483/8; local[2]=0;
    } else if (crystalType==2 || crystalType==3 || 
	       crystalType==4 || crystalType==5) {
      if(crystalInAlveolus==1){
	local[0]=37.4639/8; local[1]=-8.57573/8; local[2]=0;
      } else if(crystalInAlveolus==2) {
	local[0]=37.4639/8; local[1]=-31.1043/8; local[2]=0;
      } else if(crystalInAlveolus==3) {
	local[0]=9.52012/8; local[1]=-8.57573/8; local[2]=0;
      } else if(crystalInAlveolus==4){
	local[0]=9.52012/8; local[1]=-31.1043/8; local[2]=0;
      }
    } else if (crystalType==6 || crystalType==7) {
      if(crystalInAlveolus==1){
	local[0]=38.3282/8; local[1]=-5.49819/8; local[2]=0;
      } else if(crystalInAlveolus==2) {
	local[0]=38.3282/8; local[1]=-23.0538/8; local[2]=0;
      } else if(crystalInAlveolus==3) {
	local[0]=8.66384/8; local[1]=-5.49819/8; local[2]=0;
      } else if(crystalInAlveolus==4){
	local[0]=8.66384/8; local[1]=-23.0538/8; local[2]=0;
      }
    } else if (crystalType==8 || crystalType==9 || crystalType==10) {
      if(crystalInAlveolus==1){
	local[0]=38.3683/8; local[1]=-4.71618/8; local[2]=0;
      } else if(crystalInAlveolus==2) {
	local[0]=38.3683/8; local[1]=-19.8438/8; local[2]=0;
      } else if(crystalInAlveolus==3) {
	local[0]=8.43569/8; local[1]=-4.71618/8; local[2]=0;
      } else if(crystalInAlveolus==4){
	local[0]=8.43569/8; local[1]=-19.8438/8; local[2]=0;
      }
    } else if (crystalType==11 || crystalType==12 || crystalType==13) {
      if(crystalInAlveolus==1){
	local[0]=38.3495/8; local[1]=-4.70373/8; local[2]=0;
      } else if(crystalInAlveolus==2) {
	local[0]=38.3495/8; local[1]=-19.8403/8; local[2]=0;
      } else if(crystalInAlveolus==3) {
	local[0]=8.66654/8; local[1]=-4.70373/8; local[2]=0;
      } else if(crystalInAlveolus==4){
	local[0]=8.66654/8; local[1]=-19.8403/8; local[2]=0;
      }
    } else if (crystalType==14 || crystalType==15 || crystalType==16) {
      if(crystalInAlveolus==1){
	local[0]=37.9075/8; local[1]=-4.66458/8; local[2]=0;
      } else if(crystalInAlveolus==2) {
	local[0]=37.9075/8; local[1]=-19.8474/8; local[2]=0;
      } else if(crystalInAlveolus==3) {
	local[0]=9.07247/8; local[1]=-19.8474/8; local[2]=0;
      } else if(crystalInAlveolus==4){
	local[0]=9.07247/8; local[1]=-4.66458/8; local[2]=0;
      }
    }		

    gGeoManager->CdTop();

    if(gGeoManager->CheckPath(nameVolume)) gGeoManager->cd(nameVolume);
    else { 
      LOG(ERROR) << "R3BCalifaCrystalCal2Hit: Invalid crystal path: " << nameVolume
		 << FairLogger::endl;
      return; 
    }
    TGeoNode* currentNode = gGeoManager->GetCurrentNode();
    currentNode->LocalToMaster(local, master);
    
    sprintf(nameVolume, 
	    "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1/CrystalWithWrapping_%i_%i_%i",
	    crystalType, alveolusCopy-1, 
	    crystalType, alveoliType[crystalType-1], 
	    crystalInAlveolus, crystalInAlveolus-1);
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
    
    sprintf(nameVolume, 
	    "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1",
	    crystalType, alveolusCopy-1, crystalType);
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
    
    sprintf(nameVolume, 
	    "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	    crystalType, alveolusCopy-1);
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
    
    sprintf(nameVolume, "/cave_1/CalifaWorld_0");
    gGeoManager->cd(nameVolume);
    currentNode = gGeoManager->GetCurrentNode();
    local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
    currentNode->LocalToMaster(local, master);
    
  } else if (fGeometryVersion==11) {
    Char_t nameVolume[200];
    if (iD<3000) { 
      if(iD<33) crystalType = 1;  //Alv type 1
      else crystalType = (Int_t)((iD-33)/128) + 2;  //Alv type (2 to 16)
      if(iD<33) crystalCopy = iD;     //for Alv type 1 
      else crystalCopy = ((iD-33)%128) + 1;         //CrystalCopy (1 to 128)
      if(iD<33) alveolusCopy = iD;    //Alv type 1 
      else alveolusCopy =(Int_t)(((iD-33)%128)/4) +1; //Alveolus copy (1 to 32)
      if(iD<33) crystalInAlveolus =1;          //Alv type 1
      else crystalInAlveolus = (iD-33)%4 + 1;//Crystal number in alveolus (1 to 4)
      
      Int_t alveoliType[16]={1,2,2,2,2,3,3,4,4,4,5,5,5,6,6,6};
    
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1/CrystalWithWrapping_%i_%i_%i/Crystal_%i_%i_1",
	      crystalType, alveolusCopy-1, crystalType, 
	      alveoliType[crystalType-1], crystalInAlveolus, crystalInAlveolus-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);
      
      // The definition of the crystals is different in this particular EndCap design:
      // the origin for each crystal is the alveoli corner
      if (crystalType==1) {
	local[0]=27.108/8; local[1]=-28.0483/8; local[2]=0;
      } else if (crystalType==2 || crystalType==3 || 
		 crystalType==4 || crystalType==5) {
	if(crystalInAlveolus==1){
	  local[0]=37.4639/8; local[1]=-8.57573/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=37.4639/8; local[1]=-31.1043/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=9.52012/8; local[1]=-8.57573/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=9.52012/8; local[1]=-31.1043/8; local[2]=0;
	}
      } else if (crystalType==6 || crystalType==7) {
	if(crystalInAlveolus==1){
	  local[0]=38.3282/8; local[1]=-5.49819/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=38.3282/8; local[1]=-23.0538/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=8.66384/8; local[1]=-5.49819/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=8.66384/8; local[1]=-23.0538/8; local[2]=0;
	}
      } else if (crystalType==8 || crystalType==9 || crystalType==10) {
	if(crystalInAlveolus==1){
	  local[0]=38.3683/8; local[1]=-4.71618/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=38.3683/8; local[1]=-19.8438/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=8.43569/8; local[1]=-4.71618/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=8.43569/8; local[1]=-19.8438/8; local[2]=0;
	}
      } else if (crystalType==11 || crystalType==12 || crystalType==13) {
	if(crystalInAlveolus==1){
	  local[0]=38.3495/8; local[1]=-4.70373/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=38.3495/8; local[1]=-19.8403/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=8.66654/8; local[1]=-4.70373/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=8.66654/8; local[1]=-19.8403/8; local[2]=0;
	}
      } else if (crystalType==14 || crystalType==15 || crystalType==16) {
	if(crystalInAlveolus==1){
	  local[0]=37.9075/8; local[1]=-4.66458/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=37.9075/8; local[1]=-19.8474/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=9.07247/8; local[1]=-19.8474/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=9.07247/8; local[1]=-4.66458/8; local[2]=0;
	}
      }		
      
      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1/CrystalWithWrapping_%i_%i_%i",
	      crystalType, alveolusCopy-1, 
	      crystalType, alveoliType[crystalType-1], 
	      crystalInAlveolus, crystalInAlveolus-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1",
	      crystalType, alveolusCopy-1, crystalType);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	      crystalType, alveolusCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);  
    }
    else{//For IEM LaBr - LaCl phoswich endcap 
      crystalType = ((iD-3000)%30) + 1;
      crystalCopy = (iD-3000)/30 + 1;
      Int_t alveoliType[30]={1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,
			     10,10,11,11,12,12,13,13,14,14,15,15};

      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
              alveoliType[crystalType-1], crystalCopy-1, 
	      crystalType, crystalType);
      
      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1",
              alveoliType[crystalType-1], crystalCopy-1, crystalType);
      
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i",
	      alveoliType[crystalType-1], crystalCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
      
      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    } 
    
  } else if (fGeometryVersion==15) {
    //The present scheme here done works with 8.11
    // crystalType = alveolus type (from 1 to 17) [Alveolus number]
    // crystalCopy = alveolus copy * 4 + crystals copy +1 (from 1 to 128)
    // crystalId = 1 to 32 for the first 32 crystals 
    //                     (single crystal in each alveoli)
    // or 32 + (alveolus type-2)*128 + (alvelous copy)*4 + (crystal copy) + 1
    //                     (from 1 to 1952)
    //
    Char_t nameVolume[200];
    if (iD<3000) {
      if(iD<33) crystalType = 1;  //Alv type 1
      else crystalType = (Int_t)((iD-33)/128) + 2;  //Alv type (2 to 16)
      if(iD<33) crystalCopy = iD;     //for Alv type 1 
      else crystalCopy = ((iD-33)%128) + 1;         //CrystalCopy (1 to 128)
      if(iD<33) alveolusCopy = iD;    //Alv type 1 
      else alveolusCopy =(Int_t)(((iD-33)%128)/4) +1; //Alveolus copy (1 to 32)
      if(iD<33) crystalInAlveolus =1;          //Alv type 1
      else crystalInAlveolus = (iD-33)%4 + 1;//Crystal number in alveolus (1 to 4)
    
      Int_t alveoliType[16]={1,2,2,2,2,3,3,4,4,4,5,5,5,6,6,6};
    
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1/CrystalWithWrapping_%i_%i_%i/Crystal_%i_%i_1",
	      crystalType, alveolusCopy-1, 
	      crystalType, alveoliType[crystalType-1], 
	      crystalInAlveolus, crystalInAlveolus-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);
    
      // The definition of the crystals is different in this particular EndCap design:
      // the origin for each crystal is the alveoli corner
      if (crystalType==1) {
	local[0]=27.108/8; local[1]=-28.0483/8; local[2]=0;
      } else if (crystalType==2 || crystalType==3 || 
		 crystalType==4 || crystalType==5) {
	if(crystalInAlveolus==1){
	  local[0]=37.4639/8; local[1]=-8.57573/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=37.4639/8; local[1]=-31.1043/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=9.52012/8; local[1]=-8.57573/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=9.52012/8; local[1]=-31.1043/8; local[2]=0;
	}
      } else if (crystalType==6 || crystalType==7) {
	if(crystalInAlveolus==1){
	  local[0]=38.3282/8; local[1]=-5.49819/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=38.3282/8; local[1]=-23.0538/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=8.66384/8; local[1]=-5.49819/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=8.66384/8; local[1]=-23.0538/8; local[2]=0;
	}
      } else if (crystalType==8 || crystalType==9 || crystalType==10) {
	if(crystalInAlveolus==1){
	  local[0]=38.3683/8; local[1]=-4.71618/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=38.3683/8; local[1]=-19.8438/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=8.43569/8; local[1]=-4.71618/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=8.43569/8; local[1]=-19.8438/8; local[2]=0;
	}
      } else if (crystalType==11 || crystalType==12 || crystalType==13) {
	if(crystalInAlveolus==1){
	  local[0]=38.3495/8; local[1]=-4.70373/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=38.3495/8; local[1]=-19.8403/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=8.66654/8; local[1]=-4.70373/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=8.66654/8; local[1]=-19.8403/8; local[2]=0;
	}
      } else if (crystalType==14 || crystalType==15 || crystalType==16) {
	if(crystalInAlveolus==1){
	  local[0]=37.9075/8; local[1]=-4.66458/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=37.9075/8; local[1]=-19.8474/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=9.07247/8; local[1]=-19.8474/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=9.07247/8; local[1]=-4.66458/8; local[2]=0;
	}
      }		

      gGeoManager->CdTop();

      if(gGeoManager->CheckPath(nameVolume)) gGeoManager->cd(nameVolume);
      else { 
	LOG(ERROR) << "R3BCalifaCrystalCal2Hit: Invalid crystal path: " << nameVolume
		   << FairLogger::endl;
	return; 
      }
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);
    
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1/CrystalWithWrapping_%i_%i_%i",
	      crystalType, alveolusCopy-1, 
	      crystalType, alveoliType[crystalType-1], 
	      crystalInAlveolus, crystalInAlveolus-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1",
	      crystalType, alveolusCopy-1, crystalType);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i",
	      crystalType, alveolusCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    
      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);
    } else{
      //For TUM iPhos endcap 
      crystalType = ((iD-3000)%15) + 1;
      crystalCopy = (iD-3000 - crystalType + 1)/15 + 1;
      Int_t alveoliType[15]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
      //Char_t nameVolume[200];
      sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
              alveoliType[crystalType-1], crystalCopy-1, crystalType, crystalType);

      gGeoManager->cd(nameVolume);
      TGeoNode* currentNode = gGeoManager->GetCurrentNode();
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1",
              alveoliType[crystalType-1], crystalCopy-1, crystalType);

      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i",alveoliType[crystalType-1], crystalCopy-1);
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);

      sprintf(nameVolume, "/cave_1/CalifaWorld_0");
      gGeoManager->cd(nameVolume);
      currentNode = gGeoManager->GetCurrentNode();
      local[0]=master[0]; local[1]=master[1]; local[2]=master[2];
      currentNode->LocalToMaster(local, master);   
    }
  } else if (fGeometryVersion>=16) {
    // Use new R3BCalifaGeometry class to get geometrical information
    R3BCalifaGeometry::Instance(fGeometryVersion)->GetAngles(iD, polar, azimuthal, rho);
    return;
  } else LOG(ERROR) << "R3BCalifaCrystalCal2Hit: Geometry version not available in R3BCalifa::ProcessHits(). " << FairLogger::endl;
  
  
  TVector3 masterV(master[0],master[1],master[2]);
  //masterV.Print();
  *polar=masterV.Theta();
  *azimuthal=masterV.Phi();
  *rho=masterV.Mag();
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
