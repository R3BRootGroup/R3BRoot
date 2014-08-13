
#include "R3BGeomAnalyzer.h"

#include "TSystem.h"
#include "TSQLStatement.h"
#include "TGeoArb8.h"

#include "FairLogger.h"
#include "FairRuntimeDb.h"

#include "FairDbConnection.h"       
#include "FairDbConnectionPool.h"   
#include "FairDbStatement.h"        
#include "FairDbTableInterfaceStore.h" 
#include "FairDbTableInterface.h" 
#include "FairDbCache.h"
 

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <list>
#include <map>
#include <memory>
#include <string>


using std::auto_ptr;		
using std::cout;
using std::endl;

		
R3BGeomAnalyzer::R3BGeomAnalyzer() :
  fN_Alveoli(0),
  fN_Crystals(0),
  fGeom(NULL)
{
   cout << endl;
   cout << "-I- R3BGeomAnalyzer  created " << endl;
   cout << endl;
}

R3BGeomAnalyzer::~R3BGeomAnalyzer() {
}


Bool_t R3BGeomAnalyzer::Import(TFile *geoFile) {
  
  if (fGeom) delete fGeom;
  else { 
        fGeom = new TGeoManager(); 
  }

  TGeoVolume* top_vol = (TGeoVolume*) geoFile->Get("TOP");

  // Set it unvisible
  top_vol->SetVisibility(kFALSE);

  // Add the volume to geo manager
  fGeom->AddVolume(top_vol);

  // Set it as the top volume
  fGeom->SetTopVolume(top_vol);

  // Close the Geometry
  fGeom->CloseGeometry();

  return kTRUE;
}

Bool_t R3BGeomAnalyzer::RecReadMedia(Bool_t keepGoing){
  TGeoNode* node = fGeom->GetCurrentNode();
  
  if (node) {
	TString node_name(node->GetName());
    if (!node_name.Contains("TOP")) {
	  //cout << " -I- Crystal found " << node_name << " : " << fN_Crystals++ << endl;
      TGeoVolume* vol = node->GetVolume();
      
	  if (vol) { 
		TGeoMaterial* mat = vol->GetMaterial();
		if (mat) {
		  if (!IsMediaStored(mat->GetName())) fMapMedia[mat->GetName()]=mat; 
		}        
	  }
	}

	if (keepGoing) {
	  for (int i=0; i<node->GetNdaughters(); ++i) {
		fGeom->CdDown(i);
		RecReadMedia(keepGoing);
	  }
	}	
	// Pop this node off the "stack"
	fGeom->CdUp();
	return keepGoing;
	
  } else{
	cout << " -W- R3BGeomAnalyzer::RecReadMedia() no node found !" << endl;
	return kFALSE;
  }
  
}

Bool_t R3BGeomAnalyzer::RecReadShapes(Bool_t keepGoing){
  TGeoNode* node = fGeom->GetCurrentNode();
  
  if (node) {
	TString node_name(node->GetName());
	TGeoVolume* vol = node->GetVolume();
	TGeoShape* shape = (TGeoShape*) vol->GetShape();   
	// Crystals
    if (node_name.Contains("Crystal_")) {
	  if (!IsCrystalStored(shape)){
        cout << "-I- Crystal Unique Shape found .... Name# " << shape->GetName() << endl;
		fV_Crystals.push_back(shape);
	  }
	}
    // Alveoli 
     else if (node_name.Contains("Alveolus_")) {
      if (!IsAlveolusStored(shape)){
        cout << "-I- Alveolus Unique Shape found .... Name# " << shape->GetName() << endl;
		fV_Alveoli.push_back(shape);
	  }
	}
	
	if (keepGoing) {
	  for (int i=0; i<node->GetNdaughters(); ++i) {
		fGeom->CdDown(i);
		RecReadShapes(keepGoing);
	  }
	}	
	// Pop this node off the "stack"
	fGeom->CdUp();
	return keepGoing;
	
  } else{
	cout << " -W- R3BGeomAnalyzer::RecReadShapes() no node found !" << endl;
	return kFALSE;
  }

}




