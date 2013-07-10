//*-- AUTHOR : Marc Labiche
//*-- Created : 28/06/2012

#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoRootBuilder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairGeoMedia.h"
#include "R3BVacVesselCool.h"
#include "R3BGeoPassivePar.h"
#include "R3BGeoVacVesselCool.h"

// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoCompositeShape.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoPara.h"
#include "TGeoTube.h"
#include "TGeoPgon.h"
#include "TGeoCone.h"
#include "TGeoTorus.h"
#include "TGeoBoolNode.h"


#include <iostream>
#include <TMath.h>

using namespace std;

R3BVacVesselCool::~R3BVacVesselCool()
{
if (fVacVesselCoolName ) delete fVacVesselCoolName;
}


R3BVacVesselCool::R3BVacVesselCool()
{
fVacVesselCoolName=NULL;
}

R3BVacVesselCool::R3BVacVesselCool(const char * name,  const char * title)
  : R3BModule(name ,title)
{

fVacVesselCoolName =  new TString(name);

}


void R3BVacVesselCool::ConstructGeometry()
{
  if (*fVacVesselCoolName == "LiH") {
    return  ConstructGeometry1();  // ie: For Liquid Hydrogen target only (targt4)
  }
}


void R3BVacVesselCool::ConstructGeometry1()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    fLogger->Info(MESSAGE_ORIGIN,
                  "Constructing VACVESSEL geometry from ROOT file %s",
                  fileName.Data());
    ConstructRootGeometry();
  } else {
    fLogger->Fatal(MESSAGE_ORIGIN, "Geometry file name is not set");
  }
}


Bool_t R3BVacVesselCool::CheckIfSensitive(std::string name)
{
  return kFALSE;
}


ClassImp(R3BVacVesselCool)



