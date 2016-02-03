#ifndef R3BLOSREADER_H
#define R3BLOSREADER_H

#include "R3BReader.h"

typedef struct EXT_STR_h101_t EXT_STR_h101;
class FairLogger;

class R3BLosReader : public R3BReader
{
	public:
		R3BLosReader(EXT_STR_h101 *);
		~R3BLosReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* Reader specific data structure from ucesb */
		EXT_STR_h101* fData;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BLosxMappedItem */
		TClonesArray* fArray; /**< Output array. */


	public:
		ClassDef(R3BLosReader, 0);
};

#endif

