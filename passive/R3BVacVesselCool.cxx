//*-- AUTHOR : Marc Labiche
//*-- Created : 28/06/2012
#include <stdlib.h>

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

using namespace std;


R3BVacVesselCool::~R3BVacVesselCool()
{
  if(fVacVesselCoolName) {
    delete fVacVesselCoolName;
  }
}


R3BVacVesselCool::R3BVacVesselCool()
{
  fVacVesselCoolName = NULL;
}

R3BVacVesselCool::R3BVacVesselCool(const char* name, const char* title)
: R3BModule(name ,title)
{
  fVacVesselCoolName = new TString(name);
}

R3BVacVesselCool::R3BVacVesselCool(const char* name,
                                   TString geoFile,
                                   Float_t x,
                                   Float_t y,
                                   Float_t z,
                                   Float_t rot_x,
                                   Float_t rot_y,
                                   Float_t rot_z,
                                   const char* title)
  : R3BModule(name ,title)
{
  SetGeometryFileName(geoFile);
  SetPosition(x, y, z);
  SetRotation(rot_x, rot_y, rot_z);
  fVacVesselCoolName = new TString(name);
}

void R3BVacVesselCool::ConstructGeometry()
{
  //  if (*fVacVesselCoolName == "LiH" || *fVacVesselCoolName == "Para") {
  if (*fVacVesselCoolName == "LiH") {
    return  ConstructGeometry1();  // ie: For Liquid Hydrogen target only (targt4)
  }
}


void R3BVacVesselCool::ConstructGeometry1()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing VACVESSEL geometry from ROOT file " << fileName.Data() << FairLogger::endl;
    ConstructRootGeometry();
  } else {
    LOG(FATAL) << "VACVESSEL geometry file name is not set" << FairLogger::endl;
    exit(1);
  }
}


Bool_t R3BVacVesselCool::CheckIfSensitive(std::string name)
{
  return kFALSE;
}


ClassImp(R3BVacVesselCool)
