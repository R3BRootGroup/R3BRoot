// -------------------------------------------------------------------------
// -----                  R3BDetectorList.header file                  -----
// -----                 Created 11/02/09  by D.Bertini                -----
// -------------------------------------------------------------------------


/** Unique identifier for all Panda detector systems **/

#ifndef R3BDETECTORLIST_H
#define R3BDETECTORLIST_H 1

enum DetectorId {
    kDCH,kCAL,kLAND,kGFI,kMTOF,kTOF,kTRA};
    
/** Unique identifier for all Panda Point and Hit types **/

enum fDetectorType {  
   kUnknown,kDchPoint,kCalPoint,kLandPoint,kGfiPoint,kmTofPoint,kTofPoint,kTraPoint};


enum SensorSide { kTOP, kBOTTOM };

#endif
