
#include <iostream>
#include <stdlib.h>

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


R3BMagnet::R3BMagnet(const char * name, TString geoFile, const char *Title)
: R3BModule(name, Title, geoFile, 0., 0., 350. - 95., 0., -7., 0.)
{
}


void R3BMagnet::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing ALADIN geometry from ROOT file " << fileName.Data() << FairLogger::endl;
    ConstructRootGeometry();
  } else {
    LOG(FATAL) << "ALADIN Geometry file is not specified" << FairLogger::endl;
    exit(1);
  }
}


Bool_t R3BMagnet::CheckIfSensitive(std::string name)
{
	return kFALSE;
}


ClassImp(R3BMagnet)
