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
#include "Math/Rotation3D.h"
#include "Math/Point3D.h"
#include "Math/AxisAngle.h"
#include "TMath.h"



// -----   Default constructor   -------------------------------------------
R3BModule::R3BModule()
  : FairModule(),
    fGlobalRot(new TGeoRotation()),
    fGlobalTrans(new TGeoTranslation(0., 0., 0.)),
    fRefRot(NULL),
    fThetaX(0.), fThetaY(0.), fThetaZ(0.),
    fLabTrans(kFALSE), fLocalTrans(kFALSE),
    fX(0.), fY(0.), fZ(0.),
    fPhi(0.), fTheta(0.), fPsi(0.)
{
}



// -----   Standard constructor   ------------------------------------------
R3BModule::R3BModule( const Char_t* Name, const Char_t* title, Bool_t Active)
  : FairModule(Name,title,Active),
    fGlobalRot(new TGeoRotation()),
    fGlobalTrans(new TGeoTranslation(0., 0., 0.)),
    fRefRot(NULL),
    fThetaX(0.), fThetaY(0.), fThetaZ(0.),
    fLabTrans(kFALSE), fLocalTrans(kFALSE),
    fX(0.), fY(0.), fZ(0.),
    fPhi(0.), fTheta(0.), fPsi(0.)
{
}



// -----   Destructor   ----------------------------------------------------
R3BModule::~R3BModule()
{
  if (fGlobalRot) delete fGlobalRot; fGlobalRot=NULL;
  if (fGlobalTrans) delete fGlobalTrans; fGlobalTrans=NULL;
  if (fRefRot) delete fRefRot; fRefRot=NULL;
}



void R3BModule::SetRotAnglesEuler(Double_t phi, Double_t theta, Double_t psi)
{
  // Euler Angles definition
  // This represent the composition of : first a rotation about Z axis with
  // angle phi, then a rotation with theta about the rotated X axis, and
  // finally a rotation with psi about the new Z axis.
  
  fPhi = phi;
  fTheta = theta;
  fPsi = psi;
  
  fGlobalRot->SetAngles(phi, theta, psi); // all angles in degrees
  
  if ( fLocalTrans == kFALSE ){
    fLabTrans = kTRUE;
  } else {
    LOG(INFO) << "R3BModule::SetRotAngles() is ignored ! " << FairLogger::endl;
    LOG(INFO) << "R3BModule::SetRotAngles Local Transf. already activated ... " << FairLogger::endl;
    fLabTrans = kFALSE;
  }
}



void R3BModule::SetTranslation(Double_t tx, Double_t ty, Double_t tz)
{
  fX = tx;
  fY = ty;
  fZ = tz;
  
  fGlobalTrans->SetTranslation(tx,ty,tz);
}



void R3BModule::SetRotAnglesXYZ(Double_t xx, Double_t yy, Double_t zz)
{
  // Axis Angles definition
  // Set Axis angles based rotation in 3D and
  // Convert it to TGeoHMatrix
  fThetaX = xx * TMath::DegToRad();
  fThetaY = yy * TMath::DegToRad();
  fThetaZ = zz * TMath::DegToRad();
  
  if ( fLabTrans == kFALSE ){
    fLocalTrans = kTRUE;
  }else{
    LOG(INFO) << "R3BModule::SetRotAnglesXYZ() is ignored ! " << FairLogger::endl;
    LOG(INFO) << "R3BModule::SetRotAngles Lab Transf. already activated ... " << FairLogger::endl;
    fLocalTrans = kFALSE;
  } 
}



