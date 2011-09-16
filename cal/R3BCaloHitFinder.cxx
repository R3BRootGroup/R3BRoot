// -------------------------------------------------------------------------
// -----                   R3BCaloHitFinder source file                -----
// -----                  Created 27/08/10  by H.Alvarez               -----
// -----		Last modification 01/09/10 by H.Alvarez        -----
// -----                                  15/09/11 by Enrico Fiori     ----- 
// -------------------------------------------------------------------------
#include "R3BCaloHitFinder.h"
#include "TMath.h"
#include "TVector3.h"
#include "TGeoMatrix.h"

#include "TClonesArray.h"
#include "TRandom.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TGeoManager.h"

#include "R3BCaloCrystalHit.h"

using std::cout;
using std::endl;


R3BCaloHitFinder::R3BCaloHitFinder() : FairTask("R3B CALIFA Hit Finder ") { 
	fGeometryVersion=1; //default version 7.05
	fThreshold=0.;	   //no threshold
	fCrystalResolution=0.; //perfect crystals
	fDeltaPolar=0.25;
	fDeltaAzimuthal=0.25;
}


R3BCaloHitFinder::~R3BCaloHitFinder() {
}



void R3BCaloHitFinder::SetParContainers() {
	//
	//NO CONTAINER FOR HIT FINDER DEFINED YET.... FOLLOW THE NEXT PROCEDURE!!!!!!
	//
	/*
	// Get run and runtime database
	FairRunAna* run = FairRunAna::Instance();
	if ( ! run ) Fatal("R3BCaloHitFinder::SetParContainers", "No analysis run");
	
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	if ( ! rtdb ) Fatal("R3BCaloHitFinder::SetParContainers", "No runtime database");
	
	fCaloHitFinderPar = (R3BCaloHitFinderPar*)(rtdb->getContainer("R3BCaloHitFinderPar"));
	
	if ( fCaloHitFinderPar ) {
		cout << "-I- R3BCaloHitFinder::SetParContainers() "<< endl;
		cout << "-I- Container R3BCaloHitFinderPar  loaded " << endl;
	}
	 */
	
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BCaloHitFinder::Init() {
	
	FairRootManager* ioManager = FairRootManager::Instance();
	if ( !ioManager ) Fatal("Init", "No FairRootManager");
	fCrystalHitCA = (TClonesArray*) ioManager->GetObject("CrystalHit");
	
	
	// Register output array CaloHit
	fCaloHitCA = new TClonesArray("R3BCaloHit",1000);
	ioManager->Register("CaloHit", "CALIFA Hit", fCaloHitCA, kTRUE);
	
	// Parameter retrieval from par container
	// ...
	
	
	// Table for crystal parameters from Geometry
	
	
	// Initialization of variables, histograms, ...
	
	
	return kSUCCESS;
	
}



// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCaloHitFinder::ReInit() {
	
	
	return kSUCCESS;
	
}


