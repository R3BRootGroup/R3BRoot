#ifndef R3BLANDTCALPAR_H
#define R3BLANDTCALPAR_H

#include "FairParGenericSet.h"          // for FairParGenericSet

#include "Rtypes.h"                     // for Double_t, Int_t, UInt_t, etc

#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string

#include "TObjArray.h"                  // Store for vertices 
#include "TVector3.h"                   // Vertex
#include "TGeoArb8.h"                   // Trap

#define NCHMAX 500

using namespace std;

class FairParamList;



class R3BLandTCalPar : public FairParGenericSet
{

    using TObject::Compare;
    
  public :
  R3BLandTCalPar (const char* name="R3BLandTCalPar",
				 const char* title="Land Calibrated Time Parameter",
				 const char* context="TestDefaultContext",
				 Bool_t own=kTRUE);
    virtual ~R3BLandTCalPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    /** Accessor functions **/
    Int_t GetComboNo()       const {return fCompId;}
    Int_t GetCompId()   const {return fCompId;}
    Int_t GetBarId() const {return fBarId;}
    Int_t GetSide() const {return fSide;}
    Int_t GetNofChannels() const { return fNofChannels; }
    Double_t GetTimeAt(Int_t i) const {return fTime[i];}
    Int_t    GetBinLowAt(Int_t i) const {return fBinLow[i];}
    Int_t    GetBinUpAt(Int_t i) const {return fBinUp[i];}

    void SetCompId(Int_t x)  {SetComboNo(x); fCompId=x;}
    void SetBarId(Int_t i) {fBarId=i;}
    void SetSide(Int_t i) {fSide=i;}
    void IncrementNofChannels() { fNofChannels += 1; }
    void SetBinLowAt(Int_t ch,Int_t i) {fBinLow[i]=ch;}
	void SetBinUpAt(Int_t ch,Int_t i) {fBinUp[i]=ch;}
    void SetTimeAt(Double_t t,Int_t i) {fTime[i]= t;}

    UInt_t GetIndex(UInt_t /*def*/) const { return fBarId; }

    Bool_t Compare(const R3BLandTCalPar& that ) const; 

  private:
    // Data Parameters
    Int_t fCompId;
    Int_t fBarId; 
    Int_t fSide;
    Int_t fNofChannels;
    Int_t fBinLow[NCHMAX];// lower channel
    Int_t fBinUp[NCHMAX];// upper channel
    Double_t fTime[NCHMAX];// Cal. Time

    ClassDef(R3BLandTCalPar,1); // R3BLandTCalPar Parameter Container example
};


#endif /* !R3BLANDTCALPAR_H*/
