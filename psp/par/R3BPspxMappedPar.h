/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----               R3BPspxMappedPar header file                   -----
// -----                 Created 16/05/12  by I.Syndikus               -----
// -------------------------------------------------------------------------

#ifndef R3BPSPXMAPPEDPAR_H
#define R3BPSPXMAPPEDPAR_H

#include "FairParGenericSet.h"

#include "TArrayF.h"
#include "TArrayI.h"
#include "TObjArray.h"
#include "TObject.h"

class FairParIo;
class FairParamList;

/**
 * Class for Parameters for unpacking with PspxReader for PSPX detector data.
 * @author Ina Syndikus
 * @since May 12, 2016
 */

class R3BPspxMappedPar : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BPspxMappedPar(const char* name = "R3BPspxMappedPar",
                     const char* title = "Pspx MAPPED parameters",
                     const char* context = "Default");

    /** Destructor **/
    virtual ~R3BPspxMappedPar();

    // Getter & Setter
    inline const Int_t& GetPspxParDetector() const { return pspxmappedpardetector; }
    inline const TArrayI& GetPspxParStrip() const { return pspxmappedparstrip; }

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
    TArrayI pspxmappedparstrip;  //
    Int_t pspxmappedpardetector; //

    R3BPspxMappedPar(const R3BPspxMappedPar&);
    R3BPspxMappedPar& operator=(const R3BPspxMappedPar&);

    ClassDef(R3BPspxMappedPar, 1);
};

#endif
