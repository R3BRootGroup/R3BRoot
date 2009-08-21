/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Denis Bertini     <D.Bertini@gsi.de>
//*-- Date: 20/08/2009
//*-- Last Update: 04/03/06 by Denis Bertini
// --------------------------------------------------------------
// Description:
//   The R3B Detector base class
//
// --------------------------------------------------------------
// Comments:
//
//  20/08/09 R3B Detector base class
//              - Including local ref. frame Rotation using
//                Axis Angles.
//              - Euler based angles supported
//                Rotation/Translation / Lab
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////



#include "R3BDetector.h"

// Math ROOT package
#include "Math/Rotation3D.h"
#include "Math/Point3D.h"
#include "Math/AxisAngle.h"
#include "TMath.h"
#include <iostream>


using namespace std;

// -----   Default constructor   -------------------------------------------
R3BDetector::R3BDetector() : FairDetector() {
 // Create Matrix Unity
    fGlobalRot = new TGeoRotation();

 // Create a null translation
    fGlobalTrans = new TGeoTranslation();
    fGlobalTrans->SetTranslation(0.0,0.0,0.0);

 // Ref Transf.
    fRefRot = NULL;
 // Angles
    fThetaX = fThetaY = fThetaZ = 0.0;
    fPhi = fTheta = fPsi = 0.0 ;
    fX = fY = fZ = 0.0;
    fLocalTrans = fLabTrans = kFALSE;

}

// -----   Destructor   ----------------------------------------------------
R3BDetector::~R3BDetector() {

    if (fGlobalRot) delete fGlobalRot; fGlobalRot=NULL;
    if (fGlobalTrans) delete fGlobalTrans; fGlobalTrans=NULL;
    if (fRefRot) delete fRefRot; fRefRot=NULL;

}

// -----   Standard constructor   ------------------------------------------
R3BDetector::R3BDetector( const Char_t* Name, Bool_t Active, Int_t detid)
    : FairDetector(Name,Active,detid) {

 // Create Matrix Unity
    fGlobalRot = new TGeoRotation();

 // Create a null translation
    fGlobalTrans = new TGeoTranslation();
    fGlobalTrans->SetTranslation(0.0,0.0,0.0);
    fRefRot = NULL;

    fThetaX = fThetaY = fThetaZ = 0.0 ;
    fPhi = fTheta = fPsi = 0.0;
    fX = fY = fZ = 0.0;
    fLocalTrans = fLabTrans = kFALSE;

}


void R3BDetector::SetRotAnglesEuler(Double_t phi, Double_t theta, Double_t psi){
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
  }else{
    cout << endl;
    cout << "-I- R3BDetector::SetRotAngles() is ignored ! " << endl;
    cout << "-I- R3BDetector::SetRotAngles Local Transf. already activated ... " << endl;
    cout << endl;
     fLabTrans = kFALSE;
  }

}

void R3BDetector::SetRotAnglesXYZ(Double_t xx, Double_t yy, Double_t zz){
 // Axis Angles definition
 // Set Axis angles based rotation in 3D and
 // Convert it to TGeoHMatrix

    fThetaX = xx * TMath::DegToRad();
    fThetaY = yy * TMath::DegToRad();
    fThetaZ = zz * TMath::DegToRad();

    if ( fLabTrans == kFALSE ){
      fLocalTrans = kTRUE;
  }else{
    cout << endl;
    cout << "-I- R3BDetector::SetRotAnglesXYZ() is ignored ! " << endl;
    cout << "-I- R3BDetector::SetRotAngles Lab Transf. already activated ... " << endl;
    cout << endl;
     fLocalTrans = kFALSE;
  }

}

void R3BDetector::SetTranslation(Double_t tx, Double_t ty, Double_t tz){

    fX = tx;
    fY = ty;
    fZ = tz;
  fGlobalTrans->SetTranslation(tx,ty,tz);

}

TGeoCombiTrans* R3BDetector::GetGlobalPosition(TGeoCombiTrans *fRef){


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

	  //cout << "-I- R3BDetector calculation Axis X " << endl;
	  //cout << "-I- R3BDetector X[0]: " << mX[0] << " X[1]: " << mX[1] << " X[2]: "
	  //     << mX[2] <<  endl;
	  //cout << "-I- R3BDetector Y[0]: " << mY[0] << " Y[1]: " << mY[1] << " Y[2]: "
	  //     << mY[2] <<  endl;
	  //cout << "-I- R3BDetector Z[0]: " << mZ[0] << " Z[1]: " << mZ[1] << " Z[2]: "
	  //     << mZ[2] <<  endl;


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

	  //cout << fRotXYZ << endl;

	  Double_t fRotable[9]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	  fRotXYZ.GetComponents(
				fRotable[0],fRotable[3],fRotable[6],
				fRotable[1],fRotable[4],fRotable[7],
				fRotable[2],fRotable[5],fRotable[8]
			       );
	  TGeoRotation *pRot = new TGeoRotation();
	  pRot->SetMatrix(fRotable);
	  TGeoCombiTrans *pTmp = new TGeoCombiTrans(*fGlobalTrans,*pRot);

	  // ne peut pas etre applique ici
	  // il faut differencier trans et rot dans la multi.
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

      }else{

	  cout << "-E- R3BDetector::GetGlobalPosition() \
	      No. Ref. Transformation defined ! " << endl;
	  cout << "-E- R3BDetector::GetGlobalPosition() \
	      cannot create Local Transformation " << endl;
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


ClassImp(R3BDetector)
  
