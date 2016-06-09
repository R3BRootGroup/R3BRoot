#ifndef R3BFIBERREADER_H
#define R3BFIBERREADER_H

#include "R3BReader.h"

typedef struct EXT_STR_h101_t EXT_STR_h101;
class FairLogger;

class R3BFiberReader : public R3BReader
{
	public:
		R3BFiberReader(EXT_STR_h101 *);
		~R3BFiberReader();

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
		/* the structs of type R3BFi4MappedItem */
		TClonesArray* fArray; /**< Output array. */


	public:
		ClassDef(R3BFiberReader, 0);
};

#endif

