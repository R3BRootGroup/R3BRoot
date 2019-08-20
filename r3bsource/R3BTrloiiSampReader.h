#ifndef R3BTRLOIISAMPREADER_H
#define R3BTRLOIISAMPREADER_H
#include "R3BReader.h"

struct EXT_STR_h101_SAMP_t;
typedef struct EXT_STR_h101_SAMP_t EXT_STR_h101_SAMP;

class FairLogger;
class R3BEventHeader;

class R3BTrloiiSampReader : public R3BReader
{
	public:
		R3BTrloiiSampReader(EXT_STR_h101_SAMP *, UInt_t);
		~R3BTrloiiSampReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* An event counter */
		UInt_t fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_SAMP* fData;
		/* Offset of detector specific data in full data structure */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger *fLogger;
		/* A pointer to the R3BEventHeader structure */
		R3BEventHeader *fEventHeader;
		TClonesArray* fArray; /**< Output array. */

	public:
		ClassDef(R3BTrloiiSampReader, 0);
};

#endif
