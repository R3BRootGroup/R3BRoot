// -------------------------------------------------------------------------
// -----            R3BCosmicGenerator source file                    -----
// -----          Created 09/09/04  by Marc Labiche                    -----
// -----          Derived from FairBoxGenerator.cxx                    -----
// -------------------------------------------------------------------------

/* $Id: R3BCosmicGenerator.cxx,v 1.0 2010/02/19 10:00  $ */


#include "R3BCosmicGenerator.h"
#include "FairPrimaryGenerator.h"

#include "TRandom.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TMath.h"

using std::cout;
using std::cerr;
using std::endl;


// ------------------------------------------------------------------------
R3BCosmicGenerator::R3BCosmicGenerator() :
  fPDGType(0),fMult(0),fE_Threshold(0),fPDGMass(0),
  fPhiMin(0),fPhiMax(0),fThetaMin(0),fThetaMax(0),fX(0),fY(0),fZ(0),
  fX1(0),fY1(0),fX2(0),fY2(0),
  fThetaRangeIsSet(0),
  fCosThetaIsSet(0),
  fPointVtxIsSet(0),fBoxVtxIsSet(0),fDebug(0)
{
  // Default constructor
}

// ------------------------------------------------------------------------
R3BCosmicGenerator::R3BCosmicGenerator(Int_t pdgid, Int_t mult, Double32_t E_Threshold) :
  fPDGType(pdgid),fMult(mult),fE_Threshold(E_Threshold),fPDGMass(0),
  fThetaMin(0),fThetaMax(0),fX(0),fY(0),fZ(0),
  fX1(0),fY1(0),fX2(0),fY2(0),
  fThetaRangeIsSet(0),
  fCosThetaIsSet(0),
  fPointVtxIsSet(0),fBoxVtxIsSet(0),fDebug(0)
{

  // Constructor. Set default kinematics limits
  SetPhiRange  ();
}
// ------------------------------------------------------------------------
Bool_t R3BCosmicGenerator::Init()
{
  // Initialize generator

  if (fPhiMax-fPhiMin>360)
    Fatal("Init()","R3BCosmicGenerator: phi range is too wide: %f<phi<%f",
	  fPhiMin,fPhiMax);
  if (fPointVtxIsSet && fBoxVtxIsSet)
    Fatal("Init()","R3BCosmicGenerator: Cannot set point and box vertices simultaneously");

  // Check for particle type
  TDatabasePDG* pdgBase = TDatabasePDG::Instance();
  TParticlePDG *particle = pdgBase->GetParticle(fPDGType);
  if (! particle) Fatal("R3BCosmicGenerator","PDG code %d not defined.",fPDGType);
  fPDGMass = particle->Mass();
  return kTRUE;
}

