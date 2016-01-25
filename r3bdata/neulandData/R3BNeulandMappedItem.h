
#ifndef R3BNEULANDMAPPEDITEM_H
#define R3BNEULANDMAPPEDITEM_H

#include "R3BNeulandRawItem.h"

class R3BNeulandMappedItem : public R3BNeulandRawItem
{
  public:
    // Default Constructor
    R3BNeulandMappedItem();

    /** Standard Constructor
     **/
    R3BNeulandMappedItem(Int_t sam,
                         Int_t gtb,
                         Int_t tacAddr,
                         Int_t cal,
                         Int_t clock,
                         Int_t tacData,
                         Int_t qdcData,
                         Int_t barId,
                         Int_t side,
                         Bool_t is17);

    // Destructor
    virtual ~R3BNeulandMappedItem()
    {
    }

    // Getters
    inline const Int_t& GetBarId() const
    {
        return fBarId;
    }
    inline const Int_t& GetSide() const
    {
        return fSide;
    }
    inline const Bool_t& Is17() const
    {
        return fIs17;
    }

  private:
    Int_t fBarId; //... detector ID
    Int_t fSide;  //... PMT
    Bool_t fIs17; //... true if CH 17

  public:
    ClassDef(R3BNeulandMappedItem, 2)
};

#endif
