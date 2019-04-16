
/////////////////////////////////////////////////////////////
// R3BGeoTra
//
// Class for geometry of R3BCAL
//
/////////////////////////////////////////////////////////////

#include "FairLogger.h"
#include "FairGeoNode.h"
#include "R3BGeoTra.h"



R3BGeoTra::R3BGeoTra()
{
  // Constructor
  fName="sts";
  maxSectors=0;
  maxModules=99;
}



const char* R3BGeoTra::getModuleName(Int_t m)
{
  // Returns the module name of sts number m
  if(m < 0) {
  	LOG(ERROR) << "R3BGeoTra::getModuleName:: Module number "
    << m << " not known!";
    return "";
  }
  if(m < 9) {
    sprintf(modName,"calstation0%i",m+1);
  } else {
    sprintf(modName,"calstation%i",m+1);
  }
  return modName;
}



const char* R3BGeoTra::getEleName(Int_t m)
{
  // Returns the element name of sts number m
  sprintf(eleName,"cal%i",m+1);
  return eleName;
}



ClassImp(R3BGeoTra)
