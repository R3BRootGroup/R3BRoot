#ifndef R3BTOFDIGIPAR_H
#define R3BTOFDIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BTofDigiPar : public FairParGenericSet
{
    public :
	R3BTofDigiPar (const char* name="R3BTofDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BTofDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	void Print();
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BTofDigiPar,1); //
};

#endif /* !R3BDTOFDIGIPAR_H*/

