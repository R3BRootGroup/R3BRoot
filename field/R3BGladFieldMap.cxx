// Includes from C
#include <iomanip>
#include <iostream>
#include <fstream>

// Includes from ROOT
#include "TArrayF.h"
#include "TFile.h"
#include "TMath.h"

#include "R3BGladFieldMap.h"


using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::right;
using std::flush;
using std::setw;
using std::showpoint;
using TMath::Nint;


// -------------   Default constructor  ----------------------------------
R3BGladFieldMap::R3BGladFieldMap() {
  fPosX  = fPosY  = fPosZ  = 0.;
  fXmin  = fYmin  = fZmin  = 0.;
  fXmax  = fYmax  = fZmax  = 0.;
  fXstep = fYstep = fZstep = 0.;
  fNx    = fNy    = fNz    = 0;
  fScale = 1.;
  fBx    = fBy    = fBz    = NULL;
  fPosX = fPosY = fPosZ = 0.;
  fName     = "";
  fFileName = "";
  fType = 1;
}
// ------------------------------------------------------------------------



// -------------   Standard constructor   ---------------------------------
R3BGladFieldMap::R3BGladFieldMap(const char* mapName, const char* fileType)
  : FairField(mapName) {
  fPosX  = fPosY  = fPosZ  = 0.;
  fXmin  = fYmin  = fZmin  = 0.;
  fXmax  = fYmax  = fZmax  = 0.;
  fXstep = fYstep = fZstep = 0.;
  fNx    = fNy    = fNz    = 0;
  fScale = 1.;
  fBx    = fBy    = fBz    = NULL;
  fName  = mapName;
  TString dir = getenv("VMCWORKDIR");
  fFileName = dir + "/field/magField/R3B/" + mapName;
  if ( fileType[0] == 'R' ) fFileName += ".root";
  else                      fFileName += ".dat";
  fType = 1;
}


// ------------   Constructor from R3BGladFieldPar   --------------------------

R3BGladFieldMap::R3BGladFieldMap(R3BFieldPar* fieldPar) {
  fType = 1;
  fPosX  = fPosY  = fPosZ  = 0.;
  fXmin  = fYmin  = fZmin  = 0.;
  fXmax  = fYmax  = fZmax  = 0.;
  fXstep = fYstep = fZstep = 0.;
  fNx    = fNy    = fNz    = 0;
  fScale = 1.;
  fBx    = fBy    = fBz    = NULL;
  if ( ! fieldPar ) {
    cerr << "-W- R3BGladFieldConst::R3BGladFieldMap: empty parameter container!"
	 << endl;
    fName     = "";
    fFileName = "";
    fType     = 1;
  }
  else {
    fieldPar->MapName(fName);
    fPosX  = fieldPar->GetPositionX();
    fPosY  = fieldPar->GetPositionY();
    fPosZ  = fieldPar->GetPositionZ();
    fScale = fieldPar->GetScale();
      TString dir = getenv("VMCWORKDIR");
      fFileName = dir + "/field/magField/R3B/" + fName;
      fFileName += ".dat";
    //fType = fieldPar->GetType();
  }
}




// ------------   Destructor   --------------------------------------------
R3BGladFieldMap::~R3BGladFieldMap() {
  if ( fBx ) delete fBx;
  if ( fBy ) delete fBy;
  if ( fBz ) delete fBz;
}
// ------------------------------------------------------------------------



// -----------   Intialisation   ------------------------------------------
void R3BGladFieldMap::Init() {
//  if      (fFileName.EndsWith(".root")) ReadRootFile(fFileName, fName);
  if (fFileName.EndsWith(".dat"))  ReadAsciiFile(fFileName);
  else {
    cerr << "-E- R3BGladFieldMap::Init: No proper file name defined! ("
	 << fFileName << ")" << endl;
    Fatal("Init", "No proper file name");
  }

  fPosX = 0.0;
  fPosY = 0.0;
  fPosZ = 163.4;
  fYAngle = -14.;
  gTrans = new TVector3(-fPosX, -fPosY, -fPosZ);
}
// ------------------------------------------------------------------------



