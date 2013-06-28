
#include "R3BMagnet.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoRootBuilder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"

#include "R3BGeoMagnet.h"
#include "R3BGeoPassivePar.h"
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
#include "TGeoManager.h"

#include <iostream>

using namespace std;

R3BMagnet::~R3BMagnet()
{
}
R3BMagnet::R3BMagnet()
{
}

R3BMagnet::R3BMagnet(const char * name, const char *Title)
  : R3BModule(name ,Title)
{
}



void R3BMagnet::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    fLogger->Info(MESSAGE_ORIGIN,
		  "Constructing ALADIN geometry from ROOT file %s", 
		  fileName.Data());
    ConstructRootGeometry();
  } else {
    fLogger->Fatal(MESSAGE_ORIGIN,
		  "Geometry file is not specified");
  }
}



Bool_t R3BMagnet::CheckIfSensitive(std::string name)
{
	// just to get rid of the warrning during run, not need this is a passive element! 
	return kFALSE;
}



void R3BMagnet::ConstructASCIIGeometry(){
	FairGeoLoader *loader=FairGeoLoader::Instance();
	FairGeoInterface *GeoInterface =loader->getGeoInterface();
	R3BGeoMagnet *MGeo=new R3BGeoMagnet();
	MGeo->setGeomFile(GetGeometryFileName());
	GeoInterface->addGeoModule(MGeo);
	Bool_t rc = GeoInterface->readSet(MGeo);
	if ( rc ) MGeo->create(loader->getGeoBuilder());

        TList* volList = MGeo->getListOfVolumes();
        // store geo parameter
        FairRun *fRun = FairRun::Instance();
        FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
        R3BGeoPassivePar* par=(R3BGeoPassivePar*)(rtdb->getContainer("R3BGeoPassivePar"));
        TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
        TObjArray *fPassNodes = par->GetGeoPassiveNodes();

        TListIter iter(volList);
        FairGeoNode* node   = NULL;
        FairGeoVolume *aVol=NULL;

        while( (node = (FairGeoNode*)iter.Next()) ) {
            aVol = dynamic_cast<FairGeoVolume*> ( node );
            if ( node->isSensitive()  ) {
                fSensNodes->AddLast( aVol );
            }else{
                fPassNodes->AddLast( aVol );
            }
        }
	ProcessNodes( volList );
        par->setChanged();
        par->setInputVersion(fRun->GetRunId(),1);	
}


ClassImp(R3BMagnet)














