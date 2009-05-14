// -------------------------------------------------------------------------
// -----                    CbmFieldCreator header file                  -----
// -----                Created 15/01/07  by M. Al-Turany              -----
// -------------------------------------------------------------------------


#ifndef R3BFIELDCREATOR_H
#define R3BFIELDCREATOR_H

#include "FairField.h"
#include "FairFieldFactory.h"
#include "R3BFieldPar.h"

class R3BFieldCreator : public FairFieldFactory 
{
public:
    R3BFieldCreator();
    virtual ~R3BFieldCreator();
    virtual FairField* createFairField();
    virtual void SetParm();
    ClassDef(R3BFieldCreator,1);
protected:
    R3BFieldPar* fFieldPar;
};
#endif //R3BFIELDCREATOR_H
