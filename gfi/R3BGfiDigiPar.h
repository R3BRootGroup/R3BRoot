#ifndef R3BGFIDIGIPAR_H
#define R3BGFIDIGIPAR_H 1

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BGfiDigiPar : public FairParGenericSet
{
    public :
	R3BGfiDigiPar (const char* name="R3BGfiDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BGfiDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	void Print();
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BGfiDigiPar,1); //
};

#endif /* !R3BGFIDIGIPAR_H*/

