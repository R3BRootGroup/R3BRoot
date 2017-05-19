// ----------------------------------------------------------------
// -----              R3BPaddleTamexMappedData                -----
// -----             Created August 2nd 2016 by R.Plag        -----
// ----------------------------------------------------------------


#ifndef R3BPADDLETAMEXMAPPEDITEM_H
#define R3BPADDLETAMEXMAPPEDITEM_H

#include "TObject.h"

class R3BPaddleTamexMappedData : public TObject
{
  public:
    /* Default Constructor */
    R3BPaddleTamexMappedData();

    /* Standard Constructor */
    R3BPaddleTamexMappedData(Int_t planeId, Int_t barId);

    // Destructor
    virtual ~R3BPaddleTamexMappedData()
    {
    }

    // Getters
    inline const Int_t& GetPlaneId() const
    {
        return fPlane;
    }
    inline const Int_t& GetBarId() const
    {
        return fBar;
    }


    inline const Int_t& GetCoarseTime1LE() const
    {
        return fCoarseTime1LE;
    }
    inline const Int_t& GetFineTime1LE() const
    {
        return fFineTime1LE;
    }
    inline const Int_t& GetCoarseTime1TE() const
    {
        return fCoarseTime1TE;
    }
    inline const Int_t& GetFineTime1TE() const
    {
        return fFineTime1TE;
    }


    inline const Int_t& GetCoarseTime2LE() const
    {
        return fCoarseTime2LE;
    }
    inline const Int_t& GetFineTime2LE() const
    {
        return fFineTime2LE;
    }
    inline const Int_t& GetCoarseTime2TE() const
    {
        return fCoarseTime2TE;
    }
    inline const Int_t& GetFineTime2TE() const
    {
        return fFineTime2TE;
    }


    inline const Int_t& GetFineTime(int t,int e) const
    {
        return t ? (e ? fFineTime2TE : fFineTime2LE) :
	    (e ? fFineTime1TE : fFineTime1LE);
    }

    inline const Int_t& GetCoarseTime(int t,int e) const
    {
        return t ? (e ? fCoarseTime2TE : fCoarseTime2LE) :
	    (e ? fCoarseTime1TE : fCoarseTime1LE);
    }

    // To be compatible with Tacquila data:
    inline Bool_t Is17() const
    {
	    return false;
    }

  protected:
    Int_t fPlane; //... number of plane 1..n
    Int_t fBar;   //... number of bar   1..n

  public:
    // PM1:
    Int_t fCoarseTime1LE;  //... coarse time of leading edge
    Int_t fFineTime1LE;    //... fine time of leading edge
    Int_t fCoarseTime1TE;  //... coarse time of trailing edge
    Int_t fFineTime1TE;    //... fine time of trailing edge
    // PM2:
    Int_t fCoarseTime2LE;  //... coarse time of leading edge
    Int_t fFineTime2LE;    //... fine time of leading edge
    Int_t fCoarseTime2TE;  //... coarse time of trailing edge
    Int_t fFineTime2TE;    //... fine time of trailing edge


  public:
    ClassDef(R3BPaddleTamexMappedData, 1)
};

#endif

