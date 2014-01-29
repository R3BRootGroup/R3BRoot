//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Analysis results
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 01/09/2010
//         Comments:
//			Checks the primary, crystalHits and caloHits characteristics.
//			User configurable for additional studies.
//	
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l checkResults_Protons_50MeV.C
//                         
//     BUT FIRST, select in the //SETTINGS section the simulation features 
//
//	(the macro will plot and text information as a function of these settings)
//
// 
//Coomments: Used to study the ASCII outfile from the macro of E.Nacher and P.Prietas that takes the momentum of the outfile of L.Chulkov (GSI, 2001) program that simulate kinematics of a quasi elastic scattering of high-energy protons on a cluster inside a nuclei. This ASCII file is the input of R3BRoot and FairAsciiGenerator is activated to read the ascii file. 
//
//In this case the macro is prepared for protons of the p2p reactions in the calorimeter with the phoswich-endcap IEM-CSIC Madrid. Resolutions of the crystals are  added. Some coincidences of protons from the p,2p reactions are implemented. CLF717_Geometry_PhoswichEndcap_*.geo is the endcap file to be used (José Sánchez del Río Sáez ) 05/12/2012
//Some changes are needed in the other macros as the README File comments
// Contact for the changes: josesrs@gmail.com && jose.sanchez@csic.es
//
//  -------------------------------------------------------------------------
//----------------------------------

