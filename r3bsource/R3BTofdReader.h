#ifndef R3BTOFDREADER_H
#define R3BTOFDREADER_H

#include "R3BReader.h"

typedef struct EXT_STR_h101_t EXT_STR_h101;
class FairLogger;

class R3BTofdReader : public R3BReader
{
	public:
		R3BTofdReader(EXT_STR_h101 *);
		~R3BTofdReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* Reader specific data structure from ucesb */
		EXT_STR_h101* fData;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BTofdxMappedItem */
		TClonesArray* fArray; /**< Output array. */


	public:
		ClassDef(R3BTofdReader, 0);
};

#endif

