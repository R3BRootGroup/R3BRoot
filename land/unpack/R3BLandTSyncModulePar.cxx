#include "R3BLandTSyncModulePar.h"

#include "FairParamList.h" // for FairParamList
#include "FairLogger.h"

using namespace std;

R3BLandTSyncModulePar::R3BLandTSyncModulePar (const char* name, const char* title, const char* context, Bool_t own)
   : FairParGenericSet (name, title, context, own)
   , fModuleId (0)
   , fSide (0) {
   // Reset all parameters
   clear();
}

R3BLandTSyncModulePar::~R3BLandTSyncModulePar() {
}

void R3BLandTSyncModulePar::putParams (FairParamList* list) {
   LOG (INFO) << "R3BLandTSyncModulePar::putParams() called" << FairLogger::endl;
   if (!list) {
      return;
   }
   list->add ("module_id", fModuleId);
   list->add ("side", fSide);
   list->add ("timeoffset", fTimeOffset);
   list->add ("timeoffset_error", fTimeOffsetError);
   list->add ("energiegain", fEnergieGain);
   list->add ("energiegain_error", fEnergieGainError);
   list->add ("effectivespeed", fEffectiveSpeed);
   list->add ("effectivespeed_error", fEffectiveSpeedError);
}

Bool_t R3BLandTSyncModulePar::getParams (FairParamList* list) {
   if (!list) {
      return kFALSE;
   }
   if (!list->fill ("module_id", &fModuleId)) {
      return kFALSE;
   }
   if (!list->fill ("side", &fSide)) {
      return kFALSE;
   }
   if (!list->fill ("timeoffset", &fTimeOffset)) {
      return kFALSE;
   }
   if (!list->fill ("timeoffset_error", &fTimeOffsetError)) {
      return kFALSE;
   }
   if (!list->fill ("energiegain", &fEnergieGain)) {
      return kFALSE;
   }
   if (!list->fill ("energiegain_error", &fEnergieGainError)) {
      return kFALSE;
   }
   if (!list->fill ("effectivespeed", &fEffectiveSpeed)) {
      return kFALSE;
   }
   if (!list->fill ("effectivespeed_error", &fEffectiveSpeedError)) {
      return kFALSE;
   }

   return kTRUE;
}

void R3BLandTSyncModulePar::clear() {
}

void R3BLandTSyncModulePar::printParams() {
   LOG (INFO) << "   R3BLandTSyncModulePar: Time Calibration Parameters: " << FairLogger::endl;
   LOG (INFO) << "   fBarId: " << fModuleId << FairLogger::endl;
   LOG (INFO) << "   fSide: " << fSide << FairLogger::endl;
   FairLogger::GetLogger()->Info (MESSAGE_ORIGIN, "   fTimeOffset: %4.2f  ±   %4.2f", fTimeOffset, fTimeOffsetError);
   FairLogger::GetLogger()->Info (MESSAGE_ORIGIN, "   fEffectiveSpeed: %4.2f  ±   %4.2f", fEffectiveSpeed, fEffectiveSpeedError);
   FairLogger::GetLogger()->Info (MESSAGE_ORIGIN, "   fEnergieGain: %4.2f  ±   %4.2f", fEnergieGain, fEnergieGainError);
}

ClassImp (R3BLandTSyncModulePar);