void checkResults_Protons_p2p_50MeV() {
	char title0[250];
	char title1[250];



// C,C++ Libraries
// #include <string>
// #include <stdio.h>
// #include <stdlib.h>
// #include <iostream>
// #include <fstream> 
// #include <sstream>
// #include <math.h>
// #include <cstdlib>
// #include <ctime>
// #include <iomanip>


using std::cout;
using std::cerr;
using std::endl;
using std::ios;

	
	ofstream outFile3("SomeCheckData3.dat", ios::out | ios::app | ios::binary);
	ofstream outFile2("SomeCheckData2.dat", ios::out | ios::app | ios::binary);
	ofstream outFile("SomeCheckData.dat", ios::out | ios::app | ios::binary);

	
	
	
	
	//SETTINGS 
	char calVersion[50] = "7.09+7.17";       //Calorimeter version (5.0, 7.05, 7.07, 7.09, 7.17, 7.07+7.17,7.09+7.17, 8.??)

	//char calVersion[50] = "7.07+7.17";       //Calorimeter version (5.0, 7.05, 7.07, 7.09, 7.17, 7.07+7.17,7.09+7.17, 8.??)
	//Double_t Eproj = 0.500;              //Gamma Energy in projectile frame in MeV 
	
	Double_t Eproj = 0.5; 

	//Double_t Eproj = 10.0;              //Gamma Energy in projectile frame i
	Int_t totalEvents = 8000;        //Events
	Int_t multiplicity = 2;           //Multiplicity (particles per event)
	
	Double_t threshold=0.050;		  //Threshold in MeV
	Int_t ExpRes=5;			          //Exp. Resol in MeV  (%?)

	//FOR THE HISTOGRAMS AND PLOTS:
	//Double_t maxE = 6;               //Maximum energy in MeV in the histos

	//Double_t maxE = 400;//30.0;               //Maximum energy in MeV in the histos

	Double_t maxE =  Eproj*1000;


	//TString title0 = "califaAna.root";
	//TString title1 =    "r3bsim.root";
	sprintf(title0,"%s","./califaAna.root");  
 	sprintf(title1,"%s","./r3bsim.root");  	
	//sprintf(title0,"%s","/home/jose/r3broot_sept2012/macros/r3b/califa/califaAna_Gamma_0_5MeV.root");  
 	//sprintf(title1,"%s","/home/jose/r3broot_sept2012/macros/r3b/califa/r3bsim_Gamma_0_5MeV.root"); 

	TFile *file0 = TFile::Open(title0);
	TFile *file1 = TFile::Open(title1);
	
	//Double_t beta=0.82;

	Double_t beta=0.85;

	//Double_t beta=0.0;

	
		//END OF THE SETTING AREA
	
	Bool_t BARREL= kFALSE;
	Bool_t ENDCAP= kFALSE;
	Double_t minThetaBarrel=0 , maxThetaBarrel=0;
	Double_t minThetaEndCap=0 , maxThetaEndCap=0;

	if(calVersion=="5.0"){
		cout << "Warning: Calorimeter version 5.0 is not supported in this macro! "<< endl;
	}
	if( !strcmp(calVersion,"7.05") ){
		cout << "Using CALIFA version 7.05 "<< endl;
		minThetaBarrel= 32.9;    //Angular coverture of BARREL 7.05
		maxThetaBarrel= 134.7;   //Angular coverture of BARREL 7.05
		BARREL=kTRUE;
	}
	else if(!strcmp(calVersion,"7.07")){
		cout << "Using CALIFA version 7.07 "<< endl;
		minThetaBarrel= 32.4;  //Angular coverture of BARREL 7.07
		maxThetaBarrel= 134.2; //Angular coverture of BARREL 7.07
		BARREL=kTRUE;
	}
	else if(!strcmp(calVersion,"7.09")){
		cout << "Using CALIFA version 7.09 "<< endl;
		minThetaBarrel= 32.4;  //Angular coverture of BARREL 7.09
		maxThetaBarrel= 155.; //Angular coverture of BARREL 7.09
		BARREL=kTRUE;
	}
	else if(!strcmp(calVersion,"7.17")){
		cout << "Using CALIFA version 7.17 "<< endl;
		//minThetaEndCap= 6;  //Angular coverture of ENDCAP 7.17 adapted to barrel CLF811:   CLF717_Geometry_PhoswichEndcap_2.geo
		//maxThetaEndCap= 43; //Angular coverture of ENDCAP 7.17  adapted to barrel CLF811: CLF717_Geometry_PhoswichEndcap_2.geo

		minThetaEndCap=7.00;  //Angular coverture of ENDCAP 7.17 adapted to barrel CLF717: CLF717_Geometry_PhoswichEndcap_1.geo
		maxThetaEndCap= 35.0; //Angular coverture of ENDCAP 7.17 adapted to barrel CLF717: CLF717_Geometry_PhoswichEndcap_1.geo

		ENDCAP=kTRUE;
	}
	else if(!strcmp(calVersion,"7.07+7.17")){
		cout << "Using CALIFA version 7.07+7.17 "<< endl;
		//Obs: We could use as well CLF717_Geometry_PhoswichEndcap_4.geo for the version CLF811 and a pohoswich endcap (5%), but with 43.2 instead of 35)

		minThetaBarrel= 35;  //Angular coverture of BARREL 7.07.      CLF717_Geometry_PhoswichEndcap_3.geo
		maxThetaBarrel= 135; //Angular coverture of BARREL 7.07

		minThetaEndCap= 7;  //Angular coverture of ENDCAP 7.17.     CLF717_Geometry_PhoswichEndcap_3.geo
		maxThetaEndCap= 35; //Angular coverture of ENDCAP 7.17*/    CLF717_Geometry_PhoswichEndcap_3.geo

		
		//minThetaBarrel=32.4;  //Angular coverture barrel   CLF709
		//maxThetaBarrel= 135.2; //Angular coverture barrel    CLF709
		


		BARREL=kTRUE;
		ENDCAP=kTRUE;
	}
	else if(!strcmp(calVersion,"7.09+7.17")){
		cout << "Using CALIFA version 7.09+7.17 "<< endl;

		//outFileee<< "Using CALIFA version 7.09+7.17 ")<<endl;

		//minThetaBarrel= 32.10;  //Angular coverture of BARREL 7.09. (As well for CLF717_Geometry_PhoswichEndcap_5.geo)
		//maxThetaBarrel= 150.; //Angular coverture of BARREL 7.09

		minThetaBarrel= /*42.61*/43;  //Angular coverture of BARREL 7.09, adapted to CLF717_Geometry_PhoswichEndcap_4_2.geo
		maxThetaBarrel= 150.; //Angular coverture of BARREL 7.09

		//minThetaEndCap= 9.59;  //Angular coverture of ENDCAP 7.17
		//maxThetaEndCap= 32.10; //Angular coverture of ENDCAP 7.17
		//minThetaEndCap= 7;  //Angular coverture of ENDCAP 7.17
		//maxThetaEndCap= 32.10; //Angular coverture of ENDCAP 7.17    (As well for CLF717_Geometry_PhoswichEndcap_5.geo)

		minThetaEndCap= 6;  //Angular coverture of ENDCAP 7.17
		maxThetaEndCap= /*42.61*/43; //Angular coverture of ENDCAP 7.17    (As well for CLF717_Geometry_PhoswichEndcap_4_2.geo)

		BARREL=kTRUE;		
		ENDCAP=kTRUE;
	}
	else if(!strcmp(calVersion,"8.00")){
		cout << "Using CALIFA version 8.00 "<< endl;
		minThetaBarrel= 0.;  //Angular coverture of CALIFA 8.0
		maxThetaBarrel= 0.; //Angular coverture of CALIFA 8.0
		BARREL=kTRUE;
		ENDCAP=kTRUE;
	}
	else cout << "Error: Calorimeter version not correctly defined! "<< endl;
		
	// ----    Debug option   -------------------------------------------------
	gDebug = 0;
	// ------------------------------------------------------------------------
	
	
	gROOT->SetStyle("Default");
	//gStyle->SetOptTitle(0);
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);
	
	
	//HISTOGRAMS DEFINITION
	TH1F* h1_T = new TH1F("h1_T","Primary PDG Code",2300,0,2300);
	TH1F* h2_T = new TH1F("h2_T","Primary Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_T = new TH1F("h3_T","Primary Theta",200,0,3.2);
	TH1F* h4_T = new TH1F("h4_T","Primary Phi",200,-3.2,3.2);
	TH1F* h1_Cry = new TH1F("h1_Cry","Crystal ID",5850,0,5850);
	TH1F* h1_Cry_count = new TH1F("h1_Cry_count","Crystal ID_count",2,0,3840);
	TH1F* h2_Cry = new TH1F("h2_Cry","Crystal Energy (MeV)",200,0,3*maxE); //Change this maximum energy

	//TH2F* h2_Cry_BarrelVsEndcap  = new TH2F("h2_Cry_BarrelVsEndcap ","EnergyCryBarrel vs EnergyCryEndcap",2000,0,2*maxE,2000,0,2*maxE);

	TH1F* h3_Cry = new TH1F("h3_Cry","Crystal Type",32,0,32);
	TH1F* h4_Cry = new TH1F("h4_Cry","Crystal Copy",514,0,514);
	TH1F* h1_Cal = new TH1F("h1_Cal","Nb of Crystals in hit",20,0,20);
	TH1F* h2_Cal = new TH1F("h2_Cal","Hit Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_Cal = new TH1F("h3_Cal","Hit Theta",200,0,3.2);


	//TH1F* h2_Cal_Coincidence = new TH1F("h2_Cal_Coincidence","Hit Energy (MeV)",200,0,3*maxE); //Change this maximum energy

	//TH2F* h2_Cal_BarrelVsEndcap = new TH2F("h2_Cal_BarrelVsEndcap","EnergyCalBarrel vs EnergyCalEndcap",2000,0,2*maxE,2000,0,2*maxE);

	TH1F* h2_Cal_P1EndcapP2Barr = new TH1F("h2_Cal_P1EndcapP2Barr","Hit Energy (MeV)",200,0,3*maxE); //Change this maximum energy

	TH1F* h2_Cal_barrel_P2Barr = new TH1F("h2_Cal_barrel_P2Barr","Hit Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h2_Cal_endcap_P1Endcap = new TH1F("h2_Cal_endcap_P1Endcap","Hit Energy (MeV)",200,0,3*maxE); //Change this maximum energy

	TH1F* h2_Cal_P1Endcap = new TH1F("h2_Cal_P1Endcap","Hit Energy (MeV)P1Endcap",200,0,3*maxE); //Change this maximum energy
	
	TH1F* h2_Cal_P2Barr = new TH1F("h2_Cal_P2Barr","Hit Energy (MeV) P2Barr",200,0,3*maxE); //Change this maximum energy

	
	TH1F* h2_Cal_P1 = new TH1F("h2_Cal_P1","Hit Energy (MeV)P1",200,0,3*maxE); //Change this maximum energy

	TH1F* h2_Cal_P2 = new TH1F("h2_Cal_P2","Hit Energy (MeV) P2Barr",200,0,3*maxE); //Change this maximum energy

	
	
	


	TH2F* h2_Cal_EnergyParSumEnergySum_endcap =new TH2F("h2_Cal_EnergyParSumEnergySum_endcap ","EnergyCalParSum vs EnergyCalEndcap",2000,0,3*maxE,2000,0,3*maxE);


	TH2F* h2_Cal_EnergyBarrelEnergyEndcap=new TH2F("h2_Cal_EnergyBarrelEnergyEndcap ","EnergyCalBarrel vs EnergyCalEndcap",2000,0,3*maxE,2000,0,3*maxE);

	TH2F* h2_Cal_EnergyBarrelSumEnergyEndcapSum =new TH2F("h2_Cal_EnergyBarrelSumEnergyEndcapSum","EnergyCalBarrelSum vs EnergyCalEndcapSum",2000,0,3*maxE,2000,0,3*maxE);



	TH2F* h2_Cal_EnergyParSumEnergySum_endcap_CoincP1EndcapP2Barr =new TH2F("h2_Cal_EnergyParSumEnergySum_endcap_CoincP1EndcapP2Barr ","EnergyCalParSum vs EnergyCalEndcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);


	TH2F* h2_Cal_EnergyBarrelEnergyEndcap_CoincP1EndcapP2Barr=new TH2F("h2_Cal_EnergyBarrelEnergyEndcap_CoincP1EndcapP2Barr ","EnergyCalBarrel vs EnergyCalEndcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);

	TH2F* h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP1EndcapP2Barr =new TH2F("h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP1EndcapP2Barr","EnergyCalBarrelSum vs EnergyCalEndcapSum",2000,0,3*maxE,2000,0,3*maxE);


	TH2F* h3_Cal_Theta1VsTheta2=new TH2F("h3_Cal_Theta1VsTheta2","Theta1 vs Theta2",400,0,3.2,400,0,3.2);
	TH2F* h3_Cal_Theta1VsTheta2_CoincP1EndcapP2Barr=new TH2F("h3_Cal_Theta1VsTheta2_CoincP1EndcapP2Barr","Theta1 vs Theta2_CoincP1EndcapP2Barr",400,0,3.2,400,0,3.2);


	TH1F* h3_Cal_Theta_R = new TH1F("h3_Cal_Theta_R","Hit Theta",400,0,3.2);






	//TH1F* h3_Cal = new TH1F("h3_Cal","Hit Theta",400,0,3.2);

	TH1F* h4_Cal = new TH1F("h4_Cal","Hit Phi",200,-3.2,3.2);
	TH1F* h1_TMul = new TH1F("h1_TMul","Multiplicities",20,0,20);
	TH1F* h1_CryMul = new TH1F("h1_CryMul","Multiplicities",20,0,20);
	TH1F* h1_CalMul = new TH1F("h1_CalMul","Multiplicities",20,0,20);
	TH1F* h2_CC = new TH1F("h2_CC","Reconstructed (Lorenzt corrected) Primary Energy (MeV)",2000,0,maxE); //Change this maximum energy
	
	
	TH1F* h2_CC_P1EndcapP2Barr = new TH1F("h2_CC_P1EndcapP2Barr","Reconstructed (Lorenzt corrected) Primary Energy (MeV)_P1EndcapP2Barr",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_CC_barrel_P2Barr = new TH1F("h2_CC_barrel_P2Barr","Reconstructed (Lorenzt corrected) Primary Energy Barrel (MeV)_P2Barr",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_CC_endcap_P1Endcap = new TH1F("h2_CC_endcap_P1Endcap","Reconstructed (Lorenzt corrected) Primary Energy Endcap (MeV)_P1Endcap",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_CC_P1Endcap = new TH1F("h2_CC_P1Endcap","Reconstructed (Lorenzt corrected) Primary Energy (MeV)_P1Endcap",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_CC_P2Barr = new TH1F("h2_CC_P2Barr","Reconstructed (Lorenzt corrected) Primary Energy (MeV)_P2Barr",2000,0,maxE); //Change this maximum energy

	TH1F* h2_CC_P1 = new TH1F("h2_CC_P1","Reconstructed (Lorenzt corrected) Primary Energy (MeV)_P1",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_CC_P2 = new TH1F("h2_CC_P2","Reconstructed (Lorenzt corrected) Primary Energy (MeV)_P2",2000,0,3*maxE); //Change this maximum energy
	
	
	
	
	TH1F* h2_CC_barrel_P1Endcap = new TH1F("h2_CC_barrel_P1Endcap","Reconstructed (Lorenzt corrected) Primary Energy barrel (MeV)_P1Endcap",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_Cal_barrel_P1Endcap = new TH1F("h2_Cal_barrel_P1Endcap","Reconstructed (Lorenzt corrected) Primary Energy barrel (MeV) P1Endcap",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_CC_endcap_P1Endcap = new TH1F("h2_CC_endcap_P1Endcap","Reconstructed (Lorenzt corrected) Primary Energy endcap (MeV)_P1Endcap",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_Cal_endcap_P1Endcap = new TH1F("h2_CC_P2","Reconstructed (Lorenzt corrected) Primary Energy endcap (MeV) P1Endcap",2000,0,maxE); //Change this maximum energy
	
	
	
	TH1F* h2_CC_endcap_P2Barr = new TH1F("h2_CC_endcap_P2Barr","Reconstructed (Lorenzt corrected) Primary Energy endcap (MeV)_P2Barrel",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_Cal_endcap_P2Barr = new TH1F("h2_Cal_endcap_P2Barr","Reconstructed (Lorenzt corrected) Primary Energy endcap (MeV) P2Barrel",2000,0,3*maxE); //Change this maximum energy
	
	
	
	
	
	
	TH1F* h2_CC_barrel_P1EndcapP2Barr = new TH1F("h2_CC_barrel_P1EndcapP2Barr","Reconstructed (Lorenzt corrected) Primary Energy endcap (MeV)_P1EndcapP2Barr",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_Cal_barrel_P1EndcapP2Barr = new TH1F("h2_Cal_barrel_P1EndcapP2Barr","Reconstructed (Lorenzt corrected) Primary Energy endcap (MeV) P1EndcapP2Barr",2000,0,3*maxE); //Change this maximum energy
	
	
	TH1F* h2_CC_endcap_P1EndcapP2Barr = new TH1F("h2_CC_endcap_P1EndcapP2Barr","Reconstructed (Lorenzt corrected) Primary Energy endcap (MeV)_P1EndcapP2Barr",2000,0,3*maxE); //Change this maximum energy

	TH1F* h2_Cal_endcap_P1EndcapP2Barr = new TH1F("h2_Cal_endcap_P1EndcapP2Barr","Reconstructed (Lorenzt corrected) Primary Energy endcap (MeV) P1EndcapP2Barr",2000,0,3*maxE); //Change this maximum energy
	
	
	

	TH1F* h3_CC = new TH1F("h3_CC","Theta residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);
	TH1F* h4_CC = new TH1F("h4_CC","Phi residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);
	TH2F* h1_CC2 = new TH2F("h1_CC2","Multiplicity vs. polar angle",200,0,3.2,36,0,12);
	TH2F* h2_CC2 = new TH2F("h2_CC2","Energy reconstructed vs. polar angle",50,0,3.2,400,0,maxE); //Change this maximum energy
	TH2F* h3_CC2 = new TH2F("h3_CC2","Theta residuals vs. polar angle",100,0,3.2,100,-0.25,0.25);
	TH2F* h4_CC2 = new TH2F("h4_CC2","Phi residuals vs. polar angle",100,0,3.2,100,-0.25,0.25);

	TH1F* h1_EF = new TH1F("h1_EF","Efficiency vs. polar angle",200,0,3.2);
	//TH1F* h2_EF = new TH1F("h2_EF","Eventos vs. polar angle",1000,0,3.2);

//BARREL
	TH1F* h1_Cry_barrel = new TH1F("h1_Cry_barrel","Crystal ID",5850,0,5850);
	TH1F* h1_Cry_count_barrel = new TH1F("h1_Cry_count_barrel","Crystal ID_count",2,0,3840);
	TH1F* h2_Cry_barrel = new TH1F("h2_Cry_barrel","Crystal Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_Cry_barrel = new TH1F("h3_Cry_barrel","Crystal Type",32,0,32);
	TH1F* h4_Cry_barrel = new TH1F("h4_Cry_barrel","Crystal Copy",514,0,514);
	TH1F* h1_Cal_barrel = new TH1F("h1_Cal_barrel","Nb of Crystals in hit",20,0,20);
	TH1F* h2_Cal_barrel = new TH1F("h2_Cal_barrel","Hit Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_Cal_barrel = new TH1F("h3_Cal_barrel","Hit Theta",200,0,3.2);
	TH1F* h4_Cal_barrel = new TH1F("h4_Cal_barrel","Hit Phi",200,-3.2,3.2);
	TH1F* h1_TMul_barrel = new TH1F("h1_TMul_barrel","Multiplicities",20,0,20);
	TH1F* h1_CryMul_barrel = new TH1F("h1_CryMul_barrel","Multiplicities",20,0,20);
	TH1F* h1_CalMul_barrel = new TH1F("h1_CalMul_barrel","Multiplicities",20,0,20);
	TH1F* h2_CC_barrel = new TH1F("h2_CC_barrel","Reconstructed (Lorenzt corrected) Primary Energy (MeV)",2000,0,maxE); //Change this maximum energy
	TH1F* h3_CC_barrel = new TH1F("h3_CC_barrel","Theta residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);
	TH1F* h4_CC_barrel = new TH1F("h4_CC_barrel","Phi residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);
	//TH1F* h1_EF_barrel = new TH1F("h1_EF_barrel","Efficiency vs. polar angle",200,0,3.2);

	
	//ENDCAP
	TH1F* h1_Cry_endcap = new TH1F("h1_Cry_endcap","Crystal ID",5850,0,5850);
	TH1F* h1_Cry_count_endcap = new TH1F("h1_Cry_count_endcap","Crystal ID_count",2,0,3840);
	TH1F* h2_Cry_endcap = new TH1F("h2_Cry_endcap","Crystal Energy (MeV)",200,0,3*maxE); //Change this maximum energy

	TH1F* h3_Cry_endcap = new TH1F("h3_Cry_endcap","Crystal Type",32,0,32);
	TH1F* h4_Cry_endcap = new TH1F("h4_Cry_endcap","Crystal Copy",514,0,514);
	TH1F* h1_Cal_endcap = new TH1F("h1_Cal_endcap","Nb of Crystals in hit",20,0,20);
	TH1F* h2_Cal_endcap = new TH1F("h2_Cal_endcap","Hit Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	//TH1F* h3_Cal_endcap = new TH1F("h3_Cal_endcap","Hit Theta",200,0,3.2);

	TH1F* h3_Cal_endcap = new TH1F("h3_Cal_endcap","Hit Theta",400,0,3.2);

	TH1F* h4_Cal_endcap = new TH1F("h4_Cal_endcap","Hit Phi",200,-3.2,3.2);
	TH1F* h1_TMul_endcap = new TH1F("h1_TMul_endcap","Multiplicities",20,0,20);
	TH1F* h1_CryMul_endcap = new TH1F("h1_CryMul_endcap","Multiplicities",20,0,20);
	TH1F* h1_CalMul_endcap = new TH1F("h1_CalMul_endcap","Multiplicities",20,0,20);
	TH1F* h2_CC_endcap = new TH1F("h2_CC_endcap","Reconstructed (Lorenzt corrected) Primary Energy (MeV)",2000,0,maxE); //Change this maximum energy
	TH1F* h3_CC_endcap = new TH1F("h3_CC_endcap","Theta residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);
	TH1F* h4_CC_endcap = new TH1F("h4_CC_endcap","Phi residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);


	//TH2F* h5_Cry_EnergyImparSumEnergyParSum_endcap = new TH2F("h5_Cry_EnergyImParSumEnergyParSum_endcap","Energy1Sum vs Energy2Sum",2000,0,maxE,2000,0,maxE);
	
	//TH2F* h5_Cry_EnergyImparSumEnergySum_endcap = new TH2F("h5_Cry_EnergyImparSumEnergySum_endcap","EnergyTotSum vs Energy1Sum",2000,0,maxE,2000,0,maxE);
	
	TH2F* h5_Cry_EnergyParSumEnergySum_endcap  = new TH2F("h5_Cry_EnergyParSumEnergySum_endcap ","EnergyCryParSum vs EnergyCryEndcap",2000,0,3*maxE,2000,0,3*maxE);


	TH2F* h5_Cry_EnergyBarrelEnergyEndcap  = new TH2F("h5_Cry_EnergyBarrelEnergyEndcap ","EnergyCryBarrel vs EnergyCryEndcap",2000,0,3*maxE,2000,0,3*maxE);



	TH2F* h5_Cry_EnergyBarrelSumEnergyEndcapSum  = new TH2F("h5_Cry_EnergyBarrelSumEnergyEndcapSum ","EnergyCryBarrelSum vs EnergyCryEndcapSum",2000,0,3*maxE,2000,0,3*maxE);




	TH2F* h5_Cry_EnergyParSumEnergySum_endcap_CoincP1EndcapP2Barr  = new TH2F("h5_Cry_EnergyParSumEnergySum_endcap_CoincP1EndcapP2Barr ","EnergyCryParSum vs EnergyCryEndcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);


	TH2F* h5_Cry_EnergyBarrelEnergyEndcap_CoincP1EndcapP2Barr  = new TH2F("h5_Cry_EnergyBarrelEnergyEndcap_CoincP1EndcapP2Barr ","EnergyCryBarrel vs EnergyCryEndcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);



	TH2F* h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP1EndcapP2Barr  = new TH2F("h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP1EndcapP2Barr ","EnergyCryBarrelSum vs EnergyCryEndcapSum_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);





	TH2F* h5_Cry_EnergyParSumEnergySum_endcap_CoincP1Endcap  = new TH2F("h5_Cry_EnergyParSumEnergySum_endcap_CoincP1Endcap ","EnergyCryParSum vs EnergyCryEndcap_CoincP1Endcap",2000,0,3*maxE,2000,0,3*maxE);


	TH2F* h5_Cry_EnergyBarrelEnergyEndcap_CoincP1Endcap  = new TH2F("h5_Cry_EnergyBarrelEnergyEndcap_CoincP1Endcap ","EnergyCryBarrel vs EnergyCryEndcap_CoincP1Endcap",2000,0,3*maxE,2000,0,3*maxE);



	TH2F* h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP1Endcap  = new TH2F("h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP1Endcap ","EnergyCryBarrelSum vs EnergyCryEndcapSum_CoincP1Endcap",2000,0,3*maxE,2000,0,3*maxE);





	TH2F* h5_Cry_EnergyParSumEnergySum_endcap_CoincP2Barr  = new TH2F("h5_Cry_EnergyParSumEnergySum_endcap_CoincP2Barr ","EnergyCryParSum vs EnergyCryEndcap_CoincP2Barr",2000,0,3*maxE,2000,0,3*maxE);


	TH2F* h5_Cry_EnergyBarrelEnergyEndcap_CoincP2Barr  = new TH2F("h5_Cry_EnergyBarrelEnergyEndcap_CoincP2Barr ","EnergyCryBarrel vs EnergyCryEndcap_CoincP2Barr",2000,0,3*maxE,2000,0,3*maxE);



	TH2F* h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP2Barr  = new TH2F("h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP2Barr ","EnergyCryBarrelSum vs EnergyCryEndcapSum_CoincP2Barr",2000,0,3*maxE,2000,0,3*maxE);











	TH2F* h2_CC_EnergyParReconstrEnergyReconstruct_endcap =new TH2F("h2_CC_EnergyParReconstrEnergyReconstruct_endcap","ReconstructedLorentzCorrectedParEndcap&&Endcap",2000,0,3*maxE,2000,0,3*maxE);


	TH2F* h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap =new TH2F("h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap","ReconstructedLorentzCorrectedParSumEndcap&&SumEndcap",2000,0,3*maxE,2000,0,3*maxE);



	TH2F* h2_CC_barrelReconstrVsEndcapReconstr =  new TH2F("h2_CC_barrelReconstrVsEndcapReconstr","ReconstructedLorentzCorrectedEndcap&&Barrel",2000,0,3*maxE,2000,0,3*maxE);//Change this maximum energy


	TH2F* h2_CC_barrelReconstrSumVsEndcapReconstrSum =  new TH2F("h2_CC_barrelReconstrSumVsEndcapReconstrSum","ReconstructedLorentzCorrectedSumEndcap&&Barrel",2000,0,3*maxE,2000,0,3*maxE);//Change this maximum energy





	TH2F* h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP1EndcapP2Barr =new TH2F("h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP1EndcapP2Barr","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);


	TH2F* h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP1EndcapP2Barr =new TH2F("h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP1EndcapP2Barr","ReconstructedLorentzCorrectedParSumEndcap&&SumEndcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);



	TH2F* h2_CC_barrelReconstrVsEndcapReconstr_CoincP1EndcapP2Barr =  new TH2F("h2_CC_barrelReconstrVsEndcapReconstr_CoincP1EndcapP2Barr","ReconstructedLorentzCorrectedEndcap&&Barrel_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);//Change this maximum energy


	TH2F* h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP1EndcapP2Barr =  new TH2F("h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP1EndcapP2Barr","ReconstructedLorentzCorrectedSumEndcap&&Barrel_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);//Change this maximum energy




	TH2F*	h2_Cal_EnergyParSumEnergySum_endcap_CoincP1Endcap  = new TH2F("h2_Cal_EnergyParSumEnergySum_endcap_CoincP1Endcap","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);
	TH2F*	h2_Cal_EnergyBarrelEnergyEndcap_CoincP1Endcap= new TH2F("h2_Cal_EnergyBarrelEnergyEndcap_CoincP1Endcap","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);
	TH2F*	h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP1Endcap= new TH2F("h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP1Endcap","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);


	TH2F*	h2_Cal_EnergyParSumEnergySum_endcap_CoincP2Barr = new TH2F("h2_Cal_EnergyParSumEnergySum_endcap_CoincP2Barr","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP2Barr",2000,0,3*maxE,2000,0,3*maxE);
	TH2F*	h2_Cal_EnergyBarrelEnergyEndcap_CoincP2Barr= new TH2F("h2_Cal_EnergyBarrelEnergyEndcap_CoincP2Barr","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP2Barr",2000,0,3*maxE,2000,0,3*maxE);
	TH2F*	h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP2Barr= new TH2F("h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP2Barr","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP2Barr",2000,0,3*maxE,2000,0,3*maxE);



	TH2F*	h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP1Endcap= new TH2F("h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP1Endcap","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);
	TH2F*   h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP1Endcap= new TH2F("h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP1Endcap","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);
	TH2F*	h2_CC_barrelReconstrVsEndcapReconstr_CoincP1Endcap= new TH2F("h2_CC_barrelReconstrVsEndcapReconstr_CoincP1Endcap","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);
	TH2F*	h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP1Endcap= new TH2F("h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP1Endcap","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP1EndcapP2Barr",2000,0,3*maxE,2000,0,3*maxE);
							


	TH2F*	h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP2Barr= new TH2F("h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP2Barr","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP2Barr",2000,0,3*maxE,2000,0,3*maxE);
	TH2F*	h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP2Barr= new TH2F("h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP2Barr","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP2Barr",2000,0,3*maxE,2000,0,3*maxE);
	TH2F*	h2_CC_barrelReconstrVsEndcapReconstr_CoincP2Barr= new TH2F("h2_CC_barrelReconstrVsEndcapReconstr_CoincP2Barr","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP2Barr",2000,0,3*maxE,2000,0,3*maxE);
	TH2F*	h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP2Barr= new TH2F("h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP2Barr","ReconstructedLorentzCorrectedParEndcap&&Endcap_CoincP2Barr",2000,0,3*maxE,2000,0,3*maxE);
			

	
	
	//TH3F* h5_Cry_EnergyParSumEnergySumNprotons_endcap=new TH3F("h5_Cry_EnergyParSumEnergySum_endcapNprotons","EnergyTotSumvsEnergy2SumvsNprotons",2000,0,maxE,2000,0,maxE,300,0,3000);



	//TH1F* h5_Cry_Events_endcap_2 = new TH1F("h5_Cry_Events_2","Crystal Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	

	//TH2F* h5_Cal_EnergyImparCalSumEnergyParCalSum_endcap = new TH2F("h5_Cal_EnergyImparCalSumEnergyParCalSum_endcap","EnergyParCalSum vs EnergyImpsrCalSum",2000,0,maxE,2000,0,maxE);
	
	//TH2F* h5_Cal_EnergyImparCalSumEnergyCalSum_endcap  = new TH2F("h5_Cal_EnergyImparCalSumEnergyCalSum_endcap ","EnergyImparCalSum vs EnergyCalSum",2000,0,maxE,2000,0,maxE);
	
	//TH2F* h5_Cal_EnergyParCalSumEnergyCalSum_endcap  = new TH2F("h5_Cal_EnergyParCalSumEnergyCalSum_endcap","EnergyParCalSum vs EnergyCalSum",2000,0,maxE,2000,0,maxE);

	//TH1F* h5_Cal_Events_endcap_2 = new TH1F("h5_Cal_Events_endcap_2","energyTotalCalSum (MeV)",200,0,3*maxE); 



	
	TH1F* h1_EF_endcap = new TH1F("h1_EF_endcap","Efficiency vs. polar angle",200,0,3.2);

	
	//Additional user defined histos
	TH2F* h1_User = new TH2F("h1_User","Double hits, ... where?",400,-2.5,2.5,400,-2.5,2.5);

	TTree* TCalo = (TTree*)file0->Get("cbmsim");
	TTree* TCrystal = (TTree*)file1->Get("cbmsim");
	
	//Crystal Hits (input)
	TClonesArray* crystalHitCA;  
	//R3BCaloCrystalHit** crystalHit;
	R3BCaloCrystalHitSim** crystalHit;

	//crystalHitCA = new TClonesArray("R3BCaloCrystalHit",5);
	crystalHitCA = new TClonesArray("R3BCaloCrystalHitSim",5);
	//TBranch *branchCrystalHit = TCrystal->GetBranch("CrystalHit");
	TBranch *branchCrystalHit = TCrystal->GetBranch("CrystalHitSim");
	branchCrystalHit->SetAddress(&crystalHitCA);

	//Calo Hits (output)
	TClonesArray* caloHitCA;  
	//R3BCaloHit** caloHit;
	R3BCaloHitSim** caloHit;
	//caloHitCA = new TClonesArray("R3BCaloHit",5);
	caloHitCA = new TClonesArray("R3BCaloHitSim",5);
	//TBranch *branchCaloHit = TCalo->GetBranch("CaloHit");
	TBranch *branchCaloHit = TCalo->GetBranch("CaloHitSim");
	branchCaloHit->SetAddress(&caloHitCA);
	
	//MCTrack(input)
	TClonesArray* MCTrackCA;  
	R3BMCTrack** track;
	MCTrackCA = new TClonesArray("R3BMCTrack",5);
	TBranch *branchMCTrack = TCrystal->GetBranch("MCTrack");
	branchMCTrack->SetAddress(&MCTrackCA);
	
	Long64_t nevents = TCrystal->GetEntries();
	if(nevents!= totalEvents * multiplicity) 
		cout << "WARNING: The number of events ("<< nevents 
			<<") in the tree is not totalEvents*multiplicity ("<< totalEvents<<"*"<<multiplicity<<") !!"<<endl;

	Int_t crystalHitsPerEvent=0;
	Int_t caloHitsPerEvent=0;
	Int_t MCtracksPerEvent=0;
	Int_t crystalHitsPerEvent_barrel=0;
	Int_t crystalHitsPerEvent_endcap=0;
	Int_t caloHitsPerEvent_barrel=0;
	Int_t caloHitsPerEvent_endcap=0;
	
	Int_t primary=0;	
	Int_t primary_barrel=0;
	Int_t primary_endcap=0;

	TVector3 momentum;
	TVector3 position; //mio


	Double_t corEnergy=0;
	Int_t tirame=0;


		//gamma
	Double_t fCrystalResolutionLaBr=2.9; //a 0.662 MeV
	Double_t fCrystalResolutionLaCl=3.8; //a 0.662 MeV
	Double_t fCrystalResolutionPhoswich=(fCrystalResolutionLaCl+fCrystalResolutionLaBr)/2;
	Double_t inputEnergy=0.662;
	Double_t KLaBr=fCrystalResolutionLaBr*sqrt(inputEnergy);
	Double_t KLaCl=fCrystalResolutionLaCl*sqrt(inputEnergy);
	Double_t KPhoswich=sqrt(KLaBr*KLaBr+KLaCl*KLaCl);

	Double_t resolutionCrystal = KPhoswich/(sqrt(inputEnergy));//no lo uso

		//Protons
		//Double_t ResolutionProts=0.02;

	Double_t ResolutionProts=0.0;

	int j,k=0;


	



		
	for(Int_t i=0;i<nevents;i++){
		
	
		 
	    if(i%1000 == 0) printf("Event:%i\n",i);


				
		crystalHitCA->Clear();
		caloHitCA->Clear();
		MCTrackCA->Clear();

		TCrystal->GetEvent(i);

		TCalo->GetEvent(i);
	
		crystalHitsPerEvent = crystalHitCA->GetEntries(); 
		caloHitsPerEvent = caloHitCA->GetEntries(); 
		MCtracksPerEvent = MCTrackCA->GetEntries();
		
		if(crystalHitsPerEvent>0) {
			//crystalHit = new R3BCaloCrystalHit*[crystalHitsPerEvent];
		crystalHit = new R3BCaloCrystalHitSim*[crystalHitsPerEvent];
			for(Int_t j=0;j<crystalHitsPerEvent;j++){
				//crystalHit[j] = new R3BCaloCrystalHit;
				crystalHit[j] = new R3BCaloCrystalHitSim;
				//crystalHit[j] = (R3BCaloCrystalHit*) crystalHitCA->At(j);      
				crystalHit[j] = (R3BCaloCrystalHitSim*) crystalHitCA->At(j);      
			}
		}
		if(caloHitsPerEvent>0) {
			//caloHit = new R3BCaloHit*[caloHitsPerEvent];
			caloHit = new R3BCaloHitSim*[caloHitsPerEvent];
			for(Int_t j=0;j<caloHitsPerEvent;j++){
				//caloHit[j] = new R3BCaloHit;
				caloHit[j] = new R3BCaloHitSim;
				//caloHit[j] = (R3BCaloHit*) caloHitCA->At(j);      
				caloHit[j] = (R3BCaloHitSim*) caloHitCA->At(j);
			}
		}		
		if(MCtracksPerEvent>0) {
			track = new R3BMCTrack*[MCtracksPerEvent];
			for(Int_t j=0;j<MCtracksPerEvent;j++){
				track[j] = new R3BMCTrack;
				track[j] = (R3BMCTrack*) MCTrackCA->At(j); 
			}
		}
		
		
	double EnergyCryParSum=0;  //variables para sumar energ\EDas por evento i
	double EnergyCryImparSum=0;
	double EnergyParCalSum=0;  //variables para sumar energ\EDas por evento i
	double EnergyImparCalSum=0;

	double energyTotalCalSum=0;  //variables para sumar energ\EDas por evento i
	double energyParCalSum=0;  //variables para sumar energ\EDas por evento i
	double energyImparCalSum=0;

	double EnergyCryPar=0;
	double EnergyCryImpar=0;
	double EnergyCalPar=0;
	double EnergyCalImpar=0;



	double EnergyBarrelCalReconstr=0;
	double EnergyEndcapCalReconstr=0;
	double EnergyEndcapCalReconstructSum=0;
	double	EnergyBarrelCalReconstructSum=0;

	double EnergyCryEndcapSum=0;
	double	EnergyCryBarrelSum=0;
	double EnergyCalEndcapSum=0;
	double	EnergyCalBarrelSum=0;

	double  EnergyCryBarrel=0;
	double  EnergyCryEndcap=0;

	double  EnergyCalBarrel=0;
	double  EnergyCalEndcap=0;



	double EnergyParCalReconstructSum= 0;
	double EnergyParCalReconstruct=0;




		
		//loop in crystal Hits
		for(Int_t h=0;h<crystalHitsPerEvent;h++)

		{
			h1_Cry->Fill(crystalHit[h]->GetCrystalId());
			if(crystalHit[h]->GetEnergy()*1000>threshold)
			h1_Cry_count->Fill(crystalHit[h]->GetCrystalId());
			h2_Cry->Fill(crystalHit[h]->GetEnergy()*1000);
			h3_Cry->Fill(crystalHit[h]->GetCrystalType());
			h4_Cry->Fill(crystalHit[h]->GetCrystalCopy());

			//outFile3<<"El evento en TCrystal es "<<TCrystal->GetEvent(i)<<" y da en el cristal: "<< crystalHit[h]->GetCrystalId()<<endl;



			if(BARREL && crystalHit[h]->GetCrystalId()<3000){  //no estoy metiendo la resolucion al barrel
				crystalHitsPerEvent_barrel++;
				h1_Cry_barrel->Fill(crystalHit[h]->GetCrystalId());



				if(crystalHit[h]->GetEnergy()*1000>threshold)
				h1_Cry_count_barrel->Fill(crystalHit[h]->GetCrystalId());
				 EnergyCryBarrel=crystalHit[h]->GetEnergy()*1000;
				 EnergyCryBarrelSum=EnergyCryBarrelSum + EnergyCryBarrel;
				h2_Cry_barrel->Fill(crystalHit[h]->GetEnergy()*1000);
				h3_Cry_barrel->Fill(crystalHit[h]->GetCrystalType());
				h4_Cry_barrel->Fill(crystalHit[h]->GetCrystalCopy());
			}


			
			int crystalId = crystalHit[h]->GetCrystalId();
			/*if(MCtracksPerEvent>0) {
				for(Int_t j=0;j<MCtracksPerEvent;j++){  //a\F1adido para mostrar los tracks de un evento				
					double xIdStart= track[j]->GetStartX();
					double yIdStart= track[j]->GetStartY();
					double zIdStart= track[j]->GetStartZ();

					double rId= TMath::Sqrt(xIdStart*xIdStart + yIdStart*yIdStart + zIdStart*zIdStart);				
					//outFilePrueba<<"Event  "<<i<<"    Track  "<<j<<"   Rho   "<<rId<<"   Theta   "<<endl;
				}  
			}//aqui acabamos el track*/
			
			//outFilePrueba<<"el Disparo i "<<i<<"  da al cristal  "<<crystalId<<endl;
			if(ENDCAP && crystalId>3000 /*&& crystalId%2 == 0*/)
			{	

			Double_t Energy=crystalHit[h]->GetEnergy()*1000;
		 
			Double_t randomIs = gRandom->Gaus(0,Energy*ResolutionProts);


				crystalHitsPerEvent_endcap++;
				h1_Cry_endcap->Fill(crystalHit[h]->GetCrystalId());
				if(crystalHit[h]->GetEnergy()*1000+randomIs>threshold)
				h1_Cry_count_endcap->Fill(crystalHit[h]->GetCrystalId());
				h2_Cry_endcap->Fill(crystalHit[h]->GetEnergy()*1000+randomIs);

				// energyCryEndcap=crystalHit[h]->GetEnergy()*1000+randomIs;

				h3_Cry_endcap->Fill(crystalHit[h]->GetCrystalType());
				h4_Cry_endcap->Fill(crystalHit[h]->GetCrystalCopy());//}//llave ultima mia;
				
					
				if((crystalHit[h]->GetEnergy()*1000+randomIs)>threshold)
				{	
					 EnergyCryEndcap=crystalHit[h]->GetEnergy()*1000+randomIs;
					
					EnergyCryEndcapSum=EnergyCryEndcapSum + EnergyCryEndcap;

					//energyTotalEndcapSum=energyTotalSum;
				
					if (crystalId%2 == 0)
					{		
						EnergyCryPar=crystalHit[h]->GetEnergy()*1000+randomIs;						
						
						EnergyCryParSum= EnergyCryParSum+ EnergyCryPar;						
						
						
					} 
					else // Impares
					{	
						EnergyCryImpar=crystalHit[h]->GetEnergy()*1000+randomIs;						
						
						EnergyCryImparSum=EnergyCryImparSum+ EnergyCryImpar;							
						int crystalIdImpar = crystalHit[h]->GetCrystalId();

						
					
					}//aqui acaban cristales impares
				
					
					
					
				} //aqui acaba el thershold
			} //acaba el endcap

		//outFile3<<"el  evento i de crystal hit, dentro de h, es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<endl;

				if(track[h]->GetMotherId()<0)
			{

				//outFile3<<"el  evento i de crystal hit, dentro de h, es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<"  y la particula es   "<<track[h]->GetPdgCode()<<endl;
		
					//h2_Cry_BarrelVsEndcap->Fill(energyCryBarrel,energyCryEndcap); solo energias del primer impacto

					h5_Cry_EnergyParSumEnergySum_endcap ->Fill(EnergyCryEndcapSum,EnergyCryParSum);
		
					h5_Cry_EnergyBarrelEnergyEndcap->Fill(EnergyCryBarrel,EnergyCryEndcap);

					h5_Cry_EnergyBarrelSumEnergyEndcapSum ->Fill(EnergyCryBarrelSum,EnergyCryEndcapSum);
			}



//Condicion primer proton da en el endcap, segundo proton da en el barrel

					if(track[h]->GetMotherId()<0)
					{
						if (h==1 && ENDCAP && crystalId>3000 || h==0 && BARREL && crystalId<3000) 
						{

				//outFile3<<"el  evento i de crystal hit, dentro de h, es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<"  y la particula es   "<<track[h]->GetPdgCode()<<endl;
		
					//h2_Cry_BarrelVsEndcap->Fill(energyCryBarrel,energyCryEndcap); solo energias del primer impacto

							h5_Cry_EnergyParSumEnergySum_endcap_CoincP1EndcapP2Barr->Fill(EnergyCryEndcapSum,EnergyCryParSum);
		
							h5_Cry_EnergyBarrelEnergyEndcap_CoincP1EndcapP2Barr->Fill(EnergyCryBarrel,EnergyCryEndcap);

						h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP1EndcapP2Barr->Fill(EnergyCryBarrelSum,EnergyCryEndcapSum);
						}
						
						
						
						if ((h==1 && ENDCAP && crystalId>3000)|| (h==0 && crystalId<6000))
						{

				//outFile3<<"el  evento i de crystal hit, dentro de h, es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<"  y la particula es   "<<track[h]->GetPdgCode()<<endl;
		
					//h2_Cry_BarrelVsEndcap->Fill(energyCryBarrel,energyCryEndcap); solo energias del primer impacto

							h5_Cry_EnergyParSumEnergySum_endcap_CoincP1Endcap->Fill(EnergyCryEndcapSum,EnergyCryParSum);
		
							h5_Cry_EnergyBarrelEnergyEndcap_CoincP1Endcap->Fill(EnergyCryBarrel,EnergyCryEndcap);

						h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP1Endcap->Fill(EnergyCryBarrelSum,EnergyCryEndcapSum);
						}
						
						if ((h==0 &&BARREL && crystalId<3000 || h==1 && crystalId<6000))
						{

				//outFile3<<"el  evento i de crystal hit, dentro de h, es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<"  y la particula es   "<<track[h]->GetPdgCode()<<endl;
		
					//h2_Cry_BarrelVsEndcap->Fill(energyCryBarrel,energyCryEndcap); solo energias del primer impacto

							h5_Cry_EnergyParSumEnergySum_endcap_CoincP2Barr->Fill(EnergyCryEndcapSum,EnergyCryParSum);
		
							h5_Cry_EnergyBarrelEnergyEndcap_CoincP2Barr->Fill(EnergyCryBarrel,EnergyCryEndcap);

						h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP2Barr->Fill(EnergyCryBarrelSum,EnergyCryEndcapSum);
						}
						
						
						
						
						
						
						
					}




		}//acaba h

	
		
	

		//loop in calorimeter Hits
		for(Int_t h=0;h<caloHitsPerEvent;h++)  // h=1, means first proton, h=2, second proton
		{
			h1_Cal->Fill(caloHit[h]->GetNbOfCrystalHits());	
			h2_Cal->Fill(caloHit[h]->GetEnergy()*1000);	
			h3_Cal->Fill(caloHit[h]->GetTheta());	
			h4_Cal->Fill(caloHit[h]->GetPhi());
			h2_CC->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta));
			h2_CC2->Fill(caloHit[h]->GetTheta(),GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta));

			if(BARREL && caloHit[h]->GetTheta()>minThetaBarrel*TMath::Pi()/180.) {
				caloHitsPerEvent_barrel++;
				h1_Cal_barrel->Fill(caloHit[h]->GetNbOfCrystalHits());	
				h2_Cal_barrel->Fill(caloHit[h]->GetEnergy()*1000);	
				h3_Cal_barrel->Fill(caloHit[h]->GetTheta());	
				h4_Cal_barrel->Fill(caloHit[h]->GetPhi());
				h2_CC_barrel->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta));
				

				EnergyCalBarrel=caloHit[h]->GetEnergy()*1000;
	
				EnergyCalBarrelSum=EnergyCalBarrelSum + EnergyCalBarrel;

				EnergyBarrelCalReconstr=GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta);	

				EnergyBarrelCalReconstructSum=EnergyBarrelCalReconstructSum+EnergyBarrelCalReconstr;



			}

			
			/*if(BARREL &&caloHit[h]->GetTheta()>minThetaBarrel*TMath::Pi()/180) {
				caloHitsPerEvent_barrel++;
				Double_t EnergyBarrel=caloHit[h]->GetEnergy();

				Double_t EnergyBarrelReconstr=GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta);}
				
				
				//outFile3<<"El evento en barrel es "<<i<<" y da en el ángulo  "<< (caloHit[h]->GetTheta())*(180/TMath::Pi())<<endl;}*/

			/*if(ENDCAP &&caloHit[h]->GetTheta()< maxThetaEndCap*TMath::Pi()/180.) {
				caloHitsPerEvent_endcap++;
				//outFile3<<"El evento en endcap es "<<i<<" y da en el ángulo  "<< caloHit[h]->GetTheta()*(180/TMath::Pi())<<endl;
				
				Double_t EnergyEndcap=caloHit[h]->GetEnergy();

				Double_t EnergyEndcapReconstr=GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta);}*/



			//h2_Cal_BarrelVsEndcap->Fill(EnergyBarrel,EnergyEndcap); Esto es solo el primer impacto

			int crystalId = crystalHit[h]->GetCrystalId();
		
			//int thetaId= caloHit[h]->GetTheta();
			double phiId= caloHit[h]->GetPhi();
			double xIdStart= track[h]->GetStartX();
			double yIdStart= track[h]->GetStartY();
			double zIdStart= track[h]->GetStartZ();

			double rId= TMath::Sqrt(xIdStart*xIdStart + yIdStart*yIdStart + zIdStart*zIdStart);
			double motherId=track[h]->GetMotherId();
			double iD=track[h]->GetPdgCode();
			double nPoints=track[h]->GetNPoints(2);
			
			double Theta_0, Theta_1;



			if (h==0){
			 Theta_0= caloHit[h]->GetTheta();}

			if (h==1){
			 Theta_1= caloHit[h]->GetTheta();}



			double Theta_R=Theta_0-Theta_1;



			//cout<<Theta_0<<"     "<<Theta_1<<"      "<<Theta_R<<endl;

		





			if(ENDCAP && caloHit[h]->GetTheta()< maxThetaEndCap*TMath::Pi()/180. /*&& crystalId%2 == 0*/) 
			{

			

			Double_t Energy=caloHit[h]->GetEnergy()*1000;
		 
			Double_t randomIs = gRandom->Gaus(0,Energy*ResolutionProts);



	




				caloHitsPerEvent_endcap++;

				double EnergyCalEndcap=caloHit[h]->GetEnergy()*1000+randomIs;
					
				EnergyCalEndcapSum=EnergyCalEndcapSum + EnergyCalEndcap;	



				EnergyEndcapCalReconstr=GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta)+randomIs;	

				 EnergyEndcapCalReconstructSum=EnergyEndcapCalReconstructSum+EnergyEndcapCalReconstr;

			
					if (crystalId%2 == 0)
					{	
				
				
						double EnergyCalPar=caloHit[h]->GetEnergy()*1000+randomIs;						
						
						EnergyParCalSum= EnergyParCalSum+ EnergyCalPar;	


						double EnergyParCalReconstruct=GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta)+randomIs;						
						
						EnergyParCalReconstructSum= EnergyParCalReconstructSum+ EnergyParCalReconstruct;	
				
			
					} //aqui acaban cristales pares
				
			

					h1_Cal_endcap->Fill(caloHit[h]->GetNbOfCrystalHits());	
					
				
					h2_Cal_endcap->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
					
				
					h3_Cal_endcap->Fill(caloHit[h]->GetTheta());	
					h4_Cal_endcap->Fill(caloHit[h]->GetPhi());

				
								
					h2_CC_endcap->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
				


				

					if (crystalId%2 == 1)
					{	//empiezan cristales impares
				
				
						double EnergyCalImpar=caloHit[h]->GetEnergy()*1000+randomIs;

			 			EnergyImparCalSum=EnergyImparCalSum+ EnergyCalImpar;
													
						int crystalIdImpar = crystalHit[h]->GetCrystalId();


				
					}//se acaban los impares

			}//se acaba el endcap. 
					
		//outFile3<<"el  evento i de calohit, dentro de h, es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<endl;



		h2_Cal_EnergyParSumEnergySum_endcap ->Fill(EnergyCalEndcapSum,EnergyParCalSum);
		h2_Cal_EnergyBarrelEnergyEndcap->Fill(EnergyCalBarrel,EnergyCalEndcap);
		h2_Cal_EnergyBarrelSumEnergyEndcapSum ->Fill(EnergyCalBarrelSum,EnergyCalEndcapSum);




		h2_CC_EnergyParReconstrEnergyReconstruct_endcap ->Fill(EnergyParCalReconstruct,EnergyEndcapCalReconstr);
		h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap ->Fill(EnergyParCalReconstructSum,EnergyEndcapCalReconstructSum);
		h2_CC_barrelReconstrVsEndcapReconstr->Fill(EnergyBarrelCalReconstr,EnergyEndcapCalReconstr);
		h2_CC_barrelReconstrSumVsEndcapReconstrSum->Fill(EnergyBarrelCalReconstructSum,EnergyEndcapCalReconstructSum);



	
		h3_Cal_Theta1VsTheta2->Fill(Theta_0,Theta_1);

		h3_Cal_Theta_R->Fill(Theta_R);

		
		if (h==1 && ENDCAP && crystalId>3000 || h==0 &&BARREL && crystalHit[h]->GetCrystalId()<3000) 
		{

			h3_Cal_Theta1VsTheta2_CoincP1EndcapP2Barr->Fill(Theta_0,Theta_1);
		}






		if (h==1 && ENDCAP && crystalId>3000 || h==0 &&BARREL && crystalHit[h]->GetCrystalId()<3000) 
		{

			h2_Cal_EnergyParSumEnergySum_endcap_CoincP1EndcapP2Barr ->Fill(EnergyCalEndcapSum,EnergyParCalSum);
			h2_Cal_EnergyBarrelEnergyEndcap_CoincP1EndcapP2Barr->Fill(EnergyCalBarrel,EnergyCalEndcap);
			h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP1EndcapP2Barr->Fill(EnergyCalBarrelSum,EnergyCalEndcapSum);


			h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP1EndcapP2Barr->Fill(EnergyParCalReconstruct,EnergyEndcapCalReconstr);
			h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP1EndcapP2Barr->Fill(EnergyParCalReconstructSum,EnergyEndcapCalReconstructSum);
			h2_CC_barrelReconstrVsEndcapReconstr_CoincP1EndcapP2Barr->Fill(EnergyBarrelCalReconstr,EnergyEndcapCalReconstr);
			h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP1EndcapP2Barr->Fill(EnergyBarrelCalReconstructSum,EnergyEndcapCalReconstructSum);
		}	
			
		
		if (h==1 && ENDCAP && crystalId>3000) 
		{

			h2_Cal_EnergyParSumEnergySum_endcap_CoincP1Endcap ->Fill(EnergyCalEndcapSum,EnergyParCalSum);
			h2_Cal_EnergyBarrelEnergyEndcap_CoincP1Endcap->Fill(EnergyCalBarrel,EnergyCalEndcap);
			h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP1Endcap->Fill(EnergyCalBarrelSum,EnergyCalEndcapSum);


			h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP1Endcap->Fill(EnergyParCalReconstruct,EnergyEndcapCalReconstr);
			h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP1Endcap->Fill(EnergyParCalReconstructSum,EnergyEndcapCalReconstructSum);
			h2_CC_barrelReconstrVsEndcapReconstr_CoincP1Endcap->Fill(EnergyBarrelCalReconstr,EnergyEndcapCalReconstr);
			h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP1Endcap->Fill(EnergyBarrelCalReconstructSum,EnergyEndcapCalReconstructSum);
			

		}
		
		
		
		if ( h==0 &&BARREL && crystalHit[h]->GetCrystalId()<3000) 
		{

			h2_Cal_EnergyParSumEnergySum_endcap_CoincP2Barr ->Fill(EnergyCalEndcapSum,EnergyParCalSum);
			h2_Cal_EnergyBarrelEnergyEndcap_CoincP2Barr->Fill(EnergyCalBarrel,EnergyCalEndcap);
			h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP2Barr->Fill(EnergyCalBarrelSum,EnergyCalEndcapSum);


			h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP2Barr->Fill(EnergyParCalReconstruct,EnergyEndcapCalReconstr);
			h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP2Barr->Fill(EnergyParCalReconstructSum,EnergyEndcapCalReconstructSum);
			h2_CC_barrelReconstrVsEndcapReconstr_CoincP2Barr->Fill(EnergyBarrelCalReconstr,EnergyEndcapCalReconstr);
			h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP2Barr->Fill(EnergyBarrelCalReconstructSum,EnergyEndcapCalReconstructSum);
			
		}
		
		
		
		
		
		if (h==1 && ENDCAP && crystalId>3000 || h==0 && crystalId<3000 ) 
		{
			if(crystalId<6000)   //¿cómo es el espectro reconstruido del cal?
			{
			
			//outFile3<<"Label  h of the particle  "<<h<<"   and type  "<<  (track[h]->GetPdgCode())<<"  touches the crystal   "<<crystalId<<endl;

			h2_CC_P1EndcapP2Barr->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
			h2_Cal_P1EndcapP2Barr->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
			}
			
		}
		
		
		
			
		if (h==1 && ENDCAP && crystalId>3000 || h==0 && crystalId<3000 ) 
		{
			if(BARREL&&crystalId<3000)   //Cal reconstructed spectrum
			{
			
			//outFile2<<"the label h of the particle "<<h<<"   y type  "<<  (track[h]->GetPdgCode())<<"  touches the crystal   "<<crystalId<<endl;

			h2_CC_barrel_P1EndcapP2Barr->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
			h2_Cal_barrel_P1EndcapP2Barr->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
			}
			
		}
		
		
		
					
		if (h==1 && ENDCAP && crystalId>3000 || h==0 && crystalId<3000 ) 
		{
			if(ENDCAP &&crystalId>3000)   //Cal reconstructed Cal?
			{
			
			//outFile<<"The label h of the particle   "<<h<<"   and type  "<<  (track[h]->GetPdgCode())<<"  and deposit energy in   "<<crystalId<<endl;

			h2_CC_endcap_P1EndcapP2Barr->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
			h2_Cal_endcap_P1EndcapP2Barr->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
			}
			
		}
		

		
		
		if (h==1 && ENDCAP && crystalId>3000 || h==0 && 0<crystalId<6000 ) 
		
		if(crystalId>3000)   //Cal reconstructed for CALIFA?
		{
			{
		
				h2_CC_endcap_P1Endcap->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
				h2_Cal_endcap_P1Endcap->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
			}
		
		
		}
		
		
		if (h==1 && ENDCAP && crystalId>3000 || h==0 && 0<crystalId<6000 ) //primer prton al endcap siempre, segundo donde sea 
		{
			if(0<crystalId<6000)   //Cal ressconstructed epectrum
			{
		
			h2_CC_P1Endcap->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
			h2_Cal_P1Endcap->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
			}
		
		
		}
		
		

					
		
		if (h==1 && ENDCAP && crystalId>3000 || h==0 && 0<crystalId<6000) //primer prton al endcap siempre, segundo donde sea 
		{
			if(crystalId<3000)   //Cal spectrun reconstructed? 
			{
		
				h2_CC_barrel_P1Endcap->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
				h2_Cal_barrel_P1Endcap->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
		
			}
		}
		
		
		
		if (h==1) //solo primer prton  donde sea
		{
		
			h2_CC_P1->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
			h2_Cal_P1->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
		
		
		}
		
		
	
		
		
		
		if ( h==0 &&BARREL && crystalId<3000 || h==1 && 0<crystalId<6000) ) 
		{
			if(crystalId<3000)   //How is the barrel energy resolution?
			{
		
				h2_CC_barrel_P2Barr->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
				h2_Cal_barrel_P2Barr->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
			}
		
		
		}
		
		
		
		
				
		if ( h==0 &&BARREL && crystalId<3000 || h==1 && 0<crystalId<6000) 
		{
		
			if(0<crystalId<6000)   
			{
				//outFile3<<"Parcticle h!<<   "<<h<<"   and kind  "<<  (track[h]->GetPdgCode())<<"   touches the crystal "<<crystalId<<endl;

				h2_CC_P2Barr->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
				h2_Cal_P2Barr->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
			}
		
		}
		
		
		
			
				
		if ( h==0 &&BARREL && crystalId<3000 || h==1 && 0<crystalId<6000) 
		{
		 
			
			{
				h2_CC_endcap_P2Barr->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
				h2_Cal_endcap_P2Barr->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
			}
		
		}
		
		
		
		
		
						
		if ( h==0) 
		{
			
			//outFile3<<"Particle h is   "<<h<<"   and kind   "<<  (track[h]->GetPdgCode())<<"  touches the cyrydtsl   "<<crystalId<<endl;
			h2_CC_P2->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000+randomIs,beta));
			h2_Cal_P2->Fill(caloHit[h]->GetEnergy()*1000+randomIs);
		
		
		}
		
		

		



		} //Se acaba h.  


		//outFile3<<"el  evento i de calohit, fuera de h, es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<endl;

		//h2_Cal_BarrelVsEndcap->Fill(EnergyBarrel,EnergyEndcap);

		//if (i=i-1 && crystalId>3000){


		//h2_CC_barrelReconstrVsEndcapReconstr->Fill(EnergyBarrelReconstr,EnergyEndcapReconstr);
		


		//h2_CC_barrelReconstrVsEndcapReconstr->Fill(EnergyBarrelReconstr,EnergyEndcapReconstr);


		//h5_Cal_EnergyImparCalSumEnergyParCalSum_endcap ->Fill(energyImparCalSum,energyParCalSum);

		//h5_Cal_EnergyImparCalSumEnergyCalSum_endcap ->Fill(energyTotalCalSum,energyImparCalSum);

		//h5_Cal_EnergyParCalSumEnergyCalSum_endcap ->Fill(energyTotalCalSum,energyParCalSum);
		
	
		
		//h5_Cal_Events_endcap_2->Fill(energyTotalCalSum);

	
		
	
		
		//loop in MC mother tracks
		for(Int_t h=0;h<MCtracksPerEvent;h++)
		{

/*outFile3<<"el  evento i dentro de track, dentro de h, fuera de Getmother<0, fuera de primary, es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<"   y  la partóicula   "<<   (track[h]->GetPdgCode())<< endl;*/

			if(track[h]->GetMotherId()<0)
			{



		//outFile3<<"el  evento i dentro de track, dentro de h,dentro de Getmother<0, fuera de primary, es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<"   y  la partóicula   "<<   (track[h]->GetPdgCode())<< endl;
			
		//outFile3<<"evento i del track "   <<j <<"partícula   "<< (track[h]->GetPdgCode())<<endl;



			track[h]->GetMomentum(momentum);	
				h1_T->Fill(track[h]->GetPdgCode());
				h2_T->Fill(track[h]->GetEnergy()*1000-track[h]->GetMass()*1000);
				h3_T->Fill(momentum.Theta());
				h4_T->Fill(momentum.Phi());

				
				primary++;
				if(BARREL && momentum.Theta()>minThetaBarrel*TMath::Pi()/180.) primary_barrel++;
				if(ENDCAP && momentum.Theta()<maxThetaEndCap*TMath::Pi()/180.) primary_endcap++;
				if(primary==1)
					{//outFile3<<"el  evento i dentro de track, dentro de h, dentro de primary, fuera e calohitevent=1, es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<"   y  la partóicula   "<<   (track[h]->GetPdgCode())<< endl;

						if(caloHitsPerEvent==1)
						{  //++i;
							//only present if multiplicity=1 and it is well reconstructed (only one calorimeter Hit)
							h3_CC->Fill(momentum.Theta() - caloHit[h]->GetTheta());
							h4_CC->Fill(momentum.Phi() - caloHit[h]->GetPhi());
							h1_CC2->Fill(momentum.Theta(),crystalHitsPerEvent);
							h3_CC2->Fill(momentum.Theta(),momentum.Theta() - caloHit[h]->GetTheta());
							h4_CC2->Fill(momentum.Theta(),momentum.Phi() - caloHit[h]->GetPhi());
							h1_EF->Fill(caloHit[h]->GetTheta());
				
							int crystalId = crystalHit[h]->GetCrystalId();
//outFile3<<"el  evento i dentro de track, dentro de h, dentro de primary,dentro de calhitevent 1,  es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<"   y  la partóicula   "<<   (track[h]->GetPdgCode())<< endl;
							if(BARREL && caloHit[h]->GetTheta()>minThetaBarrel*TMath::Pi()/180.)
							{
								h3_CC_barrel->Fill(momentum.Theta() - caloHit[h]->GetTheta());
								h4_CC_barrel->Fill(momentum.Phi() - caloHit[h]->GetPhi());
								//h1_EF_barrel->Fill(caloHit[h]->GetTheta());
							}
							if(ENDCAP && caloHit[h]->GetTheta()< maxThetaEndCap*TMath::Pi()/180. /*&& crystalId%2 == 0*/) 
							{
								h3_CC_endcap->Fill(momentum.Theta() - caloHit[h]->GetTheta());
								h4_CC_endcap->Fill(momentum.Phi() - caloHit[h]->GetPhi());
								h1_EF_endcap->Fill(caloHit[h]->GetTheta());
	
							}
						}	//outFile3<<"el  evento i dentro de track, dentro de h, dentro de primary, dentro de calohitevent=1 es  "<<i<< " y h  es    "<<h<<"  y el cristal es   "<<crystalId<<"   y  la partóicula   "<<   (track[h]->GetPdgCode())<< endl;


						//else ... one could think in introduce some combinatorial here to see the 
						//real coincidences over the background of randoms...
							//outFile3<<"el  evento es "<<i<< "  h vale"<<h<< "y la particula es "<<(track[h]->GetPdgCode())<<endl;
					}
			}
		}

		h1_CryMul->Fill(crystalHitsPerEvent);
		h1_CalMul->Fill(caloHitsPerEvent);
		if(BARREL) h1_CryMul_barrel->Fill(crystalHitsPerEvent_barrel);
		if(BARREL) h1_CalMul_barrel->Fill(caloHitsPerEvent_barrel);
		if(ENDCAP) h1_CryMul_endcap->Fill(crystalHitsPerEvent_endcap);
		if(ENDCAP) h1_CalMul_endcap->Fill(caloHitsPerEvent_endcap);
		
		crystalHitsPerEvent_barrel=0;
		caloHitsPerEvent_barrel=0;
		crystalHitsPerEvent_endcap=0;
		caloHitsPerEvent_endcap=0;

		h1_TMul->Fill(primary);
		if(BARREL) h1_TMul_barrel->Fill(primary_barrel);
		if(ENDCAP) h1_TMul_endcap->Fill(primary_endcap);

		primary=0;
		primary_barrel=0;
		primary_endcap=0;
		
		//User defined additions
		if(caloHitsPerEvent==2)
		{
			//h1_User->Fill(caloHit[1]->GetTheta()-caloHit[0]->GetTheta(),
				//		  caloHit[1]->GetPhi()-caloHit[0]->GetPhi());
		}
		if(crystalHitsPerEvent) delete [] crystalHit;
		if(caloHitsPerEvent) delete [] caloHit;
		if(MCtracksPerEvent) delete [] track;
	
	}//fin de evento i


        TFile f_50MeV("/home/jose/r3broot_sept2012/macros/r3b/califa/hist_Endcap21_50MeV.root", "recreate");
	f_50MeV->cd();

	if(h1_T){h1_T->Write();}
	if(h2_T){h2_T->Write();}
	if(h3_T){h3_T->Write();}
	if(h4_T){h4_T->Write();}


	if(h1_Cry){h1_Cry->Write();}
	if(h1_Cry_count){h1_Cry_count->Write();}
	if(h2_Cry){h2_Cry->Write();}

	//if(h2_Cry_BarrelVsEndcap){h2_Cry_BarrelVsEndcap->Write();}

	if(h3_Cry){h3_Cry->Write();}
	if(h4_Cry){h4_Cry->Write();}

	if(h1_Cal){h1_Cal->Write();}
	if(h2_Cal){h2_Cal->Write();}
	if(h3_Cal){h3_Cal->Write();}
	if(h4_Cal){h4_Cal->Write();}

	if(h2_Cal_P1EndcapP2Barr){h2_Cal_P1EndcapP2Barr->Write();}
	
	if(h2_Cal_barrel_P2Barr){h2_Cal_barrel_P2Barr->Write();}
	if(h2_Cal_endcap_P1Endcap){h2_Cal_endcap_P1Endcap->Write();}



	if(h2_Cal_EnergyParSumEnergySum_endcap){h2_Cal_EnergyParSumEnergySum_endcap->Write();}
	if(h2_Cal_EnergyBarrelEnergyEndcap){h2_Cal_EnergyBarrelEnergyEndcap->Write();}
	if(h2_Cal_EnergyBarrelSumEnergyEndcapSum){h2_Cal_EnergyBarrelSumEnergyEndcapSum->Write();}

	if(h3_Cal_Theta1VsTheta2){h3_Cal_Theta1VsTheta2->Write();}
	if(h3_Cal_Theta1VsTheta2_CoincP1EndcapP2Barr){h3_Cal_Theta1VsTheta2_CoincP1EndcapP2Barr->Write();}

	if(h3_Cal_Theta_R){h3_Cal_Theta_R->Write();}




	//if (h2_Cal_BarrelVsEndcap){h2_Cal_BarrelVsEndcap->Write();}


	if(h1_TMul){h1_TMul->Write();}
	if(h1_CryMul){h1_CryMul->Write();}
	if(h1_CalMul){h1_CalMul->Write();}

	if(h2_CC){h2_CC->Write();}
	if(h3_CC){h3_CC->Write();}
	if(h4_CC){h2_CC->Write();}
	
	
		if(h2_CC_P1EndcapP2Barr){h2_CC_P1EndcapP2Barr->Write();}
		if(h2_CC_barrel_P2Barr){h2_CC_barrel_P2Barr->Write();}
		if(h2_CC_endcap_P1Endcap){h2_CC_endcap_P1Endcap->Write();}
		if(h2_CC_P1Endcap){h2_CC_P1Endcap->Write();}
		if(h2_CC_P2Barr){h2_CC_P2Barr->Write();}		
		
	

	if(h2_CC2){h2_CC2->Write();}
	if(h3_CC2){h3_CC2->Write();}
	if(h4_CC2){h2_CC2->Write();}
	if(h1_EF){h1_EF->Write();}
//barrel

	if(h1_Cry_barrel){h1_Cry_barrel->Write();}
	if(h1_Cry_count_barrel){h1_Cry_count_barrel->Write();}
	if(h2_Cry_barrel){h2_Cry_barrel->Write();}
	if(h3_Cry_barrel){h3_Cry_barrel->Write();}
	if(h4_Cry_barrel){h4_Cry_barrel->Write();}


	if(h1_Cal_barrel){h1_Cal_barrel->Write();}
	if(h2_Cal_barrel){h2_Cal_barrel->Write();}
	if(h3_Cal_barrel){h3_Cal_barrel->Write();}
	if(h4_Cal_barrel){h4_Cal_barrel->Write();}



	if(h1_TMul_barrel){h1_TMul_barrel->Write();}
	if(h1_CryMul_barrel){h1_CryMul_barrel->Write();}
	if(h1_CalMul_barrel){h1_CalMul_barrel->Write();}
	if(h2_CC_barrel){h2_CC_barrel->Write();}
	if(h3_CC_barrel){h3_CC_barrel->Write();}
	if(h4_CC_barrel){h4_CC_barrel->Write();}

//endcap

	if(h1_Cry_endcap){h1_Cry_endcap->Write();}
	if(h1_Cry_count_endcap){h1_Cry_count_endcap->Write();}
	if(h2_Cry_endcap){h2_Cry_endcap->Write();}
	if(h3_Cry_endcap){h3_Cry_endcap->Write();}
	if(h4_Cry_endcap){h4_Cry_endcap->Write();}


	if(h1_Cal_endcap){h1_Cal_endcap->Write();}
	if(h2_Cal_endcap){h2_Cal_endcap->Write();}
	if(h3_Cal_endcap){h3_Cal_endcap->Write();}
	if(h4_Cal_endcap){h4_Cal_endcap->Write();}



	if(h1_TMul_endcap){h1_TMul_endcap->Write();}
	if(h1_CryMul_endcap){h1_CryMul_endcap->Write();}
	if(h1_CalMul_endcap){h1_CalMul_endcap->Write();}
	if(h2_CC_endcap){h2_CC_endcap->Write();}
	if(h3_CC_endcap){h3_CC_endcap->Write();}
	if(h4_CC_endcap){h4_CC_endcap->Write();}


	//if(h5_Cry_EnergyImparSumEnergyParSum_endcap){h5_Cry_EnergyImparSumEnergyParSum_endcap->Write();}
	//if(h5_Cry_EnergyImparSumEnergySum_endcap ){h5_Cry_EnergyImparSumEnergySum_endcap ->Write();}	

	if(h5_Cry_EnergyParSumEnergySum_endcap){h5_Cry_EnergyParSumEnergySum_endcap ->Write();}

	if(h5_Cry_EnergyBarrelEnergyEndcap ){h5_Cry_EnergyBarrelEnergyEndcap ->Write();}

	if(h5_Cry_EnergyBarrelSumEnergyEndcapSum){h5_Cry_EnergyBarrelSumEnergyEndcapSum->Write();}



	if(h5_Cry_EnergyParSumEnergySum_endcap_CoincP1EndcapP2Barr){h5_Cry_EnergyParSumEnergySum_endcap_CoincP1EndcapP2Barr ->Write();}

	if(h5_Cry_EnergyBarrelEnergyEndcap_CoincP1EndcapP2Barr ){h5_Cry_EnergyBarrelEnergyEndcap_CoincP1EndcapP2Barr ->Write();}

	if(h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP1EndcapP2Barr){h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP1EndcapP2Barr->Write();}
	
	
	
	if(h5_Cry_EnergyParSumEnergySum_endcap_CoincP1Endcap){h5_Cry_EnergyParSumEnergySum_endcap_CoincP1Endcap ->Write();}

	if(h5_Cry_EnergyBarrelEnergyEndcap_CoincP1Endcap ){h5_Cry_EnergyBarrelEnergyEndcap_CoincP1Endcap ->Write();}

	if(h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP1Endcap){h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP1Endcap->Write();}
	
	
	if(h5_Cry_EnergyParSumEnergySum_endcap_CoincP2Barr){h5_Cry_EnergyParSumEnergySum_endcap_CoincP2Barr ->Write();}

	if(h5_Cry_EnergyBarrelEnergyEndcap_CoincP2Barr ){h5_Cry_EnergyBarrelEnergyEndcap_CoincP2Barr ->Write();}

	if(h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP2Barr){h5_Cry_EnergyBarrelSumEnergyEndcapSum_CoincP2Barr->Write();}
	



	if(h2_CC_barrelReconstrVsEndcapReconstr){h2_CC_barrelReconstrVsEndcapReconstr->Write();}

	if(h2_CC_barrelReconstrSumVsEndcapReconstrSum){h2_CC_barrelReconstrSumVsEndcapReconstrSum->Write();}

	if(h2_CC_EnergyParReconstrEnergyReconstruct_endcap){h2_CC_EnergyParReconstrEnergyReconstruct_endcap->Write();}

	if(h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap){h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap->Write();}





	if(h2_CC_barrelReconstrVsEndcapReconstr_CoincP1EndcapP2Barr){h2_CC_barrelReconstrVsEndcapReconstr_CoincP1EndcapP2Barr->Write();}

	if(h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP1EndcapP2Barr){h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP1EndcapP2Barr->Write();}

	if(h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP1EndcapP2Barr){h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP1EndcapP2Barr->Write();}

	if(h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP1EndcapP2Barr){h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP1EndcapP2Barr->Write();}
	
	
		
	if(h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP1Endcap){h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP1Endcap->Write();}

	if(h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP1Endcap){h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP1Endcap->Write();}

	if(h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP1Endcap){h2_CC_barrelReconstrSumVsEndcapReconstrSum_CoincP1Endcap->Write();}

	
	
	
		
	if(h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP2Barr){h2_CC_EnergyParReconstrEnergyReconstruct_endcap_CoincP2Barr->Write();}

	if(h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP2Barr){h2_CC_EnergyParReconstrSumEnergyReconstructSum_endcap_CoincP2Barr->Write();}

	if(h2_CC_barrelReconstrVsEndcapReconstr_CoincP2Barr){h2_CC_barrelReconstrVsEndcapReconstr_CoincP2Barr->Write();}


	
	
	
	if(h2_Cal_EnergyParSumEnergySum_endcap_CoincP1Endcap){h2_Cal_EnergyParSumEnergySum_endcap_CoincP1Endcap->Write();}

	if(h2_Cal_EnergyBarrelEnergyEndcap_CoincP1Endcap){h2_Cal_EnergyBarrelEnergyEndcap_CoincP1Endcap->Write();}

	if(h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP1Endcap){h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP1Endcap->Write();}

	
	
	
	if(h2_Cal_EnergyParSumEnergySum_endcap_CoincP2Barr){h2_Cal_EnergyParSumEnergySum_endcap_CoincP2Barr->Write();}

	if(h2_Cal_EnergyBarrelEnergyEndcap_CoincP2Barr){h2_Cal_EnergyBarrelEnergyEndcap_CoincP2Barr->Write();}

	if(h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP2Barr){h2_Cal_EnergyBarrelSumEnergyEndcapSum_CoincP2Barr->Write();}

	if(h2_Cal_barrel_P2Barr)->{h2_Cal_barrel_P2Barr->Write();}
	
	
	if(h2_Cal_P1Endcap)->{h2_Cal_P1Endcap->Write();}
	if(h2_CC_P2Barr)->{h2_CC_P2Barr->Write();}
	
	
		
	if(h2_Cal_P1)->{h2_Cal_P1->Write();}
	if(h2_Cal_P2)->{h2_Cal_P2->Write();}
	if(h2_CC_P1)->{h2_CC_P1->Write();}
	if(h2_CC_P2)->{h2_CC_P2->Write();}
	
	if(h2_CC_barrel_P1Endcap)->{h2_CC_barrel_P1Endcap->Write();}
	if(h2_Cal_barrel_P1Endcap)->{h2_Cal_barrel_P1Endcap->Write();}
	if(h2_CC_endcap_P1Endcap)->{h2_CC_endcap_P1Endcap->Write();}
	if(h2_Cal_endcap_P1Endcap)->{h2_Cal_endcap_P1Endcap->Write();}
	
	
	if(h2_CC_endcap_P2Barr)->{h2_CC_endcap_P2Barr->Write();}
	if(h2_Cal_endcap_P2Barr)->{h2_Cal_endcap_P2Barr->Write();}
	
	
	if(h2_CC_barrel_P1EndcapP2Barr)->{h2_CC_barrel_P1EndcapP2Barr->Write();}
	if(h2_Cal_barrel_P1EndcapP2Barr)->{h2_Cal_barrel_P1EndcapP2Barr->Write();}
	if(h2_CC_endcap_P1EndcapP2Barr)->{h2_CC_endcap_P1EndcapP2Barr->Write();}
	if(h2_Cal_endcap_P1EndcapP2Barr)->{h2_Cal_endcap_P1EndcapP2Barr->Write();}
	
	
	
	//if(h5_Cry_Events_endcap_2 ){h5_Cry_Events_endcap_2->Write();}
	//if(h5_Cal_EnergyImparCalSumEnergyParCalSum_endcap){h5_Cal_EnergyImparCalSumEnergyParCalSum_endcap->Write();}
	//if(h5_Cal_EnergyImparCalSumEnergyCalSum_endcap){h5_Cal_EnergyImparCalSumEnergyCalSum_endcap->Write();}
	//if(h5_Cal_EnergyParCalSumEnergyCalSum_endcap){h5_Cal_EnergyParCalSumEnergyCalSum_endcap->Write();}


	//if(h5_Cal_Events_endcap_2){h5_Cal_Events_endcap_2->Write();}
	
	
	
}	

Double_t GetCMEnergy(Double_t theta, Double_t energy, Double_t beta=0.0){ 
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



		
			

	
