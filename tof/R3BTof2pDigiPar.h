#ifndef R3BTOF2PDIGIPAR_H
#define R3BTOF2PDIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BTof2pDigiPar : public FairParGenericSet
{
    public :
	R3BTof2pDigiPar (const char* name="R3BTof2pDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BTof2pDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BTof2pDigiPar,1); //
};

#endif /* !R3BDTOFDIGIPAR_H*/

