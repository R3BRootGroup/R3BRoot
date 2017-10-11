
/////////////////////////////////////////////////////////////
// R3BGeoStartrack
//
// Class for geometry of R3BStartrack
//
/////////////////////////////////////////////////////////////
#include "R3BGeoStartrack.h"
#include "FairGeoNode.h"

#include <iostream>

using std::cout;
using std::endl;


ClassImp(R3BGeoStartrack)

R3BGeoStartrack::R3BGeoStartrack() {
  // Constructor
  fName="sts";
  maxSectors=0;
  maxModules=99;
}

const char* R3BGeoStartrack::getModuleName(Int_t m) {
  // Returns the module name of sts number m
  if ( m < 0 ) {
  	cout <<"-E- R3BGeoStartrack::getModuleName:: Module number "
  	       << m << " not known!" << endl;
	 return "";
	 }
  if ( m < 9 ) sprintf(modName,"trackerstation0%i",m+1);
  else  sprintf(modName,"trackerstation%i",m+1);
  return modName;
}

const char* R3BGeoStartrack::getEleName(Int_t m) {
  // Returns the element name of sts number m
  sprintf(eleName,"tracker%i",m+1);
  return eleName;
}
