///////////////////////////////////////////////////////////////////
//*-- AUTHOR : Nick Ashwood
//*-- Date: 11/2011
//*-- Last Update: 
//*-- Copyright:
//
// --------------------------------------------------------------
//
// analysis_code_Si_CALIFA.cxx
// Simple macro for the analysis and reconstruction of the
// Hits produced in both the tracker and CALIFA
//
// -------------------------------------------------------------- 
// --------------------------------------------------------------
// How to run this program:
//
// 1 - Run the simulation in the directory /macros/r3b:
//      root -l r3bsim.c
//      root -l califa/califaAna.c
//      root -l tra/trackerAna.c
//
// 2 - Quit the root session and go to the directory  analysis/TreeRead:
//      .q
//      cd ../../analysis/TreeRead
//
// 3 - Run this macro inside root
//      .L analysis_code_Si_CALIFA.cxx
//      analEvents(1,"outputFileName.root")
//
//  the number within brackets means the analysis type. See the code
//  for the description of each analysis type.
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////


#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TMath.h"
#include "TRandom.h"
#include "TTree.h"


void analEvents(Int_t analType=1, char* output) {

  	char title1[250];
  	char title2[250];
  
   	//
   	// Function containing the event loop
	//
	// ----    Debug option   -------------------------------------------------
   	gDebug = 0;
   	// ------------------------------------------------------------------------
     
  	// Set math variables:

  	Double_t Pi = TMath::Pi();
  	Double_t RadToDeg = TMath::RadToDeg();
  	Double_t DegToRad = TMath::DegToRad();
  
  	//gSystem->Load("../R3BSim/R3BSim/trunk/R3BSim.sl");
  	gROOT->Reset();
  	gROOT->SetStyle("Default");
  	gStyle->SetOptTitle(0);
  	gStyle->SetOptStat(1);
  	gStyle->SetOptFit(0);

	
	//opening the simulated File and Tree
	
  	sprintf(title1,"%s","../../macros/r3b/califaAna.root");  	
  	sprintf(title2,"%s","../../macros/r3b/tra_digi.root");  
	
  	TFile *file1 = TFile::Open(title1);
  	TTree* TCal = (TTree*)file1->Get("cbmsim");  // cbmsim is the name of the Tree
	
  	TFile *file2 = TFile::Open(title2);
  	TTree* TTra = (TTree*)file2->Get("cbmsim"); 
	
	struct kinematics_t{
		Float_t Qv;
		Float_t Es;
		Float_t Et;
		Float_t Er;
		Float_t E1;
		Float_t E2;
		Float_t Th1;
		Float_t Th2;
		Float_t Phi1;
		Float_t Phi2;
		Float_t SiE1;
		Float_t SiE2;
		Float_t CsIE1;
		Float_t CsIE2;
		Float_t Gamma;
        Float_t Califa;
	};
	
	kinematics_t kinematics;
	
	//opening the output file
	TFile* outFile = new TFile(output,"RECREATE");
	
	outFile->cd(); 

	TTree* Tree = new TTree("kin","Calculated kinematics of the reaction");
	Tree->Branch("kinematics",&kinematics,"Qv/F:Es:Et:Er:E1:E2:Th1:Th2:Phi1:Phi2:SiE1:SiE2:CsIE1:CsIE2:Gamma:Califa");
	
	
	// Created 1D Histograms:
	
	TH1F *QV = new TH1F("Qvalue","Energy",200,-0.1,0.1);
	TH1F *ESum = new TH1F("Esum","Energy",1000,0.,1.);
	TH1F *ETot = new TH1F("Etot","Energy",200,5.9,6.1);
	TH1F *ERec = new TH1F("Erec","Energy",1000,5.0,6.0);
	TH1F *Angle = new TH1F("Angle","Angle",1800,0.,180.);
	TH1F *CALIFA = new TH1F("Califa","Energy",500,0.,0.05);
    TH1F *GAMMA = new TH1F("Gamma","Energy",500,0.,0.05);
    TH1F *GAMMA_DOP = new TH1F("GammaDop","Energy",500,0.,0.05);
	TH1F *E1 = new TH1F("Energy1","Energy",100,0.,0.35);
	TH1F *E2 = new TH1F("Energy2","Energy",100,0.,0.35);

	// Created 2D Histograms:
	
	TH2F *Th1Th2 = new TH2F("Theta12","Theta1 vs Theta2",180,0.,180.,180,0.,180.);
	TH2F *EdE1 = new TH2F("EdE1","ECsI vs ESi",50,0.,0.005,300,0.,0.3);
	TH2F *EdE2 = new TH2F("EdE2","ECsI vs ESi",50,0.,0.005,300,0.,0.3);
    TH2F *ETHETA_CAL = new TH2F("ETheta","Theta vs ECsI",1800,0.,180.,4000,0.,0.4);

	//Califa Hits
	TClonesArray* caloHitCA;  
	R3BCalifaHitData** caloHit;
	caloHitCA = new TClonesArray("R3BCalifaHitData",5);
	TBranch *branchCaloHit = TCal->GetBranch("CalifaHitData");
	branchCaloHit->SetAddress(&caloHitCA);

	//Tracker Hits
	TClonesArray* traHitCA;  
	R3BTrackerHit** traHit;
	traHitCA = new TClonesArray("R3BTrackerHit",5);
	TBranch *branchTraHit = TTra->GetBranch("TrackerHit");
	branchTraHit->SetAddress(&traHitCA);
	
	Int_t fPDGType; 
  	Double_t Mass = 1.007825; 
  	Double_t Ebeam = 4.8;
	
	Long64_t nevents = TTra->GetEntries();
	
	cout << "nevents = " << nevents << endl;
 	cout << "detected particle mass = " << Mass << endl;
  	cout << "beam energy = " << Ebeam << endl;
		
	Int_t counter = 0;

	Double_t *CsIE;
	Double_t *CsIPhi;
	Double_t *CsITheta;
	Double_t *Energy;
	
	Double_t *DetE;
    Double_t *SiE;
    Double_t *Silicon;

	Double_t *X_track;
	Double_t *Y_track;
	Double_t *Z_track;
    
	Double_t *X_hit;
	Double_t *Y_hit;
	Double_t *Z_hit;

	Double_t X_diff = 0.;
	Double_t Y_diff = 0.;
	Double_t Z_diff = 0.;
	
	Double_t *Theta_track;
	Double_t *Phi_track;
	Double_t *ThetaZero;
	Double_t *PhiZero;	
	
	for(Int_t i=0;i<nevents;i++){
	
	   if(i%1000 == 0) printf("Event:%i\n",i);
		
		Bool_t proton = false;
		Bool_t event = false;
				
		Int_t caloHitsPerEvent = 0;
		Int_t traHitsPerEvent = 0;
        Int_t protonHit = 0; 
        Int_t hits = 0;
        
        Double_t GammaE = 0.;
        Double_t GammaE_corr = 0.;

		Double_t Erec = 0.;
		Double_t Esum = 0.;
		Double_t Etot = 0.;
		Double_t Qvalue = 0.;
		Double_t Qvalue1 = 0.;
		Double_t Qvalue2 = 0.;
		Double_t Angle_track = 0.;
		Double_t Ep = 0.;
        Double_t TotSiEnergy = 0.;
	   
		caloHitCA->Clear();
		TCal->GetEvent(i);
		caloHitsPerEvent = caloHitCA->GetEntries();
                			
		// Read in CALIFA energies and angles
		
		if(caloHitsPerEvent>0) {
			
			CsIE = new Double_t[caloHitsPerEvent+1];
			CsIPhi = new Double_t[caloHitsPerEvent+1];
			CsITheta = new Double_t[caloHitsPerEvent+1];

                        caloHit = new R3BCalifaHitData*[caloHitsPerEvent+1];
            
			for(Int_t j=0;j<caloHitsPerEvent;j++){
                
				caloHit[j] = new R3BCalifaHitData;
				caloHit[j] = (R3BCalifaHitData*) caloHitCA->At(j);
                
				CsIE[j] = caloHit[j]->GetEnergy();
				CsIPhi[j] = caloHit[j]->GetPhi();	
				CsITheta[j] = caloHit[j]->GetTheta();
                
                CALIFA->Fill(CsIE[j]);
                kinematics.Califa = CsIE[j];
                Tree->Fill();
                
			}
		}
                
        traHitCA->Clear();	
		TTra->GetEvent(i);	
		traHitsPerEvent = traHitCA->GetEntries();
						
		// Read in Si energies, positions and angles
		
		if(traHitsPerEvent>0) {	
		
			DetE = new Double_t[traHitsPerEvent+1];
			X_track = new Double_t[traHitsPerEvent+1];
			Y_track = new Double_t[traHitsPerEvent+1];
			Z_track = new Double_t[traHitsPerEvent+1];		
			X_hit = new Double_t[traHitsPerEvent+1];
			Y_hit = new Double_t[traHitsPerEvent+1];
			Z_hit = new Double_t[traHitsPerEvent+1];
            ThetaZero = new Double_t[traHitsPerEvent+1];
            PhiZero = new Double_t[traHitsPerEvent+1];
			Theta_track = new Double_t[traHitsPerEvent+1];
			Phi_track = new Double_t[traHitsPerEvent+1];
            Energy = new Double_t[traHitsPerEvent+1];
            SiE = new Double_t[traHitsPerEvent+1];
            Silicon = new Double_t[traHitsPerEvent+1];

			traHit = new R3BTrackerHit*[traHitsPerEvent+1];
			
			for(Int_t j=0;j<traHitsPerEvent;j++){
                
				traHit[j] = new R3BTrackerHit;
				traHit[j] = (R3BTrackerHit*) traHitCA->At(j);
              
                DetE[j] = traHit[j]->GetEnergy();
                SiE[j] = DetE[j];
                                
                X_track[j] = traHit[j]->GetXHit();
                Y_track[j] = traHit[j]->GetYHit();
                Z_track[j] = traHit[j]->GetZHit();
                
                ThetaZero[j] = traHit[j]->GetThetaZero();
                PhiZero[j] = traHit[j]->GetPhiZero();
                
			}
            
		}
        	
        for (Int_t j=0; j<caloHitsPerEvent; j++) { // Loop over CALIFA hits
            
            hits = 0;
            proton = false;
            
            for(Int_t k=0; k<traHitsPerEvent; k++) { // Loop over Silicon hits
                
                // combine same track hits
                                
                if(PhiZero[k]*RadToDeg >= (CsIPhi[j]*RadToDeg-5.) && PhiZero[k]*RadToDeg <= (CsIPhi[j]*RadToDeg+5.)){ 
                    
                    proton = true; // proton event 
                    
                    hits++;
                    
                    X_hit[hits] = X_track[k];
                    Y_hit[hits] = Y_track[k];
                    Z_hit[hits] = Z_track[k];
                   
                    TotSiEnergy += SiE[k]; 
                                        
                }
              
            }
            
            if(!proton){ // If no proton event assume it's a gamma
                  
                GammaE_corr = GetCMEnergy(CsITheta[j],CsIE[j],0.95);
                GammaE += GammaE_corr; 
                GAMMA->Fill(CsIE[j]);
                GAMMA_DOP->Fill(GammaE_corr);
                kinematics.Gamma = GammaE;
                Tree->Fill();

            }
            
            if(proton){ // if proton event find angle and energy of proton
                            
                if((fabs(X_hit[1]) < fabs(X_hit[2])) && (fabs(Y_hit[1]) < fabs(Y_hit[2])) && (Z_hit[1] < Z_hit[2]) && Z_hit[1] != 0. && Z_hit[2] != 0){
                    
                    protonHit++;
                                        
                    X_diff = X_hit[2] - X_hit[1]; // take hits in first and second layer
                    Y_diff = Y_hit[2] - Y_hit[1];
                    Z_diff = Z_hit[2] - Z_hit[1];
                                        
                    Theta_track[protonHit] = GetThetaScat(X_diff,Y_diff,Z_diff);
                    
                    Phi_track[protonHit] = GetPhiScat(X_diff,Y_diff);
                                        
                    Energy[protonHit] = TotSiEnergy + CsIE[j];
                    
                    Silicon[protonHit] = TotSiEnergy;
                    
                    if(Theta_track[protonHit]*RadToDeg < CsITheta[j]*RadToDeg-10. || Theta_track[protonHit]*RadToDeg > CsITheta[j]*RadToDeg+10.){ // Gets rid of some scattering events
                                           
                        protonHit--;
                        
                    }else{
                        
                        ETHETA_CAL->Fill(Theta_track[protonHit]*RadToDeg,CsIE[j]);
                        
                    }    
                }
                
            }
                        
        }
                
        if(protonHit == 2) event = true; // must be two protons only (Need to do tracking properly at some point)
        			
		if (event) {
				
			counter++;
		
			Angle_track = Theta_track[1] + Theta_track[2]; // Scattering angle
        
			Erec=GetERec(Energy[1], Energy[2], Theta_track[1], Phi_track[1], Theta_track[2], Phi_track[2], Ebeam); // Recoil energy
						
			Qvalue = -(Ebeam - Energy[1] - Energy[2] - GammaE - Erec); // Calculate the Qvalue
			Esum =  Energy[1] + Energy[2];
			Etot =  Esum + Erec;
			
			kinematics.Qv = Qvalue;
			QV->Fill(Qvalue);	  
 			kinematics.Es = Esum;
			kinematics.Et = Etot;
			kinematics.Er = Erec;
			kinematics.E1 = Energy[1];
			kinematics.E2 = Energy[2];
			kinematics.Th1 = Theta_track[1]*RadToDeg;
			kinematics.Th2 = Theta_track[2]*RadToDeg;
			kinematics.Phi1 = Phi_track[1]*RadToDeg;
			kinematics.Phi2 = Phi_track[2]*RadToDeg;
			kinematics.SiE1 = TotSiEnergy[1];
			kinematics.SiE2 = TotSiEnergy[2];
			kinematics.CsIE1 = CsIE[1];
			kinematics.CsIE2 = CsIE[2];

			ESum->Fill(Esum);
			ETot->Fill(Etot);
			ERec->Fill(Erec);
			Th1Th2->Fill(Theta_track[1]*RadToDeg,Theta_track[2]*RadToDeg);
			Angle->Fill(Angle_track*RadToDeg);
			EdE1->Fill(Silicon[1],CsIE[1]);
			EdE2->Fill(Silicon[2],CsIE[2]);
            E1->Fill(Energy[1]);
            E2->Fill(Energy[2]);

			Tree->Fill();	
			
		}
				
		if(caloHitsPerEvent){
			delete [] caloHit;
			delete [] CsIE;
			delete [] CsIPhi;
            delete [] CsITheta;
		}
		 
 		 
		if(traHitsPerEvent){
			delete [] traHit;
			delete [] DetE;
            delete [] SiE;
			delete [] X_track;
			delete [] Y_track;
			delete [] Z_track;
			delete [] X_hit;
			delete [] Y_hit;
			delete [] Z_hit;
            delete [] ThetaZero;
            delete [] PhiZero;
            delete [] Energy;
			delete [] Theta_track;
			delete [] Phi_track;
		}
  	}	
		
	cout << " " << endl;
	cout << " " << endl;
	cout << "m=2 events = " << counter <<endl;
	
	Float_t efficiency = (fabs(counter)/fabs(i))*100.;
	
	cout << "Efficiency  = " << efficiency << " % from " << i << " events" << endl;
    	
    CALIFA->Write();
    GAMMA->Write();
    GAMMA_DOP->Write();
    ETHETA_CAL->Write();
	QV->Write();
	ESum->Write();
	ETot->Write();
	ERec->Write();
	Th1Th2->Write();
	Angle->Write();
	EdE1->Write();
	EdE2->Write();
    E1->Write();
    E2->Write();
    
	Tree->Print();	
	Tree->Write();
	
	file1->Close();
	file2->Close();
	outFile->Close();
	
	TFile *file3 = TFile::Open(output) ;

    
}	


