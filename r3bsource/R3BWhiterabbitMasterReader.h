#ifndef R3BWhiterabbitMasterReader_H
#define R3BWhiterabbitMasterReader_H
#include "R3BReader.h"

struct EXT_STR_h101_WRMASTER_t;
typedef struct EXT_STR_h101_WRMASTER_t EXT_STR_h101_WRMASTER;

class FairLogger;
class TClonesArray;
class R3BEventHeader;

class R3BWhiterabbitMasterReader : public R3BReader
{
	public:
		R3BWhiterabbitMasterReader(EXT_STR_h101_WRMASTER *, UInt_t,
		    UInt_t);
		~R3BWhiterabbitMasterReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

                /** Accessor to select online mode **/
                void SetOnline(Bool_t option){fOnline=option;} 

	private:
		/* An event counter */
		UInt_t fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_WRMASTER* fData;
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
		ClassDef(R3BWhiterabbitMasterReader, 0);
};
#endif
