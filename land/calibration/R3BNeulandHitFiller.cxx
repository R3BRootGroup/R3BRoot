#include "R3BNeulandHitFiller.h"
#include "TRandom1.h"
#include "TVector3.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "R3BNeulandCalData.h"

#define _x_ (entryPoint.X())
#define _y_ (entryPoint.Y())
#define _z_ (entryPoint.Z())
#define isHorizontal(x) ((x/50)%2 == 0)

R3BNeulandHitFiller::R3BNeulandHitFiller()
{
  data = new TClonesArray("R3BNeulandCalData", 100);
  r = new TRandom1(0);
  r->SetSeed(1);
  
  for(Int_t i = 0; i < _nPlanes; i++)
    for(Int_t j = 0; j < 50; j++){
      tOffset[i][j] = r->Rndm()*200 - 100;
      vEff[i][j] = (16 + r->Rndm()*0.5 - 0.25);
      tSync[i][j] = r->Rndm()*200 - 100;
      eGain[i][j][0] =  r->Rndm()*50 + 175;
      eGain[i][j][1] =  r->Rndm()*50 + 175;
    }
}

R3BNeulandHitFiller::~R3BNeulandHitFiller()
{
  delete data, r;
}

TVector3 R3BNeulandHitFiller::getDirection(){
  return TVector3(0, -1, 0.7 + r->Rndm()*0.8);
}

InitStatus R3BNeulandHitFiller::Init(){
	FairRootManager::Instance()->Register ("NeulandCalData", "NeuLAND", data, kTRUE);
	
	return kSUCCESS;
}