// -----------   Get x component of the field   ---------------------------
Double_t R3BGladFieldMap::GetBx(Double_t x, Double_t y, Double_t z) {


// cout << "-I- get Bx called " << endl;
  // transform to local coordinates
  // local to global
  TVector3 localPoint(x,y,z);
  
  localPoint = localPoint + (*gTrans);
  localPoint.RotateY(-fYAngle*TMath::DegToRad());

  Int_t ix    = 0;
  Int_t iy    = 0;
  Int_t iz    = 0;
  Double_t dx = 0.;
  Double_t dy = 0.;
  Double_t dz = 0.;

  if ( IsInside(localPoint.X(), localPoint.Y(), localPoint.Z(), ix, iy, iz, dx, dy, dz) ) {

  // Get Bx field values at grid cell corners
  fHa[0][0][0] = fBx->At(ix    *2*fNy*fNz + iy    *fNz + iz);
  fHa[1][0][0] = fBx->At((ix+1)*2*fNy*fNz + iy    *fNz + iz);
  fHa[0][1][0] = fBx->At(ix    *2*fNy*fNz + (iy+1)*fNz + iz);
  fHa[1][1][0] = fBx->At((ix+1)*2*fNy*fNz + (iy+1)*fNz + iz);
  fHa[0][0][1] = fBx->At(ix    *2*fNy*fNz + iy    *fNz + (iz+1));
  fHa[1][0][1] = fBx->At((ix+1)*2*fNy*fNz + iy    *fNz + (iz+1));
  fHa[0][1][1] = fBx->At(ix    *2*fNy*fNz + (iy+1)*fNz + (iz+1));
  fHa[1][1][1] = fBx->At((ix+1)*2*fNy*fNz + (iy+1)*fNz + (iz+1));

  // Return interpolated field value
  Double_t val = Interpolate(dx, dy, dz);
  //cout << " (X) interpolated " << val << endl;
  return (val);
  }

  return 0.;
}
// ------------------------------------------------------------------------



// -----------   Get y component of the field   ---------------------------
Double_t R3BGladFieldMap::GetBy(Double_t x, Double_t y, Double_t z) {

  // transform to local coordinates
  // local to global
  TVector3 localPoint(x,y,z);
  
  localPoint = localPoint + (*gTrans);
  localPoint.RotateY(-fYAngle*TMath::DegToRad());

  Int_t ix    = 0;
  Int_t iy    = 0;
  Int_t iz    = 0;
  Double_t dx = 0.;
  Double_t dy = 0.;
  Double_t dz = 0.;

  if ( IsInside(localPoint.X(), localPoint.Y(), localPoint.Z(), ix, iy, iz, dx, dy, dz) ) {

  // Get By field values at grid cell corners
  fHa[0][0][0] = fBy->At(ix    *2*fNy*fNz + iy    *fNz + iz);
  fHa[1][0][0] = fBy->At((ix+1)*2*fNy*fNz + iy    *fNz + iz);
  fHa[0][1][0] = fBy->At(ix    *2*fNy*fNz + (iy+1)*fNz + iz);
  fHa[1][1][0] = fBy->At((ix+1)*2*fNy*fNz + (iy+1)*fNz + iz);
  fHa[0][0][1] = fBy->At(ix    *2*fNy*fNz + iy    *fNz + (iz+1));
  fHa[1][0][1] = fBy->At((ix+1)*2*fNy*fNz + iy    *fNz + (iz+1));
  fHa[0][1][1] = fBy->At(ix    *2*fNy*fNz + (iy+1)*fNz + (iz+1));
  fHa[1][1][1] = fBy->At((ix+1)*2*fNy*fNz + (iy+1)*fNz + (iz+1));

  // Return interpolated field value
  Double_t val = Interpolate(dx, dy, dz);
  //cout << " (Y) interpolated " << val << endl;
  return (val);

  }

  return 0.;
}
// ------------------------------------------------------------------------



