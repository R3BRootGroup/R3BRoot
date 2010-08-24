#ifndef R3BPRIMARYGENERATOR_H
#define R3BPRIMARYGENERATOR_H

#include "FairPrimaryGenerator.h"
#include "FairGenericStack.h"

#include "TNamed.h"
#include "TObjArray.h"
#include "TVector3.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "THashList.h"
#include "TDecayChannel.h"

#include "TRandom.h"
#include "TMath.h"
#include "TF1.h"

#include "TVirtualMC.h"
#include "TMCParticleType.h"

#include "Riostream.h"
#include <iostream>
#include <stdio.h>


class R3BPrimaryGenerator : public FairPrimaryGenerator
{

 public:
     /** Default constructor. **/
     R3BPrimaryGenerator();

     /** Constructor with name and title **/
     R3BPrimaryGenerator(const char* name, const char *title="R3B Generator");

     /** Destructor. **/
     virtual ~R3BPrimaryGenerator();


     virtual  void AddTrack(Int_t pdgid, Double_t px, Double_t py, Double_t pz,
			    Double_t vx, Double_t vy, Double_t vz, Int_t parent=-1,Bool_t wanttracking=true,Double_t e=-9e9);



     Bool_t ExtendIon(Int_t pdgid);
     void   ExtendIon2(Int_t pdgid);
     //Updates the pdg base for event displayer
     void WriteTable(const char* filename);


  R3BPrimaryGenerator& operator= (const  R3BPrimaryGenerator&) {return *this;}

  ClassDef(R3BPrimaryGenerator,1);

};

#endif