void R3BNeulandHitFiller::Exec(Option_t *option){
	data->Clear();
	nData = 0;

	Double_t scale = 0;
	Double_t scale_x = 0;
	Double_t scale_y = 0;
	Double_t scale_z = 0;
	
	Double_t pos, time = r->Rndm()*5;

	Int_t currentID;
	Int_t nextID;
	TVector3 from = TVector3(0,1,0);
	TVector3 entryPoint, exitPoint, direction;

	if(r->Rndm() < 0.4){ // Top
	  entryPoint = TVector3(r->Rndm()*50, 50, r->Rndm()*_nPlanes);
	  
	  currentID = ((Int_t)_z_) * 50;
	  if(((Int_t)_z_)%2 == 0) //horizontal
	    currentID += 49;
	  else //vertical
	    currentID += (Int_t)_x_;
	  
	  direction = getDirection();
	  if(_x_ < 12.5)
	    direction.RotateY(+0.4*r->Rndm()*TMath::Pi() + ((Int_t)_z_ < Planes/2 ? 0 : +0.6*TMath::Pi()));
	  else if(_x_ > 37.5)
	    direction.RotateY(-0.4*r->Rndm()*TMath::Pi() + ((Int_t)_z_ < Planes/2 ? 0 : -0.6*TMath::Pi()));
	  else
	    direction.RotateY(+0.8*r->Rndm()*TMath::Pi() - 0.4*TMath::Pi() + ((Int_t)_z_ < Planes/2 ? 0 : TMath::Pi()));
	  
	  from = TVector3(0, 1, 0);
	}
	else{ //sides
	  Double_t temp = r->Rndm();
	  if(temp < 0.3){ //Front i.e. z = 0
	    entryPoint = TVector3(r->Rndm()*50, 50-r->Rndm()*45, 0);
	    
	    currentID = (Int_t)_y_;
	    
	    direction  = getDirection();
	    if(_x_ < 12.5)
	      direction.RotateY(r->Rndm()*0.4*TMath::Pi());
	    else if(_x_ > 37.5)
	      direction.RotateY(-r->Rndm()*0.4*TMath::Pi());
	    else
	      direction.RotateY(r->Rndm()*0.8*TMath::Pi() - 0.4*TMath::Pi());
	    
	    from = TVector3(0, 0, -1);
	  }
	  else if(temp < 0.6){ //Back z = _nPlanes
	    entryPoint = TVector3(r->Rndm()*50, 50-r->Rndm()*45, _nPlanes);

	    currentID = (_nPlanes-1)*50 + (Int_t)_x_;
	    
	    direction  = getDirection();
	    if(_x_ < 12.5)
	      direction.RotateY(TMath::Pi() - r->Rndm()*0.4*TMath::Pi());
	    else if(_x_ > 37.5)
	      direction.RotateY(TMath::Pi() + r->Rndm()*0.4*TMath::Pi());
	    else
	      direction.RotateY(r->Rndm()*TMath::Pi()*0.8 + TMath::Pi()*0.6);
	    
	    from = TVector3(0, 0, -1);
	  }
	  else if(temp < 0.8){ //R-Side i.e. x = 0
	    entryPoint = TVector3(0, 50-r->Rndm()*40, r->Rndm()*_nPlanes);

	    currentID = ((Int_t)_z_) * 50;
	    if(((Int_t)_z_)%2 == 0) //horizontal
	      currentID += (Int_t)_y_;
	    else //vertical
	      currentID += 0;
	    
	    direction  = getDirection();
	    if(_z_ < _nPlanes/2)
	      direction.RotateY(r->Rndm()*TMath::Pi()*0.35 + TMath::Pi()*0.1);
	    else
	      direction.RotateY(TMath::Pi()*0.9-r->Rndm()*TMath::Pi()*0.23);
	    
	    from = TVector3(-1, 0, 0);
	  }
	  else{ //L-Side i.e. x = 50
	    entryPoint = TVector3(50, 50-r->Rndm()*40, r->Rndm()*_nPlanes);

	    currentID = ((Int_t)_z_) * 50;
	    if(((Int_t)_z_)%2 == 0) //horizontal
	      currentID += (Int_t)_y_;
	    else //vertical
	      currentID += 49;
	    
	    direction  = getDirection();
	    if(_z_ < _nPlanes/2)
	      direction.RotateY(-r->Rndm()*TMath::Pi()*0.35 - TMath::Pi()*0.1);
	    else
	      direction.RotateY(TMath::Pi()*0.9-r->Rndm()*TMath::Pi()*0.35);
	    
	    from = TVector3(1, 0, 0);
	  }
	}

	while(kTRUE){
	  if(isHorizontal(currentID)){
	    if(from.Y() != 0)
	      scale_y = -1/direction.Y();
	    else
	      scale_y = (((Int_t)_y_) - _y_)/direction.Y();
	    
	    if(from.Z() != 0)
	      scale_z = fabs(1/direction.Z());
	    else if(direction.Z() > 0)
	      scale_z = (((Int_t)_z_ + 1) - _z_)/direction.Z();
	    else
	      scale_z = (((Int_t)_z_) - _z_)/direction.Z();
	    
	    if(scale_y < scale_z){
	      scale = scale_y;
	      exitPoint = entryPoint + scale * direction;
	      nextID = currentID - 1;
	      from = TVector3(0, 1, 0);
	    }
	    else{
	      scale = scale_z;
	      exitPoint = entryPoint + scale * direction;
	      if(from.Z() != 0)
		nextID = ((Int_t)(_z_ + (direction.Z() > 0 ? 1.0001 : -1.0001))) * 50 + ((Int_t)exitPoint.X());
	      else
		nextID = ((Int_t)(_z_ + (direction.Z() > 0 ? 1 : -1))) * 50 + ((Int_t)exitPoint.X());
	      from = TVector3(0, 0, (direction.Z() > 0 ? -1 : 1));
	    }
	  }
	  else{
	    if(from.X() != 0)
	      scale_x = fabs(1/direction.X());
	    else if(direction.X() > 0){
	      scale_x = (((Int_t)_x_ + 1) - _x_)/direction.X();
	    }
	    else
	      scale_x = (((Int_t)_x_) - _x_)/direction.X();
	    
	    if(from.Z() != 0)
	      scale_z = fabs(1/direction.Z());
	    else if(direction.Z() > 0){
	      scale_z = (((Int_t)_z_ + 1) - _z_)/direction.Z();
	    }
	    else
	      scale_z = (((Int_t)_z_) - _z_)/direction.Z();
	    
	    if(scale_x < scale_z){
	      scale = scale_x;
	      exitPoint = entryPoint + scale * direction;
	      nextID = currentID + (direction.X() > 0 ? 1 : -1);
	      from = TVector3((direction.X() > 0 ? -1 : 1), 0, 0);
	    }
	    else{
	      scale = scale_z;
	      exitPoint = entryPoint + scale * direction;
	      if(from.Z() != 0)
		nextID = ((Int_t)(_z_ + (direction.Z() > 0 ? 1.0001 : -1.0001))) * 50 + ((Int_t)exitPoint.Y());
	      else
		nextID = ((Int_t)(_z_ + (direction.Z() > 0 ? 1 : -1))) * 50 + ((Int_t)exitPoint.Y());
	      from = TVector3(0, 0, (direction.Z() > 0 ? -1 : 1));
	    }
	  }
	  
	  if(scale == 0 || currentID < 0 || currentID >= _nPlanes*50){
	    std::cout << "something was wrong..." << std::endl;
	    return;
	  }
	  
	  TVector3 centralPoint = (entryPoint + 0.5 * scale * direction);
	  if(isHorizontal(currentID)){
	    pos = centralPoint.X();
	  }
	  else{
	    pos = centralPoint.Y();
	  }
	  
	  Int_t barID = currentID % 50, plane = currentID / 50;
	  
	  Double_t t = time + tSync[plane][barID] + tOffset[plane][barID]/2 + (0.5 * scale * direction).Mag() / cLight + (50-pos)*5/ (vEff[plane][barID]);
	  Int_t qdc = QDCsplitting * scale * direction.Mag() * minIonizing * eGain[plane][barID][0] * TMath::Exp(- lAtt * (50 - pos));
	  new ((*data)[nData++]) R3BNeulandCalData(currentID + 1, 1, t, qdc);

	  t = time + tSync[plane][barID] - tOffset[plane][barID]/2 + (0.5 * scale * direction).Mag() / cLight + pos*5/ (vEff[plane][barID]);
	  qdc = QDCsplitting * scale * direction.Mag() * minIonizing * eGain[plane][barID][1] * TMath::Exp(- lAtt * pos);
	  new ((*data)[nData++]) R3BNeulandCalData(currentID + 1, 2, t, qdc);
	  
	  time += (scale * direction).Mag() / cLight;
	  
	  entryPoint = exitPoint;
	  currentID = nextID; 
	  
	  if(_x_ <= 0 || _x_ >= 50 || _y_ <= 0 || _z_ <= 0 || _z_ >= _nPlanes)
	    return;
	    

	}
}

ClassImp(R3BNeulandHitFiller)