Bool_t R3BGeomAnalyzer::RecReadNodes(Bool_t keepGoing){

  TGeoNode* node = fGeom->GetCurrentNode();
  if (node) {
	TString node_name(node->GetName());
    if (node_name.Contains("Crystal_")) {
      // GLobal Transformation   
      TGeoRotation *rot = (TGeoRotation*) fGeom->GetCurrentMatrix();
      //rot->Print();       
      Double_t phi, theta, psi;
	  rot->GetAngles(psi,theta,phi);
      const Double_t *trans = rot->GetTranslation();

	  //cout << " -I- Crystal found Name  " << node_name << " : " << fN_Crystals++ << endl;
      //cout << " -I- Crystal Euler angles # " << psi << " : " << theta << " : " << phi << endl; 
      //cout << " -I- Crystal Translation # " << trans[0] << " : " << trans[1] << " : " << trans[2] << endl; 


      // Get Type Crystal 	  
	  TGeoVolume* vol = node->GetVolume();
      TGeoVolume* moth_vol = node->GetMotherVolume();    
      TGeoArb8* shape = (TGeoArb8*) vol->GetShape();   
      TGeoArb8* moth_shape = (TGeoArb8*) moth_vol->GetShape();   
      Double_t dZ = shape->GetDz(); 
      Double_t wrap_thickness = TMath::Abs(moth_shape->GetDZ()-dZ);

	  Int_t crystal_typeId = GetCrystalTypeId(shape);
      const Char_t* crystal_name = ((TGeoShape*) fV_Crystals[crystal_typeId])->GetName();     
	  //cout << " -I- Crystal type Id # " << crystal_typeId 
      //                                  <<  " crystal name# " <<  crystal_name 
	  //	                                <<  " wrapping# " <<  wrap_thickness << endl; 
	   
      // Create and convert parameters
      R3BCaloCrystalPar* par = new  R3BCaloCrystalPar();
      // No compID for now
      par->SetCompId(0);
      par->SetCrystalNodeId(fN_Crystals);
      par->SetCrystalTypeId(crystal_typeId+1);
      par->SetWrapThickness(wrap_thickness);
      par->SetCenter(trans[0],trans[1],trans[0]);
      par->SetRotEuler(psi,theta,phi);         

	  fCrystals.push_back(par);
      fN_Crystals++;

	} else if (node_name.Contains("Alveolus_")) {
      // GLobal Transformation   
      TGeoRotation *rot = (TGeoRotation*) fGeom->GetCurrentMatrix();
      //rot->Print();       
      Double_t phi, theta, psi;
	  rot->GetAngles(phi,theta,psi);
      const Double_t *trans = rot->GetTranslation();

	  //cout << " -I- Alveolus found Name  " << node_name << " : " << fN_Alveoli++ << endl;
      //cout << " -I- Alveolus Euler angles # " << phi << " : " << theta << " : " << psi << endl; 
      //cout << " -I- Alveolus Translation # " << trans[0] << " : " << trans[1] << " : " << trans[2] << endl; 


      // Get Type Alveolus 	  
	  TGeoVolume* vol = node->GetVolume();
      TGeoVolume* moth_vol = node->GetDaughter(0)->GetVolume();    
      TGeoArb8* shape = (TGeoArb8*) vol->GetShape();   
      TGeoArb8* moth_shape = (TGeoArb8*) moth_vol->GetShape();   
      Double_t dZ = shape->GetDz(); 
      Double_t cf_thickness = TMath::Abs(moth_shape->GetDZ()-dZ);   

	  Int_t alveolus_typeId = GetAlveolusTypeId(shape);
      const Char_t* alveolus_name = ((TGeoShape*) fV_Alveoli[alveolus_typeId])->GetName();     
	  cout << " -I- Alveolus type Id # " << alveolus_typeId 
                                        <<  " alveolus name# " <<  alveolus_name 
		                                <<  " CFthickness# " <<  cf_thickness << endl; 

     // Create and convert parameters
      R3BCaloAlveolusPar* par = new  R3BCaloAlveolusPar();
      // No compID for now
      par->SetCompId(0);
      par->SetAlveolusNodeId(fN_Alveoli);
      par->SetAlveolusTypeId(alveolus_typeId+1);
      par->SetCFThickness(cf_thickness);
      par->SetCenter(trans[0],trans[1],trans[0]);
      par->SetRotEuler(psi,theta,phi);         

	  fAlveoli.push_back(par);
      fN_Alveoli++;
 
	} 

	if (keepGoing) {
	  for (int i=0; i<node->GetNdaughters(); ++i) {
		fGeom->CdDown(i);
		RecReadNodes(keepGoing);
	  }
	}	
	// Pop this node off the "stack"
	fGeom->CdUp();
	return keepGoing;
	
  } else{
	cout << " -W- R3BGeomAnalyzer::RecReadNodes() no node found !" << endl;
	return kFALSE;
  }
  
}


Bool_t R3BGeomAnalyzer::CompareShape(TGeoArb8* a , TGeoArb8* b){

  Bool_t isIdentical=kTRUE;
  Double_t za = a->GetDz();
  Double_t zb = b->GetDz();
  Double_t* a_vtx = a->GetVertices();
  Double_t* b_vtx = b->GetVertices();
 
  if  (TMath::Abs(za-zb)> 1.e-16) {
	return kFALSE;
  } else {
    Double_t l_diff[8]={0.,0.,0.,0.,0.,0.,0.,0.};   
    Double_t h_diff[8]={0.,0.,0.,0.,0.,0.,0.,0.};   
	
	for(Int_t i=0;i<8;i++) {
      l_diff[i] = TMath::Abs(a_vtx[i] - b_vtx[i]);
      //cout << " -I- DIFF " << a_vtx[i] << " : " <<  b_vtx[i] << " : " << l_diff[i] << endl; 
     if ((l_diff[i]>1.e-16)) return kFALSE; 
	}
  }

  return isIdentical;
}


