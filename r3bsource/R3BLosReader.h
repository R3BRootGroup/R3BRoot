#ifndef R3BLOSREADER_H
#define R3BLOSREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_LOS_t;
typedef struct EXT_STR_h101_LOS_t EXT_STR_h101_LOS;
class FairLogger;

class R3BLosReader : public R3BReader
{
	public:
		R3BLosReader(EXT_STR_h101_LOS *, UInt_t);
		~R3BLosReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_LOS* fData;
		/* Data offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BLosxMappedItem */
		TClonesArray* fArray; /**< Output array. */


	public:
		ClassDef(R3BLosReader, 0);
};

#endif

