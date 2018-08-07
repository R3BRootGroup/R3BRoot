#ifndef R3BSCI8READER_H
#define R3BSCI8READER_H

#include "R3BReader.h"
#include "FairTask.h"
struct EXT_STR_h101_SCI8_t;
typedef struct EXT_STR_h101_SCI8_t EXT_STR_h101_SCI8;
class FairLogger;
class TH1F;
class TH2F;

class R3BSci8Reader : public R3BReader
{
	public:
		R3BSci8Reader(EXT_STR_h101_SCI8 *, UInt_t);
		~R3BSci8Reader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();
		virtual void FinishTask();
        
	private:
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_SCI8* fData;
		/* Data offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BSci8xMappedItem */
		TClonesArray* fArray; /**< Output array. */
        Int_t fNEvents = 0;
        
	public:
		ClassDef(R3BSci8Reader, 0);
};

#endif

