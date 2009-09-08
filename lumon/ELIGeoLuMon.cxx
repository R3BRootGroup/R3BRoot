
/////////////////////////////////////////////////////////////
// ELIGeoLuMon
//
// Class for geometry of ELICAL
//
/////////////////////////////////////////////////////////////
#include "ELIGeoLuMon.h"
#include "FairGeoNode.h"

#include <iostream>

using std::cout;
using std::endl;


ClassImp(ELIGeoLuMon)

ELIGeoLuMon::ELIGeoLuMon() {
  // Constructor
  fName="sts";
  maxSectors=0;
  maxModules=99;
}

const char* ELIGeoLuMon::getModuleName(Int_t m) {
  // Returns the module name of sts number m
  if ( m < 0 ) {
  	cout <<"-E- ELIGeoLuMon::getModuleName:: Module number "
  	       << m << " not known!" << endl;
	 return "";
	 }
  if ( m < 9 ) sprintf(modName,"calstation0%i",m+1);
  else  sprintf(modName,"calstation%i",m+1);
  return modName;
}

const char* ELIGeoLuMon::getEleName(Int_t m) {
  // Returns the element name of sts number m
  sprintf(eleName,"cal%i",m+1);
  return eleName;
}
