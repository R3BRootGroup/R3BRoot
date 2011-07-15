// -------------------------------------------------------------------------
// -----         Based on FairAsciiGenerator source file               -----
// -----          Created 30/09/2010  by M. Labiche                    -----
// -------------------------------------------------------------------------

//
// This event generator reads the ASCII output file of the (p,2p) event 
// generator from Leonid Chulkov 
//
//

#include "R3Bp2pGenerator.h"

#include "FairPrimaryGenerator.h"
#include "FairMCEventHeader.h"

#include "TDatabasePDG.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   ------------------------------------------
R3Bp2pGenerator::R3Bp2pGenerator() 
:fInputFile(0),
fFileName("")
{
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------
R3Bp2pGenerator::R3Bp2pGenerator(const char* fileName               ) 
:fInputFile(0),
fFileName( fileName)
{
  fFileName  = fileName;
  cout << "-I R3Bp2pGenerator: Opening input file " << fileName << endl;
  fInputFile = new ifstream(fFileName);
  if ( ! fInputFile->is_open() ) 
    Fatal("R3Bp2pGenerator","Cannot open input file.");
    
  // fPDG=TDatabasePDG::Instance();
 
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------
R3Bp2pGenerator::~R3Bp2pGenerator() {
  CloseInput();
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------
Bool_t R3Bp2pGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

  // Check for input file
  if ( ! fInputFile->is_open() ) {
    cout << "-E R3Bp2pGenerator: Input file not open!" << endl;
    return kFALSE;
  }


  // Define event variable to be read from file
  Double_t vx = 0., vy = 0., vz = 0.;
  Int_t pdgID = 2212;  // =protons
  

  // Define track variables to be read from file
  Double_t pX1, pY1, pZ1 ;
  Double_t pXA, pYA, pZA ;
  Double_t pXL, pYL, pZL ;
  Double_t AMDiff;
  Double_t T1,Theta1,Phi1;
  Double_t TA,ThetaA,PhiA;
  Double_t TB,ThetaB,PhiB;
  Double_t InvMass, t;



  // Read event header line from input file
     *fInputFile >> pX1 >> pY1 >> pZ1 >> pXA >> pYA >> pZA >> pXL >> pYL >> pZL>> AMDiff >>  T1 >> Theta1 >> Phi1 >> TA >> ThetaA >> PhiA >> TB >> ThetaB >> PhiB >> InvMass >> t ;


    pX1=pX1/1000.;
    pY1=pY1/1000.;
    pZ1=pZ1/1000.;
    pXA=pXA/1000.;
    pYA=pYA/1000.;
    pZA=pZA/1000.;
   
    Theta1=Theta1*180./3.14159;
    ThetaA=ThetaA*180./3.14159;
    Phi1=Phi1*180./3.14159;
    PhiA=PhiA*180./3.14159;



  // If end of input file is reached : close it and abort run
  if ( fInputFile->eof() ) {
    cout << "-I R3Bp2pGenerator: End of input file reached " << endl;
    CloseInput();
    return kFALSE;
  }


  // Loop over tracks in the current event
    // convert Geant3 code to PDG code
    
   // Int_t pdg= fPDG->ConvertGeant3ToPdg(pdgID);
    
    // Give track to PrimaryGenerator
    primGen->AddTrack(pdgID, pX1, pY1, pZ1, vx, vy, vz);
    primGen->AddTrack(pdgID, pXA, pYA, pZA, vx, vy, vz);

    FairMCEventHeader* event=primGen->GetEvent();
    if ( event && (! event->IsSet()) ) {
      //     event->SetEventID(eventID);
     event->SetTheta1(Theta1);
     event->SetTheta2(ThetaA);
     event->SetPhi1(Phi1);
     event->SetPhi2(PhiA);
     event->MarkSet(kTRUE);
    }

  return kTRUE;
}
// ------------------------------------------------------------------------



// -----   Private method CloseInput   ------------------------------------
void R3Bp2pGenerator::CloseInput() {
  if ( fInputFile ) {
    if ( fInputFile->is_open() ) {
       cout << "-I R3Bp2pGenerator: Closing input file " 
	    << fFileName << endl;
       fInputFile->close();
    }
    delete fInputFile;
    fInputFile = NULL;
  }
}
// ------------------------------------------------------------------------


ClassImp(R3Bp2pGenerator)

