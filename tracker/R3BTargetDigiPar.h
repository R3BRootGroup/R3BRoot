#ifndef R3BTARGETDIGIPAR_H
#define R3BTARGETDIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BTargetDigiPar : public FairParGenericSet
{
    public :
	R3BTargetDigiPar (const char* name="R3BTargetDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BTargetDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BTargetDigiPar,1); //
};

#endif /* !R3BTARGETDIGIPAR_H*/