// -----   Public method Exec   --------------------------------------------
void R3BCaloHitFinder::Exec(Option_t* opt) {
	
	// Reset entries in output arrays, local arrays 
	Reset();
	
	
	//ALGORITHMS FOR HIT FINDING
	
	Double_t energy=0.;				// caloHits energy
	Double_t polarAngle=0.;			// caloHits reconstructed polar angle
	Double_t azimuthalAngle=0.;		// caloHits reconstructed azimuthal angle
	Double_t rhoAngle=0.;		// caloHits reconstructed rho
	Double_t angle1,angle2;
	
	Int_t* usedCrystalHits=0; //array to control the CrystalHits
	Int_t crystalsInHit=0;  //used crystals in each CaloHit
	Double_t testPolar=0 ;
	Double_t testAzimuthal=0 ;
	Double_t testRho =0 ;
	
	R3BCaloCrystalHit** crystalHit;

	Int_t crystalHits;				// Nb of CrystalHits in current event
	crystalHits = fCrystalHitCA->GetEntries(); 

	if(crystalHits>0) {
		//cout << "-I- R3BCaloHitFinder: Processing an event with " << crystalHits << " crystalHits." << endl;
		crystalHit = new R3BCaloCrystalHit*[crystalHits];
		usedCrystalHits = new Int_t[crystalHits];
		for(Int_t i=0;i<crystalHits;i++){
			crystalHit[i] = new R3BCaloCrystalHit;
			crystalHit[i] = (R3BCaloCrystalHit*) fCrystalHitCA->At(i);   
			usedCrystalHits[i] = 0;
		}
	}
	
	//For the moment a simple analysis... more to come
	Int_t crystalWithHigherEnergy = 0;
	Int_t unusedCrystals = crystalHits;
	
	//removing those crystals with energy below the threshold
	for(Int_t i=0;i<crystalHits;i++){
		if(crystalHit[i]->GetEnergy()<fThreshold){
			usedCrystalHits[i] = 1;
			unusedCrystals--;
		}
	}
	
	while (unusedCrystals>0) {
		//************ First, finding the crystal with higher energy from the unused crystalHits
		for(Int_t i=1;i<crystalHits;i++) {
			if (!usedCrystalHits[i] && crystalHit[i]->GetEnergy() > crystalHit[crystalWithHigherEnergy]->GetEnergy())     
				crystalWithHigherEnergy = i;
		}
		
		usedCrystalHits[crystalWithHigherEnergy] = 1; unusedCrystals--; crystalsInHit++;
		
		//************ Second, energy and angles come from the crystal with the higher energy
		energy = ExpResSmearing(crystalHit[crystalWithHigherEnergy]->GetEnergy());
		GetAngles(crystalHit[crystalWithHigherEnergy]->GetCrystalId(),&polarAngle,&azimuthalAngle, &rhoAngle);

				
		//************ Third, finding closest hits and adding their energy
		// Clusterization: you want to put a condition on the angle between the highest 
		// energy crystal and the others. This is done by using the TVector3 classes and 
		// not with different DeltaAngle on theta and phi, to get a proper solid angle 
		// and not a "square" one.                    Enrico Fiori 
		TVector3 refAngle(1);     // EF
		refAngle.SetTheta(polarAngle);
		refAngle.SetPhi(azimuthalAngle);
		for(Int_t i=0;i<crystalHits;i++){
		  if (!usedCrystalHits[i] ) {
		    GetAngles(crystalHit[i]->GetCrystalId(), &testPolar, &testAzimuthal, &testRho);
		    
		    TVector3 testAngle(1);       //EF
		    testAngle.SetTheta(testPolar);
		    testAngle.SetPhi(testAzimuthal);

		    // Check if the angle between the two vectors is less than the reference angle.
		    switch (fClusteringAlgorithmSelector) {
		    case 1: {  //square window
		      //Dealing with the particular definition of azimuthal angles (discontinuity in pi and -pi)
		      if(azimuthalAngle + fDeltaAzimuthal > TMath::Pi()) {
			angle1 = azimuthalAngle-TMath::Pi(); angle2 = testAzimuthal-TMath::Pi();
		      }
		      else if(azimuthalAngle - fDeltaAzimuthal < -TMath::Pi()){
			angle1 = azimuthalAngle+TMath::Pi(); angle2 = testAzimuthal+TMath::Pi();
		      }
		      else {angle1 = azimuthalAngle; angle2 = testAzimuthal;}
		      if(TMath::Abs(polarAngle - testPolar) < fDeltaPolar && 
			 TMath::Abs(angle1 - angle2) < fDeltaAzimuthal ) {
			energy += ExpResSmearing(crystalHit[i]->GetEnergy()); 
			usedCrystalHits[i] = 1; unusedCrystals--; crystalsInHit++;
		      }
		      break; }
		    case 2:  //round window
		      // The angle is scaled to a reference distance (e.g. here is set to 35 cm) 
		      // to take into account Califa's non-spherical geometry. 
		      // The reference angle will then have to be defined in relation to this reference distance:
		      // for example, 10° at 35 cm corresponds to ~6cm, setting a fDeltaAngleClust=10 means that 
		      // the gamma rays will be allowed to travel 6 cm in the CsI, no matter the position of the crystal they hit.
		      if( ((refAngle.Angle(testAngle))*((testRho+rhoAngle)/(35.*2.))) < fDeltaAngleClust )  {
			energy += ExpResSmearing(crystalHit[i]->GetEnergy()); 
			usedCrystalHits[i] = 1; unusedCrystals--; crystalsInHit++;
		      }
		      break ;
		    case 3: {  //round window scaled with energy
		      // The same as before but the angular window is scaled according to the 
		      // energy of the hit in the higher energy crystal. It needs a parameter that should be calibrated.
		      Double_t fDeltaAngleClustScaled = fDeltaAngleClust * (crystalHit[crystalWithHigherEnergy]->GetEnergy()*fParCluster1);
		      if( ((refAngle.Angle(testAngle))*((testRho+rhoAngle)/(35.*2.))) < fDeltaAngleClustScaled )  {
			energy += ExpResSmearing(crystalHit[i]->GetEnergy()); 
			usedCrystalHits[i] = 1; unusedCrystals--; crystalsInHit++;
		      }
		      break; }
		    case 4: // round window scaled with the energy of the _two_ hits (to be tested and implemented!!)
		    // More advanced: the condition on the distance between the two hits is function of the energy of both hits
		      break;		    
		    }
		  }
		}

		AddHit(crystalsInHit, energy, polarAngle, azimuthalAngle);
		crystalsInHit = 0; //reset for next CaloHit
		
		//Finally, setting crystalWithHigherEnergy to the first unused crystalHit (for the next iteration)
		for(Int_t i=0;i<crystalHits;i++){
			if(!usedCrystalHits[i]){
				crystalWithHigherEnergy = i;
				break;
			}
		}
	}
}


