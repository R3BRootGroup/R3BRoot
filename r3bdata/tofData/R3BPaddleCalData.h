#ifndef R3BPADDLECALITEM_H
#define R3BPADDLECALITEM_H

#include "TObject.h"

// for the data analysis of the paddle-type detectors. 
// Introduced by Ralf, Apr 2016

class R3BPaddleCalData : public TObject
{
  public:
    // Default Constructor
    R3BPaddleCalData();

    // Standard Constructor
    R3BPaddleCalData(UInt_t plane, UInt_t bar); //, Double_t r,Double_t t,Double_t l,Double_t b,Double_t tref);

    // Destructor
    virtual ~R3BPaddleCalData() { }

    // Getters
    //inline const UChar_t&  GetDetector()  const { return fDetector; }
    UInt_t GetPlane() const {return fPlane;}
    UInt_t GetBar() const {return fBar;}

  private:
    UInt_t fPlane;
    UInt_t fBar;

  public:
    Double_t fTime1L_ns; // both PMs of a paddle
    Double_t fTime1T_ns; // both PMs of a paddle
    Double_t fTime2L_ns;
    Double_t fTime2T_ns;

    ClassDef(R3BPaddleCalData, 2)
};

#endif


