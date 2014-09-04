#ifndef R3BSTARTRADIGIPAR_H
#define R3BSTARTRADIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BSTaRTraDigiPar : public FairParGenericSet
{
    public :
	R3BSTaRTraDigiPar (const char* name="R3BSTaRTraDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BSTaRTraDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BSTaRTraDigiPar,1); //
};

#endif /* !R3BSTARTRADIGIPAR_H*/

