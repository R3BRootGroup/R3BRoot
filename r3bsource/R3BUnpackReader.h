#ifndef R3BUNPACKREADER_H
#define R3BUNPACKREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_unpack_t;
typedef struct EXT_STR_h101_unpack_t EXT_STR_h101_unpack;

class FairLogger;
class R3BEventHeader;

class R3BUnpackReader : public R3BReader
{
	public:
		R3BUnpackReader(EXT_STR_h101_unpack *, UInt_t);
		~R3BUnpackReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* An event counter */
		UInt_t fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_unpack* fData;
		/* Offset of detector specific data in full data structure */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;

		R3BEventHeader* fHeader;

	public:
		ClassDef(R3BUnpackReader, 0);
};

#endif

