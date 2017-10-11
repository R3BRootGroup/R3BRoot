#ifndef R3BWHITERABBITREADER_H
#define R3BWHITERABBITREADER_H
#include "R3BReader.h"

struct EXT_STR_h101_whiterabbit_t;
typedef struct EXT_STR_h101_whiterabbit_t EXT_STR_h101_whiterabbit;

class FairLogger;
class R3BEventHeader;

class R3BWhiterabbitReader : public R3BReader
{
	public:
		R3BWhiterabbitReader(EXT_STR_h101_whiterabbit *, UInt_t,
		    UInt_t);
		~R3BWhiterabbitReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* An event counter */
		UInt_t fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_whiterabbit* fData;
		/* Offset of detector specific data in full data structure */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger* fLogger;
		/* The whiterabbit subsystem ID */
		UInt_t fWhiterabbitId;
		/* A pointer to the R3BEventHeader structure */
		R3BEventHeader* fEventHeader;

	public:
		ClassDef(R3BWhiterabbitReader, 0);
};
#endif
