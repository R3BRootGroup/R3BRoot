
/******************************************/
/*                                        */
/*         Analysis code for use          */
/*     with external neutron trackers     */
/*                                        */
/*  saves detected hits in scenarios.dat  */
/*   and primary hits in primaries.dat    */
/*                                        */
/*        Johan Gill & Staffan Wranne     */
/*               10/2 2011                */
/*                                        */
/******************************************/

#include "R3BLandDigiAnalyser.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "FairRootManager.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "R3BLandDigi.h"
#include "R3BLandPoint.h"
#include "R3BMCTrack.h"
#include "TCanvas.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TGeoTrack.h"
#include "R3BDetectorList.h"
#include "FairBaseParSet.h"


using std::cout; using std::endl;

struct TrackInformation
{
  double motherEnergy;
  double startEnergy;
  double depositedEnergy;
  int pdgCode;
  int daughterID[100];
  int nDaughters;
};

/** Default constructor **/  
R3BLandDigiAnalyser::R3BLandDigiAnalyser(Int_t _iVerbose)
  : FairTask("R3B Land Analysation scheme ") {
  iVerbose=_iVerbose;
  emptyEventsAccepted=false;
}

void R3BLandDigiAnalyser::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fLandDigiPar = (R3BLandDigiPar*)(rtdb->getContainer("R3BLandDigiPar"));

  if ( fLandDigiPar ) {
      cout << "-I- R3BLandDigitizer::SetParContainers() "<< endl;
      cout << "-I- Container R3BLandDigiPar  loaded " << endl;
  }

  FairBaseParSet* fBaseParSet = (FairBaseParSet*) (rtdb->getContainer("FairBaseParSet"));

  land = (R3BDetector*) ((fBaseParSet->GetDetList())->FindObject("Land"));
}


/** Destructor **/
R3BLandDigiAnalyser::~R3BLandDigiAnalyser()
{ }


/** Virtual method Init **/
InitStatus R3BLandDigiAnalyser::Init()
{
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fLandPoints = (TClonesArray*) ioman->GetObject("LandPoint");
  fLandMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  fLandDigi = (TClonesArray*) ioman->GetObject("LandDigi");
  fLandGeoTracks= (TClonesArray*) ioman->GetObject("GeoTracks");

  foutScen.open("scenarios.dat");
  if (!foutScen) Fatal("Init","Unable to open scenarios.dat");

  nPaddles = fLandDigiPar->GetMaxPaddle();
  nPlanes = fLandDigiPar->GetMaxPlane();

  nPaddlesPerPlane = nPaddles/nPlanes;

  paddle_dimx=fLandDigiPar->GetPaddleLength()*2;
  paddle_dimy=fLandDigiPar->GetPaddleHeight()*2;
  paddle_dimz=(fLandDigiPar->GetPaddleDepth()+fLandDigiPar->GetPaddleWrapping())*2;
  paddle_spacing=fLandDigiPar->GetPaddleSpacing()*2;

  const Double_t* landPos = (land->GetGlobalTrans())->GetTranslation();
  landPlacementZ=landPos[2];

  att=0.008;
  v_eff=16.0;    // Effective light speed in media
  v_eff_inv=1/v_eff;

  fLandDigiPar->Print();

  initialisePlots();
  iEvent=0;

  foutPrim.open("primaries.dat");
  if (!foutPrim) Fatal("Init","Unable to open primaries.dat");

  hits=NULL;
  hitArrayCapacity=0;

  return kSUCCESS;
}


