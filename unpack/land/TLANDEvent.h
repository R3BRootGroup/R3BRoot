///////////////////////////////////////
// Chimera Event Unpacker
// for the Asy-Eos experiment
// November 2010  
// revison 3/2011
// E.d.F ver 1.0 
// defilippo@ct.infn.it
// last rev. 1/2012
///////////////////////////////////////
#ifndef TLANDEVENT_H
#define TLANDEVENT_H

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include "TH1.h"
#include "TList.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TRootEvent.h"
using namespace std;

class TLANDEvent {
protected:
 int fRawmulti;                    //raw multiplicity
 string flandcaldir;
public:
 TLANDEvent(string landcaldir);         		   //constructor
 virtual ~TLANDEvent();            //destructor
 int GetEvent(int *); 
 int ProcessCurrentEventMBS(int *, int);
 //////////////////////////////////////////////////////////////////////
 /////////////// Event Get-Information methods ////////////////////////
 //////////////////////////////////////////////////////////////////////
 void GetTiMeStamp(int *, int *, int *);
 int DoLANDMapping();
 int GetLANDRawData(TRootLANDEvent *, bool corr);
 int TLANDTSync();
 void TLANDReadPar() ;
};

extern TLANDEvent *glandevent;    // a global instance to TLANDEvent class


#endif
