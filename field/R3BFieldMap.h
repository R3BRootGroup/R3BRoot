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
// -----                      R3BFieldMap header file                  -----
// -------------------------------------------------------------------------

/** R3BFieldMap.h
 ** @since 12.01.2004
 ** @version1.0
 **
 ** Magnetic field map on a 3-D grid.
 ** Field values are hold and returned in kG.
 **/

#ifndef R3BFIELDMAP_H
#define R3BFIELDMAP_H 1

#include "FairField.h"
#include "TRotation.h"
#include "TVector3.h"

class TArrayI;
class TArrayF;
class R3BFieldPar;

class R3BFieldMap : public FairField
{

  public:
    /** Default constructor **/
    R3BFieldMap();
    /** Standard constructor
     ** @param name       Name of field map
     ** @param fileType   R = ROOT file, A = ASCII
     **/
    R3BFieldMap(const char* mapName, const char* fileType = "R");
    /** Constructor from R3BFieldPar **/
    R3BFieldMap(R3BFieldPar* fieldPar);
    R3BFieldMap(Int_t ftype, Bool_t verbosity = kFALSE);

    /** Destructor **/
    virtual ~R3BFieldMap();
    /** Initialisation (read map from file) **/
    virtual void Init();
    /** Write the field map to an ASCII file **/
    void WriteAsciiFile(const char* fileName);
    /** Write field map data to a ROOT file **/
    void WriteRootFile(const char* fileName, const char* mapName);
    /** Set the position of the field centre **/
    virtual void SetPosition(Double_t x, Double_t y, Double_t z);
    /** Set a global field scaling factor **/
    virtual void SetScale(Double_t factor) { fScale = factor; }
    /** Accessor to global scaling factor  **/
    Double_t GetScale() const { return fScale; }
    /** Screen output **/
    virtual void Print(Option_t* option = "") const;
    /** Main GetField function */
    virtual void GetFieldValue(const Double_t point[3], Double_t* bField);

    void SetVerbose(Bool_t verbosity) { fVerbose = verbosity; }

  private:
    const R3BFieldMap& operator=(const R3BFieldMap&);
    R3BFieldMap(const R3BFieldMap&);

  protected:
    /** Reset the field parameters and data **/
    void Reset();
    /** Read the field map from an ASCII file **/
    void ReadAsciiFile(const char* fileName);
    /** Read field map from a ROOT file **/
    void ReadRootFile(const char* fileName, const char* mapName);

    Int_t GetLineForPosition(TVector3* pos) const;
    Int_t GetPositionForLine(Int_t line, TVector3* pos) const;
    Int_t GetLinesArrayForPosition(TVector3* pos, TArrayI* lines);

    /** Map file name **/
    TString fFileName;
    /** Global scaling factor (w.r.t. map on file) **/
    Double_t fScale; //!

    /** Local Variables **/
    Int_t stepsInX;    //!
    Int_t stepsInY;    //!
    Int_t stepsInZ;    //! steps in the field map along each direction
    Double_t initialX; //!
    Double_t initialY; //!
    Double_t initialZ; //! initial coordinates in field map
    Double_t gridStep; //! distance between points in field map

    Double_t* Bxfield; //! field values obtained from field map
    Double_t* Byfield; //!
    Double_t* Bzfield; //!

    Int_t typeField; //! 0 for ALADIN field map,
                     // 1 for R3B map, 2 for other possib .
                     // 3 ALADIN inverted for back tracking

    TRotation* gRot;  //!
    TVector3* gTrans; //!

    Bool_t fVerbose;

    ClassDef(R3BFieldMap, 1)
};

#endif