/** Virtual method Exec **/
void R3BLandDigiAnalyser::Exec(Option_t* opt)
{
  if (iVerbose > 1)
    cout << "Event: " << iEvent << endl;

    Int_t nEntries = fLandDigi->GetEntries();
    if (iVerbose > 2)
      cout << "Entries: " << nEntries << endl;

    // Check if all primaries have reached LAND
    std::string primariesData="";
    primariesData = getPrimariesData();

     Int_t nValidHits=0;

    // If all primaries has reached LAND continue to look at the hits
    if (primariesData != "" && nEntries > 0)
    {
      Double_t qdcTot=0, qdcL, qdcR, qdcPaddle;
      R3BLandDigi *digi_obj = (R3BLandDigi*) fLandDigi->At(0);
      Double_t minHitTime = (digi_obj->GetTdcL()+digi_obj->GetTdcR())*0.5;
      Int_t indexMinHitTime = 0;
      Double_t firstEnergy = 0;
      bool hitOK;
 

      if (nEntries > hitArrayCapacity)
      {
        hits = (hitData*) realloc(hits, sizeof(hitData)*nEntries);

        hitArrayCapacity = nEntries;
        if (hits==NULL)
          Fatal("R3BLandDigiAnalyser:Exec", "Error (re)allocating memory");
      }

      // Loop through the hits from the CFD
      for (Int_t iEntry=0; iEntry<nEntries; iEntry++)
      {
        digi_obj = (R3BLandDigi*) fLandDigi->At(iEntry);
        qdcL = digi_obj->GetQdcL();
        qdcR = digi_obj->GetQdcR();

        Double_t hitTime = (digi_obj->GetTdcL() + digi_obj->GetTdcR())*0.5;

        // Time for the light to travel through half a paddle
        hitTime-=paddle_dimx/2.0*v_eff_inv;

        // If a valid hit add to hit-vector
        if (!TMath::IsNaN(hitTime))
        {
          hits[nValidHits].tdcL=digi_obj->GetTdcL();
          hits[nValidHits].tdcR=digi_obj->GetTdcR();

          hits[nValidHits].t=hitTime;
          hits[nValidHits].paddleNb=digi_obj->GetPaddleNr();

          hitOK=calculatePosition(&hits[nValidHits]);

          if(hitOK)
          {
            qdcPaddle = TMath::Sqrt(qdcL*qdcR)*exp(att*paddle_dimx/2);
            hits[nValidHits].E=qdcPaddle;
            qdcTot += qdcPaddle;
                           
            if (iVerbose > 2)
            {
              // Plot the difference between actual and reconstructed hit
/* this has to be changed. The first hits are now in R3BLandFirstHits and not anymore in R3BLandDigi	
//comment by M. Heil      
              diffHists[0]->Fill(hits[nValidHits].x-digi_obj->GetX0());
              diffHists[1]->Fill(hits[nValidHits].y-digi_obj->GetY0());
              diffHists[2]->Fill(hits[nValidHits].z-digi_obj->GetZ0()+landPlacementZ-(nPlanes/2*paddle_dimz +(nPlanes-1)/2*paddle_spacing));
              diffHists[3]->Fill(hits[nValidHits].t-digi_obj->GetT0());
*/              
              double vel= TMath::Sqrt(hits[nValidHits].x * hits[nValidHits].x + 
                                      hits[nValidHits].y * hits[nValidHits].y +
                                      hits[nValidHits].z * hits[nValidHits].z +landPlacementZ)/hits[nValidHits].t;

              
              digiHists[1]->Fill(qdcPaddle);
            }
            nValidHits++;
          }
        }
        if (iVerbose > 2)
          digiHists[0]->Fill(hitTime);
        
        // Find the time and energy of first hit
        if (hitTime <= minHitTime || TMath::IsNaN(minHitTime))
        { 
          minHitTime=hitTime;
          indexMinHitTime = iEntry;
          firstEnergy=qdcPaddle;
        }
      }
      if (iVerbose > 2)
      {
        digiHists[2]->Fill(qdcTot);
        digiHists[3]->Fill(nEntries);
        digiHists[4]->Fill(firstEnergy);
      }

//    checkForPossibleGhostHits(nEntries);
//      delete [] hits;
  }

  // If the event has valid hits, store the hits and the primaries to files
  if(emptyEventsAccepted || nValidHits > 0)
  {
    printHitsForTracker(nValidHits);
    foutPrim << primariesData;
  }
  //	double depositedEnergy = energyData();
  //	hitAnalysis();
  //	exitingData(depositedEnergy);
  
  iEvent++;
}

// Plot and save Histograms
void R3BLandDigiAnalyser::Finish()
{
  if(iVerbose > 2)
  {
    TCanvas *digiPlots = new TCanvas("digiPlots");
    digiPlots->Divide(3,2);
    for(int i=0; i < 5; i++)
    {
      digiPlots->cd(i+1);
      digiHists[i]->Draw();
    }

    TCanvas *diffPlots = new TCanvas("diffPlots");
    diffPlots->Divide(2,2);
    for(int i; i < 4; i++)
    {
      diffPlots->cd(i+1);
      diffHists[i]->Draw();
    }
  }
  foutScen.close();
  foutPrim.close();
}


