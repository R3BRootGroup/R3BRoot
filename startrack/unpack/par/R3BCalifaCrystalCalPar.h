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

#ifndef R3BCALIFACRYSTALCALPAR1_H
#define R3BCALIFACRYSTALCALPAR1_H

#include "FairParGenericSet.h" // for FairParGenericSet

#include "Rtypes.h" // for Double_t, Int_t, UInt_t, etc
#include "TGeoMaterial.h"
#include "TObjArray.h"
#include <iostream> // for operator<<, basic_ostream, etc
#include <string>   // for string

#include "R3BCalifaDUCalPar.h"

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;

class R3BCalifaCrystalCalPar : public FairDbObjTableMap
{

  public:
    R3BCalifaCrystalCalPar(const char* name = "R3BCalifaCrystalCalPar",
                           const char* title = "Califa Calibration Parameter",
                           const char* context = "TestDefaultContext",
                           Bool_t own = kTRUE);
    virtual ~R3BCalifaCrystalCalPar(void);

    void clear(void);
    void putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);
    void Print();
    void ReadFile(string file);

    // Lists handling
    void AddDUCalPar(R3BCalifaDUCalPar* tch) { fDUCalParams->Add(tch); }
    TObjArray* GetListOfDUCalPar(Int_t side) { return fDUCalParams; }
    Int_t GetNumDUCalPar() { return fDUCalParams->GetEntries(); }
    R3BCalifaDUCalPar* GetDUCalParAt(Int_t idx) { return (R3BCalifaDUCalPar*)fDUCalParams->At(idx); }

    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0)
    {
        std::string rr = "";
        return rr;
    }

    virtual FairDbObjTableMap* CreateObjTableMap() const { return new R3BCalifaCrystalCalPar(); }

    // Atomic IO (intrinsic)
    virtual void Fill(FairDbResultPool& res_in, const FairDbValRecord* valrec) { ; }
    virtual void Store(FairDbOutTableBuffer& res_out, const FairDbValRecord* valrec) const { ; }

    // Global IO using run_id
    virtual void fill(UInt_t rid);
    virtual void store(UInt_t rid);

    virtual ValCondition GetContext(UInt_t rid)
    {
        return ValCondition(FairDbDetector::kCal, DataType::kData, ValTimeStamp(rid));
    }

  private:
    TObjArray* fDUCalParams;

    ClassDef(R3BCalifaCrystalCalPar, 1); // R3BCalifaCrystalCalPar Parameter Container example
};

#endif /* !R3BCALIFACRYSTALCALPAR_H*/
