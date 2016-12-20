// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BFi4Cal2Hit                          -----
// -----		   created 09-03-2016 by J.Tscheuschner		   -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
/* Convert Cal data to Hit data
 */

#ifndef R3BFI4CAL2HIT_H
#define R3BFI4CAL2HIT_H

#include "FairTask.h"

class TClonesArray;
class R3BEventHeader;

class R3BFi4Cal2Hit : public FairTask
{
  public:
    R3BFi4Cal2Hit();
    R3BFi4Cal2Hit(const char* name, Int_t iVerbose);
    virtual ~R3BFi4Cal2Hit();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();
    virtual void FinishTask();

    void SetGeometry  (TString file=""){fGeofile=file;};
    void SetNDet(UInt_t ndet=1){fNDet=ndet;};

  private:

    void ReadGeometry();
    Float_t ZPos(UInt_t fiber, UInt_t det_type);
    Float_t YPos(UInt_t fiber, UInt_t det_type);
    Float_t XPos(UInt_t fiber, UInt_t det_type);
    R3BEventHeader *fHeader;    // do we need that?
    TClonesArray* fCalItems; // holding input data
    TClonesArray* fHitItems;    // holding output datax

    UInt_t fNDet;//number of used det for testing set to 2

    TString fGeofile;
    TString fFiberWidthfile;
    Float_t fFiber_width;        // fiber-thickness in mm
    vector<Float_t> fAngle;              // tilting of the detectors to the x-axis
    vector<vector<Float_t>> fRevPoint;   // referencepoint in space to other detectors

    //    const Float_t fReverencepoint[3]={0.,0.,0.};//reverence-point in space x,y,z in mm
    // void CreateHistos();
    // void WriteHistos();
  public:
    ClassDef(R3BFi4Cal2Hit, 1)
};

#endif
