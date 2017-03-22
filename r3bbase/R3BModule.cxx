/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Denis Bertini     <D.Bertini@gsi.de>
//*-- Date: 20/08/2009
//*-- Last Update: 04/03/06 by Denis Bertini
// --------------------------------------------------------------
// Description:
//   The R3B Module base class
//
// --------------------------------------------------------------
// Comments:
//
//  20/08/09 R3B Module base class
//              - Including local ref. frame Rotation using
//                Axis Angles.
//              - Euler based angles supported
//                Rotation/Translation / Lab
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "R3BModule.h"

// Math ROOT package
#include "TMath.h"
#include "TGeoManager.h"
#include "TVector3.h"

// -----   Default constructor   -------------------------------------------
R3BModule::R3BModule()
  : FairModule()
  , fPosition(NULL)
  , fRotation(NULL)
{
}



// -----   Standard constructor   ------------------------------------------
R3BModule::R3BModule( const Char_t* Name, const Char_t* title, Bool_t Active)
  : FairModule(Name,title,Active)
  , fPosition(NULL)
  , fRotation(NULL)
{
}

// -----   Standard constructor   ------------------------------------------
R3BModule::R3BModule(const char* Name,
                     const char* title,
                     TString geoFile,
                     Float_t x,
                     Float_t y,
                     Float_t z,
                     Float_t rot_x,
                     Float_t rot_y,
                     Float_t rot_z,
                     Bool_t Active)
    : FairModule(Name, title, Active)
    , fPosition(NULL)
    , fRotation(NULL)
{
    if (TMath::Abs(x) > 0. || TMath::Abs(y) > 0. || TMath::Abs(z) > 0.)
    {
        SetPosition(x, y, z);
    }

    if (TMath::Abs(rot_x) > 0. || TMath::Abs(rot_y) > 0. || TMath::Abs(rot_z) > 0.)
    {
        SetRotation(rot_x, rot_y, rot_z);
    }

    SetGeometryFileName(geoFile);
}

// -----   Destructor   ----------------------------------------------------
R3BModule::~R3BModule()
{
  if(fRotation)
    delete fRotation;
  if(fPosition)
    delete fPosition;
}

void R3BModule::ConstructRootGeometry()
{
  FairModule::ConstructRootGeometry();
  
  TGeoNode* n = gGeoManager->GetTopNode()->GetDaughter(gGeoManager->GetTopNode()->GetNdaughters()-1);
  TGeoCombiTrans* combtrans = (TGeoCombiTrans*)((TGeoNodeMatrix*)n)->GetMatrix();
  
  if(!fPosition)
   fPosition = new TVector3(combtrans->GetTranslation()[0], combtrans->GetTranslation()[1], combtrans->GetTranslation()[2]);

  if(fRotation)
    combtrans->SetRotation(fRotation);
  
  combtrans->SetDx(fPosition->X());  
  combtrans->SetDy(fPosition->Y());  
  combtrans->SetDz(fPosition->Z());  
}

void R3BModule::SetPosition(Double_t x, Double_t y, Double_t z)
{
  if(fPosition)
    delete fPosition;
  
  fPosition = new TVector3(x, y, z);
}

void R3BModule::SetRotation(Double_t x_deg, Double_t y_deg, Double_t z_deg)
{
  if(fRotation)
    delete fRotation;
    
  fRotation = new TGeoRotation();
  fRotation->RotateX(x_deg);
  fRotation->RotateY(y_deg);
  fRotation->RotateZ(z_deg);
}

ClassImp(R3BModule)
