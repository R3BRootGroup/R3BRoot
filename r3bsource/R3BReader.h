#ifndef _R3BREADER_H
#define _R3BREADER_H

#include "TString.h"
#include "TObject.h"

extern "C" {
#include "ext_data_struct_info.hh"
}

//#define EXP_SPECIFIC_H101_FILE "jun16_ext_h101.h"
//#define EXP_SPECIFIC_H101_FILE "ext_h101.h"

class R3BReader : public TObject
{
	public:
		R3BReader(TString const &);
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
		const char *GetName() { return fName.Data(); }

	protected:
		TString fName;

	public:
		ClassDef(R3BReader, 0);
};

#endif