Bool_t R3BGeomAnalyzer::IsMediaStored(const string& media_name){
  map<string, TGeoMaterial*>::const_iterator it = fMapMedia.find(media_name);
  return (it!=fMapMedia.end());
}

Bool_t R3BGeomAnalyzer::IsCrystalStored(TGeoShape* aShape){
  Bool_t isFound=kFALSE;
  for(int i=0; i< fV_Crystals.size(); i++ ){
	if (CompareShape((TGeoArb8*) fV_Crystals[i], (TGeoArb8*) aShape)) isFound=kTRUE;
  } 
  return isFound;
}

Bool_t R3BGeomAnalyzer::IsAlveolusStored(TGeoShape* aShape){
  Bool_t isFound=kFALSE;
  for(int i=0; i< fV_Alveoli.size(); i++ ){
	if (CompareShape((TGeoArb8*) fV_Alveoli[i], (TGeoArb8*) aShape)) isFound=kTRUE;
  } 
  return isFound;
}

void R3BGeomAnalyzer::PrintCrystals(){
  for(int i=0; i< fV_Crystals.size(); i++ ){
	fV_Crystals[i]->InspectShape();
  } 
}

void R3BGeomAnalyzer::PrintAlveoli(){
  for(int i=0; i< fV_Alveoli.size(); i++ ){
	fV_Alveoli[i]->InspectShape();
  } 
}

Int_t R3BGeomAnalyzer::GetCrystalTypeId(TGeoArb8* aShape){
 Int_t index=-1;
  for(int i=0; i< fV_Crystals.size(); i++ ){
	if (CompareShape((TGeoArb8*) fV_Crystals[i], (TGeoArb8*) aShape)) index=i;
  } 
  return (index);
}

Int_t R3BGeomAnalyzer::GetAlveolusTypeId(TGeoArb8* aShape){
 Int_t index=-1;
  for(int i=0; i< fV_Alveoli.size(); i++ ){
	if (CompareShape((TGeoArb8*) fV_Alveoli[i], (TGeoArb8*) aShape)) index=i;
  } 
  return (index);
}




void R3BGeomAnalyzer::FillGeometryPar(R3BCaloGeometryPar* par){
  // Loop over Materia
  for(map<string, TGeoMaterial*>::const_iterator it = fMapMedia.begin();it != fMapMedia.end(); it++){
    TGeoMaterial* a_mat = it->second; 
	if (a_mat){ 
	  a_mat->Print();
      par->AddMaterial(a_mat); 
	} 
  }

  //Loop over Shapes
  Int_t cid = 0;
  for(vector<TGeoShape*>::const_iterator it = fV_Crystals.begin();it != fV_Crystals.end(); it++){
    TGeoShape* a_sh = (*it); 
	if (a_sh){ 
      cid++; 
	  //a_sh->Print();
      R3BCaloCrystalGeoPar* sh_par = new R3BCaloCrystalGeoPar();
      sh_par->SetShape( (TGeoArb8*) a_sh);
      sh_par->SetCompId(cid);
      sh_par->SetCrystalType(cid);   
      par->AddCrystalShapes(sh_par); 
	} 
  }
  cid=0;
  for(vector<TGeoShape*>::const_iterator it = fV_Alveoli.begin();it != fV_Alveoli.end(); it++){
    TGeoShape* a_sh = (*it); 
	if (a_sh){ 
      cid++; 
	  //a_sh->Print();
      R3BCaloAlveolusGeoPar* sh_par = new R3BCaloAlveolusGeoPar();
      sh_par->SetShape( (TGeoArb8*) a_sh);
      sh_par->SetCompId(cid);
      sh_par->SetAlveolusType(cid);   
      par->AddAlveoliShapes(sh_par); 
	} 
  }

  //Loop over Crystal Nodes
  for(vector<R3BCaloCrystalPar*>::const_iterator it = fCrystals.begin();it != fCrystals.end(); it++){
    R3BCaloCrystalPar* c_par = (*it); 
	if (c_par){ 
      par->AddCrystals(c_par); 
	} 
  }

  //Loop over Alveolus Nodes
  for(vector<R3BCaloAlveolusPar*>::const_iterator it = fAlveoli.begin();it != fAlveoli.end(); it++){
    R3BCaloAlveolusPar* c_par = (*it); 
	if (c_par){ 
      par->AddAlveoli(c_par); 
	} 
  }



  par->setChanged();
}








ClassImp(R3BGeomAnalyzer)
