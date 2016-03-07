// ----------------------------------------------------------------
// -----              R3BNeulandTamexMappedItem               -----
// -----             Created 27-01-2015 by M.Heil             -----
// ----------------------------------------------------------------

#ifndef R3BNEULANDTAMEXMAPPEDITEM_H
#define R3BNEULANDTAMEXMAPPEDITEM_H

#include "TObject.h"

class R3BNeulandTamexMappedItem : public TObject
{
  public:
    // Default Constructor
    R3BNeulandTamexMappedItem();

    /** Standard Constructor
     **/
    R3BNeulandTamexMappedItem(Int_t planeId,
                              Int_t barId,
                              Int_t side,
                              Int_t coarseTimeLE,
                              Int_t fineTimeLE,
                              Int_t coarseTimeTE,
                              Int_t fineTimeTE,
                              Bool_t is17);
    R3BNeulandTamexMappedItem(const R3BNeulandTamexMappedItem&);
   
    // Destructor
    virtual ~R3BNeulandTamexMappedItem()
    {
    }

    // Getters
    inline const Int_t& GetPlaneId() const
    {
        return fPlaneId;
    }
    inline const Int_t& GetBarId() const
    {
        return fBarId;
    }
    inline const Int_t& GetSide() const
    {
        return fSide;
    }
    inline const Int_t& GetCoarseTimeLE() const
    {
        return fCoarseTimeLE;
    }
    inline const Int_t& GetFineTimeLE() const
    {
        return fFineTimeLE;
    }
    inline const Int_t& GetCoarseTimeTE() const
    {
        return fCoarseTimeTE;
    }   
    inline const Int_t& GetFineTimeTE() const
    {
        return fFineTimeTE;
    }
    inline const Bool_t& Is17() const
    {
        return fIs17;
    }

  protected:
    Int_t fPlaneId; //... number of plane 
    Int_t fBarId; //... number of bar
    Int_t fSide;  //... PMT 
    Int_t fCoarseTimeLE;  //... coarse time of leading edge
    Int_t fFineTimeLE;  //... fine time of leading edge
    Int_t fCoarseTimeTE;  //... coarse time of trailing edge
    Int_t fFineTimeTE;  //... fine time of trailing edge    
    Bool_t fIs17; //... true if CH 17

  public:
    ClassDef(R3BNeulandTamexMappedItem, 1)
};

#endif

