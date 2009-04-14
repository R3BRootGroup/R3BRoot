#ifndef R3BDETPOINT_H
#define R3BDETPOINT_H 1


#include "FairMCPoint.h"

#include "TObject.h"
#include "TVector3.h"

class R3BDetPoint : public FairMCPoint
{

 public:

  /** Default constructor **/
  R3BDetPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  R3BDetPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
		      Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  R3BDetPoint(const R3BDetPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~R3BDetPoint();

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


  ClassDef(R3BDetPoint,1)

};

#endif
