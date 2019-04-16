
/////////////////////////////////////////////////////////////
// R3BGeoXBall
//
// Class for geometry of R3BCAL
//
/////////////////////////////////////////////////////////////

#include "FairGeoNode.h"
#include "FairLogger.h"
#include "R3BGeoXBall.h"



R3BGeoXBall::R3BGeoXBall()
{
  // Constructor
  fName = "xball";
  maxSectors = 0;
  maxModules = 99;
}



const char* R3BGeoXBall::getModuleName(Int_t m)
{
  // Returns the module name of sts number m
  if(m < 0) {
  	LOG(ERROR) << "R3BGeoXBall::getModuleName:: Module number "
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



const char* R3BGeoXBall::getEleName(Int_t m)
{
  // Returns the element name of sts number m
  sprintf(eleName,"cal%i",m+1);
  return eleName;
}



ClassImp(R3BGeoXBall)
