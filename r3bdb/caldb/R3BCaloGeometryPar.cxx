#include "R3BCaloGeometryPar.h"


#include "R3BCaloMediaPar.h"

#include "FairDbLogFormat.h"
#include "FairDbLogService.h"
#include "FairDbOutTableBuffer.h"         // for FairDbOutRowStream
#include "FairDbStatement.h"            // for FairDbStatement

#include "FairParamList.h"              // for FairParamList

#include "Riosfwd.h"                    // for ostream
#include "TString.h"                    // for TString

#include <stdlib.h>                     // for exit
#include <memory>                       // for auto_ptr, etc
#include <vector>                       // for vector, vector<>::iterator



using namespace std;

ClassImp(R3BCaloGeometryPar);


R3BCaloGeometryPar::R3BCaloGeometryPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
	fMateria(new TObjArray()),
	fGeomCrystals(new TObjArray()),
	fGeomAlveoli(new TObjArray()),
	fNodeCrystals(new TObjArray()),
	fNodeAlveoli(new TObjArray())
{
  /*
  fMateria->SetOwner(kTRUE);
  fGeomCrystals->SetOwner(kTRUE);
  fGeomAlveoli->SetOwner(kTRUE);
  fNodeCrystals->SetOwner(kTRUE);
  fNodeAlveoli->SetOwner(kTRUE);
  */
}


R3BCaloGeometryPar::~R3BCaloGeometryPar()
{
  if (fMateria) delete fMateria;
  if (fGeomCrystals) delete fGeomCrystals;
  if (fGeomAlveoli) delete fGeomAlveoli;
  if (fNodeCrystals) delete fNodeCrystals;
  if (fNodeAlveoli) delete fNodeAlveoli;
}


void R3BCaloGeometryPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloGeometryPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->addObject("Califa Materia", fMateria);
  list->addObject("Califa Crystals Types", fMateria);
  list->addObject("Califa Alveoli Types",  fGeomAlveoli);
  list->addObject("Califa Crystals Nodes", fNodeCrystals);
  list->addObject("Califa Alveoli  Nodes", fNodeAlveoli);
}

Bool_t R3BCaloGeometryPar::getParams(FairParamList* list)
{
  if(!list) { return kFALSE;}
  if (!list->fillObject("Califa Materia", fMateria)) {return kFALSE;}
  if (!list->fillObject("Califa Crystals Types", fMateria))  {return kFALSE;}
  if (!list->fillObject("Califa Alveoli Types",  fGeomAlveoli)) {return kFALSE;}
  if (!list->fillObject("Califa Crystals Nodes", fNodeCrystals)) {return kFALSE;}
  if (!list->fillObject("Califa Alveoli  Nodes", fNodeAlveoli)) {return kFALSE;}
  return kTRUE;
}

void R3BCaloGeometryPar::clear()
{
}



