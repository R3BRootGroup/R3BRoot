#ifndef R3BPTOFHITPAR_H
#define R3BPTOFHITPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include "TArrayD.h"

class R3BPtofHitPar : public FairParGenericSet
{
    public :
	R3BPtofHitPar (const char* name="PtofHitPar",
			const char* title="Ptof Hit Calibration",
			const char* context="TestDefaultContext");
	~R3BPtofHitPar(void){};
	void putParams(FairParamList* list);
	Bool_t getParams(FairParamList* list);

	virtual void Print(Option_t *option="") const;
	/** Accessor functions **/
	

  private:
          //TODO define parameters: offset for pmt 1+2, chargescale, effective speed and their getters and setters
          //use TArrayD Objects

   ClassDef(R3BPtofHitPar,1);
};

#endif /* !R3BPTOFHITPAR_H*/

