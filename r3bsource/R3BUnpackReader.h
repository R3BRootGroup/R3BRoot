#ifndef R3BUNPACKREADER_H
#define R3BUNPACKREADER_H

#include "R3BReader.h"

typedef struct EXT_STR_h101_t EXT_STR_h101;
class FairLogger;
class R3BEventHeader;

class R3BUnpackReader : public R3BReader
{
	public:
		R3BUnpackReader(EXT_STR_h101 *);
		~R3BUnpackReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* An event counter */
		UInt_t fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101* fData;
		/* FairLogger */
		FairLogger*	fLogger;

		R3BEventHeader* fHeader;

	public:
		ClassDef(R3BUnpackReader, 0);
};

#endif

