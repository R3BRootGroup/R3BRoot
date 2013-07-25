#ifndef R3BTARGET2PDIGIPAR_H
#define R3BTARGET2PDIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BTarget2pDigiPar : public FairParGenericSet
{
    public :
	R3BTarget2pDigiPar (const char* name="R3BTarget2pDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BTarget2pDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BTarget2pDigiPar,1); //
};

#endif /* !R3BTARGET2PDIGIPAR_H*/

