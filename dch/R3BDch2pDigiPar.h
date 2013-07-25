#ifndef R3BDCH2PDIGIPAR_H
#define R3BDCH2PDIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BDch2pDigiPar : public FairParGenericSet
{
    public :
	R3BDch2pDigiPar (const char* name="R3BDch2pDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BDch2pDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BDch2pDigiPar,1); //
};

#endif /* !R3BDCH2PDIGIPAR_H*/