// -----------   Get z component of the field   ---------------------------
Double_t R3BGladFieldMap::GetBz(Double_t x, Double_t y, Double_t z) {

  Int_t ix    = 0;
  Int_t iy    = 0;
  Int_t iz    = 0;
  Double_t dx = 0.;
  Double_t dy = 0.;
  Double_t dz = 0.;

  // transform to local coordinates
  // local to global
  TVector3 localPoint(x,y,z);
  
  localPoint = localPoint + (*gTrans);
  localPoint.RotateY(-fYAngle*TMath::DegToRad());



  if ( IsInside(localPoint.X(), localPoint.Y(), localPoint.Z(), ix, iy, iz, dx, dy, dz) ) {

  // Get Bz field values at grid cell corners
  fHa[0][0][0] = fBz->At(ix    *2*fNy*fNz + iy    *fNz + iz);
  fHa[1][0][0] = fBz->At((ix+1)*2*fNy*fNz + iy    *fNz + iz);
  fHa[0][1][0] = fBz->At(ix    *2*fNy*fNz + (iy+1)*fNz + iz);
  fHa[1][1][0] = fBz->At((ix+1)*2*fNy*fNz + (iy+1)*fNz + iz);
  fHa[0][0][1] = fBz->At(ix    *2*fNy*fNz + iy    *fNz + (iz+1));
  fHa[1][0][1] = fBz->At((ix+1)*2*fNy*fNz + iy    *fNz + (iz+1));
  fHa[0][1][1] = fBz->At(ix    *2*fNy*fNz + (iy+1)*fNz + (iz+1));
  fHa[1][1][1] = fBz->At((ix+1)*2*fNy*fNz + (iy+1)*fNz + (iz+1));

  // Return interpolated field value
  Double_t val = Interpolate(dx, dy, dz);
  //cout << " (Z) interpolated " << val << endl;
  return (val);

  }

  return 0.;
}
// ------------------------------------------------------------------------



// -----------   Check whether a point is inside the map   ----------------
Bool_t R3BGladFieldMap::IsInside(Double_t x, Double_t y, Double_t z,
			     Int_t& ix, Int_t& iy, Int_t& iz,
			     Double_t& dx, Double_t& dy, Double_t& dz) {

  // --- Transform into local coordinate system
  Double_t xl = x;
  Double_t yl = y;
  Double_t zl = z;

  // ---  Check for being outside the map range
  if ( ! ( xl > -fXmax && xl < fXmax && yl > -fYmax && yl < fYmax &&
	   zl >= fZmin && zl < fZmax ) ) {
    ix = iy = iz = 0;
    dx = dy = dz = 0.;
    return kFALSE;
  }


  // --- Determine grid cell
  ix = Int_t( (xl+fXmax) / fXstep );
  iy = Int_t( (yl+fYmax) / fYstep );
  iz = Int_t( (zl-fZmin) / fZstep );


  // Relative distance from grid point (in units of cell size)
  dx = (xl+fXmax) / fXstep - Double_t(ix);
  dy = (yl+fYmax) / fYstep - Double_t(iy);
  dz = (zl-fZmin) / fZstep - Double_t(iz);

  //cout << "-I- isInside true " << endl;

  return kTRUE;

}
// ------------------------------------------------------------------------



