#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

void digiAnalyser(){

	//Output file
//	TSTring digiOutFile 		= "land_analyse.root";

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGenVector");

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");


  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");

  //----  Load R3B specific libraries ---------------------------------------
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BGen");
  gSystem->Load("libR3BPassive");
  gSystem->Load("libR3BData");
  gSystem->Load("libR3BLand");

	//----	Init Tree structure ------------------------------------------------

	gROOT->LoadMacro("DigiData.C");
	gROOT->LoadMacro("SimData.C");

	DigiData* landDigi = new DigiData();
	SimData *simdata = new SimData();

	Int_t nEvent=10000;
		

	//----	Create digitalisation plots -------------------------------------------------------

	TCanvas *digiPlots = new TCanvas("DigiPlots", "Digitalisation histograms");
	digiPlots->Divide(2,3);

	digiPlots->cd(1);
  TH1F *hPMtime = new TH1F("PMtime","Arrival times of PMs",1000,0.,100.);
	hPMtime->GetXaxis()->SetTitle("Time (ns)");
	hPMtime->GetYaxis()->SetTitle("Counts");
	landDigi->fChain->Project("PMtime","(LandDigi.fTdcL+LandDigi.fTdcR)/2");
	hPMtime->DrawNormalized();

  TH1F *hPaddleEnergy = new TH1F("PaddleEnergy","Energy deposit in one paddle",500,0.,50.);
  hPaddleEnergy->GetXaxis()->SetTitle("Energy (MeV)");
  hPaddleEnergy->GetYaxis()->SetTitle("Counts");

  TH1F *hTotalEnergy = new TH1F("TotalEnergy","Energy deposit in Land",500,0.,100.);
  hTotalEnergy->GetXaxis()->SetTitle("Energy (MeV)");
  hTotalEnergy->GetYaxis()->SetTitle("Counts");

	TH1F *hMult1 = new TH1F("Multiplicity1","Paddle multiplicity (any PM)",20,-0.5,19.5);
	hMult1->GetXaxis()->SetTitle("Multiplicity");
	hMult1->GetYaxis()->SetTitle("Counts");

	TH1F *hFirstEnergy = new TH1F("FirstEnergy","Energy of first hit",500,0.,50);
	hFirstEnergy->GetXaxis()->SetTitle("Energy");
	hFirstEnergy->GetYaxis()->SetTitle("Counts");

// ----- Caluclate plot data -------------------------------------------------

	Int_t nFileEvent = landDigi->fChain->GetEntries();
	if (nFileEvent < nEvent)
	{
	  cout << "\n-------------------Warning---------------------------" << endl;
	  cout << " -W digiAnalyser : File has less events than requested!!" << endl;
	  cout << " File contains : " << nFileEvent  << " Events" << endl;
	  cout << " Requested number of events = " <<  nEvent <<  " Events"<< endl;
	  cout << " The number of events is set to " << nFileEvent << " Events"<< endl;
	  cout << "-----------------------------------------------------" << endl;
		nEvent=nFileEvent;
	}
	TString selection;
	Int_t nEventEntries;
	Double_t qdcTot, qdcL, qdcR, qdcPaddle;

	for (Int_t i=0; i < nEvent; i++)
	{
		qdcTot=0;
		landDigi->fChain->GetEntry(i);
		nEventEntries=landDigi->LandDigi_;
		if( nEventEntries>0)
		{
			Double_t minHitTime = landDigi->LandDigi_fTdcL[0] + landDigi->LandDigi_fTdcR[0];
			Int_t indexMinHitTime = 0;
			Double_t firstEnergy = 0;

			for(Int_t j=0; j < nEventEntries; j++)
			{
				qdcL = landDigi->LandDigi_fQdcL[j];
				qdcR = landDigi->LandDigi_fQdcR[j];
				qdcPaddle = sqrt(qdcL*qdcR);
				qdcTot += qdcPaddle;
				hPaddleEnergy->Fill(qdcPaddle);

				Double_t hitTime = landDigi->LandDigi_fTdcL[j] + landDigi->LandDigi_fTdcR[j];
				if (hitTime <= minHitTime || TMath::IsNaN(minHitTime))
				{ 
					minHitTime=hitTime;
					indexMinHitTime = j;
					firstEnergy=qdcPaddle;
				}
			}
			hFirstEnergy->Fill(firstEnergy);
			hTotalEnergy->Fill(qdcTot);
			hMult1->Fill(nEventEntries);
		}
	}

	digiPlots->cd(2);
	hPaddleEnergy->DrawNormalized();

	digiPlots->cd(3);
	hTotalEnergy->DrawNormalized();

	digiPlots->cd(4);
	hMult1->DrawNormalized();

	digiPlots->cd(5);
	hFirstEnergy->DrawNormalized();
	
//----- Init data plots ------------------------------------------------------

	TCanvas *dataPlots = new TCanvas("DataPlots", "Control histograms");
	dataPlots->Divide(2,3);

	THStack *hPdgTimeEnergy = new THStack("PdgTimeEnergy","Energy in LAND for different pdg");
	THStack *hPdgEnergy = new THStack("PdgEnergy","Energy in LAND for different pdg");

	TH2F *protonTimeE = new TH2F("protonTimeE","Proton Time:Energy",100,35,60,10000,0.0001,100);
	protonTimeE->SetMarkerColor(1);
	protonTimeE->SetFillColor(1);
	protonTimeE->GetXaxis()->SetTitle("Time");
	protonTimeE->GetYaxis()->SetTitle("Energy");
	hPdgTimeEnergy->Add(protonTimeE);

	TH2F *electronTimeE = new TH2F("electronTimeE","Electron Time:Energy",100,35,60,10000,0.0001,100);
	electronTimeE->SetMarkerColor(2);
	electronTimeE->SetFillColor(2);
	electronTimeE->GetXaxis()->SetTitle("Time");
	electronTimeE->GetYaxis()->SetTitle("Energy");
	hPdgTimeEnergy->Add(electronTimeE);

	TH2F *gammaTimeE = new TH2F("gammaTimeE","Gamma Time:Energy",100,35,60,10000,0.0001,100);
	gammaTimeE->SetMarkerColor(3);
	gammaTimeE->SetFillColor(3);
	gammaTimeE->GetXaxis()->SetTitle("Time");
	gammaTimeE->GetYaxis()->SetTitle("Energy");
	hPdgTimeEnergy->Add(gammaTimeE);

	TH2F *otherTimeE = new TH2F("otherTimeE","Other Time:Energy",100,35,60,10000,0.0001,100);
	otherTimeE->SetMarkerColor(4);
	otherTimeE->SetFillColor(4);
	otherTimeE->GetXaxis()->SetTitle("Time");
	otherTimeE->GetYaxis()->SetTitle("Energy");
	hPdgTimeEnergy->Add(otherTimeE);

// ----- In Iron-----

	TH2F *protonIronTimeE = new TH2F("protonIronTimeE","Proton Time:Energy (Iron)",1000,35,1000,10000,0.0001,100);
	protonIronTimeE->SetMarkerColor(1);
	protonIronTimeE->SetFillColor(1);
	protonIronTimeE->GetXaxis()->SetTitle("Time");
	protonIronTimeE->GetYaxis()->SetTitle("Energy");

	TH2F *electronIronTimeE = new TH2F("electronIronTimeE","Electron Time:Energy (Iron)",1000,35,1000,10000,0.0001,50);
	electronIronTimeE->SetMarkerColor(2);
	electronIronTimeE->SetFillColor(2);
	electronIronTimeE->GetXaxis()->SetTitle("Time");
	electronIronTimeE->GetYaxis()->SetTitle("Energy");

	TH2F *gammaIronTimeE = new TH2F("gammaIronTimeE","Gamma Time:Energy (Iron)",1000,35,1000,1000,0,2);
	gammaIronTimeE->SetMarkerColor(3);
	gammaIronTimeE->SetFillColor(3);
	gammaIronTimeE->GetXaxis()->SetTitle("Time");
	gammaIronTimeE->GetYaxis()->SetTitle("Energy");

	TH2F *piMinusIronTimeE = new TH2F("piMinusIronTimeE","Pi- Time:Energy (Iron)",1000,35,1000,10000,0,100);
	piMinusIronTimeE->SetMarkerColor(6);
	piMinusIronTimeE->SetFillColor(6);
	piMinusIronTimeE->GetXaxis()->SetTitle("Time");
	piMinusIronTimeE->GetYaxis()->SetTitle("Energy");

	TH2F *otherIronTimeE = new TH2F("otherIronTimeE","Other Time:Energy (Iron)",1000,35,1000,10000,0.0001,80);
	otherIronTimeE->SetMarkerColor(4);
	otherIronTimeE->SetFillColor(4);
	otherIronTimeE->GetXaxis()->SetTitle("Time");
	otherIronTimeE->GetYaxis()->SetTitle("Energy");

// ----- In Scint ------

	TH2F *protonScintTimeE = new TH2F("protonScintTimeE","Proton Time:Energy (Scint)",1000,35,1000,10000,0.0001,100);
	protonScintTimeE->SetMarkerColor(1);
	protonScintTimeE->SetFillColor(1);
	protonScintTimeE->GetXaxis()->SetTitle("Time");
	protonScintTimeE->GetYaxis()->SetTitle("Energy");

	TH2F *electronScintTimeE = new TH2F("electronScintTimeE","Electron Time:Energy (Scint)",1000,35,1000,10000,0.0001,50);
	electronScintTimeE->SetMarkerColor(2);
	electronScintTimeE->SetFillColor(2);
	electronScintTimeE->GetXaxis()->SetTitle("Time");
	electronScintTimeE->GetYaxis()->SetTitle("Energy");

	TH2F *gammaScintTimeE = new TH2F("gammaScintTimeE","Gamma Time:Energy (Scint)",1000,35,1000,10000,0,2);
	gammaScintTimeE->SetMarkerColor(3);
	gammaScintTimeE->SetFillColor(3);
	gammaScintTimeE->GetXaxis()->SetTitle("Time");
	gammaScintTimeE->GetYaxis()->SetTitle("Energy");

	TH2F *piMinusScintTimeE = new TH2F("piMinusScintTimeE","Pi- Time:Energy (Scint)",1000,35,1000,10000,0,100);
	piMinusScintTimeE->SetMarkerColor(6);
	piMinusScintTimeE->SetFillColor(6);
	piMinusScintTimeE->GetXaxis()->SetTitle("Time");
	piMinusScintTimeE->GetYaxis()->SetTitle("Energy");

	TH2F *otherScintTimeE = new TH2F("otherScintTimeE","Other Time:Energy (Scint)",1000,35,1000,10000,0.0001,80);
	otherScintTimeE->SetMarkerColor(4);
	otherScintTimeE->SetFillColor(4);
	otherScintTimeE->GetXaxis()->SetTitle("Time");
	otherScintTimeE->GetYaxis()->SetTitle("Energy");

//---------------------

	TH1F *protonE = new TH1F("protonE","Proton energy",500,0,50);
	protonE->SetFillColor(1);
	protonE->GetXaxis()->SetTitle("Energy");
	protonE->GetYaxis()->SetTitle("Count");
	hPdgEnergy->Add(protonE);

	TH1F *electronE = new TH1F("electronE","Electron energy",500,0,50);
	electronE->SetFillColor(2);
	electronE->GetXaxis()->SetTitle("Energy");
	electronE->GetYaxis()->SetTitle("Count");
	hPdgEnergy->Add(electronE);

	TH1F *gammaE = new TH1F("gammaE","Gamma energy",500,0,50);
	gammaE->SetFillColor(3);
	gammaE->GetXaxis()->SetTitle("Energy");
	gammaE->GetYaxis()->SetTitle("Count");
	hPdgEnergy->Add(gammaE);

	TH1F *otherE = new TH1F("otherE","Other energy",500,0,50);
	otherE->SetFillColor(4);
	otherE->GetXaxis()->SetTitle("Energy");
	otherE->GetYaxis()->SetTitle("Count");
	hPdgEnergy->Add(otherE);

//----- Calculate plot data -----------------------------------------------------------

	Int_t trackID;
	Double_t energy;
	Double_t time;
	Int_t pdgCode;
	Int_t paddleType;

	for(Int_t i=0; i < nEvent; i++)
	{
		simdata->fChain->GetEntry(i);
		nEventEntries=simdata->LandPoint_;
		if( nEventEntries > 0)
		{
			for(Int_t j = 0; j< nEventEntries; j++)
			{
				trackID=simdata->LandPoint_fTrackID[j];
				energy=simdata->LandPoint_fELoss[j]*1000;
				time=simdata->LandPoint_fTime[j];
				pdgCode=simdata->MCTrack_fPdgCode[trackID];
				paddleType=simdata->LandPoint_fPaddleTyp[j];

				if (energy>0)
				{
					//Proton
					if (pdgCode == 2212)
					{
						protonTimeE->Fill(time,energy); //[MeV]
						protonE->Fill(energy); //[MeV]

						if (paddleType == 3)
							protonScintTimeE->Fill(time,energy);
						else
							protonIronTimeE->Fill(time,energy);
					}
					//Gamma
					else if (pdgCode == 22)
					{
						gammaTimeE->Fill(time,energy); //[MeV]
						gammaE->Fill(energy); //[MeV]

						if (paddleType == 3)
							gammaScintTimeE->Fill(time,energy);
						else
							gammaIronTimeE->Fill(time,energy);
					}
					//Electron
					else if (pdgCode == 11)
					{
						electronTimeE->Fill(time,energy); //[MeV]
						electronE->Fill(energy); //[MeV]

						if (paddleType == 3)
							electronScintTimeE->Fill(time,energy);
						else
							electronIronTimeE->Fill(time,energy);
					}
					//Pi-
					else if(pdgCode==-211)
					{
						if (paddleType == 3)
							piMinusScintTimeE->Fill(time,energy);
						else
							piMinusIronTimeE->Fill(time,energy);
					}
					//Other
					else
					{	// 211				- pi+
						// -11			  -	e+
						// -13				- my+
						// 13					- my-
											
						otherTimeE->Fill(time,energy); //[MeV]
						otherE->Fill(energy); //[MeV]
						if (pdgCode==13)
							cout << "energy: " << energy << endl;

						if (paddleType== 3)
						{
							otherScintTimeE->Fill(time,energy);

						}
						else
							otherIronTimeE->Fill(time,energy);
					}
				}
			}
		}
	}

	dataPlots->cd(1);
//	gPad->SetLogx();
//  gPad->SetLogy();
	hPdgTimeEnergy->Draw("nostack");
	hPdgTimeEnergy->GetXaxis()->SetTitle("Time");
	hPdgTimeEnergy->GetYaxis()->SetTitle("Energy");
	TLegend *timeEnergyLegend = new TLegend(0.7,0.65,0.86,0.88);
	timeEnergyLegend->AddEntry(protonTimeE,"Proton","f");
	timeEnergyLegend->AddEntry(electronTimeE,"Electron","f");
	timeEnergyLegend->AddEntry(gammaTimeE,"Gamma","f");
	timeEnergyLegend->AddEntry(otherTimeE,"Other","f");
	timeEnergyLegend->Draw();

	dataPlots->cd(2);
//	gPad->SetLogx();
//  gPad->SetLogy();
	hPdgEnergy->Draw();
	hPdgEnergy->GetXaxis()->SetTitle("Energy");
	hPdgEnergy->GetYaxis()->SetTitle("Counts");
	TLegend *energyLegend = new TLegend(0.7,0.65,0.86,0.88);
	energyLegend->AddEntry(protonE,"Proton","f");
	energyLegend->AddEntry(electronE,"Electron","f");
	energyLegend->AddEntry(gammaE,"Gamma","f");
	energyLegend->AddEntry(otherE,"Other","f");
	energyLegend->Draw();

	dataPlots->cd(3);
//	gPad->SetLogx();
//  gPad->SetLogy();
	protonTimeE->Draw();

	dataPlots->cd(4);
//	gPad->SetLogx();
//  gPad->SetLogy();
	electronTimeE->Draw();

	dataPlots->cd(5);
//	gPad->SetLogx();
//  gPad->SetLogy();
	gammaTimeE->Draw();

	dataPlots->cd(6);
//	gPad->SetLogx();
//  gPad->SetLogy();
	otherTimeE->Draw();

 // --- Data plots 2 ---

	TCanvas *dataPlots2 = new TCanvas("DataPlots2", "Control histograms2");
	dataPlots2->Divide(2,5);

	dataPlots2->cd(1);
	protonScintTimeE->Draw();
	
	dataPlots2->cd(2);
	protonIronTimeE->Draw();

	dataPlots2->cd(3);
	electronScintTimeE->Draw();
	
	dataPlots2->cd(4);
	electronIronTimeE->Draw();

	dataPlots2->cd(5);
	gammaScintTimeE->Draw();
	
	dataPlots2->cd(6);
	gammaIronTimeE->Draw();

	dataPlots2->cd(7);
	piMinusScintTimeE->Draw();
	
	dataPlots2->cd(8);
	piMinusIronTimeE->Draw();

	dataPlots2->cd(9);
	otherScintTimeE->Draw();
	
	dataPlots2->cd(10);
	otherIronTimeE->Draw();
}
