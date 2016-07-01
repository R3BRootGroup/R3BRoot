#ifndef R3BNEULANDTAMEXREADER_H
#define R3BNEULANDTAMEXREADER_H

#include "R3BReader.h"
class TClonesArray;
class FairLogger;

struct EXT_STR_h101_raw_nnp_tamex_t;
typedef struct EXT_STR_h101_raw_nnp_tamex_t EXT_STR_h101_raw_nnp_tamex;

class R3BNeulandTamexReader : public R3BReader
{
	public:
		R3BNeulandTamexReader(EXT_STR_h101_raw_nnp_tamex *, UInt_t);
		~R3BNeulandTamexReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* An event counter */
		unsigned int fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_raw_nnp_tamex* fData;
		/* Data offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
	        TClonesArray* fArray; /**< Output array. */

	public:
		ClassDef(R3BNeulandTamexReader, 0);
};

#endif