// ----------   Write the map to an ASCII file   --------------------------
void R3BGladFieldMap::WriteAsciiFile(const char* fileName) {

  // Open file
  cout << "-I- R3BGladFieldMap: Writing field map to ASCII file " 
       << fileName << endl;
  ofstream mapFile(fileName);
  if ( ! mapFile.is_open() ) {
    cerr << "-E- R3BGladFieldMap:ReadAsciiFile: Could not open file! " << endl;
    return;
  }

  // Write field map grid parameters
  mapFile.precision(6);
  mapFile << showpoint;
  if ( fType == 1 ) mapFile << "nosym" << endl;
  if ( fType == 2 ) mapFile << "sym2" << endl;
  if ( fType == 3 ) mapFile << "sym3" << endl;
  mapFile << fXmin << " " << fXmax << " " << fNx << endl;
  mapFile << fYmin << " " << fYmax << " " << fNy << endl;
  mapFile << fZmin << " " << fZmax << " " << fNz << endl;

  // Write field values
  Double_t factor = 10. * fScale;  // Takes out scaling and converts kG->T
  cout << right;
  Int_t nTot = fNx * fNy * fNz;
  cout << "-I- R3BGladFieldMap: " << fNx*fNy*fNz << " entries to write... " 
       << setw(3) << 0 << " % ";
  Int_t index=0;
  div_t modul;
  Int_t iDiv = TMath::Nint(nTot/100.);
  for(Int_t ix=0; ix<fNx; ix++) {
    for(Int_t iy=0; iy<fNy; iy++) {
      for(Int_t iz=0; iz<fNz; iz++) {
	index =ix*fNy*fNz + iy*fNz + iz;
	modul = div(index,iDiv);
	if ( modul.rem == 0 ) {
	  Double_t perc = TMath::Nint(100.*index/nTot);
	  cout << "\b\b\b\b\b\b" << setw(3) << perc << " % " << flush;
	}
	mapFile << fBx->At(index)/factor << " " << fBy->At(index)/factor 
		<< " " << fBz->At(index)/factor << endl;
      } // z-Loop
    }   // y-Loop
  }     // x-Loop
  cout << "   " << index+1 << " written" << endl;
  mapFile.close();		

}	
// ------------------------------------------------------------------------



// -------   Write field map to a ROOT file   -----------------------------
/*
void R3BGladFieldMap::WriteRootFile(const char* fileName,
				const char* mapName) {

  R3BGladFieldMapData* data = new R3BGladFieldMapData(mapName, *this);
  TFile* oldFile = gFile;
  TFile* file = new TFile(fileName, "RECREATE");
  data->Write();
  file->Close();
  if(oldFile) oldFile->cd();

}
*/

// ------------------------------------------------------------------------



// -----  Set the position of the field centre in global coordinates  -----
void R3BGladFieldMap::SetPosition(Double_t x, Double_t y, Double_t z) {
  fPosX = x;
  fPosY = y;
  fPosZ = z;
}
// ------------------------------------------------------------------------



// ---------   Screen output   --------------------------------------------
void R3BGladFieldMap::Print(Option_t *option) const
{
  TString type = "Map";
  if ( fType == 2 ) type = "Map sym2";
  if ( fType == 3 ) type = "Map sym3";
  cout << "======================================================" << endl;
  cout.precision(4);
  cout << showpoint;
  cout << "----  " << fTitle << " : " << fName << endl;
  cout << "----" << endl;
  cout << "----  Field type     : " << type << endl;
  cout << "----" << endl;
  cout << "----  Field map grid : " << endl;
  cout << "----  x = " << setw(4) << fXmin << " to " << setw(4) << fXmax 
       << " cm, " << fNx << " grid points, dx = " << fXstep << " cm" << endl;
  cout << "----  y = " << setw(4) << fYmin << " to " << setw(4) << fYmax 
       << " cm, " << fNy << " grid points, dy = " << fYstep << " cm" << endl;
  cout << "----  z = " << setw(4) << fZmin << " to " << setw(4) << fZmax 
       << " cm, " << fNz << " grid points, dz = " << fZstep << " cm" << endl;
  cout << endl;
  cout << "----  Field centre position: ( " << setw(6) << fPosX << ", "
       << setw(6) << fPosY << ", " << setw(6) << fPosZ << ") cm" << endl;
  cout << "----  Field rotation Y: " << setw(6) << fYAngle << " deg" << endl;
  cout << "----  Field scaling factor: " << fScale << endl;
//  Double_t bx = GetBx(0.,0.,0.);
//  Double_t by = GetBy(0.,0.,0.);
//  Double_t bz = GetBz(0.,0.,0.);
//  cout << "----" << endl;
//  cout << "----  Field at origin is ( " << setw(6) << bx << ", " << setw(6)
//       << by << ", " << setw(6) << bz << ") kG" << endl;
 cout << "======================================================" << endl;
}
// ------------------------------------------------------------------------  



