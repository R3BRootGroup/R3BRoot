// -------------------------------------------------------------------------
// -----                        R3BCalifa source file                  -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -----	     Last modification 15/12/16 by P.Cabanelas         -----
// -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
//HAPOL-10/01/2014 (hector.alvarez@usc.es)
// Moving to the new geometry scheme where all TGeo structures loads from
// a precreated root file.
// Next geometries are possible:
//
// 0 - OLD CALIFA 5.0, including BARREL and ENDCAP:
//   NOTE: THERE IS NO WARRANTY THAT THIS VERSION WORKS AT ALL IN R3BROOT
//   Contains 30 different crystal types, repeated 64 times (64 copies)
//
// 1- CALIFA 7.05, only BARREL
//   Each ring is made of 40 alveoli of 4 crystals each.  There are 24 
//   alveoli along the polar angle for a total of 40x24=960 alveoli or 
//   3840 crystals. There are 12 different crystal shapes:
//     @alveoliType=(1,1,2,2,2,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,6,6,6);
//     Volumes: Alveolus_[1,24] made of CrystalWithWrapping_[1,6][A,B] 
//              made of Crystal_[1,6][A,B]
//
// 2- CALIFA 7.07, only BARREL
//   Each ring is made of 32 alveoli of 4 crystals each. There are 20 
//   alveoli along the polar angle for a total of 32x20=640 alveoli or 
//   2560 crystals. There are 16 different crystal shapes:
//     @alveoliType=(1,1,2,2,2,3,3,4,4,4,5,5,6,6,6,7,7,7,8,8);
//     Volumes: Alveolus_[1,20] made of CrystalWithWrapping_[1,8][A,B] 
//              made of Crystal_[1,8][A,B]
//
// 3- CALIFA 7.09, only BARREL (BP: benjamin.pietras@usc.es)
//   The first 16 rings are made of 32 alveoli of 4 crystals each. The 
//   last 3 rings are made of 32 alveoli of 1 crystal each. There are 
//   19 alveoli along the polar angle for a total of 32x19=608 alveoli 
//   or 2144 crystals. There are 11 different crystal shapes:
//     @alveoliType[19]={1,1,2,2,3,3,3,3,3,3,4,4,4,5,5,5,6,6,6};
//
// 4- CALIFA 7.17, only ENDCAP (in CsI[Tl])
//   Each ring is made of 32 alveoli of 8, 8 and 7 crystals each. There 
//   are 3 alveoli along the polar angle for a total of 32x3=96 alveoli or 
//   736 crystals. There are 23 different crystal shapes:
//     @alveoliType=(8,8,7);
//     Volumes: Alveolus_EC_[1,3] made of CrystalWithWrapping_[1,23] 
//              made of Crystal_[1,23]
//
// 5- CALIFA 7.07+7.17 CsI
//   See above the two components (2 & 4)
//
// 6- CALIFA 7.09+7.17 CsI
//   See above the two components (3 & 4) 
//   Use for LaBr-LaCl endcaps CLF717_Geometry_PhoswichEndcap_5.geo
//
// 7- CALIFA 717PHOSWICH, only phoswich ENDCAP (JSDR: josesrs@gmail.com) 
//   (CLF717_Geometry_PhoswichEndcap_1,2.geo)  We can add here the stand 
//   alone phoswich endcap of LaBr and LaCl scintillator crystals. The 
//   endcap that is adapted to the barrel CLF707 is 
//   CLF717_Geometry_PhoswichEndcap_1.geo and the one adapts to the barrel 
//   CLF811 is CLF717_Geometry_PhoswichEndcap_2.geo
//   The first 10 rings are made of 60 alveoli of 60 crystals each ring 
//   (azimuthal plane). The other 5 rings are made of 30 alveoli of 30 
//   crystals each ring. There are 15 alveoli along the polar angle for a 
//   total of 10x60+5x30= 750 alveoli or 1500 crystals (750 phoswich 
//   crystals). There are 30 different crystal shapes:
//     @alveoliType=(2,2,2,2,2,2,2,2,2,2,2,2,2,2,2);
//	 Volumes: Alveolus_EC_[1,10] made of CrystalWithWrapping_[1,60]
//              made of Crystal_[1,60] and 
//              Alveolus_EC_[10,15] made of CrystalWithWrapping_[1,30] 
//              made of Crystal_[1,30]
//
// 8- CALIFA 7.07+7.17PHOSWICH 
//   See above the two components (2 & 7)
//   Phoswich LaBr-LaCl endcaps: If we want the phoswich endcap adapted 
//   to the barrel CLF707, we use: CLF717_Geometry_PhoswichEndcap_3.geo, 
//   and with the barrel CLF811, CLF717_Geometry_PhoswichEndcap_4.geo
//
// 9- CALIFA 7.09+7.17PHOSWICH
//   See above the two components (3 & 7) 
//   Use for LaBr-LaCl endcaps CLF717_Geometry_PhoswichEndcap_5.geo
//
// 10- CALIFA 8.11, only BARREL
//   The first 15 rings are made of 32 alveoli of 4 crystals each. The 
//   last ring are made of 32 alveoli of 1 crystal each. There are 16 
//   alveoli along the polar angle for a total of 32x16=512 alveoli and 
//   32x15x4+32=1952 crystals. There are 11 (actually 5x2+1) different 
//   crystal shapes:
//
// 11- PHOSWICH ENDCAP ADAPTED TO CALIFA BARREL 8.11
//   It is adapted to the IEM-CSIC Madrid LaBr3-LaCl3 Phoswich 
//   Endcap (CLF717_Geometry_PhoswichEndcap_6.geo)
// 
// 12- CALIFA 8.11 +7.17 CsI
//
// 13- CALIFA 8.13d, only BARREL
//
// 14-  
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Pablo Cabanelas - 15-12-2016:
// Changing classes name from Calo to Califa acording to name convention
//
////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>

#include "R3BCalifa.h"

#include "R3BCalifaPoint.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaCrystalCalDataSim.h"
#include "R3BMCStack.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TMCProcess.h"

// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TVirtualMCStack.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoTube.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"

using std::cout;
using std::cerr;
using std::endl;

#define U_MEV 931.4940954

inline double BETA(const double M, const double E_kin)
{
  return sqrt(1. - M*M/((M+E_kin)*(M+E_kin)));
}

inline double GAMMA(const double M, const double E_kin)
{
  return 1. + E_kin/M;
}

