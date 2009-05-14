// -------------------------------------------------------------------------
// -----                    CbmFieldMapData header file                -----
// -------------------------------------------------------------------------


/** R3BFieldMapData.h
 ** @author
 ** @since 14.02.2006
 ** @version1.0
 **
 ** This class holds the real data arrays of a magnetic field map,
 ** which are read from / written to file. Nota bene: Field values
 ** are in Tesla, in contrast to R3BFieldMap, whcih holds the
 ** field in kG.
 **/


#ifndef R3BMAGFIELDMAPDATA_H
#define R3BMAGFIELDMAPDATA


#include "TNamed.h"


class TArrayF;

class R3BFieldMap;



class R3BFieldMapData : public TNamed {

public:


  /** Default constructor **/
  CbmFieldMapData();


  /** Standard constructor **/
  CbmFieldMapData(const char* name);


  /** Constructor from an existing CbmFieldMap **/
  CbmFieldMapData(const char* name, const CbmFieldMap& map);
  

  /** Destructor **/
  virtual ~CbmFieldMapData();


  /** Accessors to field parameters in local coordinate system **/
  Int_t    GetType()  const { return fType; }
  Double_t GetXmin()  const { return fXmin; } 
  Double_t GetYmin()  const { return fYmin; }
  Double_t GetZmin()  const { return fZmin; }
  Double_t GetXmax()  const { return fXmax; }  
  Double_t GetYmax()  const { return fYmax; }
  Double_t GetZmax()  const { return fZmax; }
  Int_t    GetNx()    const { return fNx; }
  Int_t    GetNy()    const { return fNy; }
  Int_t    GetNz()    const { return fNz; }


  /** Accessors to the field value arrays **/
  TArrayF* GetBx() const { return fBx; }
  TArrayF* GetBy() const { return fBy; }
  TArrayF* GetBz() const { return fBz; }

	
	
 private:

  /** Type of map. 1 = CbmFieldMap, 2 = Sym2, 3 = Sym3 **/
  Int_t fType;

  /** Field limits in local coordinate system **/
  Double_t fXmin, fXmax;
  Double_t fYmin, fYmax;
  Double_t fZmin, fZmax;


  /** Number of grid points  **/
  Int_t fNx, fNy, fNz;


  /** Arrays with the field values in T **/
  TArrayF* fBx;
  TArrayF* fBy;
  TArrayF* fBz;


  ClassDef(CbmFieldMapData,1) 

};


#endif
