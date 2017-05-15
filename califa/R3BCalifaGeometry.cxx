#include <vector>

#include <TString.h>
#include <TSystem.h>
#include <TFile.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TMath.h>
#include <TVector3.h>
#include <TGeoNavigator.h>

#include <FairLogger.h>

#include "R3BCalifaGeometry.h"

R3BCalifaGeometry* R3BCalifaGeometry::inst = NULL;

R3BCalifaGeometry* R3BCalifaGeometry::Instance(int version)
{
  if(!inst)
    inst = new R3BCalifaGeometry(version);
  else if(inst->fGeometryVersion != version)
  {
    LOG(ERROR) << "R3BCalifaGeometry::Instance(): Existing instance with different geometry version than requested. "
              << "Undefined beheaviour possible!" << FairLogger::endl;

    inst = new R3BCalifaGeometry(version);
  }

  return inst;
}

R3BCalifaGeometry::R3BCalifaGeometry(int version)  : fGeometryVersion(version), fNavigator(NULL)
{
  LOG(DEBUG) << "Creating new R3BCalifaGeometry for version " << version << FairLogger::endl;

  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  geoPath += "/geometry/";

  switch(version)
  {
  case 16:
    // Barrel 8.11 with displaced crystal centers
    geoPath += "califa_16_v8.11_cc0.2.geo.root";
    break;

  case 17:
    // Barrel 8.11 with crystal centers at (0,0,0) in local system
    geoPath += "califa_17_v8.11_cc0.2.geo.root";
    break;

  case 0x438b:
    // s438b geometry (subset of 8.11 Barrel + rotation)
    geoPath += "califa_demo.geo.root";
    break;

  default:
    LOG(ERROR) << "R3BCalifaGeometry: Unsupported geometry version: " << version << FairLogger::endl;
    return;
  }

  if(gGeoManager && strcmp(gGeoManager->GetTopVolume()->GetName(), "cave") == 0)
  {
    // Already set up (MC mode)
    LOG(DEBUG) << "R3BCalifaGeometry: Using existing geometry" << FairLogger::endl;
    return;
  }

  // Stand alone mode
  LOG(DEBUG) << "R3BCalifaGeometry: Creating new geometry" << FairLogger::endl;
  TFile *f = new TFile(geoPath, "READ");
  TGeoVolume *v = dynamic_cast<TGeoVolume*>(f->Get("TOP"));
  if(!v)
  {
    LOG(ERROR) << "R3BCalifaGeometry: Could not open CALIFA geometry file: No TOP volume" << FairLogger::endl;
    return;
  }

  v->SetName("cave");
  if(!gGeoManager)
    gGeoManager = new TGeoManager();
  gGeoManager->SetTopVolume(v);

  fNavigator = new TGeoNavigator(gGeoManager);
}

