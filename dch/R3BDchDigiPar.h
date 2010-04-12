#ifndef R3BDCHDIGIPAR_H
#define R3BDCHDIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BDchDigiPar : public FairParGenericSet
{
    public :
	R3BDchDigiPar (const char* name="R3BDchDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BDchDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	void Print();
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BDchDigiPar,1); //
};

#endif /* !R3BDCHDIGIPAR_H*/

