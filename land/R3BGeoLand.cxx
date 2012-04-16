

/////////////////////////////////////////////////////////////
// R3BGeoLand
//
// Class for geometry of R3BCAL
//
/////////////////////////////////////////////////////////////
#include "R3BGeoLand.h"
#include "FairGeoNode.h"

#include <iostream>

using std::cout;
using std::endl;


ClassImp(R3BGeoLand)

R3BGeoLand::R3BGeoLand() {
  // Constructor
  fName="sts";
  maxSectors=0;
  maxModules=99;
}

const char* R3BGeoLand::getModuleName(Int_t m) {
  // Returns the module name of sts number m
  if ( m < 0 ) {
  	cout <<"-E- R3BGeoLand::getModuleName:: Module number "
  	       << m << " not known!" << endl;
	 return "";
	 }
  if ( m < 9 ) sprintf(modName,"calstation0%i",m+1);
  else  sprintf(modName,"calstation%i",m+1);
  return modName;
}

const char* R3BGeoLand::getEleName(Int_t m) {
  // Returns the element name of sts number m
  sprintf(eleName,"cal%i",m+1);
  return eleName;
}
