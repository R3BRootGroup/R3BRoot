#ifndef R3BMTOFDIGIPAR_H
#define R3BMTOFDIGIPAR_H 1


#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BmTofDigiPar : public FairParGenericSet
{
    public :
	R3BmTofDigiPar (const char* name="R3BmTofDigiParTest",
			const char* title="Tutorial  parameter",
			const char* context="TestDefaultContext");
	~R3BmTofDigiPar(void){};
	void clear(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/

  private:
  // Digi. Parameters


   ClassDef(R3BmTofDigiPar,1); //
};

#endif /* !R3BMTOFDIGIPAR_H*/