// ---- Public method Reset   --------------------------------------------------
void R3BCaloHitFinder::Reset(){
	// Clear the CA structure
	// cout << " -I- R3BCaloHitFinder:Reset() called " << endl;
	
	if (fCaloHitCA) fCaloHitCA->Clear();
}   




// ---- Public method Finish   --------------------------------------------------
void R3BCaloHitFinder::Finish()
{
	// here event. write histos
	//   cout << " -I- Digit Finish() called " << endl;
	// Write control histograms
		
}


// -----  Public method SelectGeometryVersion  ----------------------------------
void R3BCaloHitFinder::SelectGeometryVersion(Int_t version)
{	
	fGeometryVersion=version;
}


// -----  Public method SetExperimentalResolution  ----------------------------------
void R3BCaloHitFinder::SetExperimentalResolution(Double_t crystalRes)
{	
	fCrystalResolution=crystalRes;
	cout << "-I- R3BCaloHitFinder::SetExperimentalResolution to " << fCrystalResolution << "% @ 1 MeV." << endl;
}


// -----  Public method SetDetectionThreshold  ----------------------------------
void R3BCaloHitFinder::SetDetectionThreshold(Double_t thresholdEne)
{	
	fThreshold=thresholdEne;
	cout << "-I- R3BCaloHitFinder::SetDetectionThreshold to " << fThreshold << " GeV." << endl;
}


