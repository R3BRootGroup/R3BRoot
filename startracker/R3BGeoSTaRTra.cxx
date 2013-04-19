
/////////////////////////////////////////////////////////////
// R3BGeoSTaRTra
//
// Class for geometry of R3BCAL
//
/////////////////////////////////////////////////////////////
#include "R3BGeoSTaRTra.h"
#include "FairGeoNode.h"

#include <iostream>

using std::cout;
using std::endl;


ClassImp(R3BGeoSTaRTra)

R3BGeoSTaRTra::R3BGeoSTaRTra() {
  // Constructor
  fName="sts";
  maxSectors=0;
  maxModules=99;
}

const char* R3BGeoSTaRTra::getModuleName(Int_t m) {
  // Returns the module name of sts number m
  if ( m < 0 ) {
  	cout <<"-E- R3BGeoSTaRTra::getModuleName:: Module number "
  	       << m << " not known!" << endl;
	 return "";
	 }
  if ( m < 9 ) sprintf(modName,"trackerstation0%i",m+1);
  else  sprintf(modName,"trackerstation%i",m+1);
  return modName;
}

const char* R3BGeoSTaRTra::getEleName(Int_t m) {
  // Returns the element name of sts number m
  sprintf(eleName,"tracker%i",m+1);
  return eleName;
}
