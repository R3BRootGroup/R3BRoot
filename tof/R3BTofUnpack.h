
#ifndef _R3BTOF_UNPACK_
#define _R3BTOF_UNPACK_

#include <map>

#include "FairUnpack.h"


class TH1F;


class R3BTofUnpack : public FairUnpack {

public:
    R3BTofUnpack(Short_t type, Short_t subType,
                 Short_t procId, Short_t subCrate, Short_t control);
    virtual ~R3BTofUnpack();
    
    virtual Bool_t Init();
    virtual Bool_t DoUnpack(Int_t *data, Int_t size);
    virtual void Reset();
    
    
private:
    std::map<Int_t, Int_t> fmap_leading_t;
    std::map<Int_t, Int_t> fmap_leading_c;
    std::map<Int_t, Int_t> fmap_trailing_t;
    std::map<Int_t, Int_t> fmap_trailing_c;

    
protected:
    virtual void Register();

    
public:
    ClassDef(R3BTofUnpack, 1)

};



#endif // _R3BTOF_UNPACK_