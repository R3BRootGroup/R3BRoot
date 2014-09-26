//
// Taken from R3BLANDParamRead.h
//
#ifndef R3BSTARTRACKPARAMREAD_H
#define R3BSTARTRACKPARAMREAD_H

#include "FairTask.h"                   // for FairTask, InitStatus

#include "Rtypes.h"                     // for ClassDef

#include <stddef.h>                     // for NULL

class R3BStarTrackCalPar;

class R3BStarTrackParamRead : public FairTask
{
  public:

    /** Default constructor **/
    R3BStarTrackParamRead() : FairTask(), fCalPar(NULL) {};

    /** Destructor **/
    virtual ~R3BStarTrackParamRead() {};


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    /** Virtual method Finish **/
    virtual void Finish();

    R3BStarTrackCalPar* fCalPar;

    R3BStarTrackParamRead(const R3BStarTrackParamRead&);
    R3BStarTrackParamRead& operator=(const R3BStarTrackParamRead&);

    ClassDef(R3BStarTrackParamRead,0);
};

#endif
