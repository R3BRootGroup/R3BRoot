#ifndef R3BFI7DIGIPAR_H
#define R3BFI7DIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BFi7DigiPar : public FairParGenericSet
{
    public :
	R3BFi7DigiPar (const char* name="R3BFi7DigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BFi7DigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BFi7DigiPar,1); //
};

#endif /* !R3BFI7DIGIPAR_H*/

