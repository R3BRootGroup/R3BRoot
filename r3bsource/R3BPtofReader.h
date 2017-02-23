#ifndef R3BPTOFREADER_H
#define R3BPTOFREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_PTOF_t;
typedef struct EXT_STR_h101_PTOF_t EXT_STR_h101_PTOF;
class FairLogger;

class R3BPtofReader : public R3BReader
{
	public:
		R3BPtofReader(EXT_STR_h101_PTOF *, UInt_t);
		~R3BPtofReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_PTOF* fData;
		/* Data offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BPtofxMappedItem */
		TClonesArray* fArray; /**< Output array. */


	public:
		ClassDef(R3BPtofReader, 0);
};

#endif

