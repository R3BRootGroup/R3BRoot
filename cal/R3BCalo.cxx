// -------------------------------------------------------------------------
// -----                        R3BCalo source file                    -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -----	     Last modification 10/01/14 by H. Alvarez          -----
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

#include <iostream>
#include <stdlib.h>

#include "R3BCalo.h"

#include "R3BGeoCalo.h"
#include "R3BCaloPoint.h"
#include "R3BCaloCrystalHit.h"
#include "R3BCaloCrystalHitSim.h"
#include "R3BGeoCaloPar.h"
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



// -----   Default constructor   -------------------------------------------
R3BCalo::R3BCalo() : R3BDetector("R3BCalo", kTRUE, kCALIFA)
{
  ResetParameters();
  fCaloCollection = new TClonesArray("R3BCaloPoint");
  fCaloCrystalHitCollection = new TClonesArray("R3BCaloCrystalHitSim");
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
R3BCalo::R3BCalo(const char* name, Bool_t active)
    : R3BDetector(name, active, kCALIFA)
{
  ResetParameters();
  fCaloCollection = new TClonesArray("R3BCaloPoint");
  fCaloCrystalHitCollection = new TClonesArray("R3BCaloCrystalHitSim");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fNonUniformity = 0.;
  fGeometryVersion = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BCalo::~R3BCalo()
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
}
// -------------------------------------------------------------------------
void R3BCalo::Initialize()
{
  FairDetector::Initialize();

  LOG(INFO) << "R3BCalo: initialisation" << FairLogger::endl;
  LOG(DEBUG) << "-I- R3BCalo: Vol (McId) def" << FairLogger::endl;

  TGeoVolume *vol = gGeoManager->GetVolume("CalifaWorld");
  vol->SetVisibility(kFALSE);
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BCalo::ProcessHits(FairVolume* vol)
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
  Int_t crystalType = 0;
  Int_t crystalCopy = 0;
  Int_t crystalId = 0;

  if (fGeometryVersion==0) {
    //The present scheme here done works nicely with 5.0
    // crystalType = crystal type (from 1 to 30)
    // crystalCopy = crystal copy (from 1 to 512 for crystal types from 1 to 6
    //          (BARREL), from 1 to 64 for crystal types from 7 to 30 (ENDCAP))
    // crystalId = (crystal type-1) *512 + crystal copy  (from 1 to 3072) 
    //          for the BARREL
    // crystalId = 3072 + (crystal type-7) *64 + crystal copy  (from 3073 to 
    //          4608) for the ENDCAP
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification: 
    const char *crystalPrefix = "crystalLog";
    const char *volumeName = gMC->VolName(volId1);
    if (strncmp(crystalPrefix, volumeName,9) == 0) {
      crystalType = atoi(volumeName+10);//converting to int the alveolus index
      crystalCopy = cp1 + 1; 
      if (crystalType>0 && crystalType<7) {
	//from 1 to 6, there are 512 each type; from 7 to 30, only 64 each type
	crystalId = (crystalType-1)*512+crystalCopy;
      } else if (crystalType>6 && crystalType<31) {
	crystalId = 3072+(crystalType-7)*64+crystalCopy;
      } else 
	LOG(ERROR) << "R3BCalo: Impossible crystalType for geometry 0 (v5.0)" 
		   << FairLogger::endl;
    }
    else 
      LOG(ERROR) << "R3BCalo: Energy collected in non-sensitive volume for geometry 0 (v5.0)" 
		 << FairLogger::endl;
  } else if (fGeometryVersion==1)  {
    //The present scheme here done works nicely with 7.05
    // crystalType = alveolus type (from 1 to 24)   [Basically the alveolus number]
    // crystalCopy = (alveolus copy - 1) * 4 + crystals copy (from 1 to 160)  
    //           [Not exactly azimuthal]
    // crystalId = (alveolus type-1)*160 + (alvelous copy-1)*4 + (crystal copy)  
    //           (from 1 to 3840) crystalID is a single identifier per crystal!
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      crystalType = atoi(volumeName+9);//converting to int the alveolus index
      crystalCopy = cpAlv * 4 + cpCry;
      crystalId = (crystalType-1)*160 + cpAlv * 4 + cpCry;
      if (crystalType>24 || crystalType<1 || crystalCopy>160 || 
	  crystalCopy<1 || crystalId>3840 || crystalId<1)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 1 (v7.05). " 
		   << FairLogger::endl;
    } else LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 1 (v7.05)" 
		      << FairLogger::endl;
  } else if (fGeometryVersion==2)  {
    //The present scheme here done works nicely with 7.07
    // crystalType = alveolus type (from 1 to 20)   [Alveolus number]
    // crystalCopy = (alveolus copy - 1) * 4 + crystals copy (from 1 to 128)  
    //            [Not exactly azimuthal]
    // crystalId = (alveolus type-1)*128 + (alvelous copy-1)*4 + (crystal copy)  
    //            (from 1 to 2560)
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      crystalType = atoi(volumeName+9);//converting to int the alveolus index
      crystalCopy = cpAlv * 4 + cpCry;
      crystalId = (crystalType-1)*128 + cpAlv * 4 + cpCry;
      if (crystalType>20 || crystalType<1 || crystalCopy>128 || 
	  crystalCopy<1 || crystalId>2560 || crystalId<1)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 2 (v7.07)" 
		   << FairLogger::endl;
    }
    else LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 2 (v7.07)" 
		    << FairLogger::endl;  
  } else if (fGeometryVersion==3) {
    //The present scheme here done works with 7.09

    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if(crystalType>1 && crystalType<17) {
	crystalCopy = cpAlv * 4 + cpCry;
	crystalId = (crystalType-1)*128 + cpAlv * 4 + cpCry;
      } else if (crystalType>16 && crystalType<20) {
	// Crystaltypes 17-19 are large crystals which fill type 6 alveoli, as 
	// opposed to the smaller crystals of which 4 fit in the other alveoli.
	crystalCopy = cpAlv + cpCry;
	crystalId = 2048 + (crystalType-17)*32 + cpAlv + cpCry;
      }
      if (crystalType>19 || crystalType<1 || crystalCopy>128 || 
	  crystalCopy<1 || crystalId>2144 || crystalId<1)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 3 (v7.07). " 
		   << FairLogger::endl;
    } else  LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 3 (v7.07)" 
		       << FairLogger::endl;
  } else if (fGeometryVersion==4) {
    //The present scheme here done works nicely with 7.17
    // crystalType = crystals type (from 1 to 23)
    // crystalCopy = alveolus copy (from 1 to 32)
    // crystalId = 3000 + (alvelous copy-1)*23 + (crystal copy-1)  
    //          (from 3000 to 3736)

    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      crystalType = atoi(volumeName+12);//converting to int the alveolus index
      crystalCopy = cpAlv+1;
      crystalId = 3000 + cpAlv*23 + (crystalType-1); 
      if(crystalType>23 || crystalType<1 || crystalCopy>32 || 
	 crystalCopy<1 || crystalId<3000 || crystalId>3736)  
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 4 (v7.17). " 
		   << FairLogger::endl;
    } else  LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 4 (v7.17)" 
		       << FairLogger::endl;
  } else if (fGeometryVersion==5) {
    //The present scheme here done works nicely with 7.07+7.17
    //see the explanation for geometries 2 and 4
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      crystalType = atoi(volumeName+12);//converting to int the alveolus index
      crystalCopy = cpAlv+1;
      crystalId = 3000 + cpAlv*23 + (crystalType-1); 
      if(crystalType>23 || crystalType<1 || crystalCopy>32 || 
	 crystalCopy<1 || crystalId<3000 || crystalId>3736)  
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 5 (v7.17). " 
		   << FairLogger::endl;
    }
    else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      crystalType = atoi(volumeName+9);//converting to int the alveolus index
      crystalCopy = cpAlv * 4 + cpCry;
      crystalId = (crystalType-1)*128 + cpAlv * 4 + cpCry;
      if (crystalType>20 || crystalType<1 || crystalCopy>128 || 
	  crystalCopy<1 || crystalId>2560 || crystalId<1)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 5 (v7.07)" 
		   << FairLogger::endl;
    }
    else LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 5." 
		    << FairLogger::endl; 
  } else if (fGeometryVersion==6) {
    //The present scheme here done works nicely with 7.09+7.17
    //see the explanation for geometries 3 and 4
 
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      crystalType = atoi(volumeName+12);//converting to int the alveolus index
      crystalCopy = cpAlv+1;
      crystalId = 3000 + cpAlv*23 + (crystalType-1); 
      if(crystalType>23 || crystalType<1 || crystalCopy>32 || 
	 crystalCopy<1 || crystalId<3000 || crystalId>3736)  
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 6 (v7.17)." 
		   << FairLogger::endl;
    } else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if(crystalType>1 && crystalType<17) {
	crystalCopy = cpAlv * 4 + cpCry;
	crystalId = (crystalType-1)*128 + cpAlv * 4 + cpCry;
      } else if (crystalType>16 && crystalType<20) {
	// Crystaltypes 17-19 are large crystals which fill type 6 alveoli, as 
	// opposed to the smaller crystals of which 4 fit in the other alveoli.
	crystalCopy = cpAlv + cpCry;
	crystalId = 2048 + (crystalType-17)*32 + cpAlv + cpCry;
      }
      if (crystalType>19 || crystalType<1 || crystalCopy>128 || 
	  crystalCopy<1 || crystalId>2144 || crystalId<1)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 6 (v7.07). " 
		   << FairLogger::endl;
    } else  LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 6." 
		       << FairLogger::endl;
  } else if (fGeometryVersion==7) { 
    //RESERVED FOR CALIFA 717PHOSWICH, only phoswich ENDCAP 
    //For IEM LaBr - LaCl phoswich endcap
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      crystalType = atoi(volumeName+12);//converting to int the alveolus index
      crystalCopy = cpAlv+1;
      crystalId = 3000 + cpAlv*30 + (crystalType-1);
      if (crystalType>30 || crystalType<1 ||
	  crystalCopy>60 || crystalCopy<1 || crystalId<3000 || crystalId>4800)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 10 (endcap). " 
		   << FairLogger::endl;
    }else  LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 7." 
		      << FairLogger::endl;
  } else if (fGeometryVersion==8) {
    //RESERVED FOR CALIFA 7.07 BARREL + 717PHOSWICH 
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      crystalType = atoi(volumeName+12);//converting to int the alveolus index
      crystalCopy = cpAlv+1;
      crystalId = 3000 + cpAlv*30 + (crystalType-1);
      if (crystalType>30 || crystalType<1 ||
	  crystalCopy>60 || crystalCopy<1 || crystalId<3000 || crystalId>4800)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 10 (endcap). " 
		   << FairLogger::endl;
    } else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      crystalType = atoi(volumeName+9);//converting to int the alveolus index
      crystalCopy = cpAlv * 4 + cpCry;
      crystalId = (crystalType-1)*128 + cpAlv * 4 + cpCry;
      if (crystalType>20 || crystalType<1 || crystalCopy>128 || 
	  crystalCopy<1 || crystalId>2560 || crystalId<1)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 5 (v7.07)" 
		   << FairLogger::endl;
    } else  
      LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 8." 
		 << FairLogger::endl;
  } else if (fGeometryVersion==9) {
    //RESERVED FOR CALIFA 7.09 BARREL + 717PHOSWICH
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdAlv);    
    if (strncmp(alveolusECPrefix, volumeName,11) == 0) {
      crystalType = atoi(volumeName+12);//converting to int the alveolus index
      crystalCopy = cpAlv+1;
      crystalId = 3000 + cpAlv*30 + (crystalType-1);
      if (crystalType>30 || crystalType<1 ||
	  crystalCopy>60 || crystalCopy<1 || crystalId<3000 || crystalId>4800)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 9 (endcap). " 
		   << FairLogger::endl;
    } else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if(crystalType>1 && crystalType<17) {
	crystalCopy = cpAlv * 4 + cpCry;
	crystalId = (crystalType-1)*128 + cpAlv * 4 + cpCry;
      } else if (crystalType>16 && crystalType<20) {
	// Crystaltypes 17-19 are large crystals which fill type 6 alveoli, as 
	// opposed to the smaller crystals of which 4 fit in the other alveoli.
	crystalCopy = cpAlv + cpCry;
	crystalId = 2048 + (crystalType-17)*32 + cpAlv + cpCry;
      }
      if (crystalType>19 || crystalType<1 || crystalCopy>128 || 
	  crystalCopy<1 || crystalId>2144 || crystalId<1)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 9 (v7.07). " 
		   << FairLogger::endl;
    } else  
      LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 9." 
		 << FairLogger::endl;
  } else if (fGeometryVersion==10) {
    //The present scheme here done works with 8.11:
    // crystalType = alveolus type (from 1 to 17) [Alveolus number]
    // crystalCopy = alveolus copy * 4 + crystals copy +1 (from 1 to 128) 
    // crystalId = 1 to 32 for the first 32 crystals 
    //                     (single crystal in each alveoli)
    // or 32 + (alveolus type-2)*128 + (alvelous copy)*4 + (crystal copy) + 1        
    //                     (in this way, crystalId runs from 1 to 1952)
    //     
    
    // HAPOL-10/11/2013 (hector.alvarez@usc.es)
    // Moving to a new way getting crystal identification:  
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeName = gMC->VolName(volIdSupAlv);
    if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if (crystalType==1) {
	//only one crystal per alveoli in this ring, running from 1 to 32
        crystalCopy = cpSupAlv+1; 
        crystalId = cpSupAlv+1;                    
      } else if (crystalType>1 && crystalType<17) {
	//running from 0*4+0+1=1 to 31*4+3+1=128
        crystalCopy = cpSupAlv*4+cpCry+1;
	//running from 32+0*128+0*4+0+1=1 to 32+14*128+31*4+3+1=1952
        crystalId = 32+(crystalType-2)*128+cpSupAlv*4+cpCry+1; 
      }
      if (crystalType>16 || crystalType<1 || crystalCopy>128 || 
	  crystalCopy<1 || crystalId>1952 || crystalId<1) 
        LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 10. " 
		   << FairLogger::endl;
    } else LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 10." 
		      << FairLogger::endl;
  } else if (fGeometryVersion==11) {
    //RESERVED FOR CALIFA 8.11 BARREL + PHOSWICH
    
    const char *alveolusECPrefix = "Alveolus_EC_";
    const char *alveolusPrefix = "Alveolus_";
    const char *volumeNameEC = gMC->VolName(volIdAlv);    
    const char *volumeName   = gMC->VolName(volIdSupAlv);
    if (strncmp(alveolusECPrefix, volumeNameEC,11) == 0) {
      crystalType = atoi(volumeNameEC+12);//converting to int the alveolus index
      crystalCopy = cpAlv+1;
      crystalId = 3000 + cpAlv*30 + (crystalType-1);
      if (crystalType>30 || crystalType<1 ||
	  crystalCopy>60 || crystalCopy<1 || crystalId<3000 || crystalId>4800)
	LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 11 (endcap). " 
		   << FairLogger::endl;
    } else if (strncmp(alveolusPrefix, volumeName,8) == 0) {
      crystalType = atoi(volumeName+9);//converting to int the alveolus index
      if (crystalType==1) {
	//only one crystal per alveoli in this ring, running from 1 to 32
        crystalCopy = cpSupAlv+1; 
        crystalId = cpSupAlv+1;                    
      } else if (crystalType>1 && crystalType<17) {
	//running from 0*4+0+1=1 to 31*4+3+1=128
        crystalCopy = cpSupAlv*4+cpCry+1;
	//running from 32+0*128+0*4+0+1=1 to 32+14*128+31*4+3+1=1952
        crystalId = 32+(crystalType-2)*128+cpSupAlv*4+cpCry+1; 
      }
      if (crystalType>16 || crystalType<1 || crystalCopy>128 || 
	  crystalCopy<1 || crystalId>1952 || crystalId<1) 
        LOG(ERROR) << "R3BCalo: Wrong crystal number in geometryVersion 11. " 
		   << FairLogger::endl;
    } else LOG(ERROR) << "R3BCalo: Impossible crystalType for geometryVersion 11." 
		      << FairLogger::endl;
  } else LOG(ERROR) << "R3BCalo: Geometry version not available in R3BCalo::ProcessHits(). " 
		    << FairLogger::endl;
  
  if (fVerboseLevel>1)
    LOG(INFO) << "R3BCalo: Processing Points in Alveolus Nb " 
	      << volIdAlv << ", copy Nb " << cpAlv
	      << ", crystal copy Nb " << cpCry << " and unique crystal identifier " 
	      << crystalId << FairLogger::endl;
  
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fNSteps  = 0; // FIXME
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
    fEinc   = gMC->Etot();
  }
  
  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();
  fNSteps++;
  
  // Set additional parameters at exit of active volume. Create R3BCaloPoint.
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
    
    AddHit(fTrackID, fVolumeID, crystalType , crystalCopy , crystalId,
           TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
           TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
           TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
           TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
           fTime, fLength, fELoss);
    
    // Increment number of CaloPoints for this track
    R3BStack* stack = (R3BStack*) gMC->GetStack();
    stack->AddPoint(kCALIFA);
    
    //Adding a crystalHit support
    Int_t nCrystalHits = fCaloCrystalHitCollection->GetEntriesFast();
    Bool_t existHit = 0;
    
    if (nCrystalHits==0) AddCrystalHit(fGeometryVersion , crystalType , crystalCopy , crystalId, 
				       NUSmearing(fELoss), fTime, fNSteps, 
				       fEinc, fTrackID, fVolumeID, 
				       fParentTrackID, fTrackPID, fUniqueID);
    else {
      for (Int_t i=0; i<nCrystalHits; i++) {
        if ( ((R3BCaloCrystalHitSim *)(fCaloCrystalHitCollection->At(i)))
	     ->GetCrystalId() == crystalId ) {
          ((R3BCaloCrystalHitSim *)(fCaloCrystalHitCollection->At(i)))
	    ->AddMoreEnergy(NUSmearing(fELoss));
          if ( ((R3BCaloCrystalHitSim *)(fCaloCrystalHitCollection->At(i)))
	       ->GetTime() > fTime ) {
            ((R3BCaloCrystalHitSim *)(fCaloCrystalHitCollection->At(i)))
	      ->SetTime(fTime);
          }
          existHit=1; //to avoid the creation of a new CrystalHit
          break;
        }
      }
      if (!existHit) AddCrystalHit(fGeometryVersion , crystalType , crystalCopy , crystalId, 
				   NUSmearing(fELoss), fTime, fNSteps, 
				   fEinc, fTrackID, fVolumeID, 
				   fParentTrackID, fTrackPID, fUniqueID);
    }
    
    existHit=0;
    
    ResetParameters();
  }
  
  return kTRUE;
}
// ----------------------------------------------------------------------------
//void R3BCalo::SaveGeoParams(){
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


