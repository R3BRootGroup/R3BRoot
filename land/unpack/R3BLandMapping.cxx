// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandMapping                                    -----
// -----                           Version 0.1                                       -----
// -----                      Created  @ 03.2014 by M.I. Cherciu                     -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#include "TClonesArray.h"
//----------------------------------------------------------------------
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"
//----------------------------------------------------------------------
#include "R3BLandMapping.h"
#include "R3BLandRawHit.h"
#include "R3BLandRawHitMapped.h"
//----------------------------------------------------------------------



R3BLandMapping::R3BLandMapping()
{
  nEntry = 0;
  fnEvents = 0;
  nMappedElements= 0;
  fNofBarsPerPlane = 0;
  fIsMappingDefined = kFALSE;
  fLandHit = new TClonesArray("R3BLandRawHitMapped");
}



R3BLandMapping::~R3BLandMapping()
{
}



InitStatus R3BLandMapping::Init()
{
  if ( fname != NULL ) {
    LOG(INFO) <<"Opened File Name is ="<< GetFileName() << FairLogger::endl;
    infile.open (fname, ios_base::in );
    FairRootManager *fMan = FairRootManager::Instance();
    fMan->Register("LandHit", "Land", fLandHit, kTRUE);
    fRawData = (TClonesArray*) fMan->GetObject("LandRawHit");
  } else {
    LOG(ERROR) << "R3BLandMapping Error: No mapping file ..." << FairLogger::endl;
    return kERROR;
  }
	DoMapping();
	return kSUCCESS;
}



Bool_t R3BLandMapping::DoMapping()
{
  string stringFromFile;
  char l_char [9];
	string m_string;
  char stmp[4];
  Int_t tmp;
	
	if ( infile.is_open() )	{
	  LOG(INFO) << "File Name = "<<fname<< FairLogger::endl;
		while(getline(infile,stringFromFile))
		{
      const char *l_cchar = stringFromFile.c_str();
      strncpy(l_char, l_cchar, sizeof(l_char));
      l_char[sizeof(l_char)-1] = '\0';
      m_string = string(l_char);
      
      
      if(m_string == "SIG_BEAM") {
        sscanf(stringFromFile.c_str(),
               "%s NNP%d_%d_%d, %31c SAM%d_GTB%d_TAC%d, %d, %4s",
               not_used_1, &i_plane,&i_bar,&i_side, not_used_2,
               &i_sam, &i_gtb, &i_tac_addr, &i_tac_ch, stmp);
        
        LOG(INFO) << " PLANE = "<<i_plane<<";  BAR = "<<i_bar<<"; SIDE = "<<i_side<<";"
        << " SAM = "<<i_sam<<";  GTB = "<<i_gtb
        <<";  TAC ADDR = "<<i_tac_addr<<"; TAC CH = "<<i_tac_ch << "; "
        << stmp <<FairLogger::endl;

        if(16 == i_plane) {
          continue;
        }
        
        v1map.insert(v1map.begin()+nMappedElements,i_tac_addr);
        v2map.insert(v2map.begin()+nMappedElements,i_tac_ch-1);
        v3map.insert(v3map.begin()+nMappedElements,i_sam);
        v4map.insert(v4map.begin()+nMappedElements,i_gtb);
        v5map.insert(v5map.begin()+nMappedElements,(i_plane-1)*fNofBarsPerPlane+i_bar);
        v6map.insert(v6map.begin()+nMappedElements,i_side);
        nMappedElements++;

        // Read 17-th channel
        if('S' == stmp[0]) {
          sscanf(stringFromFile.c_str(),
                 "%s NNP%d_%d_%d, %31c SAM%d_GTB%d_TAC%d, %d, SAM%d_GTB%d_TAC%d, %d",
                 not_used_1, &i_plane,&i_bar,&i_side, not_used_2,
                 &tmp, &tmp, &tmp, &tmp,
                 &i_sam, &i_gtb, &i_tac_addr, &i_tac_ch);

          LOG(INFO) << " PLANE = "<<i_plane<<";  BAR = "<<i_bar<<"; SIDE = "<<i_side<<";"
          << " SAM = "<<i_sam<<";  GTB = "<<i_gtb
          <<";  TAC ADDR = "<<i_tac_addr<<"; TAC CH = "<<i_tac_ch << "; "
          << stmp <<FairLogger::endl;
          
          if(16 == i_plane) {
            continue;
          }
          
          v1map.insert(v1map.begin()+nMappedElements,i_tac_addr);
          v2map.insert(v2map.begin()+nMappedElements,i_tac_ch-1);
          v3map.insert(v3map.begin()+nMappedElements,i_sam);
          v4map.insert(v4map.begin()+nMappedElements,i_gtb);
          v5map.insert(v5map.begin()+nMappedElements,-1);
          v6map.insert(v6map.begin()+nMappedElements,-1);
          nMappedElements++;
        }
      }
		}
    infile.close();
    LOG(INFO) << "Total Mapped Elements = " << nMappedElements << FairLogger::endl;
	}	else {
    LOG(WARNING) << "R3BLandMapping Warning: The file \"" << fname << "\" is not open ..."<< FairLogger::endl;
    return kFALSE;
	}
  return kTRUE;
}



void R3BLandMapping::Exec(Option_t *option)
{
  // -------- Paddle identification ----------------------
  nHits = fRawData->GetEntries();
  R3BLandRawHit *hit;
  for(Int_t i = 0; i < nHits; i++)
  {
    hit = (R3BLandRawHit*) fRawData->At(i);
    ttime = hit->GetTacData();
    charge = hit->GetQdcData();
    tacaddr = hit->GetTacAddr();
    sam = hit->GetSam();
    gtb = hit->GetGtb();
    tach = hit->GetTacCh();
    clock = hit->GetClock();
    for (Int_t j = 0; j<v1map.size();j++) {
	    if(v1map[j] == tacaddr && v2map[j] == tach && v3map[j] == sam && v4map[j] == gtb) {
	      LOG(DEBUG)<<" [I] < > TACADDR = "<<v1map[j]<<" < > TACCH = "<<v2map[j]<<" Bar Id ="<<v5map[j]<<FairLogger::endl;
        if(16 == tach) { // 17-th channel
          new ((*fLandHit)[nEntry]) R3BLandRawHitMapped(kTRUE, v5map[j], v6map[j], ttime, charge, clock);
        } else { // physics channel
          new ((*fLandHit)[nEntry]) R3BLandRawHitMapped(kFALSE, v5map[j], v6map[j], ttime, charge, clock);
        }
	      nEntry++;
      }
    }
  }
  if(nEntry > 0) {
    LOG(INFO) << "Event: " << fnEvents << ",  multiplicity: " << nEntry << FairLogger::endl;
  }
  fnEvents += 1;
}



void R3BLandMapping::FinishEvent()
{
  fLandHit->Clear();
  nEntry = 0;
}



ClassImp(R3BLandMapping)
