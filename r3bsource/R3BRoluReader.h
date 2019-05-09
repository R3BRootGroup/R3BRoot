#ifndef R3BROLUREADER_H
#define R3BROLUREADER_H

#include "R3BReader.h"
#include "FairTask.h"
struct EXT_STR_h101_ROLU_t;
typedef struct EXT_STR_h101_ROLU_t EXT_STR_h101_ROLU;
class FairLogger;
class TH1F;
class TH2F;

class R3BRoluReader : public R3BReader
{
	public:
		R3BRoluReader(EXT_STR_h101_ROLU *, UInt_t);
		~R3BRoluReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();
		virtual void FinishTask();

                /** Accessor to select online mode **/
                void SetOnline(Bool_t option){fOnline=option;} 
        
	private:
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_ROLU* fData;
		/* Data offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BRoluxMappedItem */
		TClonesArray* fArray; /**< Output array. */
                //Don't store data for online
                Bool_t fOnline;
        Int_t fNEvents = 0;
        R3BEventHeader* header;
        
	public:
		ClassDef(R3BRoluReader, 0);
};

#endif