// -----   Public method EndOfEvent   -----------------------------------------
void R3BCalo::BeginEvent()
{

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BCalo::EndOfEvent()
{
  if (fVerboseLevel) Print();
  
  fCaloCollection->Clear();
  fCaloCrystalHitCollection->Clear();
  
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BCalo::Register()
{
  //FairRootManager::Instance()->Register("CrystalPoint", GetName(), 
  //                                      fCaloCollection, kTRUE);
  FairRootManager::Instance()->Register("CrystalHitSim", GetName(), 
					fCaloCrystalHitCollection, kTRUE);
  
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BCalo::GetCollection(Int_t iColl) const
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
void R3BCalo::Print(Option_t* option) const
{
  Int_t nHits = fCaloCollection->GetEntriesFast();
  LOG(INFO) << "R3BCalo: " << nHits << " points registered in this event" 
	    << FairLogger::endl;
  Int_t nCrystalHits = fCaloCrystalHitCollection->GetEntriesFast();
  LOG(INFO) << "R3BCalo: " << nCrystalHits << " sim hits registered in this event." 
	    << FairLogger::endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BCalo::Reset()
{
  fCaloCollection->Clear();
  fCaloCrystalHitCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BCalo::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  LOG(INFO) << "R3BCalo: " << nEntries << " entries to add" << FairLogger::endl;
  TClonesArray& clref = *cl2;
  R3BCaloPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (R3BCaloPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BCaloPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(INFO) << "R3BCalo: " << cl2->GetEntriesFast() << " merged entries" 
	    << FairLogger::endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BCaloPoint* R3BCalo::AddHit(Int_t trackID, Int_t detID, Int_t volid , 
			      Int_t copy, Int_t ident, TVector3 posIn, 
			      TVector3 posOut, TVector3 momIn, TVector3 momOut,
                              Double_t time, Double_t length, Double_t eLoss)
{
  TClonesArray& clref = *fCaloCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1)
    LOG(INFO) << "R3BCalo: Adding Point at (" << posIn.X() << ", " << posIn.Y()
	      << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	      << trackID << ", energy loss " << eLoss*1e06 << " keV" 
	      << FairLogger::endl;
  return new(clref[size]) R3BCaloPoint(trackID, detID, volid, copy , ident, 
				       posIn, posOut, momIn, momOut, time, 
				       length, eLoss);
}

// -----   Private method AddCrystalHit   --------------------------------------------
R3BCaloCrystalHitSim* R3BCalo::AddCrystalHit(Int_t geover, Int_t type, Int_t copy, Int_t ident,
					     Double_t energy, Double_t time, 
					     Int_t steps, Double_t einc,
					     Int_t trackid, Int_t volid, 
					     Int_t partrackid, Int_t pdgtype, 
					     Int_t uniqueid)
{
  TClonesArray& clref = *fCaloCrystalHitCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) {
    LOG(INFO) << "-I- R3BCalo: Adding Sim Hit in detector type " << type 
	      << ", and copy " << copy << " with unique identifier " << ident 
	      << " entering with " << einc*1e06 << " keV, depositing " << energy*1e06 
	      << " keV" << FairLogger::endl;
    LOG(INFO) << " -I- trackid: " << trackid << " volume id: " << volid 
	      << " partrackid : " << partrackid << " type: " << pdgtype 
	      << " unique id: " << uniqueid << FairLogger::endl;
  }
  return new(clref[size]) R3BCaloCrystalHitSim(geover, type, copy, ident, energy, time, 
					       steps, einc, trackid, volid, 
					       partrackid, pdgtype, uniqueid);
}

// -----   Private method NUSmearing  --------------------------------------------
Double_t R3BCalo::NUSmearing(Double_t inputEnergy)
{
  // Very simple preliminary scheme where the NU is introduced as a flat random
  // distribution with limits fNonUniformity (%) of the energy value.
  //
  return gRandom->Uniform(inputEnergy-inputEnergy*fNonUniformity/100,
			  inputEnergy+inputEnergy*fNonUniformity/100);
}



// -----  Public method SelectGeometryVersion  ----------------------------------
void R3BCalo::SelectGeometryVersion(Int_t version)
{
  fGeometryVersion=version;
}


// -----  Public method SetNonUniformity  ----------------------------------
void R3BCalo::SetNonUniformity(Double_t nonU)
{
  fNonUniformity = nonU;
  LOG(INFO) << "R3BCalo::SetNonUniformity to " << fNonUniformity << " %" 
	    << FairLogger::endl;
}


// -----   Public method ConstructGeometry   ----------------------------------
void R3BCalo::ConstructGeometry()
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


Bool_t R3BCalo::CheckIfSensitive(std::string name)
{
  if(TString(name).Contains("Crystal_")) {
    return kTRUE;
  }
  return kFALSE;
}


ClassImp(R3BCalo)