// ---- Public method GetAngles   --------------------------------------------------
void R3BCaloHitFinder::GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal, Double_t* rho)
{
   	
	Double_t local[3]={0,0,0};
	Double_t master[3];
	Int_t crystalType = 0;
	Int_t crystalCopy = 0;
	Int_t alveolusCopy =0;
	Int_t crystalInAlveolus=0;
	
	TGeoVolume *pAWorld  =  gGeoManager->GetTopVolume();
	if(fGeometryVersion==0){
		//The present scheme here done works nicely with 5.0
		// crystalType = crystal type (from 1 to 30)   
		// crystalCopy = crystal copy (from 1 to 512 for crystal types from 1 to 6 (BARREL), 
		//							   from 1 to 64 for crystal types from 7 to 30 (ENDCAP))
		// crystalId = (crystal type-1) *512 + crystal copy  (from 1 to 3072) for the BARREL
		// crystalId = 3072 + (crystal type-7) *64 + crystal copy  (from 3073 to 4608) for the ENDCAP
		//
		if(iD<3073)
			crystalType = (Int_t)((iD-1)/512) + 1;	//crystal type (from 1 to 30)
		else	
			crystalType = (Int_t)((iD-3073)/64) + 7;	//crystal type (from 1 to 30)

		if(iD<3073)	
			crystalCopy = ((iD-1)%512) + 1;			//CrystalCopy (from 1 to 512)
		else	
			crystalCopy = ((iD-3073)%64) + 1;			//CrystalCopy (from 1 to 160)

		
		Char_t nameVolume[200]; 
		//in the crystalLog creation description in cal/R3BCalo.cxx for v5.0, the crystalCopy begins in 0!!!
		sprintf(nameVolume, "/cave_1/CalifaWorld_0/crystalLog%i_%i",crystalType,crystalCopy-1); 
		
		gGeoManager->cd(nameVolume);
		TGeoNode* currentNode = gGeoManager->GetCurrentNode();
		currentNode->LocalToMaster(local, master);
				
		sprintf(nameVolume, "/cave_1/CalifaWorld_0");
		gGeoManager->cd(nameVolume);
		currentNode = gGeoManager->GetCurrentNode();
		local[0]=master[0];local[1]=master[1];local[2]=master[2];
		currentNode->LocalToMaster(local, master);		
		
	}
	else if(fGeometryVersion==1){
		//The present scheme here done works nicely with 7.05
		// crystalType = alveolus type (from 1 to 24)   [Basically the alveolus number]
		// crystalCopy = (alveolus copy - 1) * 4 + crystals copy (from 1 to 160)  [Not exactly azimuthal]
		// crystalId = (alveolus type-1)*160 + (alvelous copy-1)*4 + (crystal copy)  (from 1 to 3840)
		//				crystalID is asingle identifier per crystal!
		//That is:
		// crystalId = (crystalType-1)*160 + cpAlv * 4 + cpCry;
		//
		crystalType = (Int_t)((iD-1)/160) + 1;	//Alv type (from 1 to 24) 
		crystalCopy = ((iD-1)%160) + 1;			//CrystalCopy (from 1 to 160)
		alveolusCopy =(Int_t)(((iD-1)%160)/4) +1; //Alveolus copy (from 1 to 40)
		crystalInAlveolus = (iD-1)%4 + 1;         //Crystal number in alveolus (from 1 to 4)
	
		Int_t alveoliType[24]={1,1,2,2,2,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,6,6,6};
		Char_t nameVolume[200];
		if(crystalInAlveolus<3) 
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
					crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus, alveoliType[crystalType-1]);
		else 
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i/Crystal_%iB_1", 
					crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus, alveoliType[crystalType-1]);
		
		//cout << gGeoManager->GetPath()<<endl;
		gGeoManager->cd(nameVolume);
		TGeoNode* currentNode = gGeoManager->GetCurrentNode();
		currentNode->LocalToMaster(local, master);
		if(crystalInAlveolus<3) 
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
					crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus);
		else 
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i", 
					crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus);
		
		gGeoManager->cd(nameVolume);
		currentNode = gGeoManager->GetCurrentNode();
		local[0]=master[0];local[1]=master[1];local[2]=master[2];
		currentNode->LocalToMaster(local, master);
		
		sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i",crystalType, alveolusCopy-1);
		gGeoManager->cd(nameVolume);
		currentNode = gGeoManager->GetCurrentNode();
		local[0]=master[0];local[1]=master[1];local[2]=master[2];
		currentNode->LocalToMaster(local, master);
		
		sprintf(nameVolume, "/cave_1/CalifaWorld_0");
		gGeoManager->cd(nameVolume);
		currentNode = gGeoManager->GetCurrentNode();
		local[0]=master[0];local[1]=master[1];local[2]=master[2];
		currentNode->LocalToMaster(local, master);		
	}
	else if(fGeometryVersion==2){
		//The present scheme here done works nicely with 7.07
		// crystalType = alveolus type (from 1 to 20)   [Basically the alveolus number]
		// crystalCopy = (alveolus copy - 1) * 4 + crystals copy (from 1 to 128)  [Not exactly azimuthal]
		// crystalId = (alveolus type-1)*128 + (alvelous copy-1)*4 + (crystal copy)  (from 1 to 2560)
		//				crystalID is asingle identifier per crystal!
		//That is:
		// crystalId = (crystalType-1)*128 + cpAlv * 4 + cpCry;
		//
		crystalType = (Int_t)((iD-1)/128) + 1;	//Alv type (from 1 to 20) 
		crystalCopy = ((iD-1)%128) + 1;			//CrystalCopy (from 1 to 128)
		alveolusCopy =(Int_t)(((iD-1)%128)/4) +1; //Alveolus copy (from 1 to 32)
		crystalInAlveolus = (iD-1)%4 + 1;         //Crystal number in alveolus (from 1 to 4)
		
		Int_t alveoliType[20]={1,1,2,2,2,3,3,4,4,4,5,5,6,6,6,7,7,7,8,8};
		
		Char_t nameVolume[200];
		if(crystalInAlveolus<3) 
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
					crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus, alveoliType[crystalType-1]);
		else 
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i/Crystal_%iB_1", 
					crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus, alveoliType[crystalType-1]);
		
		//cout << gGeoManager->GetPath()<<endl;
		gGeoManager->cd(nameVolume);
		TGeoNode* currentNode = gGeoManager->GetCurrentNode();
		currentNode->LocalToMaster(local, master);
		if(crystalInAlveolus<3) 
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
					crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus);
		else 
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i", 
					crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus);
		
		gGeoManager->cd(nameVolume);
		currentNode = gGeoManager->GetCurrentNode();
		local[0]=master[0];local[1]=master[1];local[2]=master[2];
		currentNode->LocalToMaster(local, master);
		
		sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i",crystalType, alveolusCopy-1);
		gGeoManager->cd(nameVolume);
		currentNode = gGeoManager->GetCurrentNode();
		local[0]=master[0];local[1]=master[1];local[2]=master[2];
		currentNode->LocalToMaster(local, master);
		
		sprintf(nameVolume, "/cave_1/CalifaWorld_0");
		gGeoManager->cd(nameVolume);
		currentNode = gGeoManager->GetCurrentNode();
		local[0]=master[0];local[1]=master[1];local[2]=master[2];
		currentNode->LocalToMaster(local, master);
	}
	else if(fGeometryVersion==3){
		//The present scheme here done works with 7.09 (ongoing work)

	}
	else if(fGeometryVersion==4){ 
		//The present scheme here done works nicely with 7.17
		// crystalType = crystals type (from 1 to 23)
		// crystalCopy = alveolus copy (from 1 to 32) 
		// crystalId = 3000 + (alvelous copy-1)*23 + (crystal copy-1)  (from 3000 to 3736)
		
		crystalType = ((iD-3000)%23) + 1; 
		crystalCopy = (iD-3000)/23 + 1; 
		//cout << " -------       crystalID="<< iD <<  "         crystalType="
		//	<< crystalType <<  "        crystalCopy="<< crystalCopy <<endl;
		
		//here the alveoliType array meaning is opposite to the other BARREL cases...
		//the array shows the alveoli number where each crystal of the EndCap belong
		Int_t alveoliType[23]={1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3};
		
		Char_t nameVolume[200];
		sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
					alveoliType[crystalType-1], crystalCopy-1, crystalType, crystalType);
		
		// The definition of the crystals is different in this particular EndCap design:
		// the origin for each crystal is the alveoli corner
		if(crystalType==1) {local[0]=-1.24654194026499;local[1]=0.648218850702041;local[2]=9.75;}
		else if(crystalType==2) {local[0]=-4.57345805973491;local[1]=0.64821885070204;local[2]=9.75; }
		else if(crystalType==3) {local[0]=-1.33137201955507;local[1]=2.59875;local[2]=9.75;}
		else if(crystalType==4) {local[0]=-4.48862798044483;local[1]=2.59875;local[2]=9.75;}
		else if(crystalType==5) {local[0]=-1.40646166097653;local[1]=4.33125;local[2]=9.75;}
		else if(crystalType==6) {local[0]=-4.41353833902337;local[1]=4.33125;local[2]=9.75;}
		else if(crystalType==7) {local[0]=-1.49184443219704;local[1]=6.29334272612279;local[2]=9.75;}
		else if(crystalType==8) {local[0]=-4.32815556780286;local[1]=6.29334272612279;local[2]=9.75;}
		else if(crystalType==9) {local[0]=-1.01201040487607;local[1]=0.739366638062432;local[2]=9.75;}
		else if(crystalType==10) {local[0]=-3.64595915673333;local[1]=0.739366638062431;local[2]=9.75;}
		else if(crystalType==11) {local[0]=-1.10703474322362;local[1]=2.8015;local[2]=9.75;}
		else if(crystalType==12) {local[0]=-3.55093481838578;local[1]=2.8015;local[2]=9.75;}
		else if(crystalType==13) {local[0]=-1.19303835409747;local[1]=4.669;local[2]=9.75;}
		else if(crystalType==14) {local[0]=-3.46496164590243;local[1]=4.669;local[2]=9.75;}
		else if(crystalType==15) {local[0]=-1.28957028029965;local[1]=6.76354164083689;local[2]=9.75;}
		else if(crystalType==16) {local[0]=-3.36842971970024;local[1]=6.76354164083688;local[2]=9.75;}
		else if(crystalType==17) {local[0]=-0.70032959626342;local[1]=0.836296001993993;local[2]=9.75;}
		else if(crystalType==18) {local[0]=-2.53967040373648;local[1]=0.836296001993992;local[2]=9.75;}
		else if(crystalType==19) {local[0]=-0.797829723064974;local[1]=2.95125;local[2]=9.75;}
		else if(crystalType==20) {local[0]=-2.44217027693492;local[1]=2.95125;local[2]=9.75;}
		else if(crystalType==21) {local[0]=-0.888582051402232;local[1]=4.91875;local[2]=9.75;}
		else if(crystalType==22) {local[0]=-2.35141794859767;local[1]=4.91875;local[2]=9.75;}
		else if(crystalType==23) {local[0]=-1.61999999999997;local[1]=7.22933631874703;local[2]=9.75;}
			
		//cout << gGeoManager->GetPath()<<endl;
		gGeoManager->cd(nameVolume);
		TGeoNode* currentNode = gGeoManager->GetCurrentNode();
		currentNode->LocalToMaster(local, master);
		
		sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1",
					alveoliType[crystalType-1], crystalCopy-1, crystalType);

		gGeoManager->cd(nameVolume);
		currentNode = gGeoManager->GetCurrentNode();
		local[0]=master[0];local[1]=master[1];local[2]=master[2];
		currentNode->LocalToMaster(local, master);
		
		sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i",alveoliType[crystalType-1], crystalCopy-1);
		gGeoManager->cd(nameVolume);
		currentNode = gGeoManager->GetCurrentNode();
		local[0]=master[0];local[1]=master[1];local[2]=master[2];
		currentNode->LocalToMaster(local, master);
		
		sprintf(nameVolume, "/cave_1/CalifaWorld_0");
		gGeoManager->cd(nameVolume);
		currentNode = gGeoManager->GetCurrentNode();
		local[0]=master[0];local[1]=master[1];local[2]=master[2];
		currentNode->LocalToMaster(local, master);
		
	}
	else if(fGeometryVersion==5){
		//The present scheme here done works nicely with 7.07+7.17
		//see the explanation for geometries 2 and 4
		if(iD<3000){
			crystalType = (Int_t)((iD-1)/128) + 1;	//Alv type (from 1 to 20) 
			crystalCopy = ((iD-1)%128) + 1;			//CrystalCopy (from 1 to 128)
			alveolusCopy =(Int_t)(((iD-1)%128)/4) +1; //Alveolus copy (from 1 to 32)
			crystalInAlveolus = (iD-1)%4 + 1;         //Crystal number in alveolus (from 1 to 4)
			
			Int_t alveoliType[20]={1,1,2,2,2,3,3,4,4,4,5,5,6,6,6,7,7,7,8,8};
		
			Char_t nameVolume[200];
			if(crystalInAlveolus<3) 
				sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i/Crystal_%iA_1",
						crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus, alveoliType[crystalType-1]);
			else 
				sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i/Crystal_%iB_1", 
						crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus, alveoliType[crystalType-1]);
			
			//cout << gGeoManager->GetPath()<<endl;
			gGeoManager->cd(nameVolume);
			TGeoNode* currentNode = gGeoManager->GetCurrentNode();
			currentNode->LocalToMaster(local, master);
			if(crystalInAlveolus<3) 
				sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iA_%i",
						crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus);
			else 
				sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i/CrystalWithWrapping_%iB_%i", 
						crystalType, alveolusCopy-1, alveoliType[crystalType-1], crystalInAlveolus);
		
			gGeoManager->cd(nameVolume);
			currentNode = gGeoManager->GetCurrentNode();
			local[0]=master[0];local[1]=master[1];local[2]=master[2];
			currentNode->LocalToMaster(local, master);
		
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_%i_%i",crystalType, alveolusCopy-1);
			gGeoManager->cd(nameVolume);
			currentNode = gGeoManager->GetCurrentNode();
			local[0]=master[0];local[1]=master[1];local[2]=master[2];
			currentNode->LocalToMaster(local, master);
		
			sprintf(nameVolume, "/cave_1/CalifaWorld_0");
			gGeoManager->cd(nameVolume);
			currentNode = gGeoManager->GetCurrentNode();
			local[0]=master[0];local[1]=master[1];local[2]=master[2];
			currentNode->LocalToMaster(local, master);
		}
		else{
			crystalType = ((iD-3000)%23) + 1; 
			crystalCopy = (iD-3000)/23 + 1; 
			//cout << " -------       crystalID="<< iD <<  "         crystalType="
			//	<< crystalType <<  "        crystalCopy="<< crystalCopy <<endl;
			
			//here the alveoliType array meaning is opposite to the other BARREL cases...
			//the array shows the alveoli number where each crystal of the EndCap belong
			Int_t alveoliType[23]={1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3};
			
			Char_t nameVolume[200];
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
					alveoliType[crystalType-1], crystalCopy-1, crystalType, crystalType);
			
			// The definition of the crystals is different in this particular EndCap design:
			// the origin for each crystal is the alveoli corner
			if(crystalType==1) {local[0]=-1.24654194026499;local[1]=0.648218850702041;local[2]=9.75;}
			else if(crystalType==2) {local[0]=-4.57345805973491;local[1]=0.64821885070204;local[2]=9.75; }
			else if(crystalType==3) {local[0]=-1.33137201955507;local[1]=2.59875;local[2]=9.75;}
			else if(crystalType==4) {local[0]=-4.48862798044483;local[1]=2.59875;local[2]=9.75;}
			else if(crystalType==5) {local[0]=-1.40646166097653;local[1]=4.33125;local[2]=9.75;}
			else if(crystalType==6) {local[0]=-4.41353833902337;local[1]=4.33125;local[2]=9.75;}
			else if(crystalType==7) {local[0]=-1.49184443219704;local[1]=6.29334272612279;local[2]=9.75;}
			else if(crystalType==8) {local[0]=-4.32815556780286;local[1]=6.29334272612279;local[2]=9.75;}
			else if(crystalType==9) {local[0]=-1.01201040487607;local[1]=0.739366638062432;local[2]=9.75;}
			else if(crystalType==10) {local[0]=-3.64595915673333;local[1]=0.739366638062431;local[2]=9.75;}
			else if(crystalType==11) {local[0]=-1.10703474322362;local[1]=2.8015;local[2]=9.75;}
			else if(crystalType==12) {local[0]=-3.55093481838578;local[1]=2.8015;local[2]=9.75;}
			else if(crystalType==13) {local[0]=-1.19303835409747;local[1]=4.669;local[2]=9.75;}
			else if(crystalType==14) {local[0]=-3.46496164590243;local[1]=4.669;local[2]=9.75;}
			else if(crystalType==15) {local[0]=-1.28957028029965;local[1]=6.76354164083689;local[2]=9.75;}
			else if(crystalType==16) {local[0]=-3.36842971970024;local[1]=6.76354164083688;local[2]=9.75;}
			else if(crystalType==17) {local[0]=-0.70032959626342;local[1]=0.836296001993993;local[2]=9.75;}
			else if(crystalType==18) {local[0]=-2.53967040373648;local[1]=0.836296001993992;local[2]=9.75;}
			else if(crystalType==19) {local[0]=-0.797829723064974;local[1]=2.95125;local[2]=9.75;}
			else if(crystalType==20) {local[0]=-2.44217027693492;local[1]=2.95125;local[2]=9.75;}
			else if(crystalType==21) {local[0]=-0.888582051402232;local[1]=4.91875;local[2]=9.75;}
			else if(crystalType==22) {local[0]=-2.35141794859767;local[1]=4.91875;local[2]=9.75;}
			else if(crystalType==23) {local[0]=-1.61999999999997;local[1]=7.22933631874703;local[2]=9.75;}
			
			//cout << gGeoManager->GetPath()<<endl;
			gGeoManager->cd(nameVolume);
			TGeoNode* currentNode = gGeoManager->GetCurrentNode();
			currentNode->LocalToMaster(local, master);
			
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1",
					alveoliType[crystalType-1], crystalCopy-1, crystalType);
			
			gGeoManager->cd(nameVolume);
			currentNode = gGeoManager->GetCurrentNode();
			local[0]=master[0];local[1]=master[1];local[2]=master[2];
			currentNode->LocalToMaster(local, master);
			
			sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i",alveoliType[crystalType-1], crystalCopy-1);
			gGeoManager->cd(nameVolume);
			currentNode = gGeoManager->GetCurrentNode();
			local[0]=master[0];local[1]=master[1];local[2]=master[2];
			currentNode->LocalToMaster(local, master);
			
			sprintf(nameVolume, "/cave_1/CalifaWorld_0");
			gGeoManager->cd(nameVolume);
			currentNode = gGeoManager->GetCurrentNode();
			local[0]=master[0];local[1]=master[1];local[2]=master[2];
			currentNode->LocalToMaster(local, master);		
		}
	}
	else if(fGeometryVersion==6){
		//The present scheme here done works with 7.09+7.17
		//see the explanation for geometries 3 and 4
	}
	else if(fGeometryVersion==10){
		//The present scheme here done works with 8.??
		
	}
	else cout << "-E- R3BCaloHitFinder: Geometry version not available in R3BCalo::ProcessHits(). " << endl;
	
	
	//cout << "-I- R3BCaloHitFinder::GetAngles: position of crystal center: "<<master[0] << ", "<<master[1] << ", "<<master[2] << endl;
	TVector3 masterV(master[0],master[1],master[2]);
	//masterV.Print();
	*polar=masterV.Theta();
	*azimuthal=masterV.Phi();
	*rho=masterV.Mag();
	//cout << "-I- R3BCaloHitFinder::GetAngles: theta: "<< *polar <<", phi: "<< *azimuthal << "for crystal iD " << iD <<endl;
}