void R3BCalifaGeometry::GetAngles(Int_t iD, Double_t* polar, 
				 Double_t* azimuthal, Double_t* rho)
{

  Double_t local[3]={0,0,0};
  Double_t master[3];
  Int_t crystalType = 0;
  Int_t crystalCopy = 0;
  Int_t alveolusCopy =0;
  Int_t crystalInAlveolus=0;

 if (fGeometryVersion==16) {
    //The present scheme here done works with 8.11
    // crystalType = alveolus type (from 1 to 16) [Alveolus number]
    // crystalCopy = alveolus copy * 4 + crystals copy +1 (from 1 to 128)
    // crystalId = 1 to 32 for the first 32 crystals 
    //                     (single crystal in each alveoli)
    // or 32 + (alveolus type-2)*128 + (alvelous copy)*4 + (crystal copy) + 1
    //                     (from 1 to 1952)
    //
    Char_t nameVolume[200];
    if (iD<3000) {
      if(iD<33) crystalType = 1;  //Alv type 1
      else crystalType = (Int_t)((iD-33)/128) + 2;  //Alv type (2 to 16)
      if(iD<33) crystalCopy = iD;     //for Alv type 1 
      else crystalCopy = ((iD-33)%128) + 1;         //CrystalCopy (1 to 128)
      if(iD<33) alveolusCopy = iD;    //Alv type 1 
      else alveolusCopy =(Int_t)(((iD-33)%128)/4) +1; //Alveolus copy (1 to 32)
      if(iD<33) crystalInAlveolus =1;          //Alv type 1
      else crystalInAlveolus = (iD-33)%4 + 1;//Crystal number in alveolus (1 to 4)
    
      Int_t alveoliType[16]={1,2,2,2,2,3,3,4,4,4,5,5,5,6,6,6};
    
      sprintf(nameVolume, 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1/CrystalWithWrapping_%i_%i_%i/Crystal_%i_%i_1",
	      crystalType, alveolusCopy-1, 
	      crystalType, alveoliType[crystalType-1], 
	      crystalInAlveolus, crystalInAlveolus-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);
    
      // The definition of the crystals is different in this particular EndCap design:
      // the origin for each crystal is the alveoli corner
      if (crystalType==1) {
	local[0]=27.108/8; local[1]=-28.0483/8; local[2]=0;
      } else if (crystalType==2 || crystalType==3 || 
		 crystalType==4 || crystalType==5) {
	if(crystalInAlveolus==1){
	  local[0]=37.4639/8; local[1]=-8.57573/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=37.4639/8; local[1]=-31.1043/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=9.52012/8; local[1]=-8.57573/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=9.52012/8; local[1]=-31.1043/8; local[2]=0;
	}
      } else if (crystalType==6 || crystalType==7) {
	if(crystalInAlveolus==1){
	  local[0]=38.3282/8; local[1]=-5.49819/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=38.3282/8; local[1]=-23.0538/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=8.66384/8; local[1]=-5.49819/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=8.66384/8; local[1]=-23.0538/8; local[2]=0;
	}
      } else if (crystalType==8 || crystalType==9 || crystalType==10) {
	if(crystalInAlveolus==1){
	  local[0]=38.3683/8; local[1]=-4.71618/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=38.3683/8; local[1]=-19.8438/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=8.43569/8; local[1]=-4.71618/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=8.43569/8; local[1]=-19.8438/8; local[2]=0;
	}
      } else if (crystalType==11 || crystalType==12 || crystalType==13) {
	if(crystalInAlveolus==1){
	  local[0]=38.3495/8; local[1]=-4.70373/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=38.3495/8; local[1]=-19.8403/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=8.66654/8; local[1]=-4.70373/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=8.66654/8; local[1]=-19.8403/8; local[2]=0;
	}
      } else if (crystalType==14 || crystalType==15 || crystalType==16) {
	if(crystalInAlveolus==1){
	  local[0]=37.9075/8; local[1]=-4.66458/8; local[2]=0;
	} else if(crystalInAlveolus==2) {
	  local[0]=37.9075/8; local[1]=-19.8474/8; local[2]=0;
	} else if(crystalInAlveolus==3) {
	  local[0]=9.07247/8; local[1]=-19.8474/8; local[2]=0;
	} else if(crystalInAlveolus==4){
	  local[0]=9.07247/8; local[1]=-4.66458/8; local[2]=0;
	}
      }		

      gGeoManager->CdTop();

      if(gGeoManager->CheckPath(nameVolume)) gGeoManager->cd(nameVolume);
      else { 
	LOG(ERROR) << "R3BCalifaCrysta2Hit: Invalid crystal path: " << nameVolume
		   << FairLogger::endl;
	return; 
      }
      gGeoManager->LocalToMaster(local, master);

    } else{
      //For CC iPhos+phoswich endcap
      crystalType = ((iD - 3000) % 24) + 1;
      crystalCopy = (iD-3000 - crystalType + 1) / 24 + 1;
      Int_t alveoliType[24]={1,1,2,2,3,3,4,4,5,6,7,8,9,9,10,10,11,11,12,12,13,13,14,14};
      //Int_t alveoliType[24]={1,2,3,4,0,0,0,0,5,6,7,8,9,9,10,10,11,11,12,12,13,13,14,14};
      Int_t wrappingType[24]={1,1,2,2,3,3,4,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
      //Char_t nameVolume[200];

      sprintf(nameVolume, "/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
              alveoliType[crystalType-1], crystalCopy-1, wrappingType[crystalType-1], crystalType);
      gGeoManager->cd(nameVolume);
      gGeoManager->LocalToMaster(local, master);
    }
  }
  else if(fGeometryVersion == 17 || fGeometryVersion == 0x438b)
  {
    const char *nameVolume = GetCrystalVolumePath(iD);
    if(!nameVolume)
      return;

    if(!gGeoManager->cd(nameVolume))
    {
      LOG(ERROR) << "R3BCalifaGeometry: Invalid volume path: " << nameVolume << FairLogger::endl;
      return;
    }

    // TGeoManager::LocalToMaster does the whole magic:
    // Local to Master transformation along the whole geometry tree up to the top volume
    // Note: Requires the crystal centers (reference point) to be at (0,0,0) in local coordinates
    gGeoManager->LocalToMaster(local, master);

  }
 else
 {
   LOG(ERROR) << "R3BCalifaGeometry: Geometry version not available in R3BCalifaGeometry::GetAngles(). " << FairLogger::endl;
   return;
 }
  
  
  TVector3 masterV(master[0],master[1],master[2]);
  //masterV.Print();
  *polar=masterV.Theta();
  *azimuthal=masterV.Phi();
  *rho=masterV.Mag();
}

const char * R3BCalifaGeometry::GetCrystalVolumePath(int iD)
{
  // Must be static since function returns string
  // owned by nameVolume, wich must not be destroyed
  static TString nameVolume;

  Int_t crystalType = 0;
  Int_t crystalCopy = 0;
  Int_t alveolusCopy =0;
  Int_t crystalInAlveolus=0;


  switch(fGeometryVersion)
  {
    case 16:
    case 17:
    case 0x438b:
    if (iD >= 1 && iD <= 1952)
    {
      // Barrel
      if(iD<33)
      {
        // First ring (single crystals)
        crystalType = 1;  //Alv type 1
        crystalCopy = iD;     //for Alv type 1 
        alveolusCopy = iD;    //Alv type 1 
        crystalInAlveolus =1;          //Alv type 1
      }
      else
      {
        // Ring 2 - 16: 2x2 crystals
        crystalType = (Int_t)((iD-33)/128) + 2;  //Alv type (2 to 16)
        crystalCopy = ((iD-33)%128) + 1;         //CrystalCopy (1 to 128)
        alveolusCopy =(Int_t)(((iD-33)%128)/4) +1; //Alveolus copy (1 to 32)
        crystalInAlveolus = (iD-33)%4 + 1;//Crystal number in alveolus (1 to 4)
      }
      Int_t alveoliType[16]={1,2,2,2,2,3,3,4,4,4,5,5,5,6,6,6};
    
      nameVolume = TString::Format( 
	      "/cave_1/CalifaWorld_0/Alveolus_%i_%i/AlveolusInner_%i_1/CrystalWithWrapping_%i_%i_%i/Crystal_%i_%i_1",
	      crystalType, alveolusCopy-1, 
	      crystalType, alveoliType[crystalType-1], 
	      crystalInAlveolus, crystalInAlveolus-1, 
	      alveoliType[crystalType-1], crystalInAlveolus);
    }
    else if(iD >= 3000)
    {
      //For CC iPhos+phoswich endcap
      crystalType = ((iD - 3000) % 24) + 1;
      crystalCopy = (iD-3000 - crystalType + 1) / 24 + 1;
      Int_t alveoliType[24]={1,1,2,2,3,3,4,4,5,6,7,8,9,9,10,10,11,11,12,12,13,13,14,14};
      //Int_t alveoliType[24]={1,2,3,4,0,0,0,0,5,6,7,8,9,9,10,10,11,11,12,12,13,13,14,14};
      Int_t wrappingType[24]={1,1,2,2,3,3,4,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
      //Char_t nameVolume[200];

      nameVolume = TString::Format("/cave_1/CalifaWorld_0/Alveolus_EC_%i_%i/CrystalWithWrapping_%i_1/Crystal_%i_1",
              alveoliType[crystalType-1], crystalCopy-1, wrappingType[crystalType-1], crystalType);

    }
    else
    {
      LOG(ERROR) << "R3BCalifaGeometry: Invalid crystal ID " << iD << " for geometry version 17" << FairLogger::endl;
      return NULL;
    }
    break;

    default:
      LOG(ERROR) << "R3BCalifaGeometry: Invalid geometry version for GetCrystalVolumePath()" << FairLogger::endl;
      return NULL;
  }

  return nameVolume.Data();
}

double R3BCalifaGeometry::GetDistanceThroughCrystals(TVector3 &startVertex, TVector3 &direction, TVector3 *hitPos, int *numCrystals, int *crystalIds)
{
  int maxNumCrystals = 0;

  if(numCrystals != NULL && crystalIds != NULL)
  {
    maxNumCrystals = *numCrystals;
    *numCrystals = 0;
  }

  TGeoNode *n;

  gGeoManager->InitTrack(startVertex.X(), startVertex.Y(), startVertex.Z(),
      direction.X()/direction.Mag(), direction.Y()/direction.Mag(), direction.Z()/direction.Mag());

  double distance = 0;
  const Double_t *pos;
  bool inCrystal = false, wasInCrystal = false;
  TString nodeName;

  while((n = gGeoManager->FindNextBoundaryAndStep()))
  {
    nodeName = n->GetName();

    if(inCrystal)
      distance += gGeoManager->GetStep();

    inCrystal = nodeName.BeginsWith("Crystal_");

    if(inCrystal && maxNumCrystals != 0)
    {
      int cid = GetCrystalId(gGeoManager->GetPath());
      if(cid != -1 && (*numCrystals == 0 || cid != crystalIds[(*numCrystals)-1]))
      {
        crystalIds[(*numCrystals)++] = cid;
        maxNumCrystals--;
      }
    }

    if(hitPos != NULL && inCrystal && !wasInCrystal)
    {
      pos = gGeoManager->GetCurrentPoint();
      hitPos->SetXYZ(pos[0], pos[1], pos[2]);
      wasInCrystal = true;
    }
  }

  return distance;
}

int R3BCalifaGeometry::GetCrystalId(const char *volumePath)
{
  std::vector<const char*> volumeNames;
  std::vector<int> nodeCopies;
  TGeoNode *n;

  int crystalId = -1;

  for(fNavigator->cd(volumePath); (n = fNavigator->GetCurrentNode()) != NULL; fNavigator->CdUp())
  {
    volumeNames.push_back(n->GetName());
    nodeCopies.push_back(n->GetNumber());
  }

  if (fGeometryVersion==16 || fGeometryVersion==17 || fGeometryVersion==0x438b) {
    //RESERVED FOR CALIFA 8.11 BARREL + CC 0.2
    
    if(volumeNames.size() < 4)
    {
      LOG(ERROR) << "R3BCalifaGeometry::GetCrystalId(): Invalid path: " << volumePath << FairLogger::endl;
      return -1;
    }

    int cp1 = nodeCopies[0];
    int cpCry = nodeCopies[1];
    int cpAlv = nodeCopies[2];
    int cpSupAlv = nodeCopies[3];

    int crystalType, crystalCopy;

    const char *alveolusECPrefix = "Alveolus_EC";
    const char *alveolusPrefix = "Alveolus";
    const char *volumeName = volumeNames[3];
    const char *volumeNameCrystal ="";

    // Workaround to fix the hierarchy difference between Barrel and Endcap
    if (strncmp("CalifaWorld", volumeName,10) == 0) {
      volumeName = volumeNames[2];
      volumeNameCrystal = volumeNames[0];
    }
    if (strncmp(alveolusECPrefix, volumeName, 11) == 0) {
      crystalType = atoi(volumeNameCrystal+8);     //converting to int the crystal index
      crystalCopy = cpAlv+1;

      if(crystalType < 9 && crystalType%2 == 0) {
	  crystalType -= 1;
      }
      crystalId = 3000 + cpAlv*24 + (crystalType-1);
      
      if (crystalType>24 || crystalType<1 ||
	  crystalCopy>32 || crystalCopy<1 || 
          crystalId<3000 || crystalId>4800)
      {
	LOG(ERROR) << "R3BCalifaGeometry: Wrong crystal number in geometryVersion 16+ (CC). " 
		   << FairLogger::endl;
        return -1;
      }
    //if BARREL
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
      {
        LOG(ERROR) << "R3BCalifaGeometry: Wrong crystal number in geometryVersion 16+ (BARREL)." 
		   << FairLogger::endl;
        return -1;
      }
    }
    else
    {
      LOG(ERROR) << "R3BCalifaGeometry: Impossible crystalType for geometryVersion 16+." 
		      << FairLogger::endl;
      return -1;
    }
  }
  else
  {
    LOG(ERROR) << "R3BCalifaGeometry: Geometry version not available in R3BCalifaGeometry::GetCrystalId(). " 
		    << FairLogger::endl;
    return -1;
  }

  return crystalId;
}

ClassImp(R3BCalifaGeometry);
