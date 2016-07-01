#ifndef R3BPSPXREADER_H
#define R3BPSPXREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_PSP_t;
typedef struct EXT_STR_h101_PSP_t EXT_STR_h101_PSP;
class FairLogger;
class R3BPspxMappedPar;

class R3BPspxReader : public R3BReader
{
	public:
		R3BPspxReader(EXT_STR_h101_PSP *, UInt_t);
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
		EXT_STR_h101_PSP* fData;
		/* Data Offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BPspxMappedItem */
		TClonesArray* fArray; /**< Output array. */

		R3BPspxMappedPar* fMappedPar;

	public:
		ClassDef(R3BPspxReader, 2);
};

#endif

