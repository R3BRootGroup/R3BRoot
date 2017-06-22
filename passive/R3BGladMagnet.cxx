
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

/******************************************/
//Edited on: 22.06.2017
//by: Lorenzo Zanetti -- lzanetti@ikp.tu-darmstadt.de
//Description:
// Adding back compatibility for "just-by-name" constructor.
// The macros __GLAD_POS_D{X,Y,Z} and __GLAD_ROT are being defined
// in order to fix the placement from here
// NOTE: overrides the placement in the geometry file!
/*****************************************/

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

//New-ish constructor: calls the fully parametric constructor of the parent class
//                     passing the translation (and only the translation).
//                     the rotation is defined in ConstrucGeometry().
R3BGladMagnet::R3BGladMagnet(const char* name, TString geoFile, const char* Title)
    : R3BModule(name, Title, geoFile, __GLAD_POS_DX, __GLAD_POS_DY, __GLAD_POS_DZ)
{
}

void R3BGladMagnet::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing GLAD geometry from ROOT file " << fileName << FairLogger::endl;
    ConstructRootGeometry(); //the translation is applied HERE. Also, the geometry file's
                             //rotation is preserved.

    TGeoRotation* rot_glob = new TGeoRotation();
    rot_glob->RotateY(90.0);
    rot_glob->RotateZ(-90.0);
    rot_glob->RotateY(__GLAD_ROT); //__GLAD_ROT is a macro, #define'ed. See header.
    rot_glob->RotateZ(180.0);

    //override the geometry file's given rotation.
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