void R3BCaloGeometryPar::fill(UInt_t rid){
  // Fill the lists with correspondin TimeStamps (runID) 
  cout << "-I- R3BCaloGeometryPar::fill() called with RID# " << rid << endl; 

  R3BCaloMediaPar mpar;
  R3BCaloDetectionUnitSimGeoPar cgpar;
  R3BCaloAlveolusGeoPar agpar;
  R3BCaloDetectionUnitSimPar cpar;
  R3BCaloAlveolusPar apar;
 
  FairDbReader<R3BCaloMediaPar>* r_mpar = mpar.GetParamReader();  
  FairDbReader<R3BCaloDetectionUnitSimGeoPar>* r_cgpar = cgpar.GetParamReader();  
  FairDbReader<R3BCaloDetectionUnitSimPar>* r_cpar = cpar.GetParamReader();  
  FairDbReader<R3BCaloAlveolusGeoPar>* r_agpar = agpar.GetParamReader();  
  FairDbReader<R3BCaloAlveolusPar>* r_apar = apar.GetParamReader();  

  // Define a Global Context
  ValTimeStamp ts(rid);
  ValCondition context(FairDbDetector::EDetector::kCal,DataType::kData,ts);

  // Activate reading for this Context
  r_mpar->Activate(context, GetVersion());
  r_cgpar->Activate(context, GetVersion());  
  r_cpar->Activate(context, GetVersion());  
  r_agpar->Activate(context, GetVersion());  
  r_apar->Activate(context, GetVersion());  

  Int_t numMedia = r_mpar->GetNumRows(); 
  for (int i = 0; i < numMedia; ++i) {
    R3BCaloMediaPar* media_par = (R3BCaloMediaPar*) r_mpar->GetRowByIndex(i);
    if (!media_par) { continue; }
    fMateria->Add(media_par);
  }

  Int_t numCrystalGeom = r_cgpar->GetNumRows(); 
  for (int i = 0; i < numCrystalGeom; ++i) {
    R3BCaloDetectionUnitSimGeoPar* cgeo_par = (R3BCaloDetectionUnitSimGeoPar*) r_cgpar->GetRowByIndex(i+1);
    if (!cgeo_par) { continue; }
    fGeomCrystals->Add(cgeo_par);
  }


  Int_t numAlveolusGeom = r_agpar->GetNumRows(); 
  for (int i = 0; i < numAlveolusGeom; ++i) {
    R3BCaloAlveolusGeoPar* ageo_par = (R3BCaloAlveolusGeoPar*) r_agpar->GetRowByIndex(i+1);
    if (!ageo_par) { continue; }
    fGeomAlveoli->Add(ageo_par);
  }


  Int_t numCrystals = r_cpar->GetNumRows(); 
  for (int i = 0; i < numCrystals; ++i) {
    R3BCaloDetectionUnitSimPar* c_par = (R3BCaloDetectionUnitSimPar*) r_cpar->GetRowByIndex(i);
    if (!c_par) { continue; }
    fNodeCrystals->Add(c_par);
  }

  Int_t numAlveoli = r_apar->GetNumRows(); 
  for (int i = 0; i < numAlveoli; ++i) {
    R3BCaloAlveolusPar* a_par = (R3BCaloAlveolusPar*) r_apar->GetRowByIndex(i);
    if (!a_par) { continue; }
    fNodeAlveoli->Add(a_par);
  }

}


void R3BCaloGeometryPar::store(UInt_t rid){
  //<DB> store the lists
  cout << "-I- R3BCaloGeometryPar::store() called with RID# " << rid << endl; 

  for (Int_t i=0;i<fMateria->GetEntries();i++){
	TGeoMaterial* mat = (TGeoMaterial*) fMateria->At(i);

    if (mat) {
	  R3BCaloMediaPar par;
	  par.SetCompId(i);
	  par.SetID(i);
	  par.SetMixtName(mat->GetName());
	  par.SetDensity(mat->GetDensity());
	  par.SetRadLen(mat->GetRadLen());
	  par.SetIntLen(mat->GetIntLen());
	  par.SetPressure(mat->GetPressure());
	  par.SetTemp(mat->GetTemperature());
	  par.SetN(mat->GetNelements());
      if (mat->GetNelements()>1){  
		par.SetAmixt(((TGeoMixture*) mat)->GetAmixt());
		par.SetZmixt(((TGeoMixture*) mat)->GetZmixt());
		par.SetWmixt(((TGeoMixture*) mat)->GetWmixt());
	  }else{
        Double_t a[1]={mat->GetA()};
		Double_t z[1]={mat->GetZ()};
        Double_t w[1]={1.};
        par.SetAmixt(a);
        par.SetZmixt(z);
        par.SetWmixt(w);
      }

      // Write this parameter 
      par.store(rid);  
	}
  }

  // Crystal Geometries
  for(Int_t i=0;i<fGeomCrystals->GetEntries();i++){
	R3BCaloDetectionUnitSimGeoPar* cgeo_par = (R3BCaloDetectionUnitSimGeoPar*) fGeomCrystals->At(i);
    if (cgeo_par) cgeo_par->store(rid); 
  }

  // Crystal Geometries
  for(Int_t i=0;i<fGeomAlveoli->GetEntries();i++){
	R3BCaloAlveolusGeoPar* ageo_par = (R3BCaloAlveolusGeoPar*) fGeomAlveoli->At(i);
    if (ageo_par) ageo_par->store(rid); 
  }


  // Crystal nodes stored row-wized
  R3BCaloDetectionUnitSimPar iPar;
  FairDbWriter<R3BCaloDetectionUnitSimPar>* cW = iPar.ActivateWriter(rid); 

  if (cW) {
	// Crystals node
	for(Int_t i=0;i<fNodeCrystals->GetEntries();i++){
	  R3BCaloDetectionUnitSimPar* c_par = (R3BCaloDetectionUnitSimPar*) fNodeCrystals->At(i);
	  if (c_par) *cW << *c_par; 
	}
	// Reset the Writer
	if(!cW->Close()){
	  cout << " -W- R3BCaloGeometryPar:: IO Error writing crystal data node ******** "<< endl;  
	}
  }//! (cWriter)
  else {
	cout << " -W- R3BCaloGeometryPar:: Writer for R3BCaloDetectionUnitSimPar not created "<< endl;  
  }


  // Alveolus nodes stored row-wized
  R3BCaloAlveolusPar aPar;
  FairDbWriter<R3BCaloAlveolusPar>* aW = aPar.ActivateWriter(rid); 

  if (aW) {
	// Alveoli node
	for(Int_t i=0;i<fNodeAlveoli->GetEntries();i++){
	  R3BCaloAlveolusPar* a_par = (R3BCaloAlveolusPar*) fNodeAlveoli->At(i);
	  if (a_par) *aW << *a_par; 
	}
	// Reset the Writer
	if(!aW->Close()){
	  cout << " -W- R3BCaloGeometryPar:: IO Error writing alveolus data node ******** "<< endl;  
	}
  }//! (aWriter)
  else {
	cout << " -W- R3BCaloGeometryPar:: Writer for R3BCaloAlveolusPar not created "<< endl;  
  }

}