/** Private method initialisePlots **/
void R3BLandDigiAnalyser::initialisePlots()
{
  digiHists = new TH1F*[5];

  digiHists[0] = new TH1F("PMtime","Arrival times of PMs",1000,0.,100.);
  digiHists[0]->GetXaxis()->SetTitle("Time (ns)");
  digiHists[0]->GetYaxis()->SetTitle("Counts");

  digiHists[1] = new TH1F("PaddleEnergy","Energy deposit in one paddle",500,0.,50.);
  digiHists[1]->GetXaxis()->SetTitle("Energy (MeV)");
  digiHists[1]->GetYaxis()->SetTitle("Counts");

  digiHists[2] = new TH1F("particleEnergy","Energy deposit in Land",1000,0.,1000.);
  digiHists[2]->GetXaxis()->SetTitle("Energy (MeV)");
  digiHists[2]->GetYaxis()->SetTitle("Counts");

  digiHists[3] = new TH1F("Multiplicity1","Paddle multiplicity (any PM)",20,-0.5,19.5);
  digiHists[3]->GetXaxis()->SetTitle("Multiplicity");
  digiHists[3]->GetYaxis()->SetTitle("Counts");

  digiHists[4] = new TH1F("FirstEnergy","Energy of first hit",500,0.,50);
  digiHists[4]->GetXaxis()->SetTitle("Energy (MeV)");
  digiHists[4]->GetYaxis()->SetTitle("Counts");

  /***** difference Hists *****/
  diffHists = new TH1F*[4];
  
  diffHists[0] = new TH1F("xdiff", "Difference in x-axis", 100,-50,50);
  diffHists[0]->GetXaxis()->SetTitle("length (cm)");
  diffHists[0]->GetYaxis()->SetTitle("Counts");
  
  diffHists[1] = new TH1F("ydiff", "Difference in y-axis", 100,-50,50);
  diffHists[1]->GetXaxis()->SetTitle("length (cm)");
  diffHists[1]->GetYaxis()->SetTitle("Counts");
  
  diffHists[2] = new TH1F("zdiff", "Difference in z-axis", 500,-100,100);
  diffHists[2]->GetXaxis()->SetTitle("length (cm)");
  diffHists[2]->GetYaxis()->SetTitle("Counts");
  
  diffHists[3] = new TH1F("tdiff", "Difference in t-axis", 100,-10,10);
  diffHists[3]->GetXaxis()->SetTitle("time (ns)");
  diffHists[3]->GetYaxis()->SetTitle("Counts");
}

// Print hits to file for aco to analyse
void R3BLandDigiAnalyser::printHitsForTracker(int nHits)
{	
  for(int i=0; i < nHits; i++)
  {
    foutScen << "N " << hits[i].x << " " << hits[i].y << " " << hits[i].z 
         << " "  << hits[i].t << " " << hits[i].E << endl;
  }
  foutScen << "Event " << iEvent << endl;
}

// Check if all primaries have reached LAND
// and return primary data
std::string R3BLandDigiAnalyser::getPrimariesData()
{
  int nEntries=fLandMCTrack->GetEntries();
  int nPrimaries=0, nPrimariesInLand=0;
  Double_t firstDaughterTime;
  bool daughterFound;

  Double_t halfPaddleLength = paddle_dimx/(Double_t)2;
  Double_t halfPaddlez = paddle_dimz/(Double_t)2+paddle_spacing;
  Double_t epsilon = 0.1;

  std::stringstream soutPrim, soutPrimTemp;
  R3BMCTrack* mcTrack;
  for(Int_t iEntry = 0; iEntry< nEntries; iEntry++)
  {
    mcTrack = (R3BMCTrack*) fLandMCTrack->At(iEntry);
    
    // Check if primary
    if(mcTrack->GetMotherId() == -1)
    {
      nPrimaries++;
      firstDaughterTime=1E6;
      daughterFound =false;

      // Look for particle with our primary as mother
      for (Int_t index = iEntry+1; index < nEntries; index++)
      {
        R3BMCTrack* temp = (R3BMCTrack*) fLandMCTrack->At(index);
        
        if (temp->GetMotherId() == iEntry && temp->GetStartT() < firstDaughterTime)
        {
          // Check if it is a landHit
          bool inX = temp->GetStartX() > -halfPaddleLength-epsilon && temp->GetStartX() < halfPaddleLength+epsilon;
          bool inY = temp->GetStartY() > -halfPaddleLength-epsilon && temp->GetStartY() < halfPaddleLength+epsilon;
          bool inZ = temp->GetStartZ() > landPlacementZ-(nPlanes/2*paddle_dimz +(nPlanes-1)/2*paddle_spacing)-epsilon && 
            temp->GetStartZ() < landPlacementZ +(nPlanes/2*paddle_dimz +(nPlanes-1)/2*paddle_spacing) + epsilon;

          if (inX && inY && inZ)
          {
            soutPrimTemp.str("");
            // Take the position of our primary hit
            soutPrimTemp << "N " << temp->GetStartX() << " ";
            soutPrimTemp << temp->GetStartY() << " ";
            soutPrimTemp << temp->GetStartZ() - landPlacementZ + (nPlanes/2*paddle_dimz +(nPlanes-1)/2*paddle_spacing) << " "; // To get origin at the front of LAND
            soutPrimTemp << temp->GetStartT() << " ";
            soutPrimTemp << (temp->GetEnergy()-temp->GetMass())*1000 << endl;
            if (! daughterFound)
            {
              nPrimariesInLand++;
              daughterFound=true;
            }
          }
        }
      }
      soutPrim << soutPrimTemp.str();
    }
  }

  // Check if all mothers has entered LAND
  if (emptyEventsAccepted || nPrimaries == nPrimariesInLand)
  {
    soutPrim << "Event " << iEvent << endl;
    return soutPrim.str();
  }
  else
    return "";
}

