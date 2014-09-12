#ifndef R3BCALOGEOMETRYPAR_H
#define R3BCALOGEOMETRYPAR_H

#include "FairParGenericSet.h"          // for FairParGenericSet

#include "TObjArray.h"
#include "TGeoMaterial.h" 
#include "Rtypes.h"                     // for Double_t, Int_t, UInt_t, etc
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string

#include "R3BCaloAlveolusGeoPar.h"
#include "R3BCaloDetectionUnitSimGeoPar.h"
#include "R3BCaloAlveolusPar.h"
#include "R3BCaloDetectionUnitSimPar.h"

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;


class R3BCaloGeometryPar : public FairParGenericSet
{

using TObject::Compare;

  public :
    R3BCaloGeometryPar (const char* name="R3BCaloGeometryPar",
                  const char* title="Califa Geometry Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloGeometryPar(void);

    void   clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);
    void   Print();

	// Lists handling  
    void   AddMaterial(TGeoMaterial *mat){fMateria->Add(mat);}  
    void   AddCrystalShapes(R3BCaloDetectionUnitSimGeoPar* sh){fGeomCrystals->Add(sh);}  
    void   AddAlveoliShapes(R3BCaloAlveolusGeoPar* sh){fGeomAlveoli->Add(sh);}
    void   AddCrystals(R3BCaloDetectionUnitSimPar* sh){fNodeCrystals->Add(sh);}  
    void   AddAlveoli(R3BCaloAlveolusPar* sh){fNodeAlveoli->Add(sh);}  

    TObjArray* GetListOfMateria() {return fMateria;}
    Int_t GetNumMedia() {return fMateria->GetEntries();} 
    TGeoMaterial* GetMaterialAt(Int_t idx){return (TGeoMaterial*) fMateria->At(idx);} 

    Int_t GetNumCrystalGeom() {return fGeomCrystals->GetEntries();} 
    Int_t GetNumCrystalNodes() {return fNodeCrystals->GetEntries();}
    Int_t GetNumAlveolusGeom() {return fGeomAlveoli->GetEntries();} 
    Int_t GetNumAlveolusNodes() {return fNodeAlveoli->GetEntries();}
   
    // Global IO using run_id
    virtual void fill(UInt_t rid);
    virtual void store(UInt_t rid);

  private:
    TObjArray* fMateria;
    TObjArray* fGeomCrystals;
    TObjArray* fGeomAlveoli;
    TObjArray* fNodeCrystals;
    TObjArray* fNodeAlveoli;
 
    ClassDef(R3BCaloGeometryPar,1); // R3BCaloGeometryPar Parameter Container example
};

#endif /* !R3BCALOGEOMETRYPAR_H*/