// Calculate Energy from momenta

Double_t GetEnergy(Double_t X, Double_t Y, Double_t Z, Double_t M){
	
	Double_t E;
	Double_t T;
	Double_t P;
	Double_t M; // GeV/C^2
	Double_t amugev = 931.5016/1000;
	
	// E=(pow(X,2)+pow(Y,2)+pow(Z,2))/(2.*M); //Non-relativistic
	
	P=sqrt(pow(X,2) + pow(Y,2) + pow(Z,2));
	E=sqrt(pow(P,2) + pow(M*amugev,2));
	T=E-M*amugev;
		
	return T;
	
	//return E;
}

		
// Calculate Scattering angle

Double_t GetThetaScat(Double_t X, Double_t Y, Double_t Z){
	
	Double_t Theta;
    
	Theta=atan(sqrt(pow(X,2)+pow(Y,2))/Z);
    	
	return Theta;
}



// Calculate Phi

Double_t GetPhiScat(Double_t X, Double_t Y){
	
	Double_t Phi;
    
    if(X==0. && Y>0.) Phi = TMath::PiOver2();
    if(X==0. && Y<0.) Phi = -TMath::PiOver2();
	
	if(X>0. && Y>0.)  Phi = atan(Y/X);
	if(X<0. && Y>=0.) Phi = TMath::Pi()+atan(Y/X);
	if(X<0. && Y<0.)  Phi = -TMath::Pi()+atan(Y/X);
	if(X>0. && Y<=0.) Phi = atan(Y/X);
    			
	return Phi;
}

