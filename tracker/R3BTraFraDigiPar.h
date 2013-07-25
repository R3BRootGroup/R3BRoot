#ifndef R3BTRAFRADIGIPAR_H
#define R3BTRAFRADIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BTraFraDigiPar : public FairParGenericSet
{
    public :
	R3BTraFraDigiPar (const char* name="R3BTraFraDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BTraFraDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BTraFraDigiPar,1); //
};

#endif /* !R3BTRAFRADIGIPAR_H*/

