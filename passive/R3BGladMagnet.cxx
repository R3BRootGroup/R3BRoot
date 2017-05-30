
#include <iostream>
#include <stdlib.h>

#include "TGeoManager.h"

#include "R3BGladMagnet.h"
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


R3BGladMagnet::~R3BGladMagnet()
{
}


R3BGladMagnet::R3BGladMagnet()
{
}

R3BGladMagnet::R3BGladMagnet(const char* name, const char* Title)
    : R3BModule(name, Title)
{
}

R3BGladMagnet::R3BGladMagnet(const char* name, TString geoFile, const char* Title)
    : R3BModule(name, Title, geoFile, -42., 0., 308.8)
    , fGladAngle(14.)
{
}

void R3BGladMagnet::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing GLAD geometry from ROOT file " << fileName << FairLogger::endl;
    ConstructRootGeometry();

    TGeoRotation* rot_glob = new TGeoRotation();
    rot_glob->RotateY(90.0);
    rot_glob->RotateZ(-90.0);
    rot_glob->RotateY(fGladAngle);
    rot_glob->RotateZ(180.0);

    TGeoNode* glad_node = gGeoManager->GetTopVolume()->GetNode("Glad_box_0");

    TGeoCombiTrans* combtrans = (TGeoCombiTrans*)((TGeoNodeMatrix*)glad_node)->GetMatrix();
    combtrans->SetRotation(rot_glob);
  } else {
    LOG(FATAL) << "GLAD geometry file name is not set" << FairLogger::endl;
    exit(1);
  }
}


Bool_t R3BGladMagnet::CheckIfSensitive(std::string name)
{
 	return kFALSE;
}


ClassImp(R3BGladMagnet)
