#ifndef R3BTRADIGIPAR_H
#define R3BTRADIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BTraDigiPar : public FairParGenericSet
{
    public :
	R3BTraDigiPar (const char* name="R3BTraDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BTraDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BTraDigiPar,1); //
};

#endif /* !R3BTRADIGIPAR_H*/

