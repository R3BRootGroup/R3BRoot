#ifndef R3BLANDDIGITIZER_H
#define R3BLANDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;
class TH1F;


class R3BLandDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BLandDigitizer();


  /** Destructor **/
  ~R3BLandDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();

  protected:
  TClonesArray* fLandPoints;

  //- Table data 
  Double_t x_pos[200],y_pos[200],z_pos[200];
  Double_t tof[200],pm[200][2];
  Double_t paddle_E[200][9];
  Double_t part_E[50],part_n[50],first_hit[4];
  Int_t nuhits[200];


  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BLandDigitizer,1);
  
};

#endif
