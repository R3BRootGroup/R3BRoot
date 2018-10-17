//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

#include "R3BGeoFi7Par.h"

#include "FairParamList.h"

#include "TObjArray.h"

#include <iostream>
#include <iomanip>

ClassImp(R3BGeoFi7Par)

R3BGeoFi7Par::R3BGeoFi7Par(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

R3BGeoFi7Par::~R3BGeoFi7Par(void) {
}

void R3BGeoFi7Par::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void R3BGeoFi7Par::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t R3BGeoFi7Par::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
