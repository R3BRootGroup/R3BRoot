//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// R3BGeoPipe
//
// Class for geometry of support structure
//
/////////////////////////////////////////////////////////////

#include "R3BGeoPipe.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
    
ClassImp(R3BGeoPipe)

R3BGeoPipe::R3BGeoPipe() {
  // Constructor
  fName="pipe";
  strcpy(modName,"p");
  strcpy(eleName,"p");
  maxSectors=0;
  maxModules=1;
}
Bool_t  R3BGeoPipe::create ( FairGeoBuilder * build ) {
    Bool_t rc = FairGeoSet::create( build );
    if ( rc ) {
	FairGeoLoader *loader=FairGeoLoader::Instance();
	FairGeoInterface *GeoInterface =loader->getGeoInterface();
	
	GeoInterface->getMasterNodes()->Add( (TObject*)  getVolume("pipeCentral") );
    }
 return rc;
}

R3BGeoPipe::~R3BGeoPipe() {

}