// -----   Default constructor   -------------------------------------------
R3BCalifa::R3BCalifa() : R3BDetector("R3BCalifa", kTRUE, kCALIFA)
{
  ResetParameters();
  fCrystal = NULL;
  fCaloCollection = new TClonesArray("R3BCalifaPoint");
  fCaloCrystalHitCollection = new TClonesArray("R3BCalifaCrystalCalDataSim");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fNonUniformity = 0.;
  fGeometryVersion = 1;

}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCalifa::R3BCalifa(const char* name, Bool_t active)
    : R3BDetector(name, active, kCALIFA)
{
  ResetParameters();
  fCrystal = NULL;
  fCaloCollection = new TClonesArray("R3BCalifaPoint");
  fCaloCrystalHitCollection = new TClonesArray("R3BCalifaCrystalCalDataSim");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fNonUniformity = 0.;
  fGeometryVersion = 1;

//  tf_p_dNs = new TF1("tf_p_dNs","-[0]*[1]*exp(-[1]*(x-[3]))+[2]",0,1000);
//  tf_p_dNf = new TF1("tf_p_dNf","-[0]*[1]*exp(-[1]*(x-[3]))+[2]",0,1000);
//  tf_g_dNs = new TF1("tf_g_dNs","[2]",0,1000);
//  tf_g_dNf = new TF1("tf_g_dNf","[2]",0,1000);
//
//  tf_p_dNs->SetParameter(0,18.88 / 7.46);
//  tf_p_dNs->SetParameter(1,0.0868);
//  tf_p_dNs->SetParameter(2,4.228 / 7.46);
//  tf_p_dNs->SetParameter(3,0);
//
//  tf_p_dNf->SetParameter(0,-18.88 / 7.46);
//  tf_p_dNf->SetParameter(1,0.0868);
//  tf_p_dNf->SetParameter(2,3.232 / 7.46);
//  tf_p_dNf->SetParameter(3,0);
//
//  // tf_p_dNs->SetParameter(0,18.88);
//  // tf_p_dNs->SetParameter(1,0.0868);
//  // tf_p_dNs->SetParameter(2,4.228);
//  // tf_p_dNs->SetParameter(3,4.117);
//  // tf_p_dNs->SetParameter(4,4.259);
//
//  // tf_p_dNf->SetParameter(0,-32.66);
//  // tf_p_dNf->SetParameter(1,0.07729);
//  // tf_p_dNf->SetParameter(2,3.155);
//  // tf_p_dNf->SetParameter(3,0);
//  // tf_p_dNf->SetParameter(4,-3.947);
//
//  tf_g_dNs->SetParameter(2, tf_p_dNs->GetParameter(2));
//  tf_g_dNf->SetParameter(2, tf_p_dNf->GetParameter(2));

  tf_dNf_dE = new TF1("tf_dNf_dE", "1./([0]+[1]*(x^[2])+[3]/(x^[4]))");
  tf_dNs_dE = new TF1("tf_dNs_dE", "1./([0]+[1]*(x^[2])+[3]/(x^[4]))");

  tf_dNf_dE->SetParameters(-1.79, 1.36e-2, 7.84e-1, 4.97, 1.75e-1);
  tf_dNs_dE->SetParameters(-1.24e2, 6.3e-3, 1.27, 1.262e2, 2.3e-3);

}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
R3BCalifa::R3BCalifa(const char* name,
                 TString geoFile,
                 Bool_t active,
                 Float_t x,
                 Float_t y,
                 Float_t z,
                 Float_t rot_x,
                 Float_t rot_y,
                 Float_t rot_z)
    : R3BDetector(name, active, kCALIFA)
{
    ResetParameters();
    fCrystal = NULL;
    fCaloCollection = new TClonesArray("R3BCalifaPoint");
    fCaloCrystalHitCollection = new TClonesArray("R3BCalifaCrystalCalDataSim");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName( GetName());
    fVerboseLevel = 1;
    fNonUniformity = 0.;
    fGeometryVersion = 1;

    SetGeometryFileName(geoFile);
    SetPosition(x, y, z);
    SetRotation(rot_x, rot_y, rot_z);

//  tf_p_dNs = new TF1("tf_p_dNs","-[0]*[1]*exp(-[1]*(x-[3]))+[2]",0,1000);
//  tf_p_dNf = new TF1("tf_p_dNf","-[0]*[1]*exp(-[1]*(x-[3]))+[2]",0,1000);
//  tf_g_dNs = new TF1("tf_g_dNs","[2]",0,1000);
//  tf_g_dNf = new TF1("tf_g_dNf","[2]",0,1000);
//
//  tf_p_dNs->SetParameter(0,18.88 / 7.46);
//  tf_p_dNs->SetParameter(1,0.0868);
//  tf_p_dNs->SetParameter(2,4.228 / 7.46);
//  tf_p_dNs->SetParameter(3,0);
//
//  tf_p_dNf->SetParameter(0,-18.88 / 7.46);
//  tf_p_dNf->SetParameter(1,0.0868);
//  tf_p_dNf->SetParameter(2,3.232 / 7.46);
//  tf_p_dNf->SetParameter(3,0);
//
//  // tf_p_dNs->SetParameter(0,18.88);
//  // tf_p_dNs->SetParameter(1,0.0868);
//  // tf_p_dNs->SetParameter(2,4.228);
//  // tf_p_dNs->SetParameter(3,4.117);
//  // tf_p_dNs->SetParameter(4,4.259);
//
//  // tf_p_dNf->SetParameter(0,-32.66);
//  // tf_p_dNf->SetParameter(1,0.07729);
//  // tf_p_dNf->SetParameter(2,3.155);
//  // tf_p_dNf->SetParameter(3,0);
//  // tf_p_dNf->SetParameter(4,-3.947);
//
//  tf_g_dNs->SetParameter(2, tf_p_dNs->GetParameter(2));
//  tf_g_dNf->SetParameter(2, tf_p_dNf->GetParameter(2));

    tf_dNf_dE = new TF1("tf_dNf_dE", "1./([0]+[1]*(x^[2])+[3]/(x^[4]))");
    tf_dNs_dE = new TF1("tf_dNs_dE", "1./([0]+[1]*(x^[2])+[3]/(x^[4]))");

    tf_dNf_dE->SetParameters(-1.79, 1.36e-2, 7.84e-1, 4.97, 1.75e-1);
    tf_dNs_dE->SetParameters(-1.24e2, 6.3e-3, 1.27, 1.262e2, 2.3e-3);

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BCalifa::~R3BCalifa()
{

  if ( flGeoPar ) delete flGeoPar;
  if (fCaloCollection) {
    fCaloCollection->Delete();
    delete fCaloCollection;
  }
  if (fCaloCrystalHitCollection) {
    fCaloCrystalHitCollection->Delete();
    delete fCaloCrystalHitCollection;
  }
//  delete tf_p_dNs;
//  delete tf_p_dNf;
//  delete tf_g_dNs;
//  delete tf_g_dNf;
  delete tf_dNf_dE;
  delete tf_dNs_dE;
}
// -------------------------------------------------------------------------
void R3BCalifa::Initialize()
{
  FairDetector::Initialize();

  LOG(INFO) << "R3BCalifa: initialisation" << FairLogger::endl;
  LOG(DEBUG) << "-I- R3BCalifa: Vol (McId) def" << FairLogger::endl;

  TGeoVolume *vol = gGeoManager->GetVolume("CalifaWorld");
  vol->SetVisibility(kFALSE);
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BCalifa::ProcessHits(FairVolume* vol)
{
  // While tracking a single particle within a crystal (volume)
  // we can rely on the latest crystal information for each step
  if(gMC->IsTrackEntering() || fCrystal == NULL)
  {
    // Try to get crystal information from hash table
    // TODO: Still a performance benefit to use hash table?
    gGeoManager->cd(gMC->CurrentVolPath());
    Int_t nodeId = gGeoManager->GetNodeId();
    std::map<Int_t, sCrystalInfo>::iterator it = fCrystalMap.find(nodeId);
    if(it == fCrystalMap.end())
    {
      // Not found in map => Create crystal information for crystal
      sCrystalInfo tmpInfo;
      memset(&tmpInfo, 0, sizeof(sCrystalInfo));
      if(GetCrystalInfo(tmpInfo))
      {
        fCrystal = &(fCrystalMap[nodeId] = tmpInfo);
        fCrystal->density = gGeoManager->GetCurrentVolume()->GetMaterial()->GetDensity();
      }
      else
        fCrystal = NULL;
    }
    else
      fCrystal = &(it->second);
  }

  if(fCrystal == NULL)
  {
    // Still no crystal info at this point?
    // -> Something went wrong, but the user should
    //    already have been informed
    // -> Silently bail out
    return kFALSE;
  }
  
  if (fVerboseLevel>1)
    LOG(INFO) << "R3BCalifa: Processing Points in Alveolus Nb " 
	      << fCrystal->volIdAlv << ", copy Nb " << fCrystal->cpAlv
	      << ", crystal copy Nb " << fCrystal->cpCry << " and unique crystal identifier " 
	      << fCrystal->crystalId << FairLogger::endl;
  
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fNf = 0.;
    fNs = 0.;
    fNSteps  = 0; // FIXME
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
    fEinc   = gMC->Etot() - gMC->TrackMass();                  //be aware!! Relativistic mass!
  }
  
  // Sum energy loss for all steps in the active volume
  Double_t dE = gMC->Edep() * 1000.;         //in MeV
  Double_t post_E = (gMC->Etot() - gMC->TrackMass()) * 1000.;      //in MeV
  TString ptype = gMC->GetStack()->GetCurrentTrack()->GetName();
  Double_t dx = gMC->TrackStep() * fCrystal->density;

  Double_t M_in = gMC->TrackMass()*1000.;
  Double_t A_in = M_in/U_MEV;
  Double_t Z_in = gMC->TrackCharge();

  const double Z_CsI = 54.;
  const double A_CsI = 129.905; // g/mol
  const double E_delta = 5.30227; //MeV
  const double m_e = .5109989461; //MeV
  const double slope_e = 1.33055;
  const double K = .307075; // MeV cm**2/mol
  //quenching
  const double q_1 = 0.0396113;
  const double q_2 = -0.0828619;
  const double q_3 = 0.780435;

  fELoss += dE / 1000.;       //back to GeV

  if(dE > 0 && dx > 0)
  {

//    cout << ptype << " E = " << post_E << " MeV, dE = " << dE << " MeV, dx = " << dx << " g/cm**2" << ", dE/dx = " << (dE/dx) << " MeV cm**2/g" << endl;

  if(fCrystal->fEndcapIdentifier == 1) {
    //CC Phoswich
    if(fCrystal->fPhoswichIdentifier == 1) {
      //LaBr
      fNf += dE;
    } else if(fCrystal->fPhoswichIdentifier == 2) {
      //LaCl
      fNs += dE;
    } else {
    LOG(ERROR) << "R3BCalifa: fPhoswichIdentifier not valid in R3BCalifa::ProcessHits(). " 
		    << FairLogger::endl;
  }
    
  } else if (fCrystal->fEndcapIdentifier == 0)  {
//    if (ptype == "e-" || ptype == "e+" || ptype == "gamma") {
//      fNs += tf_g_dNs->Integral(post_E, post_E + dE);
//      fNf += tf_g_dNf->Integral(post_E, post_E + dE);
//    } else if(ptype == "proton") {
//      fNs += tf_p_dNs->Integral(post_E, post_E + dE);
//      fNf += tf_p_dNf->Integral(post_E, post_E + dE);
//    }

      if(ptype != "gamma" && post_E >= A_in*E_delta)
      {
	    double beta_cut = BETA(M_in, A_in*E_delta);
	    double gamma_cut = GAMMA(M_in, A_in*E_delta);
	    double beta = BETA(M_in, post_E);
	    double gamma = GAMMA(M_in, post_E);
	    double T_cut = 2.*m_e*beta_cut*beta_cut*gamma_cut*gamma_cut/(1.+2.*gamma_cut*m_e/M_in+(m_e/M_in)*(m_e/M_in));
	    double T_max = 2.*m_e*beta*beta*gamma*gamma/(1.+2.*gamma*m_e/M_in+(m_e/M_in)*(m_e/M_in));
	    double C = 0.5*K*Z_in*Z_in*Z_CsI/(A_CsI*beta*beta);
	    
	    //quenching
	    double part1 = q_1/q_2 * (1/T_max - 1/T_cut + (log(T_cut/T_max) + log((T_max - q_2)/(T_cut - q_2))/q_2));
	    double part2 = q_1 * beta * beta/T_max * (log(T_cut/T_max) + log((T_max - q_2)/(T_cut - q_2))/q_2);
	    double N = 1/T_cut - 1/T_max - (beta*beta)/T_max * log(T_max/T_cut);
	    double part3 = q_3 * N;
	    double scaling = 1.;

	    double dE_dxe = C*(log(T_max/T_cut) - beta*beta*(T_max - T_cut)/T_max);
	    double dE_e = dE_dxe * dx;
	    if(dE_e > dE){
	      dE_e = dE;
	    } 
	    if(T_max < 2.){
	      scaling = (part1 + part2 + part3)/N;
	    }
	    else{
	      scaling = q_3;
	    }

//            cout << "  dE_e = " << dE_e << " MeV, N = " << N << ", scaling = " << scaling << endl;

	    //std::cout << T_max << "  " << scaling << std::endl;
	    fNf += (dE_e*scaling/(1+slope_e))/1000.;
	    fNs += (dE_e*scaling/(1./slope_e + 1))/1000.;
	    dE -= dE_e;
      }

    fNf += tf_dNf_dE->Eval(dE/dx)*dE/1000.;
    fNs += tf_dNs_dE->Eval(dE/dx)*dE/1000.;
  } else {
    LOG(ERROR) << "R3BCalifa: fEndcapIdentifier not valid in R3BCalifa::ProcessHits(). " 
		    << FairLogger::endl;
  }
  }
  
  fNSteps++;

  // Set additional parameters at exit of active volume. Create R3BCalifaPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    
    fTrackID        = gMC->GetStack()->GetCurrentTrackNumber();
    fParentTrackID  = gMC->GetStack()->GetCurrentParentTrackNumber();
    fVolumeID       = vol->getMCid();
    fTrackPID       = gMC->TrackPid();
    fUniqueID       = gMC->GetStack()->GetCurrentTrack()->GetUniqueID();
    
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
    
    if (fELoss == 0. ) return kFALSE;
    
/*    
 *
 * Can somebody explain to me, what the following should do
 * (or how it's meant to do what it should do)?
 * I (Max W.) think it's supposed to calculate the exact point
 * at the crystal border where the track leaves the crystal, but
 * I don't see how this code could work.
 *
 * Anyhow, the result is never used for anything.
 *
    if (gMC->IsTrackExiting()) {
      const Double_t* oldpos;
      const Double_t* olddirection;
      Double_t newpos[3];
      Double_t newdirection[3];
      Double_t safety;
      
      gGeoManager->FindNode(fPosOut.X(),fPosOut.Y(),fPosOut.Z());
      oldpos = gGeoManager->GetCurrentPoint();
      olddirection = gGeoManager->GetCurrentDirection();
      
      
      for (Int_t i=0; i<3; i++) {
        newdirection[i] = -1*olddirection[i];
      }
      
      gGeoManager->SetCurrentDirection(newdirection);
      safety = gGeoManager->GetSafeDistance();
      
      gGeoManager->SetCurrentDirection(-newdirection[0],
				       -newdirection[1],
				       -newdirection[2]);
      
      for (Int_t i=0; i<3; i++) {
        newpos[i] = oldpos[i] - (3*safety*olddirection[i]);
      }
      
      fPosOut.SetX(newpos[0]);
      fPosOut.SetY(newpos[1]);
      fPosOut.SetZ(newpos[2]);
    }
    
    AddPoint(fTrackID, fVolumeID, fCrystal->crystalType , fCrystal->crystalCopy , fCrystal->crystalId,
           TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
           TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
           TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
           TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
           fTime, fLength, fELoss, fNf, fNs);
    */
    
    // Increment number of CalifaPoints for this track
    R3BStack* stack = (R3BStack*) gMC->GetStack();
    stack->AddPoint(kCALIFA);
    
    //Adding a crystalHit support
    Int_t nCrystalHits = fCaloCrystalHitCollection->GetEntriesFast();
    Bool_t existHit = 0;
    
    if (nCrystalHits==0) AddCrystalHit(fCrystal->crystalType , fCrystal->crystalCopy , fCrystal->crystalId, 
				       NUSmearing(fELoss), fNf, fNs, fTime, fNSteps, 
				       fEinc, fTrackID, fVolumeID, 
				       fParentTrackID, fTrackPID, fUniqueID);
    else {
      for (Int_t i=0; i<nCrystalHits; i++) {
        if ( ((R3BCalifaCrystalCalDataSim *)(fCaloCrystalHitCollection->At(i)))
	     ->GetCrystalId() == fCrystal->crystalId ) {
          ((R3BCalifaCrystalCalDataSim *)(fCaloCrystalHitCollection->At(i)))
	    ->AddMoreEnergy(NUSmearing(fELoss));
	  ((R3BCalifaCrystalCalDataSim *)(fCaloCrystalHitCollection->At(i)))
	    ->AddMoreNf(fNf);
	  ((R3BCalifaCrystalCalDataSim *)(fCaloCrystalHitCollection->At(i)))
	    ->AddMoreNs(fNs);
          if ( ((R3BCalifaCrystalCalDataSim *)(fCaloCrystalHitCollection->At(i)))
	       ->GetTime() > fTime ) {
            ((R3BCalifaCrystalCalDataSim *)(fCaloCrystalHitCollection->At(i)))
	      ->SetTime(fTime);
          }
          existHit=1; //to avoid the creation of a new CrystalHit
          break;
        }
      }
      if (!existHit) AddCrystalHit(fCrystal->crystalType , fCrystal->crystalCopy , fCrystal->crystalId, 
				   NUSmearing(fELoss), fNf, fNs, fTime, fNSteps, 
				   fEinc, fTrackID, fVolumeID, 
				   fParentTrackID, fTrackPID, fUniqueID);
    }
    
    existHit=0;
    
    ResetParameters();
  }
  
  return kTRUE;
}
// ----------------------------------------------------------------------------
//void R3BCalifa::SaveGeoParams(){
//
//  cout << " -I Save STS geo params " << endl;
//
//  TFolder *mf = (TFolder*) gDirectory->FindObjectAny("cbmroot");
//  cout << " mf: " << mf << endl;
//  TFolder *stsf = NULL;
//  if (mf ) stsf = (TFolder*) mf->FindObjectAny(GetName());
//  cout << " stsf: " << stsf << endl;
//  if (stsf) stsf->Add( flGeoPar0 ) ;
//  FairRootManager::Instance()->WriteFolder();
//  mf->Write("cbmroot",TObject::kWriteDelete);
//}


