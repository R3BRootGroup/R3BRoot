#ifndef R3BNEULANDQCALFILLER_H
#define R3BNEULANDQCALFILLER_H

#include "FairTask.h"

class TClonesArray;
class R3BEventHeader;

class R3BNeulandQCalFiller : public FairTask{
    public:
	R3BNeulandQCalFiller();
	~R3BNeulandQCalFiller();
	  
	virtual InitStatus Init();
	virtual void Exec(Option_t* option); 
	  
    private:	
	TClonesArray* data;
	R3BEventHeader* header;
	Int_t i = 0;
	
    public:
	ClassDef(R3BNeulandQCalFiller,1);
};

#endif
