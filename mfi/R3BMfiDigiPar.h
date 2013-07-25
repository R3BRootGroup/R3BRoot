#ifndef R3BMFIDIGIPAR_H
#define R3BMFIDIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BMfiDigiPar : public FairParGenericSet
{
    public :
	R3BMfiDigiPar (const char* name="R3BMfiDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BMfiDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BMfiDigiPar,1); //
};

#endif /* !R3BGFIDIGIPAR_H*/