void R3BCaloGeometryPar::Print(){

  std::cout<<" -----------  Califa Geometry Parameters -------------  "<<std::endl;

  std::cout<<" Califa Media : "<< fMateria->GetEntries() << std::endl;
  for(Int_t i=0;i<fMateria->GetEntries(); i++){
	R3BCaloMediaPar* media_par = (R3BCaloMediaPar*) fMateria->At(i);  
    cout << "----------------------------------------------------------------------" << endl;
	if (media_par) media_par->Print();
  }
  
  std::cout<<" Califa Crystal Geometry : "<< fGeomCrystals->GetEntries() << std::endl;
  for(Int_t i=0;i<fGeomCrystals->GetEntries(); i++){
	R3BCaloDetectionUnitSimGeoPar* cgeo_par = (R3BCaloDetectionUnitSimGeoPar*) fGeomCrystals->At(i);  
    cout << "----------------------------------------------------------------------" << endl;
	if (cgeo_par) cgeo_par->Print();
  }

  std::cout<<" Califa Alveolus Geometry : "<< fGeomAlveoli->GetEntries() << std::endl;
  for(Int_t i=0;i<fGeomAlveoli->GetEntries(); i++){
	R3BCaloAlveolusGeoPar* ageo_par = (R3BCaloAlveolusGeoPar*) fGeomAlveoli->At(i);  
    cout << "----------------------------------------------------------------------" << endl;
	if (ageo_par) ageo_par->Print();
  }


  
  std::cout<<" Califa Crystals Node : "<< fNodeCrystals->GetEntries() << std::endl;
  for(Int_t i=0;i<fNodeCrystals->GetEntries(); i++){
	R3BCaloDetectionUnitSimPar* c_par = (R3BCaloDetectionUnitSimPar*) fNodeCrystals->At(i);  
    cout << "----------------------------------------------------------------------" << endl;
	if (c_par) c_par->Print();
  }

  std::cout<<" Califa Alveolus Node : "<< fNodeAlveoli->GetEntries() << std::endl;
  for(Int_t i=0;i<fNodeAlveoli->GetEntries(); i++){
	R3BCaloAlveolusPar* a_par = (R3BCaloAlveolusPar*) fNodeAlveoli->At(i);  
    cout << "----------------------------------------------------------------------" << endl;
	if (a_par) a_par->Print();
  }
  
  
}