// Calculate recoil energy

Double_t GetERec(Double_t Ep3, Double_t Ep4, Double_t Th3, Double_t Phi3, Double_t Th4, Double_t Phi4, Double_t Ebeam){
	
	Double_t pb,p3l,p3x,p3y,p3z;
	Double_t p4l,p4x,p4y,p4z;
	Double_t prec,erec,trec;
	Double_t amugev = 931.5016/1000.;
	Double_t Ex = 0.0;
    	
	pb = sqrt((12*amugev*2*Ebeam) + pow(Ebeam,2));
		
	p3l = sqrt(pow((Ep3+1.007825*amugev),2) - pow((1.007825*amugev+Ex),2));
	p4l = sqrt(pow((Ep4+1.007825*amugev),2) - pow((1.007825*amugev+Ex),2));
	
	p3x = p3l*cos(Phi3)*sin(Th3);
	p3y = p3l*sin(Phi3)*sin(Th3);
	p3z = p3l*cos(Th3);
		
	p4x = p4l*cos(Phi4)*sin(Th4);
	p4y = p4l*sin(Phi4)*sin(Th4);
	p4z = p4l*cos(Th4);
	
	prec = sqrt(pow((p3x+p4x),2) + pow((p3y+p4y),2) + pow((pb-p3z-p4z),2));
	
	erec = sqrt(pow(prec,2) + pow(11.0093054*amugev,2));
	
	trec = erec - 11.0093054*amugev;
		
	return trec;
	
}

Double_t GetCMEnergy(Double_t theta, Double_t energy, Double_t beta){
	//
	// Calculating the CM energy from the lab energy and the polar angle
	//
	Double_t gamma = 1/sqrt(1-beta*beta);
	//Lorenzt boost correction
	//E' = gamma E + beta gamma P|| = gamma E + beta gamma P cos(theta) 
	//In photons E=P
    
	Double_t energyCorrect = gamma*energy - beta*gamma*energy*cos(theta);
    
	return energyCorrect;
}
