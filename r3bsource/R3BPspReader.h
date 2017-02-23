#ifndef R3BPSPREADER_H
#define R3BPSPREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_PSP;
class FairLogger;

class R3BPspReader : public R3BReader
{
	public:
		R3BPspReader(EXT_STR_h101_PSP *, UInt_t);
		~R3BPspReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* An event counter */
		//unsigned int fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_PSP* fData;
		/* Offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BPspxMappedItem */
		TClonesArray* fArray; /**< Output array. */


	public:
		ClassDef(R3BPspReader, 0);
};

#endif

