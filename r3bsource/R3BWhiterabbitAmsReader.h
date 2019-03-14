#ifndef R3BWhiterabbitAmsReader_H
#define R3BWhiterabbitAmsReader_H
#include "R3BReader.h"

struct EXT_STR_h101_WRAMS_t;
typedef struct EXT_STR_h101_WRAMS_t EXT_STR_h101_WRAMS;

class FairLogger;
class TClonesArray;
class R3BEventHeader;

/**
 * A reader of AMS white rabbit data with UCESB.
 * Receives mapped raw data and converts it to R3BRoot objects.
 * @author J.L. Rodriguez
 * @since Feb 28, 2019
 */

class R3BWhiterabbitAmsReader : public R3BReader
{
	public:
		R3BWhiterabbitAmsReader(EXT_STR_h101_WRAMS *, UInt_t,
		    UInt_t);
		~R3BWhiterabbitAmsReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

                /** Accessor to select online mode **/
                void SetOnline(Bool_t option){fOnline=option;} 

	private:
		/* An event counter */
		UInt_t fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_WRAMS* fData;
		/* Offset of detector specific data in full data structure */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger* fLogger;
		/* The whiterabbit subsystem ID */
		UInt_t fWhiterabbitId;
		/* A pointer to the R3BEventHeader structure */
		R3BEventHeader* fEventHeader;
                //Don't store data for online
                Bool_t fOnline;
                /**< Output array. */
	        TClonesArray* fArray; 

	public:
		ClassDef(R3BWhiterabbitAmsReader, 0);
};
#endif
