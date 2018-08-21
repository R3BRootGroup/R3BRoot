/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----               R3BPspxHitPar header file                   -----
// -----                 Created 16/06/22  by I.Syndikus               -----
// -------------------------------------------------------------------------

#ifndef R3BPSPXHITPAR_H
#define R3BPSPXHITPAR_H

#include "FairParGenericSet.h"

#include "TArrayF.h"
#include "TArrayI.h"
#include "TObjArray.h"
#include "TObject.h"

class FairParIo;
class FairParamList;

/**
 * Class for Parameters for Cal2Hit Conversion for PSPX detector data.
 * @author Ina Syndikus
 * @since June 22, 2016
 */

class R3BPspxHitPar : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BPspxHitPar(const char* name = "R3BPspxHitPar",
                  const char* title = "Pspx HIT parameters",
                  const char* context = "Default");

    /** Destructor **/
    virtual ~R3BPspxHitPar();

    // Getter & Setter
    inline const Int_t& GetPspxParDetector() const { return pspxhitpardetector; }
    inline const TArrayI& GetPspxParStrip() const { return pspxhitparstrip; }
    inline const TArrayI& GetPspxParOrientation() const { return pspxhitparorientation; }
    inline const TArrayI& GetPspxParOrientationXPosition() const { return pspxhitparorientationxposition; }
    inline const TArrayI& GetPspxParOrientationYPosition() const { return pspxhitparorientationyposition; }
    inline const TArrayI& GetPspxParOrientationXStrip() const { return pspxhitparorientationxstrip; }
    inline const TArrayI& GetPspxParOrientationYStrip() const { return pspxhitparorientationystrip; }
    inline const TArrayF& GetPspxParLength() const { return pspxhitparlength; }
    inline const TArrayF& GetPspxParLinearParam() const { return pspxhitparlinearparam; }

    // Initialisation from input device
    // virtual Bool_t init(FairParIo* input);

    // Output to file
    //  virtual Int_t write(FairParIo* output);

    //  virtual void print();

    /** Print parameters **/
    virtual void printparams();

    /** Reset all parameters **/
    virtual void clear();

    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);

  private:
    Int_t pspxhitpardetector;               //
    TArrayI pspxhitparstrip;                //
    TArrayI pspxhitparorientation;          //
    TArrayI pspxhitparorientationxposition; //
    TArrayI pspxhitparorientationyposition; //
    TArrayI pspxhitparorientationxstrip;    //
    TArrayI pspxhitparorientationystrip;    //
    TArrayF pspxhitparlength;               //
    TArrayF pspxhitparlinearparam;          //

    R3BPspxHitPar(const R3BPspxHitPar&);
    R3BPspxHitPar& operator=(const R3BPspxHitPar&);

    ClassDef(R3BPspxHitPar, 3);
};

#endif
