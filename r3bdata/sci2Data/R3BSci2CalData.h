#ifndef R3BSCI2CALITEM_H
#define R3BSCI2CALITEM_H

#include "TObject.h"

// for the data analysis of the Sci2 detectors.

class R3BSci2CalData : public TObject
{
  public:
    // Default Constructor
    R3BSci2CalData();

    // Standard Constructor
    R3BSci2CalData(UInt_t detector);

    // Destructor
    virtual ~R3BSci2CalData() {}

    // Getters
    inline const UInt_t& GetDetector() const { return fDetector; }

    Double_t GetMeanTime();
    Double_t GetMeanTimeTAMEXL();
    Double_t GetMeanTimeTAMEXT();
    Double_t GetMeanTimeVFTX();
    UInt_t GetVFTXNcha();
    UInt_t GetTAMEXLNcha();
    UInt_t GetTAMEXTNcha();

  private:
    UInt_t fDetector;

  public:
    Double_t fTimeV_r_ns; // right VFTX
    Double_t fTimeV_l_ns; // left VFTX
    Double_t fTimeL_r_ns; // right TAMEX leading
    Double_t fTimeL_l_ns; // left TAMEX leading
    Double_t fTimeT_r_ns; // right  TAMEX trailing
    Double_t fTimeT_l_ns; // left  TAMEX trailing

    ClassDef(R3BSci2CalData, 1)
};

#endif