// ---------    Reset parameters and data (private)  ----------------------
void R3BGladFieldMap::Reset() {
  fPosX = fPosY = fPosZ = 0.;
  fXmin = fYmin = fZmin = 0.;
  fXmax = fYmax = fZmax = 0.;
  fXstep = fYstep = fZstep = 0.;
  fNx = fNy = fNz = 0;
  fScale = 1.;
  if ( fBx ) { delete fBx; fBx = NULL; }
  if ( fBy ) { delete fBy; fBy = NULL; }
  if ( fBz ) { delete fBz; fBz = NULL; }
}
// ------------------------------------------------------------------------  



// -----   Read field map from ASCII file (private)   ---------------------
void R3BGladFieldMap::ReadAsciiFile(const char* fileName) {

  Double_t bx=0., by=0., bz=0.;
  Double_t ax=0., ay=0., az=0.;
  // Open file
  cout << "-I- R3BGladFieldMap: Reading field map from ASCII file " 
       << fileName << endl;
  ifstream mapFile(fileName);
  if ( ! mapFile.is_open() ) {
    cerr << "-E- R3BGladFieldMap:ReadAsciiFile: Could not open file! " << endl;
    Fatal("ReadAsciiFile","Could not open file");
  }

  // Read map type
  TString type;
  mapFile >> type;
  Int_t iType = 0;
  if ( type == "nosym" ) iType = 1;
  if ( type == "sym2"  ) iType = 2;
  if ( type == "sym3"  ) iType = 3;
  if ( fType != iType ) {
    cout << "-E- R3BGladFieldMap::ReadAsciiFile: Incompatible map types!"
	 << endl;
    cout << "    Field map is of type " << fType 
	 << " but map on file is of type " << iType << endl;
    Fatal("ReadAsciiFile","Incompatible map types");
  }

  // Read grid parameters
  mapFile >> fXmin >> fXmax >> fNx;
  mapFile >> fYmin >> fYmax >> fNy;
  mapFile >> fZmin >> fZmax >> fNz;
  fXstep = ( fXmax - fXmin ) / Double_t( fNx );
  fYstep = ( fYmax - fYmin ) / Double_t( fNy );
  fZstep = ( fZmax - fZmin ) / Double_t( fNz );
  
  // Create field arrays
    fNx += 1;
    fNy += 1;
    fNz += 1;
  fBx = new TArrayF(2*fNx * 2*fNy * fNz);
  fBy = new TArrayF(2*fNx * 2*fNy * fNz);
  fBz = new TArrayF(2*fNx * 2*fNy * fNz);

  // Read the field values
  Double_t factor = fScale * 10.;   // Factor 10 for T -> kG
  cout << right;
  Int_t nTot = fNx * fNy * fNz;
  cout << "-I- R3BGladFieldMap: " << nTot << " entries to read... " 
       << setw(3) << 0 << " % ";
  Int_t index = 0;
  div_t modul;
  Int_t iDiv = TMath::Nint(nTot/100.);
  for (Int_t ix=0; ix<fNx; ix++) {
    for (Int_t iy = 0; iy<fNy; iy++) {
      for (Int_t iz = 0; iz<fNz; iz++) {
	if (! mapFile.good()) cerr << "-E- R3BGladFieldMap::ReadAsciiFile: "
				   << "I/O Error at " << ix << " "
				   << iy << " " << iz << endl;
	index = ix*fNy*fNz + iy*fNz + iz;
	modul = div(index,iDiv);
	if ( modul.rem == 0 ) {
	  Double_t perc = TMath::Nint(100.*index/nTot);
	  cout << "\b\b\b\b\b\b" << setw(3) << perc << " % " << flush;
	}
	mapFile >> ax >> ay >> az >> bx >> by >> bz;
    
          // Implement mirroring ----------------------------------------------------------------------
          Double_t x = ax;
          Double_t y = ay;
          Double_t z = az;
          if(x < 0 || y < 0)
          {
              continue;
          }
          Int_t index1 = Int_t((x + fXmax)/fXstep)*2*fNy*fNz + Int_t((y + fYmax)/fYstep)*fNz + Int_t((z-fZmin)/fZstep);
          Int_t index2 = Int_t((-x + fXmax)/fXstep)*2*fNy*fNz + Int_t((y + fYmax)/fYstep)*fNz + Int_t((z-fZmin)/fZstep);
          Int_t index3 = Int_t((-x + fXmax)/fXstep)*2*fNy*fNz + Int_t((-y + fYmax)/fYstep)*fNz + Int_t((z-fZmin)/fZstep);
          Int_t index4 = Int_t((x + fXmax)/fXstep)*2*fNy*fNz + Int_t((-y + fYmax)/fYstep)*fNz + Int_t((z-fZmin)/fZstep);
          
          TVector3 B1(bx, by, bz);
          B1.RotateY(fYAngle*TMath::DegToRad());

          TVector3 B2(-bx, by, bz);
          B2.RotateY(fYAngle*TMath::DegToRad());

          TVector3 B3(-bx, by, -bz);
          B3.RotateY(fYAngle*TMath::DegToRad());

          TVector3 B4(-bx, by, -bz);
          B4.RotateY(fYAngle*TMath::DegToRad());

          fBx->AddAt(factor*B1.X(), index1);
          fBy->AddAt(factor*B1.Y(), index1);
          fBz->AddAt(factor*B1.Z(), index1);

          fBx->AddAt(factor*B2.X(), index2);
          fBy->AddAt(factor*B2.Y(), index2);
          fBz->AddAt(factor*B2.Z(), index2);

          fBx->AddAt(factor*B3.X(), index3);
          fBy->AddAt(factor*B3.Y(), index3);
          fBz->AddAt(factor*B3.Z(), index3);

          fBx->AddAt(factor*B4.X(), index4);
          fBy->AddAt(factor*B4.Y(), index4);
          fBz->AddAt(factor*B4.Z(), index4);
          // ------------------------------------------------------------------------------------------

      //  cout << "-I- " << bx << " : " << by << " : "  << bz  << " : " << endl;
	if ( mapFile.eof() ) {
	  cerr << endl << "-E- R3BGladFieldMap::ReadAsciiFile: EOF"
	       << " reached at " << ix << " " << iy << " " << iz << endl;
	  mapFile.close();
	  break;
	}
      }   // z-Loop
    }     // y-Loop0)
  }       // x-Loop

  cout << "   " << index+1 << " read" << endl;

  mapFile.close();
//  exit(0);

}
// ------------------------------------------------------------------------



