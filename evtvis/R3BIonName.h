// -------------------------------------------------------------------------
// -----                   R3BIonName header file                      -----
// -----              Created 04/10/2011  by P.Cabanelas               -----
// -------------------------------------------------------------------------

#ifndef R3BIONNAME_H
#define R3BIONNAME_H

#include "TNamed.h"

class R3BIonName: public TNamed
{
  public:

    /** Default constructor **/
    R3BIonName();


    /** Destructor **/
    virtual ~R3BIonName();


    /** Accessors **/
    /**
    * Return the ion name if exists
    */
    Bool_t GetIonName(Int_t element, Char_t *name);

  private:

    /** Data members **/

    ClassDef(R3BIonName,1);

};

#endif
