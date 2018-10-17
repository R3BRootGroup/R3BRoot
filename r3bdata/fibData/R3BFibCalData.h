#ifndef R3BFIBCALITEM_H
#define R3BFIBCALITEM_H

#include "TObject.h"

// for the data analysis of fiber detectors. 
// Introduced by M. Heil, Jan 2018

class R3BFibCalData : public TObject
{
  public:
    // Default Constructor
    R3BFibCalData();

    // Standard Constructor
    R3BFibCalData(UInt_t plane, UInt_t fiber); //, Double_t r,Double_t t,Double_t l,Double_t b,Double_t tref);

    // Destructor
    virtual ~R3BFibCalData() { }

    // Getters
    //inline const UChar_t&  GetDetector()  const { return fDetector; }
    UInt_t GetPlane() const {return fPlane;}
    UInt_t GetFiber() const {return fFiber;}
    
    // tube: 0,1 edge: 0,1 time: in ns
    void SetTime(int tube, int edge,Double_t t)
    {
		if (tube==0) 
		{
			if (edge==0) fTime1L_ns=t;
			else      	 fTime1T_ns=t;
		}
		else
		{
			if (edge==0) fTime2L_ns=t;
			else      	 fTime2T_ns=t;
		}
	}

  private:
    UInt_t fPlane;
    UInt_t fFiber;

  public:
	// ToDo: Would it be better to have arrays here?
	Double_t fTime1L_ns; // both PMs of a fiber
	Double_t fTime1T_ns; // both PMs of a fiber
	Double_t fTime2L_ns;
	Double_t fTime2T_ns;

	ClassDef(R3BFibCalData, 1)
};

#endif