//---------------------------------------------------------------------------
Double32_t R3BCosmicGenerator::E_Cosmic()
{

//    ------------------------------------------------------------------
//    I                                                                I
//    I   This function calculates random energy of a cosmic muon      I
//    I   according to the energy distribution of cosmic rays flux     I
//    I   published by J.F.Ziegler, Nucl. Inst. Meth 191,419 (1981);   I
//    I   see also R.C.Fernow, Introduction to Experimental Particle   I
//    I   Physics, Cambridge Univesity Press, 1986 (4.5).              I
//    I   The data points have been extracted from Fig.1.              I
//    I   It is assumed, that above 100 GeV the slope does not change. I
//    I                                                                I
//    I   In the first CALL E (in GeV) is used as a low energy         I
//    I   threshold.                                                   I
//    I                                                                I
//    I    PARAMETERs:                                                 I
//    I      E = random muon energy in GeV (low energy threshold       I
//    I          during the first CALL)                                I
//    I                                                                I
//    I                                                                I
//    I      T. Matulewicz        GANIL     NOVEMBER 1991  (FORTRAN)   I 
//    I      M. Labiche         Daresbury   FEBRUARY 2010  (C++)       I
//    I                                                                I
//    ------------------------------------------------------------------

  Int_t N_PNT=14;
  Int_t i_low = 0, i_RN;
  Int_t i,k;

  Double32_t SUM_INT;
  Double32_t POWR[14], FACT[14], X_INT[14];
  Double32_t E_PNT[14]={.004,.01,.02,.04,.1,.2,.4,1.,2.,4.,10.,20.,40.,100}; 
  Double32_t F_PNT[14]={5.89E-7,1.05E-6,1.63E-6,2.34E-6,3.13E-6,3.89E-6,
		      4.18E-6,2.02E-6,9.11E-7,3.81E-7,1.11E-7,3.00E-8,3.40E-9,9.01E-11}; 

  Double32_t Energy;

  Bool_t ini=false;

  Double32_t RN;


//initialisation:
  if(!ini){


    for(i_low=0; i_low<N_PNT; i_low++){
     if(fE_Threshold<=E_PNT[i_low])goto skip1;
    }
    //    cout << " The energy threshold must be lower than 100 GeV " << endl;
    return 0;
    
  skip1:  /*i_low=i_low*/;

    if(i_low==0){
      fE_Threshold=E_PNT[0];
    }

    
    for(i=0; i<(i_low-1); i++)
      {
	POWR[i]=0.;
	FACT[i]=0.;
	X_INT[i]=0.;
      }

    SUM_INT = 0.;


    for(i=i_low; i<(N_PNT-1); i++){   // i=0-12

      POWR[i] = log(F_PNT[i+1]/F_PNT[i])/log(E_PNT[i+1]/E_PNT[i]);
      FACT[i] = F_PNT[i]/pow(E_PNT[i],POWR[i]);
      POWR[i] = POWR[i] + 1.;
      X_INT[i] = FACT[i]*(pow(E_PNT[i+1],POWR[i])-pow(E_PNT[i],POWR[i]))/POWR[i];
      SUM_INT = SUM_INT + X_INT[i];

    }
    
    // i=13:
    FACT[i] = FACT[i-1];
    POWR[i] = POWR[i-1];
    X_INT[i] = -FACT[i]*pow(E_PNT[i],POWR[i])/POWR[i];
    SUM_INT = SUM_INT + X_INT[i];

    for(i=i_low; i<N_PNT; i++){

      X_INT[i] = X_INT[i]/SUM_INT;
      FACT[i] = FACT[i] /SUM_INT;

    } 
      

    for(i=N_PNT-1; i>i_low; i--){

      for( k=i_low; k<i; k++){
        
	X_INT[i] = X_INT[i] + X_INT[k];
      }
    }

    X_INT[N_PNT-1] = 1.0; // X_INT[13]


    ini = true;
  }
  // end of initialisation

       RN=( (Double_t)rand() / ((Double_t)(RAND_MAX)+(Double_t)(1)) ); // RandNb between [0,1[

       for(i_RN=i_low;i_RN<N_PNT;i_RN++)
	 {
	   if(RN<=X_INT[i_RN])goto skip2;
	 }

    skip2: 
       if(i_RN > 1) RN=RN-X_INT[i_RN-1];

       Energy=RN*POWR[i_RN]/FACT[i_RN]+pow(E_PNT[i_RN],POWR[i_RN]);

       Energy=pow(Energy,(1./POWR[i_RN]));

       return (Energy);

}


// ------------------------------------------------------------------------
Bool_t R3BCosmicGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
  // Generate one event: produce primary particles emitted from one vertex.
  // Primary particles are distributed uniformly along
  // those kinematics variables which were limitted by setters.
  // if SetCosTheta() function is used, the distribution will be uniform in 
  // cos(theta)

  Double32_t phi, KE=0, theta=0, ptot=0, px, py, pz=0;


  // Generate particles
  for (Int_t k = 0; k < fMult; k++) {

    KE=E_Cosmic();  // in GeV

    //cout << "KE= " << KE << " fPDGMass " << fPDGMass << endl;

    phi = gRandom->Uniform(fPhiMin,fPhiMax) * TMath::DegToRad();

    if      (fThetaRangeIsSet) {
      if (fCosThetaIsSet)
	theta = acos(gRandom->Uniform(cos(fThetaMin* TMath::DegToRad()),
                cos(fThetaMax* TMath::DegToRad())));
      else
	theta = gRandom->Uniform(fThetaMin,fThetaMax) * TMath::DegToRad();
    }
    

    ptot=TMath::Sqrt(KE*KE+2*fPDGMass*KE);

    px = ptot*TMath::Sin(theta)*TMath::Cos(phi);
    py = ptot*TMath::Sin(theta)*TMath::Sin(phi);
    pz = ptot*TMath::Cos(theta);

    if (fBoxVtxIsSet) {
      fX = gRandom->Uniform(fX1,fX2);
      fY = gRandom->Uniform(fY1,fY2);
    }

    if (fDebug)
      printf("CosmicGen: kf=%d, p=(%.2f, %.2f, %.2f) GeV, x=(%.1f, %.1f, %.1f) cm\n",
	     fPDGType, px, py, pz, fX, fY, fZ);

    //primGen->AddTrack(fPDGType, px, py, pz, fX, fY, fZ);
    primGen->AddTrack(fPDGType, px, py, pz, fX, fZ, fY);  // swap fY<->fZ to generate muons from space
  }
  return kTRUE;

}
// ------------------------------------------------------------------------


ClassImp(R3BCosmicGenerator)