Bool_t R3BCalifa::GetCrystalInfo(sCrystalInfo &info)
{

  // Getting the Infos from Crystal Volumes
  Int_t cp1 = -1; Int_t volId1 = -1; 
  Int_t cpAlv = -1; Int_t cpSupAlv = -1; 
  Int_t volIdAlv = -1; Int_t volIdSupAlv = -1; 
  Int_t cpCry = -1; Int_t volIdCry = -1;

  // Crystals Ids
  int crysNum;
  const char* bufferName = gMC->CurrentVolName();
  volId1 = gMC->CurrentVolID(cp1);
  volIdCry = gMC->CurrentVolOffID(1,cpCry);
  volIdAlv = gMC->CurrentVolOffID(2,cpAlv);
  //next is needed for versions 8.# and later
  volIdSupAlv = gMC->CurrentVolOffID(3,cpSupAlv);
  //LOG(ERROR) << "TEST INITIAL. " <<  gMC->CurrentVolPath()<< FairLogger::endl;

  info.volIdAlv = volIdAlv;
  info.cpAlv = cpAlv;
  info.cpCry = cpCry;

  if (fGeometryVersion==0) {
    //The present scheme here done works nicely with 5.0
    //info.crystalType = crystal type (from 1 to 30)
    // info.crystalCopy = crystal copy (from 1 to 512 for crystal types from 1 to 6
    //          (BARREL), from 1 to 64 for crystal types from 7 to 30 (ENDCAP))
    // info.crystalId = (crystal type-1) *512 + crystal copy  (from 1 to 3072) 
    //          for the BARREL
    // info.crystalId = 3072 + (crystal type-7) *64 + crystal copy  (from 3073 to 
    //          4608) for the ENDCAP
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification: 
    const char *crystalPrefix = "crystalLog";
    const char *volumeName = gMC->VolName(volId1);
    if (strncmp(crystalPrefix, volumeName,9) == 0) {
      info.crystalType = atoi(volumeName+10);//converting to int the alveolus index
      info.crystalCopy = cp1 + 1; 
      if (info.crystalType>0 && info.crystalType<7) {
	//from 1 to 6, there are 512 each type; from 7 to 30, only 64 each type
	info.crystalId = (info.crystalType-1)*512+info.crystalCopy;
      } else if (info.crystalType>6 &&info.crystalType<31) {
	info.crystalId = 3072+(info.crystalType-7)*64+info.crystalCopy;
      } else {
	LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometry 0 (v5.0)" 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else 
    {
      LOG(ERROR) << "R3BCalifa: Energy collected in non-sensitive volume for geometry 0 (v5.0)" 
		 << FairLogger::endl;
      return kFALSE;
    }
  } else if (fGeometryVersion==1)  {
    //The present scheme here done works nicely with 7.05
    //info.crystalType = alveolus type (from 1 to 24)   [Basically the alveolus number]
    // info.crystalCopy = (alveolus copy - 1) * 4 + crystals copy (from 1 to 160)  
    //           [Not exactly azimuthal]
    // info.crystalId = (alveolus type-1)*160 + (alvelous copy-1)*4 + (crystal copy)  
    //           (from 1 to 3840) info.crystalId is a single identifier per crystal!
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      info.crystalCopy = cpAlv * 4 + cpCry;
      info.crystalId = (info.crystalType-1)*160 + cpAlv * 4 + cpCry;
      if (info.crystalType>24 ||info.crystalType<1 || info.crystalCopy>160 || 
	  info.crystalCopy<1 || info.crystalId>3840 || info.crystalId<1)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 1 (v7.05). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    } else {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 1 (v7.05)" 
		      << FairLogger::endl;
      return kFALSE;
    }
  } else if (fGeometryVersion==2)  {
    //The present scheme here done works nicely with 7.07
    // info.crystalType = alveolus type (from 1 to 20)   [Alveolus number]
    // info.crystalCopy = (alveolus copy - 1) * 4 + crystals copy (from 1 to 128)  
    //            [Not exactly azimuthal]
    // info.crystalId = (alveolus type-1)*128 + (alvelous copy-1)*4 + (crystal copy)  
    //            (from 1 to 2560)
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      info.crystalCopy = cpAlv * 4 + cpCry;
      info.crystalId = (info.crystalType-1)*128 + cpAlv * 4 + cpCry;
      if (info.crystalType>20 ||info.crystalType<1 || info.crystalCopy>128 || 
	  info.crystalCopy<1 || info.crystalId>2560 || info.crystalId<1)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 2 (v7.07)" 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 2 (v7.07)" 
		    << FairLogger::endl;  
      return kFALSE;
    }
  } else if (fGeometryVersion==3) {
    //The present scheme here done works with 7.09

    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if(info.crystalType>1 &&info.crystalType<17) {
	info.crystalCopy = cpAlv * 4 + cpCry;
	info.crystalId = (info.crystalType-1)*128 + cpAlv * 4 + cpCry;
      } else if (info.crystalType>16 &&info.crystalType<20) {
	// info.crystalTypes 17-19 are large crystals which fill type 6 alveoli, as 
	// opposed to the smaller crystals of which 4 fit in the other alveoli.
	info.crystalCopy = cpAlv + cpCry;
	info.crystalId = 2048 + (info.crystalType-17)*32 + cpAlv + cpCry;
      }
      if (info.crystalType>19 ||info.crystalType<1 || info.crystalCopy>128 || 
	  info.crystalCopy<1 || info.crystalId>2144 || info.crystalId<1)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 3 (v7.07). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 3 (v7.07)" 
		       << FairLogger::endl;
      return kFALSE;
    }
  } else if (fGeometryVersion==4) {
    //The present scheme here done works nicely with 7.17
    // info.crystalType = crystals type (from 1 to 23)
    // info.crystalCopy = alveolus copy (from 1 to 32)
    // info.crystalId = 3000 + (alvelous copy-1)*23 + (crystal copy-1)  
    //          (from 3000 to 3736)

    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      info.crystalType = atoi(volumeName+12);//converting to int the alveolus index
      info.crystalCopy = cpAlv+1;
      info.crystalId = 3000 + cpAlv*23 + (info.crystalType-1); 
      if(info.crystalType>23 ||info.crystalType<1 || info.crystalCopy>32 || 
	 info.crystalCopy<1 || info.crystalId<3000 || info.crystalId>3736)  
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 4 (v7.17). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 4 (v7.17)" 
		       << FairLogger::endl;
      return kFALSE;
    }
  } else if (fGeometryVersion==5) {
    //The present scheme here done works nicely with 7.07+7.17
    //see the explanation for geometries 2 and 4
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      info.crystalType = atoi(volumeName+12);//converting to int the alveolus index
      info.crystalCopy = cpAlv+1;
      info.crystalId = 3000 + cpAlv*23 + (info.crystalType-1); 
      if(info.crystalType>23 ||info.crystalType<1 || info.crystalCopy>32 || 
	 info.crystalCopy<1 || info.crystalId<3000 || info.crystalId>3736)  
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 5 (v7.17). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      info.crystalCopy = cpAlv * 4 + cpCry;
      info.crystalId = (info.crystalType-1)*128 + cpAlv * 4 + cpCry;
      if (info.crystalType>20 ||info.crystalType<1 || info.crystalCopy>128 || 
	  info.crystalCopy<1 || info.crystalId>2560 || info.crystalId<1)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 5 (v7.07)" 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 5." 
		    << FairLogger::endl; 
      return kFALSE;
    }
  } else if (fGeometryVersion==6) {
    //The present scheme here done works nicely with 7.09+7.17
    //see the explanation for geometries 3 and 4
 
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      info.crystalType = atoi(volumeName+12);//converting to int the alveolus index
      info.crystalCopy = cpAlv+1;
      info.crystalId = 3000 + cpAlv*23 + (info.crystalType-1); 
      if(info.crystalType>23 ||info.crystalType<1 || info.crystalCopy>32 || 
	 info.crystalCopy<1 || info.crystalId<3000 || info.crystalId>3736)  
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 6 (v7.17)." 
		   << FairLogger::endl;
        return kFALSE;
      }
    } else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if(info.crystalType>1 &&info.crystalType<17) {
	info.crystalCopy = cpAlv * 4 + cpCry;
	info.crystalId = (info.crystalType-1)*128 + cpAlv * 4 + cpCry;
      } else if (info.crystalType>16 &&info.crystalType<20) {
	// info.crystalTypes 17-19 are large crystals which fill type 6 alveoli, as 
	// opposed to the smaller crystals of which 4 fit in the other alveoli.
	info.crystalCopy = cpAlv + cpCry;
	info.crystalId = 2048 + (info.crystalType-17)*32 + cpAlv + cpCry;
      }
      if (info.crystalType>19 ||info.crystalType<1 || info.crystalCopy>128 || 
	  info.crystalCopy<1 || info.crystalId>2144 || info.crystalId<1)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 6 (v7.07). " 
		   << FairLogger::endl;
        return false;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 6." 
		       << FairLogger::endl;
      return kFALSE;
    }
  } else if (fGeometryVersion==7) { 
    //RESERVED FOR CALIFA 717PHOSWICH, only phoswich ENDCAP 
    //For IEM LaBr - LaCl phoswich endcap
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      info.crystalType = atoi(volumeName+12);//converting to int the alveolus index
      info.crystalCopy = cpAlv+1;
      info.crystalId = 3000 + cpAlv*30 + (info.crystalType-1);
      if (info.crystalType>30 ||info.crystalType<1 ||
	  info.crystalCopy>60 ||info.crystalCopy<1 || info.crystalId<3000 || info.crystalId>4800)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 10 (endcap). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 7." 
		      << FairLogger::endl;
      return kFALSE;
    }
  } else if (fGeometryVersion==8) {
    //RESERVED FOR CALIFA 7.07 BARREL + 717PHOSWICH 
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      info.crystalType = atoi(volumeName+12);//converting to int the alveolus index
      info.crystalCopy = cpAlv+1;
      info.crystalId = 3000 + cpAlv*30 + (info.crystalType-1);
      if (info.crystalType>30 ||info.crystalType<1 ||
	  info.crystalCopy>60 ||info.crystalCopy<1 || info.crystalId<3000 || info.crystalId>4800)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 10 (endcap). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    } else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      info.crystalCopy = cpAlv * 4 + cpCry;
      info.crystalId = (info.crystalType-1)*128 + cpAlv * 4 + cpCry;
      if (info.crystalType>20 ||info.crystalType<1 || info.crystalCopy>128 || 
	  info.crystalCopy<1 || info.crystalId>2560 || info.crystalId<1)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 5 (v7.07)" 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 8." 
		 << FairLogger::endl;
      return kFALSE;
    }
  } else if (fGeometryVersion==9) {
    //RESERVED FOR CALIFA 7.09 BARREL + 717PHOSWICH
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      info.crystalType = atoi(volumeName+12);//converting to int the alveolus index
      info.crystalCopy = cpAlv+1;
      info.crystalId = 3000 + cpAlv*30 + (info.crystalType-1);
      if (info.crystalType>30 ||info.crystalType<1 ||
	  info.crystalCopy>60 ||info.crystalCopy<1 || info.crystalId<3000 || info.crystalId>4800)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 9 (endcap). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    } else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if(info.crystalType>1 &&info.crystalType<17) {
	info.crystalCopy = cpAlv * 4 + cpCry;
	info.crystalId = (info.crystalType-1)*128 + cpAlv * 4 + cpCry;
      } else if (info.crystalType>16 &&info.crystalType<20) {
	// info.crystalTypes 17-19 are large crystals which fill type 6 alveoli, as 
	// opposed to the smaller crystals of which 4 fit in the other alveoli.
	info.crystalCopy = cpAlv + cpCry;
	info.crystalId = 2048 + (info.crystalType-17)*32 + cpAlv + cpCry;
      }
      if (info.crystalType>19 ||info.crystalType<1 || info.crystalCopy>128 || 
	  info.crystalCopy<1 || info.crystalId>2144 || info.crystalId<1)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 9 (v7.07). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 9." 
		 << FairLogger::endl;
      return kFALSE;
    }
  } else if (fGeometryVersion==10) {
    //The present scheme here done works with 8.11:
    // info.crystalType = alveolus type (from 1 to 17) [Alveolus number]
    // info.crystalCopy = alveolus copy * 4 + crystals copy +1 (from 1 to 128) 
    // info.crystalId = 1 to 32 for the first 32 crystals 
    //                     (single crystal in each alveoli)
    // or 32 + (alveolus type-2)*128 + (alvelous copy)*4 + (crystal copy) + 1        
    //                     (in this way, info.crystalId runs from 1 to 1952)
    //     
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdSupAlv);
    if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if (info.crystalType==1) {
	//only one crystal per alveoli in this ring, running from 1 to 32
        info.crystalCopy = cpSupAlv+1; 
        info.crystalId = cpSupAlv+1;                    
      } else if (info.crystalType>1 &&info.crystalType<17) {
	//running from 0*4+0+1=1 to 31*4+3+1=128
        info.crystalCopy = cpSupAlv*4+cpCry+1;
	//running from 32+0*128+0*4+0+1=1 to 32+14*128+31*4+3+1=1952
        info.crystalId = 32+(info.crystalType-2)*128+cpSupAlv*4+cpCry+1; 
      }

      LOG(INFO) << "volIdAlv: " << volIdAlv << ", volIdSupAlv: " << volIdSupAlv << ", volumeName: " << volumeName
		<< FairLogger::endl;

      if (info.crystalType>16 ||info.crystalType<1 || info.crystalCopy>128 || 
	  info.crystalCopy<1 || info.crystalId>1952 || info.crystalId<1) 
      {
        LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 10. " 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 10."
		      << FairLogger::endl;
      return kFALSE;
    }
  } else if (fGeometryVersion==11) {
    //RESERVED FOR CALIFA 8.11 BARREL + PHOSWICH
    
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeNameEC = gMC->VolName(volIdAlv);    
    const char *volumeName   = gMC->VolName(volIdSupAlv);
    if (strncmp(alveolusECPrefix, volumeNameEC,11) == 0) {
      info.crystalType = atoi(volumeNameEC+12);//converting to int the alveolus index
      info.crystalCopy = cpAlv+1;
      info.crystalId = 3000 + cpAlv*30 + (info.crystalType-1);
      if (info.crystalType>30 ||info.crystalType<1 ||
	  info.crystalCopy>60 ||info.crystalCopy<1 || info.crystalId<3000 || info.crystalId>4800)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 11 (endcap). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    } else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if (info.crystalType==1) {
	//only one crystal per alveoli in this ring, running from 1 to 32
        info.crystalCopy = cpSupAlv+1; 
        info.crystalId = cpSupAlv+1;                    
      } else if (info.crystalType>1 &&info.crystalType<17) {
	//running from 0*4+0+1=1 to 31*4+3+1=128
        info.crystalCopy = cpSupAlv*4+cpCry+1;
	//running from 32+0*128+0*4+0+1=1 to 32+14*128+31*4+3+1=1952
        info.crystalId = 32+(info.crystalType-2)*128+cpSupAlv*4+cpCry+1; 
      }
      if (info.crystalType>16 ||info.crystalType<1 || info.crystalCopy>128 || 
	  info.crystalCopy<1 || info.crystalId>1952 || info.crystalId<1) 
      {
        LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 11. " 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 11." 
		      << FairLogger::endl;
      return kFALSE;
    }
  } else if (fGeometryVersion==15) {
    //RESERVED FOR CALIFA 8.11 BARREL + iPhos 1.00
    
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdSupAlv);
    // Workaround to fix the hierarchy difference between Barrel and Endcap
    if (strncmp("CalifaWorld", volumeName,10) == 0) {
      volumeName = gMC->VolName(volIdAlv);  
    }
    //LOG(INFO) << "volIdSupAlv: " << volIdSupAlv << ", volumeName: " << volumeName << FairLogger::endl;
    //if ENDCAP
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      info.crystalType = atoi(volumeName+12);//converting to int the alveolus index
      info.crystalCopy = cpAlv+1;
      info.crystalId = 3000 + cpAlv*15 + (info.crystalType-1);
      if (info.crystalType>15 ||info.crystalType<1 ||
	  info.crystalCopy>60 ||info.crystalCopy<1 || info.crystalId<3000 || info.crystalId>4800)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 15 (iPhos). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    //if BARREL
    } else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if (info.crystalType==1) {
	//only one crystal per alveoli in this ring, running from 1 to 32
        info.crystalCopy = cpSupAlv+1; 
        info.crystalId = cpSupAlv+1;                    
      } else if (info.crystalType>1 &&info.crystalType<17) {
	//running from 0*4+0+1=1 to 31*4+3+1=128
        info.crystalCopy = cpSupAlv*4+cpCry+1;
	//running from 32+0*128+0*4+0+1=1 to 32+14*128+31*4+3+1=1952
        info.crystalId = 32+(info.crystalType-2)*128+cpSupAlv*4+cpCry+1; 
      }
      if (info.crystalType>16 ||info.crystalType<1 || info.crystalCopy>128 || 
	  info.crystalCopy<1 || info.crystalId>1952 || info.crystalId<1) 
      {
        LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 15 (BARREL)." 
	  //<< "volIdAlv: " << volIdAlv << ", volIdSupAlv: " << volIdSupAlv << ", volumeName: " << volumeName
	  //<< "info.crystalType: " <<info.crystalType << ", info.crystalCopy: " <<info.crystalCopy << ", info.crystalId: " << info.crystalId
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 15." 
	     //<< "volIdAlv: " << volIdAlv << ", volIdSupAlv: " << volIdSupAlv << ", volumeName: " << volumeName
		      << FairLogger::endl;
      return kFALSE;
    }



  } else if (fGeometryVersion==16 || fGeometryVersion==17 || fGeometryVersion==0x438b) {
    //RESERVED FOR CALIFA 8.11 BARREL + CC 0.2
    
    const char *alveolusECPrefix = "Alveolus_EC";
    const char *alveolusPrefix = "Alveolus";
    const char *volumeName = gMC->VolName(volIdSupAlv);
    const char *volumeNameCrystal ="";

    // Workaround to fix the hierarchy difference between Barrel and Endcap
    if (strncmp("CalifaWorld", volumeName,10) == 0) {
      volumeName = gMC->VolName(volIdAlv);
      volumeNameCrystal = gMC->VolName(volId1);
    }
    if (strncmp(alveolusECPrefix, volumeName, 11) == 0) {
      info.crystalType = atoi(volumeNameCrystal+8);     //converting to int the crystal index
      info.crystalCopy = cpAlv+1;

      if(info.crystalType < 9) {
	//CC Phoswich crystals are combined in one crystal
	//Energies are contained in Nf -> LaBr and Ns -> LaCl
	
	//info.fEndcapIdentifier = 0 -> iPhos
	//info.fEndcapIdentifier = 1 -> Phoswich
	info.fEndcapIdentifier = 1;

	if(info.crystalType%2 == 0) {
	  //info.fPhoswichIdentifier = 2 -> LaCl
	  info.fPhoswichIdentifier = 2;
	  info.crystalType -= 1;
	} else {
	  //info.fPhoswichIdentifier = 1 -> LaBr
	  info.fPhoswichIdentifier = 1;
	}
      }
      info.crystalId = 3000 + cpAlv*24 + (info.crystalType-1);
      if (info.crystalType>24 ||info.crystalType<1 ||
	  info.crystalCopy>32 ||info.crystalCopy<1 || info.crystalId<3000 || info.crystalId>4800)
      {
	LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 16 (CC). " 
		   << FairLogger::endl;
        return kFALSE;
      }
    //if BARREL
    } else if (strncmp(alveolusPrefix, volumeName,8) == 0) {

      info.crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if (info.crystalType==1) {
	//only one crystal per alveoli in this ring, running from 1 to 32
        info.crystalCopy = cpSupAlv+1; 
        info.crystalId = cpSupAlv+1;                    
      } else if (info.crystalType>1 &&info.crystalType<17) {
	//running from 0*4+0+1=1 to 31*4+3+1=128
        info.crystalCopy = cpSupAlv*4+cpCry+1;
	//running from 32+0*128+0*4+0+1=1 to 32+14*128+31*4+3+1=1952
        info.crystalId = 32+(info.crystalType-2)*128+cpSupAlv*4+cpCry+1; 
      }
      if (info.crystalType>16 ||info.crystalType<1 || info.crystalCopy>128 || 
	  info.crystalCopy<1 || info.crystalId>1952 || info.crystalId<1) 
      {
        LOG(ERROR) << "R3BCalifa: Wrong crystal number in geometryVersion 16 (BARREL)." 
		   << FairLogger::endl;
        return kFALSE;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifa: Impossible info.crystalType for geometryVersion 16." 
		      << FairLogger::endl;
      return kFALSE;
    }
  }
  else
  {
    LOG(ERROR) << "R3BCalifa: Geometry version not available in R3BCalifa::ProcessHits(). " 
		    << FairLogger::endl;
    return kFALSE;
  }

  return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BCalifa::BeginEvent()
{

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BCalifa::EndOfEvent()
{
  if (fVerboseLevel) Print();
  
  fCaloCollection->Clear();
  fCaloCrystalHitCollection->Clear();
  
  ResetParameters();

  fCrystal = NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BCalifa::Register()
{
  //FairRootManager::Instance()->Register("CrystalPoint", GetName(), 
  //                                      fCaloCollection, kTRUE);
  FairRootManager::Instance()->Register("CalifaCrystalCalDataSim", GetName(), 
					fCaloCrystalHitCollection, kTRUE);
  
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BCalifa::GetCollection(Int_t iColl) const
{
  //HAPOL TODO -- DO I NEED TO RETURN A fCaloCrystalHitColletion????
  if(iColl == 0) {
    return fCaloCollection;
  } else if(iColl == 2) {
    return fCaloCrystalHitCollection;
  }
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BCalifa::Print(Option_t* option) const
{
  Int_t nHits = fCaloCollection->GetEntriesFast();
  LOG(INFO) << "R3BCalifa: " << nHits << " points registered in this event" 
	    << FairLogger::endl;
  Int_t nCrystalHits = fCaloCrystalHitCollection->GetEntriesFast();
  LOG(INFO) << "R3BCalifa: " << nCrystalHits << " sim crystal hits registered in this event." 
	    << FairLogger::endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BCalifa::Reset()
{
  fCaloCollection->Clear();
  fCaloCrystalHitCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BCalifa::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  LOG(INFO) << "R3BCalifa: " << nEntries << " entries to add" << FairLogger::endl;
  TClonesArray& clref = *cl2;
  R3BCalifaPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (R3BCalifaPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BCalifaPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(INFO) << "R3BCalifa: " << cl2->GetEntriesFast() << " merged entries" 
	    << FairLogger::endl;
}

// -----   Private method AddPoint   --------------------------------------------
R3BCalifaPoint* R3BCalifa::AddPoint(Int_t trackID, Int_t detID, Int_t volid , 
			      Int_t copy, Int_t ident, TVector3 posIn, 
			      TVector3 posOut, TVector3 momIn, TVector3 momOut,
                              Double_t time, Double_t length, Double_t eLoss, Double_t Nf, Double_t Ns)
{
  TClonesArray& clref = *fCaloCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1)
    LOG(INFO) << "R3BCalifa: Adding Point at (" << posIn.X() << ", " << posIn.Y()
	      << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	      << trackID << ", energy loss " << eLoss*1e06 << " keV" 
	      << FairLogger::endl;
  return new(clref[size]) R3BCalifaPoint(trackID, detID, volid, copy , ident, 
				       posIn, posOut, momIn, momOut, time, 
				       length, eLoss, Nf, Ns);
}

// -----   Private method AddCrystalHit   --------------------------------------------
R3BCalifaCrystalCalDataSim* R3BCalifa::AddCrystalHit(Int_t type, Int_t copy, Int_t ident,
					     Double_t energy, Double_t Nf, Double_t Ns, Double_t time, 
					     Int_t steps, Double_t einc,
					     Int_t trackid, Int_t volid, 
					     Int_t partrackid, Int_t pdgtype, 
					     Int_t uniqueid)
{
  TClonesArray& clref = *fCaloCrystalHitCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) {
    LOG(INFO) << "-I- R3BCalifa: Adding Sim Crystal Hit in detector type " << type 
	      << ", and copy " << copy << " with unique identifier " << ident 
	      << " entering with " << einc*1e06 << " keV, depositing " << energy*1e06 
	      << " keV" << FairLogger::endl;
    LOG(INFO) << " -I- trackid: " << trackid << " volume id: " << volid 
	      << " partrackid : " << partrackid << " type: " << pdgtype 
	      << " unique id: " << uniqueid << FairLogger::endl;
  }
  return new(clref[size]) R3BCalifaCrystalCalDataSim(type, copy, ident, energy, Nf, Ns, time, 
					       steps, einc, trackid, volid, 
					       partrackid, pdgtype, uniqueid);
}

// -----   Private method NUSmearing  --------------------------------------------
Double_t R3BCalifa::NUSmearing(Double_t inputEnergy)
{
  // Very simple preliminary scheme where the NU is introduced as a flat random
  // distribution with limits fNonUniformity (%) of the energy value.
  //
  return gRandom->Uniform(inputEnergy-inputEnergy*fNonUniformity/100,
			  inputEnergy+inputEnergy*fNonUniformity/100);
}



// -----  Public method SelectGeometryVersion  ----------------------------------
void R3BCalifa::SelectGeometryVersion(Int_t version)
{
  fGeometryVersion=version;
}


// -----  Public method SetNonUniformity  ----------------------------------
void R3BCalifa::SetNonUniformity(Double_t nonU)
{
  fNonUniformity = nonU;
  LOG(INFO) << "R3BCalifa::SetNonUniformity to " << fNonUniformity << " %" 
	    << FairLogger::endl;
}


// -----   Public method ConstructGeometry   ----------------------------------
void R3BCalifa::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing CALIFA geometry from ROOT file " << fileName.Data() 
	      << FairLogger::endl;
    ConstructRootGeometry();
  } else {
    LOG(FATAL) << "CALIFA geometry file name is not specified" << FairLogger::endl;
    exit(1);
  }
}


Bool_t R3BCalifa::CheckIfSensitive(std::string name)
{
  if(TString(name).Contains("Crystal_")) {
    return kTRUE;
  }
  return kFALSE;
}


ClassImp(R3BCalifa)
