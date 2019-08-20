#ifndef R3BSCI2READER_H
#define R3BSCI2READER_H

#include "R3BReader.h"
#include "FairTask.h"
struct EXT_STR_h101_SCI2_t;
typedef struct EXT_STR_h101_SCI2_t EXT_STR_h101_SCI2;
class FairLogger;
class TH1F;
class TH2F;

class R3BSci2Reader : public R3BReader
{
	public:
		R3BSci2Reader(EXT_STR_h101_SCI2 *, UInt_t);
		~R3BSci2Reader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();
		virtual void FinishTask();
        
	private:
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_SCI2* fData;
		/* Data offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BSci2xMappedItem */
		TClonesArray* fArray; /**< Output array. */
        Int_t fNEvents = 0;
        
	public:
		ClassDef(R3BSci2Reader, 0);
};

#endif

