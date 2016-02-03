#ifndef R3BPSPREADER_H
#define R3BPSPREADER_H

#include "R3BReader.h"

typedef struct EXT_STR_h101_t EXT_STR_h101;
class FairLogger;

class R3BPspReader : public R3BReader
{
	public:
		R3BPspReader(EXT_STR_h101 *);
		~R3BPspReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* An event counter */
		//unsigned int fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101* fData;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BPspxMappedItem */
		TClonesArray* fArray; /**< Output array. */


	public:
		ClassDef(R3BPspReader, 0);
};

#endif

