#ifndef R3BLOSCALITEM_H
#define R3BLOSCALITEM_H

#include "TObject.h"

// for the data analysis of the LOS detectors. 
// Introduced by Ralf, FEB 2016
// September 2016 - TAMEX readout of MCFD amplitudes added by Aleksandra.
class R3BLosCalData : public TObject
{
  public:
    // Default Constructor
    R3BLosCalData();

    // Standard Constructor
    R3BLosCalData(UInt_t); 

    // Destructor
    virtual ~R3BLosCalData() { }

    UInt_t  GetDetector()  const;
    UInt_t GetVFTXNcha();
    UInt_t GetTAMEXLNcha();
    UInt_t GetTAMEXTNcha();
     
    Double_t GetTimeV_ns(UInt_t)     const;
    Double_t GetTimeL_ns(UInt_t)     const;
    Double_t GetTimeT_ns(UInt_t)     const;

    Double_t GetMeanTime();
    Double_t GetMeanTimeTAMEXL();
    Double_t GetMeanTimeTAMEXT();
    Double_t GetMeanTimeVFTX();

   
  private:
    UInt_t  fDetector;  
    void Reset();
  public:
    Double_t fTimeV_ns[8];   // VFTX
    Double_t fTimeL_ns[8];   // TAMEX leading
    Double_t fTimeT_ns[8];   // TAMEX trailing
    

    ClassDef(R3BLosCalData, 1) 
};

#endif


