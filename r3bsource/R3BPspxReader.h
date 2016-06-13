#ifndef R3BPSPXREADER_H
#define R3BPSPXREADER_H

#include "R3BReader.h"

typedef struct EXT_STR_h101_t EXT_STR_h101;
class FairLogger;
class R3BPspxMappedPar;

class R3BPspxReader : public R3BReader
{
	public:
		R3BPspxReader(EXT_STR_h101 *);
		~R3BPspxReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t ReInit();
		void SetParContainers();
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

		R3BPspxMappedPar* fMappedPar;

	public:
		ClassDef(R3BPspxReader, 2);
};

#endif