// -------------   Read field map from ROOT file (private)  ---------------
/*
void R3BGladFieldMap::ReadRootFile(const char* fileName,
			       const char* mapName) {

  // Store gFile pointer
  TFile* oldFile = gFile;

  // Open root file
  cout << "-I- R3BGladFieldMap: Reading field map from ROOT file " 
       << fileName << endl; 
  TFile* file = new TFile(fileName, "READ");		
  if (!(file->IsOpen())) {
    cerr << "-E- R3BGladFieldMap::ReadRootfile: Cannot read from file! " 
	 << endl;
    Fatal("ReadRootFile","Cannot read from file");
  }

  // Get the field data object
  R3BGladFieldMapData* data = NULL;
  file->GetObject(mapName, data);
  if ( ! data ) {
    cout << "-E- R3BGladFieldMap::ReadRootFile: data object " << fileName
	 << " not found in file! " << endl;
    exit(-1);
  }

  // Get the field parameters
  SetField(data);

  // Close the root file and delete the data object
  file->Close();
  delete data;
  if ( oldFile ) oldFile->cd();

}
*/

// ------------------------------------------------------------------------



// ------------   Set field parameters and data (private)  ----------------
/*
void R3BGladFieldMap::SetField(const R3BGladFieldMapData* data) {

  // Check compatibility
  if ( data->GetType() != fType ) {
    if (!((data->GetType()==3)&&(fType==5)))                   // E.Litvinenko
      {
	cout << "-E- R3BGladFieldMap::SetField: Incompatible map types!"
	     << endl;
	cout << "    Field map is of type " << fType 
	     << " but map on file is of type " << data->GetType() << endl;
	Fatal("SetField","Incompatible map types");
      }
    else
      cout << "   R3BGladFieldMap::SetField: Warning:  You are using PosDepScaled map (original map type = 3)" << endl;
  }
  
  
  fXmin = data->GetXmin();
  fYmin = data->GetYmin();
  fZmin = data->GetZmin();
  fXmax = data->GetXmax();
  fYmax = data->GetYmax();
  fZmax = data->GetZmax();
  fNx = data->GetNx();
  fNy = data->GetNy();
  fNz = data->GetNz();
  fXstep = ( fXmax - fXmin ) / Double_t( fNx - 1 );
  fYstep = ( fYmax - fYmin ) / Double_t( fNy - 1 );
  fZstep = ( fZmax - fZmin ) / Double_t( fNz - 1 );
  if ( fBx ) delete fBx;
  if ( fBy ) delete fBy;
  if ( fBz ) delete fBz;
  fBx = new TArrayF(*(data->GetBx()));
  fBy = new TArrayF(*(data->GetBy()));
  fBz = new TArrayF(*(data->GetBz()));

  // Scale and convert from T to kG
  Double_t factor = fScale * 10.;
  Int_t index = 0;
  for (Int_t ix=0; ix<fNx; ix++) {
    for (Int_t iy=0; iy<fNy; iy++) {
      for (Int_t iz=0; iz<fNz; iz++) {
	index = ix*fNy*fNz + iy*fNz + iz;
	if ( fBx ) (*fBx)[index] = (*fBx)[index] * factor;
	if ( fBy ) (*fBy)[index] = (*fBy)[index] * factor;
	if ( fBz ) (*fBz)[index] = (*fBz)[index] * factor;
      }
    }
  }

}
*/

// ------------   Interpolation in a grid cell (private)  -----------------
Double_t R3BGladFieldMap::Interpolate(Double_t dx, Double_t dy, Double_t dz) {

  // Interpolate in x coordinate
  fHb[0][0] = fHa[0][0][0] + ( fHa[1][0][0]-fHa[0][0][0] ) * dx;
  fHb[1][0] = fHa[0][1][0] + ( fHa[1][1][0]-fHa[0][1][0] ) * dx;
  fHb[0][1] = fHa[0][0][1] + ( fHa[1][0][1]-fHa[0][0][1] ) * dx;
  fHb[1][1] = fHa[0][1][1] + ( fHa[1][1][1]-fHa[0][1][1] ) * dx;

  // Interpolate in y coordinate
  fHc[0] = fHb[0][0] + ( fHb[1][0] - fHb[0][0] ) * dy;
  fHc[1] = fHb[0][1] + ( fHb[1][1] - fHb[0][1] ) * dy;

  // Interpolate in z coordinate
  return fHc[0] + ( fHc[1] - fHc[0] ) * dz;

}
// ------------------------------------------------------------------------



ClassImp(R3BGladFieldMap)
