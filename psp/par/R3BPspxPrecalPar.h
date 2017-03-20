/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                R3BPspxPrecalPar header file                   -----
// -----                 Created 17/03/13  by I.Syndikus               -----
// -------------------------------------------------------------------------

#ifndef R3BPSPXPRECALPAR_H
#define R3BPSPXPRECALPAR_H

#include "FairParGenericSet.h"

#include "TArrayF.h"
#include "TArrayI.h"
#include "TObjArray.h"
#include "TObject.h"

class FairParIo;
class FairParamList;

/**
 * Class for Parameters for Mapped2Precal Conversion for PSPX detector data.
 * @author Ina Syndikus
 * @since March 13, 2017
 */

class R3BPspxPrecalPar : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BPspxPrecalPar(const char* name = "R3BPspxPrecalPar",
                     const char* title = "Pspx PRECAL parameters",
                     const char* context = "Default");

    /** Destructor **/
    virtual ~R3BPspxPrecalPar();

    // Getter & Setter
    inline const Int_t& GetPspxParDetector() const { return pspxprecalpardetector; }
    inline const TArrayI& GetPspxParStrip() const { return pspxprecalparstrip; }
    inline const TArrayI& GetPspxParOrientation() const { return pspxprecalparorientation; }
    inline const TArrayF& GetPspxParGain() const { return pspxprecalpargain; }
    inline const TArrayI& GetPspxParEnergyThreshold() const { return pspxprecalparenergythreshold; }

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
    Int_t pspxprecalpardetector;          //
    TArrayI pspxprecalparstrip;           //
    TArrayI pspxprecalparorientation;     //
    TArrayF pspxprecalpargain;            //
    TArrayI pspxprecalparenergythreshold; //

    R3BPspxPrecalPar(const R3BPspxPrecalPar&);
    R3BPspxPrecalPar& operator=(const R3BPspxPrecalPar&);

    ClassDef(R3BPspxPrecalPar, 1);
};

#endif