TGeoCombiTrans* R3BModule::GetGlobalPosition(TGeoCombiTrans *fRef)
{
  if (fLocalTrans == kTRUE ){
    if ( ( fThetaX == 0 )  && ( fThetaY==0 )  && ( fThetaZ == 0 )
	 &&
	 ( fX == 0 ) && ( fY == 0 ) && ( fZ == 0 )
	 )  return fRef;
    
    // X axis
    Double_t xAxis[3] = { 1. , 0. , 0. };
    Double_t yAxis[3] = { 0. , 1. , 0. };
    Double_t zAxis[3] = { 0. , 0. , 1. };
    // Reference Rotation
    fRefRot = fRef;
    
    if (fRefRot) {
      Double_t mX[3] = {0.,0.,0.};
      Double_t mY[3] = {0.,0.,0.};
      Double_t mZ[3] = {0.,0.,0.};
      
      fRefRot->LocalToMasterVect(xAxis,mX);
      fRefRot->LocalToMasterVect(yAxis,mY);
      fRefRot->LocalToMasterVect(zAxis,mZ);
      
      Double_t a[4]={ mX[0],mX[1],mX[2], fThetaX };
      Double_t b[4]={ mY[0],mY[1],mY[2], fThetaY };
      Double_t c[4]={ mZ[0],mZ[1],mZ[2], fThetaZ };
      
      ROOT::Math::AxisAngle aX(a,a+4);
      ROOT::Math::AxisAngle aY(b,b+4);
      ROOT::Math::AxisAngle aZ(c,c+4);
      
      ROOT::Math::Rotation3D fMatX( aX );
      ROOT::Math::Rotation3D fMatY( aY );
      ROOT::Math::Rotation3D fMatZ( aZ );
      
      ROOT::Math::Rotation3D  fRotXYZ = (fMatZ * (fMatY * fMatX));
      
      Double_t fRotable[9]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
      fRotXYZ.GetComponents(
			    fRotable[0],fRotable[3],fRotable[6],
			    fRotable[1],fRotable[4],fRotable[7],
			    fRotable[2],fRotable[5],fRotable[8]
			    );
      
      TGeoRotation *pRot = new TGeoRotation();
      pRot->SetMatrix(fRotable);
      TGeoCombiTrans *pTmp = new TGeoCombiTrans(*fGlobalTrans,*pRot);
      
      // new way <DB>
      TGeoRotation rot_id;
      rot_id.SetAngles(0.0,0.0,0.0);
      
      TGeoCombiTrans c1;
      c1.SetRotation(rot_id);
      const Double_t *t = pTmp->GetTranslation();
      c1.SetTranslation(t[0],t[1],t[2]);
      
      TGeoCombiTrans c2;
      c2.SetRotation(rot_id);
      const Double_t *tt = fRefRot->GetTranslation();
      c2.SetTranslation(tt[0],tt[1],tt[2]);
      
      TGeoCombiTrans cc = c1 * c2 ;
      
      /////////////////////////////////////////////////
      
      
      TGeoCombiTrans c3;
      c3.SetRotation(pTmp->GetRotation());
      TGeoCombiTrans c4;
      c4.SetRotation(fRefRot->GetRotation());
      
      TGeoCombiTrans ccc = c3 * c4;
      
      TGeoCombiTrans pGlobal;
      pGlobal.SetRotation(ccc.GetRotation());
      const Double_t *allt = cc.GetTranslation();
      pGlobal.SetTranslation(allt[0],allt[1],allt[2]);
      
      return  ( new TGeoCombiTrans( pGlobal ) );
      
    } else {
      LOG(ERROR) << "R3BModule::GetGlobalPosition() \
	      No. Ref. Transformation defined ! " << FairLogger::endl;
      LOG(ERROR) << "R3BModule::GetGlobalPosition() \
	      cannot create Local Transformation " << FairLogger::endl;
      return NULL;
    } //! fRefRot
    
  } else {
    
    // Lab Transf.
    if ( ( fPhi == 0 )  && ( fTheta==0 )  && ( fPsi == 0 )
	 &&
	 ( fX == 0 ) && ( fY == 0 ) && ( fZ == 0 )
	 )  return fRef;
    
    return ( new TGeoCombiTrans(*fGlobalTrans,*fGlobalRot) );
    
  }
}



ClassImp(R3BModule)
