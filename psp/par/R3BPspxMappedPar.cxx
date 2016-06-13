/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----               R3BPspxMappedPar source file                   -----
// -----                 Created 16/05/12  by I.Syndikus               -----
// -------------------------------------------------------------------------

#include "R3BPspxMappedPar.h"

#include "FairParamList.h"
#include "FairDetParIo.h"
#include "FairParIo.h"
#include "FairLogger.h"
#include "TString.h"
#include "TMath.h"


// -----   Standard constructor   ------------------------------------------
R3BPspxMappedPar::R3BPspxMappedPar(const char* name,
    const char* title,
    const char* context)
  : FairParGenericSet(name, title, context),
    pspxmappedparstrip(),
    pspxmappedparstation(-1)
{
  detName="Pspx";
  //  ftutdetdigipar= new TArrayF(10);
  //ftutdetdigipar= new TArrayF(10);
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BPspxMappedPar::~R3BPspxMappedPar()
{
  clear();

}
// -------------------------------------------------------------------------


// -----   Public method clear   -------------------------------------------
void R3BPspxMappedPar::clear()
{
  status = kFALSE;
  resetInputVersions();
}
// -------------------------------------------------------------------------

// -----   Public method printParams ---------------------------------------
void R3BPspxMappedPar::printparams()
{

  LOG(INFO) <<"Print"<<FairLogger::endl;
  LOG(INFO) << "pspxmappedparstation: " << pspxmappedparstation 
	    << FairLogger::endl;
  Int_t size =  pspxmappedparstrip.GetSize();
  LOG(INFO) <<"size: " << size << FairLogger::endl;
  for ( Int_t i=0; i< size; i++) {
    LOG(INFO) << i <<" :" << pspxmappedparstrip.GetAt(i) << FairLogger::endl;
  }

}
// -------------------------------------------------------------------------

void R3BPspxMappedPar::putParams(FairParamList* l)
{
  //   print();
  LOG(INFO) << "I am in R3BPspxMappedPar::putParams " 
	    << FairLogger::endl;
  if (!l) { return; }
  l->add("R3BPspxMappedStations", pspxmappedparstation);
  l->add("R3BPspxMappedStripsPerStation", pspxmappedparstrip);
  
}

//------------------------------------------------------

Bool_t R3BPspxMappedPar::getParams(FairParamList* l)
{
  //print();

  LOG(INFO) << "I am in R3BPspxMappedPar::getParams " 
	    << FairLogger::endl;

  if (!l) { return kFALSE; }
  if ( ! l->fill("R3BPspxMappedStations", &pspxmappedparstation) ) { return kFALSE; }
  pspxmappedparstrip.Set(pspxmappedparstation);

  if ( ! l->fill("R3BPspxMappedStripsPerStation", &pspxmappedparstrip) ) { return kFALSE; }

  return kTRUE;
}



ClassImp(R3BPspxMappedPar)
