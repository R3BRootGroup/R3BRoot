#ifndef R3BTCALMODULEPAR_H
#define R3BTCALMODULEPAR_H

#include "FairParGenericSet.h" // for FairParGenericSet

#define NCHMAX 5000

class FairParamList;

class R3BTCalModulePar : public FairParGenericSet
{

    using TObject::Compare;

  public:
    R3BTCalModulePar(const char* name = "TCalModulePar", const char* title = "TCAL calibration of a module", const char* context = "TestDefaultContext", Bool_t own = kTRUE);
    virtual ~R3BTCalModulePar(void);
    void clear(void);
    void putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);
    void printParams();

    /** Accessor functions **/
    Int_t GetModuleId() const
    {
        return fModuleId;
    }
    Int_t GetSide() const
    {
        return fSide;
    }
    Int_t GetNofChannels() const
    {
        return fNofChannels;
    }
    Double_t GetTimeAt(Int_t i) const
    {
        return fTime[i];
    }
    Int_t GetBinLowAt(Int_t i) const
    {
        return fBinLow[i];
    }
    Int_t GetBinUpAt(Int_t i) const
    {
        return fBinUp[i];
    }

    void SetModuleId(Int_t i)
    {
        fModuleId = i;
    }
    void SetSide(Int_t i)
    {
        fSide = i;
    }
    void IncrementNofChannels()
    {
        fNofChannels += 1;
    }
    void SetBinLowAt(Int_t ch, Int_t i)
    {
        fBinLow[i] = ch;
    }
    void SetBinUpAt(Int_t ch, Int_t i)
    {
        fBinUp[i] = ch;
    }
    void SetTimeAt(Double_t t, Int_t i)
    {
        fTime[i] = t;
    }

    Bool_t Compare(const R3BTCalModulePar& that) const;

  private:
    // Data Parameters
    Int_t fModuleId;
    Int_t fSide;
    Int_t fNofChannels;
    Int_t fBinLow[NCHMAX];  // lower channel
    Int_t fBinUp[NCHMAX];   // upper channel
    Double_t fTime[NCHMAX]; // Cal. Time

    ClassDef(R3BTCalModulePar, 1); // R3BTCalModulePar Parameter Container example
};

#endif /* !R3BTCALMODULEPAR_H*/
