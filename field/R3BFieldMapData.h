/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

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
#define R3BMAGFIELDMAPDATA_H

#include "TNamed.h"

class TArrayF;

class R3BFieldMap;

class R3BFieldMapData : public TNamed
{

  public:
    /** Default constructor **/
    R3BFieldMapData();

    /** Standard constructor **/
    R3BFieldMapData(const char* name);

    /** Constructor from an existing R3BFieldMap **/
    R3BFieldMapData(const char* name, const R3BFieldMap& map);

    /** Destructor **/
    virtual ~R3BFieldMapData();

    /** Accessors to field parameters in local coordinate system **/
    Int_t GetType() const { return fType; }
    Double_t GetXmin() const { return fXmin; }
    Double_t GetYmin() const { return fYmin; }
    Double_t GetZmin() const { return fZmin; }
    Double_t GetXmax() const { return fXmax; }
    Double_t GetYmax() const { return fYmax; }
    Double_t GetZmax() const { return fZmax; }
    Int_t GetNx() const { return fNx; }
    Int_t GetNy() const { return fNy; }
    Int_t GetNz() const { return fNz; }

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

    ClassDef(R3BFieldMapData, 1)
};

#endif
