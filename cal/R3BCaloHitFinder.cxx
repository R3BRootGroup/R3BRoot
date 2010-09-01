// -------------------------------------------------------------------------
// -----                   R3BCaloHitFinder source file                -----
// -----                  Created 27/08/10  by H.Alvarez               -----
// -----			Last modification 01/09/10 by H.Alvarez			   -----
// -------------------------------------------------------------------------
#include "R3BCaloHitFinder.h"
#include "TMath.h"
#include "TVector3.h"
#include "TGeoMatrix.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TGeoManager.h"

#include "R3BCaloCrystalHit.h"

using std::cout;
using std::endl;


R3BCaloHitFinder::R3BCaloHitFinder() : FairTask("R3B CALIFA Hit Finder ") { 
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
	Double_t angle1,angle2;
	
	Int_t* usedCrystalHits=0; //array to control the CrystalHits
	Int_t crystalsInHit=0;  //used crystals in each CaloHit
	Double_t testPolar, testAzimuthal;
	
	//THIS COULD COME FROM THE PARAMETERS IN THE FUTURE???
	Double_t deltaPolar = 0.25; //around 14.3 degrees
	Double_t deltaAzimuthal = 0.25;

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
	
	while (unusedCrystals>0) {
		//First, finding the crystal with higher energy from the unused crystalHits
		
		for(Int_t i=1;i<crystalHits;i++) {
			if (!usedCrystalHits[i] && crystalHit[i]->GetEnergy() > crystalHit[crystalWithHigherEnergy]->GetEnergy())     
				crystalWithHigherEnergy = i;
		}
		
		usedCrystalHits[crystalWithHigherEnergy] = 1; unusedCrystals--; crystalsInHit++;
		
		//Second, energy and angles come from the crystal with the higher energy
		energy = crystalHit[crystalWithHigherEnergy]->GetEnergy();
		GetAngles(crystalHit[crystalWithHigherEnergy]->GetCrystalId(),&polarAngle,&azimuthalAngle);
				
		//Third, finding closest hits and adding their energy
		for(Int_t i=0;i<crystalHits;i++){
			if (!usedCrystalHits[i] ) {
				GetAngles(crystalHit[i]->GetCrystalId(), &testPolar, &testAzimuthal);
			
				//Dealing with the particular definition of azimuthal angles (discontinuity in pi and -pi)
				if(azimuthalAngle + deltaAzimuthal > TMath::Pi()) {
					angle1 = azimuthalAngle-TMath::Pi(); angle2 = testAzimuthal-TMath::Pi();
				}
				else if(azimuthalAngle - deltaAzimuthal < -TMath::Pi()){
					angle1 = azimuthalAngle+TMath::Pi(); angle2 = testAzimuthal+TMath::Pi();
				}
				else {angle1 = azimuthalAngle; angle2 = testAzimuthal;}
			
				if(TMath::Abs(polarAngle - testPolar) < deltaPolar && 
				   TMath::Abs(angle1 - angle2) < deltaAzimuthal ) {
					energy += crystalHit[i]->GetEnergy(); 
					usedCrystalHits[i] = 1; unusedCrystals--; crystalsInHit++;
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


// ---- Public method GetAngles   --------------------------------------------------
void R3BCaloHitFinder::GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal)
{
	//The present scheme here done works nicely with 7.05
	// crystalType = alveolus type (from 1 to 24)   [Basically the alveolus number]
	// crystalCopy = (alveolus copy - 1) * 4 + crystals copy (from 1 to 160)  [Not exactly azimuthal]
	// crystalId = (alveolus type-1)*160 + (alvelous copy-1)*4 + (crystal copy)  (from 1 to 3840)
	//				crystalID is asingle identifier per crystal!
	//That is:
	// crystalId = (crystalType-1)*160 + cpAlv * 4 + cpCry;
	//
	Int_t crystalType = (Int_t)((iD-1)/160) + 1;	//Alv type (from 1 to 24) 
	Int_t crystalCopy = ((iD-1)%160) + 1;			//CrystalCopy (from 1 to 160)
	Int_t alveolusCopy =(Int_t)(((iD-1)%160)/4) +1; //Alveolus copy (from 1 to 40)
	Int_t crystalInAlveolus = (iD-1)%4 + 1;         //Crystal number in alveolus (from 1 to 4)
	
	Int_t alveoliType[24]={1,1,2,2,2,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,6,6,6};

	Double_t local[3]={0,0,0};
	Double_t master[3];
	
	TGeoVolume *pAWorld  =  gGeoManager->GetTopVolume();

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
		
	
	//cout << "-I- R3BCaloHitFinder::GetAngles: position of crystal center: "<<master[0] << ", "<<master[1] << ", "<<master[2] << endl;
	TVector3 masterV(master[0],master[1],master[2]);
	//masterV.Print();
	*polar=masterV.Theta();
	*azimuthal=masterV.Phi();
	//cout << "-I- R3BCaloHitFinder::GetAngles: theta: "<< *polar <<", phi: "<< *azimuthal <<endl;
}



R3BCaloHit* R3BCaloHitFinder::AddHit(UInt_t Nbcrystals,Double_t ene,Double_t pAngle,Double_t aAngle){   
	// It fills the R3BLandDigi array
	TClonesArray& clref = *fCaloHitCA;
	Int_t size = clref.GetEntriesFast();	
	cout << "-I- R3BCaloHitFinder: Adding CaloHit from " << Nbcrystals 
		<< " crystalHits, depositing " << ene*1e06 << " keV" << endl;
	return new(clref[size]) R3BCaloHit(Nbcrystals, ene, pAngle, aAngle);  
}


/*
Double_t diffuseEnergy(Double_t rawEnergy){
	//
	// Diffuses the energy in the crystal in an gaussian way...
	// Trying to simulate the intrinsic crystal resolution
	// The formula is   TF1("name","0.058*x/sqrt(x)",0,10) for 3% at 1MeV (3.687 @ 662keV)
	//
	//  ( % * energy ) / sqrt( energy )
	//
	// and then the % is given at 1 MeV!!
	Double_t crystalResolution = theDiffusionParameters.getResolution(); 
	if(rawEnergy<=0) return rawEnergy;
	if(crystalResolution == 0) return rawEnergy;
	else{
		Double_t randomIs = rGen->Gaus(0,rawEnergy*crystalResolution/(235*sqrt(rawEnergy)));
		//cout << "randomIs " << randomIs  << " for and Energy of "<< rawEnergy  << endl;
		return rawEnergy + randomIs;
	}
}
 
 
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
