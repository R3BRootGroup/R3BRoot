#ifndef R3BGEOMANALYZER_H
#define R3BGEOMANALYZER_H 1


#include <map>
#include <string>
#include <vector>

#include "TObject.h"
#include "TGeoManager.h"
#include "TFile.h"

#include "R3BCaloGeometryPar.h"
#include "R3BCaloCrystalPar.h"
#include "R3BCaloAlveolusPar.h"

using namespace std;

class TGeoArb8;


class R3BGeomAnalyzer : public TObject
{

 public:

  R3BGeomAnalyzer();
  ~R3BGeomAnalyzer();

 
  // Readers
  virtual Bool_t Import(TFile *geoFile);
  Bool_t RecReadMedia( Bool_t keepGoing=kTRUE);
  Bool_t RecReadShapes( Bool_t keepGoing=kTRUE);
  Bool_t RecReadNodes( Bool_t keepGoing=kTRUE);

  // Getters
  Int_t GetNumMedia()    { return fMapMedia.size(); }
  Int_t GetNumCrystals() { return fV_Crystals.size(); }
  Int_t GetNumAlveoli()  { return fV_Alveoli.size(); }
  Int_t GetAllCrystals() { return fN_Crystals; }
  Int_t GetAllAlveoli()  { return fN_Alveoli; }
  Int_t GetCrystalTypeId(TGeoArb8* aShape);
  Int_t GetAlveolusTypeId(TGeoArb8* aShape);


  // Logicals
  Bool_t IsMediaStored(const string& media_name);
  Bool_t IsCrystalStored(TGeoShape* aShape);
  Bool_t IsAlveolusStored(TGeoShape* aShape);
  Bool_t CompareShape(TGeoArb8* a, TGeoArb8* b);

  // Dumps
  void PrintCrystals();
  void PrintAlveoli();

  // Fill Geo. Param
  void FillGeometryPar(R3BCaloGeometryPar* par);

  protected:
  Int_t fN_Alveoli;
  Int_t fN_Crystals;
  TGeoManager *fGeom;  

  // Elements Store
  vector<TGeoShape*> fV_Crystals;
  vector<TGeoShape*> fV_Alveoli;
  map<string, TGeoMaterial*> fMapMedia;
  vector<R3BCaloCrystalPar*> fCrystals;
  vector<R3BCaloAlveolusPar*> fAlveoli;
  
 
  ClassDef(R3BGeomAnalyzer,1);

};

#endif
