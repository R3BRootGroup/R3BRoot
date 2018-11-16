#ifndef R3BTIMESTAMPMASTERREADER_H
#define R3BTIMESTAMPMASTERREADER_H

#include "R3BReader.h"
#include "R3BWhiterabbitReaderImpl.h"

struct EXT_STR_h101_timestamp_master_t;
typedef struct EXT_STR_h101_timestamp_master_t EXT_STR_h101_timestamp_master;

class FairLogger;
class R3BEventHeader;

class R3BTimestampMasterReader : public R3BReader
{
	public:
		R3BTimestampMasterReader(EXT_STR_h101_timestamp_master *,
		    UInt_t);

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		UInt_t fNEvent;
		EXT_STR_h101_timestamp_master *fData;
		UInt_t fOffset;
		FairLogger *fLogger;
		UInt_t fWhiterabbitId;
		R3BEventHeader *fEventHeader;

	public:
		ClassDef(R3BTimestampMasterReader, 0);
};

#endif
