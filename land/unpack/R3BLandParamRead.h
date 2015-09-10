#ifndef R3BLANDPARAMREAD_H
#define R3BLANDPARAMREAD_H

#include "FairTask.h" // for FairTask, InitStatus

#include "Rtypes.h" // for ClassDef

#include <stddef.h> // for NULL

class R3BTCalPar;

class R3BLandParamRead : public FairTask
{
  public:
    /** Default constructor **/
    R3BLandParamRead()
        : FairTask()
        , fCalPar(NULL){};

    /** Destructor **/
    virtual ~R3BLandParamRead(){};

    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    /** Virtual method Finish **/
    virtual void Finish();

    R3BTCalPar* fCalPar;

    R3BLandParamRead(const R3BLandParamRead&);
    R3BLandParamRead& operator=(const R3BLandParamRead&);

    ClassDef(R3BLandParamRead, 0);
};

#endif