// Calculate the position from TDC data
bool R3BLandDigiAnalyser::calculatePosition(hitData* hit)
{
  int plane = hit->paddleNb/nPaddlesPerPlane;
  int paddleInPlane = hit->paddleNb % nPaddlesPerPlane;
  hit->z=(plane + 0.5)*paddle_dimz + plane*paddle_spacing;
  // Horisontal
  if(plane % 2 == 0)
  {
    hit->x=(hit->tdcL-hit->tdcR)/2*v_eff;
    hit->y=(paddleInPlane + 0.5)*paddle_dimy + paddleInPlane*paddle_spacing - (double)nPaddlesPerPlane/2*paddle_dimy - (((double)nPaddlesPerPlane-1)/2)*paddle_spacing;
  }
  // Vertical
  else
  {
    hit->x=(paddleInPlane + 0.5)*paddle_dimy + paddleInPlane*paddle_spacing - (double)nPaddlesPerPlane/2*paddle_dimy - (((double)nPaddlesPerPlane-1)/2)*paddle_spacing;
    hit->y=(hit->tdcL-hit->tdcR)/2*v_eff;
  }

  Double_t halfPaddleLength = paddle_dimx/(Double_t)2;
  Double_t epsilon = 0.001;

  // Check if it is a landHit
  bool inX = hit->x > -halfPaddleLength-epsilon && hit->x < halfPaddleLength+epsilon;
  bool inY = hit->y > -halfPaddleLength-epsilon && hit->y < halfPaddleLength+epsilon;
  bool inZ = hit->z > 0.0 - epsilon && hit->z < nPlanes*paddle_dimz +(nPlanes-1)*paddle_spacing + epsilon;

  return inX && inY && inZ;  
}


/***** Private method checkForPossibleGhosthits *****/
void R3BLandDigiAnalyser::checkForPossibleGhostHits(int nHits)
{
  int nEntries=fLandPoints->GetEntries();
  bool ghost=true;
  int lastGhostPaddle=-1;
  if (nEntries > 0)        
  {
    R3BMCTrack* mcTrack;
    R3BLandPoint *land_obj;
    for (int iEntry = 0; iEntry < nEntries-1; iEntry++)
    {
      land_obj = (R3BLandPoint*) fLandPoints->At(iEntry);
      int paddleNb=land_obj->GetPaddleNb();
      int trackId = land_obj->GetTrackID();
      
      ghost = false;
      R3BLandPoint *nextPoint = (R3BLandPoint*) fLandPoints->At(iEntry+1);
      int nextPointID = nextPoint->GetTrackID();
      int nextPointPaddleNb = nextPoint->GetPaddleNb();
      mcTrack = (R3BMCTrack*) fLandMCTrack->At(nextPointID);
      if (paddleNb != lastGhostPaddle)
      {
        bool nextPointDaughter = (nextPointID  == trackId 
                                || mcTrack->GetMotherId() == trackId );

        bool nextRegistered = false;
        for (int i=0; i < nHits; i++)
        {
          if (hits[i].paddleNb == nextPointPaddleNb)
            nextRegistered = true;
        }
        
        if(nextPointDaughter && nextRegistered)
        {
          ghost=true;
          for(int i=0; i < nHits; i++)
          {
            if (hits[i].paddleNb == paddleNb)
            {
              ghost=false;
              break;
            }
          }
        }

        if (ghost)
        {
          double daughterE = (mcTrack->GetEnergy()-mcTrack->GetMass())*1000;
          if (daughterE > 1)
          {
            cout << "Unrecognized hit in paddle " << paddleNb << endl;
//          cout << "\tTrackID: " << trackId << endl;
            cout << "\tDaughter energy: " << daughterE << " MeV\n";
            lastGhostPaddle = paddleNb;
          }
        }
      }
    }
  }
}

ClassImp(R3BLandDigiAnalyser)
