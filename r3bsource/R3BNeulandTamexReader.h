#ifndef R3BNEULANDTAMEXREADER_H
#define R3BNEULANDTAMEXREADER_H

#include "R3BReader.h"
class TClonesArray;
class FairLogger;

typedef struct EXT_STR_h101_t EXT_STR_h101;

class R3BNeulandTamexReader : public R3BReader
{
	public:
		R3BNeulandTamexReader(EXT_STR_h101 *);
		~R3BNeulandTamexReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* An event counter */
		unsigned int fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101* fData;
		/* FairLogger */
		FairLogger*	fLogger;
        TClonesArray* fArray; /**< Output array. */

	public:
		ClassDef(R3BNeulandTamexReader, 0);
};

#endif

