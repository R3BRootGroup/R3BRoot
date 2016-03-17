#ifndef _R3BREADER_H
#define _R3BREADER_H

#include "TObject.h"

extern "C" {
#include "ext_data_struct_info.hh"

}

//#define EXP_SPECIFIC_H101_FILE "jun16_ext_h101.h"
#define EXP_SPECIFIC_H101_FILE "ext_h101.h"

typedef struct EXT_STR_h101_t EXT_STR_h101;

class R3BReader : public TObject
{
	public:
		R3BReader(const char *);
		virtual ~R3BReader();

		/* Setup structure information */
		virtual Bool_t Init(ext_data_struct_info *) = 0;
        virtual void SetParContainers() {}
        virtual Bool_t ReInit() { return kTRUE; }
		/* Read data from full event structure */
		virtual Bool_t Read() = 0;
		/* Reset */
		virtual void Reset() = 0;
		/* Return actual name of the reader */
		const char *GetName() { return fName; }

	protected:
		const char *fName;

	public:
		ClassDef(R3BReader, 0)
};

#endif
