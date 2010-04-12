// -------------------------------------------------------------------------
// -----                      R3BDchDigi header file                  -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------


/**  R3BDchDigi.h
 **/


#ifndef R3BDCHDIGI_H
#define R3BDCHDDIGI_H


#include "TObject.h"
#include "TVector3.h"



class R3BDchDigi : public TObject
{

 public:

  /** Default constructor **/
  R3BDchDigi();
  /** Copy constructor **/
  R3BDchDigi(const R3BDchDigi& point) { *this = point; };

  /** Destructor **/
  virtual ~R3BDchDigi();

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:

  ClassDef(R3BDchDigi,1)

};

#endif
