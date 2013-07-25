#ifndef R3BPSPDIGIPAR_H
#define R3BPSPDIGIPAR_H 1


#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BPspDigiPar : public FairParGenericSet
{
    public :
	R3BPspDigiPar (const char* name="R3BPspDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BPspDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BPspDigiPar,1); //
};

#endif /* !R3BMTOFDIGIPAR_H*/

