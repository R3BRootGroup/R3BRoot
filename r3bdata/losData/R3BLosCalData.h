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
    R3BLosCalData(UInt_t detector); //, Double_t r,Double_t t,Double_t l,Double_t b,Double_t tref);

    // Destructor
    virtual ~R3BLosCalData() { }

    // Getters
    inline const UInt_t&  GetDetector()  const { return fDetector; }
//    inline const UInt_t&  GetVFTXNcha()  const { return fVFTXNcha; }
//    inline const UInt_t&  GetTAMEXNcha()  const { return fTAMEXNcha; }
    
    Double_t GetMeanTime();
    Double_t GetMeanTimeTAMEXL();
    Double_t GetMeanTimeTAMEXT();
    Double_t GetMeanTimeVFTX();
    UInt_t GetVFTXNcha();
    UInt_t GetTAMEXLNcha();
    UInt_t GetTAMEXTNcha();
    
  private:
    UInt_t  fDetector;

  public:
    Double_t fTimeV_r_ns;   // right VFTX
    Double_t fTimeV_t_ns;   // top VFTX
    Double_t fTimeV_l_ns;   // left VFTX
    Double_t fTimeV_b_ns;   // bottom VFTX
    Double_t fTimeL_r_ns;   // right TAMEX leading
    Double_t fTimeL_t_ns;   // top TAMEX leading
    Double_t fTimeL_l_ns;   // left TAMEX leading
    Double_t fTimeL_b_ns;   // bottom  TAMEX leading
    Double_t fTimeT_r_ns;   // right  TAMEX trailing
    Double_t fTimeT_t_ns;   // top  TAMEX trailing
    Double_t fTimeT_l_ns;   // left  TAMEX trailing
    Double_t fTimeT_b_ns;   // bottom  TAMEX trailing         
    Double_t fTimeV_rt_ns;   // right-top
    Double_t fTimeV_rb_ns;   // right-bottom
    Double_t fTimeV_lt_ns;   // left-top
    Double_t fTimeV_lb_ns;   // left-bottom
    Double_t fTimeL_rt_ns;   // right-top
    Double_t fTimeL_rb_ns;   // right-bottom
    Double_t fTimeL_lt_ns;   // left-top
    Double_t fTimeL_lb_ns;   // left-bottom
    Double_t fTimeT_rt_ns;   // right-top
    Double_t fTimeT_rb_ns;   // right-bottom
    Double_t fTimeT_lt_ns;   // left-top
    Double_t fTimeT_lb_ns;   // left-bottom    
    Double_t fTimeV_ref_ns;   // reference VFTX
    Double_t fTimeL_ref_ns;   // reference TAMEX leading  
    Double_t fTimeT_ref_ns;   // reference  TAMEX trailing
    Double_t fTimeV_pix_ns;   // pixel VFTX
    Double_t fTimeL_pix_ns;   // pixel TAMEX leading  
    Double_t fTimeT_pix_ns;   // pixel  TAMEX trailing
      
    // Pairs are: r & l; t & b; rt & lb; rb & lt  


    ClassDef(R3BLosCalData, 1) //3
};

#endif


