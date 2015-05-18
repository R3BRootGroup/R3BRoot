#ifndef R3BDTOFDIGIPAR_H
#define R3BDTOFDIGIPAR_H 1


#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BdTofDigiPar : public FairParGenericSet
{
    public :
	R3BdTofDigiPar (const char* name="R3BdTofDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BdTofDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BdTofDigiPar,1); //
};

#endif /* !R3BDTOFDIGIPAR_H*/

