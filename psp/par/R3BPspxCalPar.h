/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----               R3BPspxCalPar header file                   -----
// -----                 Created 16/05/12  by I.Syndikus               -----
// -------------------------------------------------------------------------

#ifndef R3BPSPXCALPAR_H
#define R3BPSPXCALPAR_H


#include "FairParGenericSet.h"

#include "TObject.h"
#include "TObjArray.h"
#include "TArrayF.h"
#include "TArrayI.h"

class FairParIo;
class FairParamList;


class R3BPspxCalPar : public FairParGenericSet
{

  public:

    /** Standard constructor **/
    R3BPspxCalPar(const char* name    = "R3BPspxCalPar",
                            const char* title   = "Pspx CAL parameters",
                            const char* context = "Default");


    /** Destructor **/
    virtual ~R3BPspxCalPar();
    
    /** Getter & Setter **/
    inline const Int_t& GetPspxParDetector() const { return pspxcalpardetector; }
    inline const TArrayI& GetPspxParStrip() const { return pspxcalparstrip; }
    inline const TArrayI& GetPspxParOrientation() const { return pspxcalparorientation; }
    inline const TArrayF& GetPspxParGain() const { return pspxcalpargain; }
    inline const TArrayI& GetPspxParEnergyThreshold() const { return pspxcalparenergythreshold; }


    /** Initialisation from input device**/
    //virtual Bool_t init(FairParIo* input);


    /** Output to file **/
    //  virtual Int_t write(FairParIo* output);

    //  virtual void print();
    virtual void printparams();


    /** Reset all parameters **/
    virtual void clear();

    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);

  private:

    Int_t pspxcalpardetector; //
    TArrayI pspxcalparstrip; //
    TArrayI pspxcalparorientation; //
    TArrayF pspxcalpargain;  //
    TArrayI pspxcalparenergythreshold;  //

    R3BPspxCalPar(const R3BPspxCalPar&);
    R3BPspxCalPar& operator=(const R3BPspxCalPar&);

    ClassDef(R3BPspxCalPar,2);


};


#endif





