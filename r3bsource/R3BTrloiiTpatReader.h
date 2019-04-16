#ifndef R3BTRLOIITPATREADER_H
#define R3BTRLOIITPATREADER_H
#include "R3BReader.h"

struct EXT_STR_h101_TPAT_t;
typedef struct EXT_STR_h101_TPAT_t EXT_STR_h101_TPAT;

class FairLogger;
class R3BEventHeader;

class R3BTrloiiTpatReader : public R3BReader
{
	public:
		R3BTrloiiTpatReader(EXT_STR_h101_TPAT *, UInt_t);
		~R3BTrloiiTpatReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* An event counter */
		UInt_t fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_TPAT* fData;
		/* Offset of detector specific data in full data structure */
		UInt_t fOffset;
		/* A pointer to the R3BEventHeader structure */
		R3BEventHeader *fEventHeader;

	public:
		ClassDef(R3BTrloiiTpatReader, 0);
};

#endif
