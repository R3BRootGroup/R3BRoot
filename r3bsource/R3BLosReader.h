#ifndef R3BLOSREADER_H
#define R3BLOSREADER_H

#include "R3BReader.h"
#include "FairTask.h"
struct EXT_STR_h101_LOS_TAMEX_t;
typedef struct EXT_STR_h101_LOS_TAMEX_t EXT_STR_h101_LOS_TAMEX;
class FairLogger;
class TH1F;
class TH2F;

class R3BLosReader : public R3BReader
{
	public:
		R3BLosReader(EXT_STR_h101_LOS_TAMEX *, UInt_t);
		~R3BLosReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();
		virtual void FinishTask();
        
	private:
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_LOS_TAMEX* fData;
		/* Data offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BLosxMappedItem */
		TClonesArray* fArray; /**< Output array. */
        Int_t fNEvents = 0;
        
	public:
		ClassDef(R3BLosReader, 0);
};

#endif