// -----   Private method ExpResSmearing  --------------------------------------------
Double_t R3BCaloHitFinder::ExpResSmearing(Double_t inputEnergy) {
	// Smears the energy according to some Experimental Resolution distribution 
	// Very simple preliminary scheme where the Experimental Resolution 
	// is introduced as a gaus random distribution with a width given by the 
	// parameter fCrystalResolution(in % @ MeV). Scales according to 1/sqrt(E)	
	//
	// The formula is   TF1("name","0.058*x/sqrt(x)",0,10) for 3% at 1MeV (3.687 @ 662keV)
	//  ( % * energy ) / sqrt( energy )
	// and then the % is given at 1 MeV!!
	//
	if(fCrystalResolution == 0) return inputEnergy;
	else{
		//Energy in MeV, that is the reason for the factor 1000...
		Double_t randomIs = gRandom->Gaus(0,inputEnergy*fCrystalResolution*1000/(235*sqrt(inputEnergy*1000)));
		//cout << "randomIs " << randomIs  << " for and Energy of "<< rawEnergy  << endl;
		return inputEnergy + randomIs/1000;
	}
}


// -----   Public method SetClusteringAlgorithm  --------------------------------------------
void R3BCaloHitFinder::SetClusteringAlgorithm(Int_t ClusteringAlgorithmSelector, Double_t ParCluster1) {
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
void R3BCaloHitFinder::SetAngularWindow(Double_t deltaPolar, Double_t deltaAzimuthal, Double_t DeltaAngleClust) {
	//
	// Set the angular window open around the crystal with the largest energy
	// to search for additional crystal hits and addback to the same cal hit
	// [0.25 around 14.3 degrees, 3.2 for the complete calorimeter]
	fDeltaPolar = deltaPolar;
	fDeltaAzimuthal = deltaAzimuthal;
	fDeltaAngleClust = DeltaAngleClust ;

}


// -----   Private method AddHit  --------------------------------------------
R3BCaloHit* R3BCaloHitFinder::AddHit(UInt_t Nbcrystals,Double_t ene,Double_t pAngle,Double_t aAngle){   
	// It fills the R3BLandDigi array
	TClonesArray& clref = *fCaloHitCA;
	Int_t size = clref.GetEntriesFast();	
	//cout << "-I- R3BCaloHitFinder: Adding CaloHit from " << Nbcrystals 
	//	<< " crystalHits, depositing " << ene*1e06 << " keV" << endl;
	return new(clref[size]) R3BCaloHit(Nbcrystals, ene, pAngle, aAngle);  
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


ClassImp(R3BCaloHitFinder)
