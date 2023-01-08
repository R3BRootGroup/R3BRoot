/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ----------------------------------------------------------------
// -----              R3BStartrackMappedData                    -----
// -----          (Replacing   R3BStartrackckerDigitHit)        -----
// -----             Created 26-05-2017 by M.Labiche          -----
// ----------------------------------------------------------------
#ifndef R3BSTARTRACKMAPPEDITEM_H
#define R3BSTARTRACKMAPPEDITEM_H

#include "TObject.h"

/* if no problem with ucesb

class R3BStartrackMappedData : public TObject
{
  public:
    // Default Constructor
    R3BStartrackMappedData();

    // Standard Constructor

    R3BStartrackMappedData( Int_t ladderId,
                              Int_t side,
                              Int_t asicId,
                              Int_t stripId,
                              Double_t edep,
                              Double_t timestp);
    R3BStartrackMappedData(const R3BStartrackMappedData&);

    // Destructor
    virtual ~R3BStartrackMappedData()
    {
    }

    // Getters
    inline const Int_t& GetLadderId() const
    {
        return fLadder;
    }
    inline const Int_t& GetAsicId() const
    {
        return fAsic;
    }
    inline const Int_t& GetSide() const
    {
        return fSide;
    }
    inline const Int_t& GetStripId() const
    {
        return fStrip;
    }
     inline const Double_t& GetEdep() const
    {
        return fEdep;
    }
    inline const Double_t& GetTimeStp() const
    {
        return fTimeStp;
    }


  protected:
    Int_t fLadder;     //... number of ladder 1..n
    Int_t fSide;       //... Ladder Side
    Int_t fAsic;       //... Asic Id: 1 to 12(inner layer) or 16(outer layer)
    Int_t fStrip;      //... Strip Id: 1 to 128
    Double_t fEdep;       //... Energy deposited in the ladder
    Double_t fTimeStp;  //... Time stamp information


  public:
    ClassDef(R3BStartrackMappedData, 1)
};

*/

class R3BStartrackMappedData : public TObject
{
  public:
    // Default Constructor
    R3BStartrackMappedData();

    // Standard Constructor

    R3BStartrackMappedData(Int_t data);
    R3BStartrackMappedData(const R3BStartrackMappedData&); // read only 1 32 bit word at time
    R3BStartrackMappedData(const R3BStartrackMappedData&,
                           const R3BStartrackMappedData&); // read 2 32-bit words each time

    // Destructor
    virtual ~R3BStartrackMappedData() {}

    // Getters
    inline const Int_t& GetData() const { return fData; }
    inline const Int_t& GetData2() const { return fData2; }

  protected:
    Int_t fData;  //... number of ladder 1..n
    Int_t fData2; //... number of ladder 1..n

  public:
    ClassDef(R3BStartrackMappedData, 1)
};
#endif
