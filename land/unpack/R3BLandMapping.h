// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandMapping                                    -----
// -----                           Version 0.1                                       -----
// -----                      Created  @ 03.2014 by M.I. Cherciu                     -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLAND_MAPPING_H
#define R3BLAND_MAPPING_H
//c++ include
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>    // std::for_each

//old C include
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//ROOT include
//#include "Riostream.h"
#include "TROOT.h"
#include "TH1.h"
#include "TClonesArray.h"

// Fair include
#include "FairTask.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%  R3BLandMapping  %%%%%%%%%%%%%%%%%%%%%%%%%%
using namespace std;
class TClonesArray;

/// Class R3BLandMapping allow to read mapping configuration from file and get identificators for each modules and channels.
class R3BLandMapping : public FairTask {
  
  /// Count of mapped elements. If is equal 0 that means, mapping wasn't done or was failed
  Int_t nMappedElements ;
  
  /// True if is arleady defined;
  Bool_t fIsMappingDefined;
  Int_t Init_GetMappingFromFile( void );
  
public:
  
  R3BLandMapping();
  virtual ~R3BLandMapping();
  R3BLandMapping& operator=(const R3BLandMapping&) { return *this; }
  
  virtual InitStatus Init();
  virtual void Exec(Option_t *option);
  virtual void FinishEvent();
  Bool_t DoMapping();
  
  void SetFileName (const char* A_FileName) {fname = A_FileName;}
  const char* GetFileName () {return fname;}
  inline void SetNofBarsPerPlane(Int_t nBars) { fNofBarsPerPlane = nBars; }
  void SetBarID(int bid){i_bar = bid;}
  int GetBarId(){return i_bar;}
  
  ifstream infile;
  const char* fname;
  FILE * landmap;

public:
	Int_t nEntry;
	Int_t nHits;
	int i_sam;
	int sam;
  int i_gtb;
	int gtb;
  int i_tac_addr;
	UShort_t tacaddr;
	int i_tac_ch;
	UShort_t tach;
	int i_plane;
	int i_bar;
	int i_side;
	UShort_t ttime;
	UShort_t charge;
  UShort_t cntl;
	char not_used_1[10];
	char not_used_2[31];
	vector<Int_t> v1map;
	vector<Int_t> v2map;
	vector<Int_t> v3map;
	vector<Int_t> v4map;
	vector<Int_t> v5map;
	vector<Int_t> v6map;
  
private:
  Int_t fnEvents;
  Int_t fNofBarsPerPlane;
  TClonesArray *fRawData;
	TClonesArray *fLandHit;
  
public:
  Bool_t IsMappingDefined(){
    return fIsMappingDefined;
  };
  
  ClassDef( R3BLandMapping, 0 );
  
};

#endif
