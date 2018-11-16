#ifndef R3BBEAMMONITORREADER_H
#define R3BBEAMMONITORREADER_H

#include "R3BReader.h"
#include "FairTask.h"
struct EXT_STR_h101_BMON_t;
typedef struct EXT_STR_h101_BMON_t EXT_STR_h101_BMON;
class FairLogger;
class TH1F;
class TH2F;

class R3BBeamMonitorReader : public R3BReader
{
	public:
		R3BBeamMonitorReader(EXT_STR_h101_BMON *, UInt_t);
		~R3BBeamMonitorReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();
		virtual void FinishTask();
        
	private:
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_BMON* fData;
		/* Data offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BLosxMappedItem */
		TClonesArray* fArray; /**< Output array. */
        Int_t fNEvents = 0;
        R3BEventHeader* header;
        
	public:
		ClassDef(R3BBeamMonitorReader, 0);
};

#endif

