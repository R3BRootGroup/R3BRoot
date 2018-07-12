#include "FairLogger.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BPtofReader.h"
#include "R3BPaddleTamexMappedData.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_ptof.h"
}

#define NUM_TUBES 64

R3BPtofReader::R3BPtofReader(EXT_STR_h101_PTOF* data, UInt_t offset)
	: R3BReader("R3BPtofReader")
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
	, fArray(new TClonesArray("R3BPaddleTamexMappedData"))
	, fCoarseReference(0)
{
}

R3BPtofReader::~R3BPtofReader()
{}

Bool_t R3BPtofReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_PTOF_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_PTOF, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

	// Register output array in tree
	FairRootManager::Instance()->Register("PtofMapped", "Land", fArray,
	    kTRUE);


	// initial clear (set number of hits to 0)
	EXT_STR_h101_PTOF_onion* data = (EXT_STR_h101_PTOF_onion*)fData;
	data->PTOF_TFLM = 0;

	return kTRUE;
}

Bool_t R3BPtofReader::ReadLeadingEdges(EXT_STR_h101_PTOF_onion *data,
				       int t)
{
  // # of channels with data. not necessarily number
  // of hits! (b/c multi hit)
  uint32_t numChannels = data->PTOF_TFLM;
  LOG(DEBUG) << "mult leading "<<numChannels<<"---------------------------"<< FairLogger::endl;
  
  // loop over channels
  // index in v for first item of current channel
  uint32_t curChannelStart = 0;
  for (int i = 0; i < numChannels; i++)
	{
	  // bar number 1..65
	  uint32_t pmt;
	  uint32_t bar;
	  uint32_t tube;
	  // index in v for first item of next channel
	  uint32_t nextChannelStart;
	  
	  pmt = data->PTOF_TFLMI[i];
	  nextChannelStart = data->PTOF_TFLME[i];
	  Int_t num_cha=4; // for the full PToF this is 32 because the first PMT of the upper side is in channel 33 
	  if(pmt>num_cha) {
	    if (pmt == 8)
	      {
		bar = 8;
	      }
	    else
	      {
		bar=(pmt-num_cha)*2;
	      }
	    tube=1;
	  }
	  else{
	    if (pmt == 4)
	      {
		bar = 8;
	      }
	    else
	      {
		bar=pmt*2-1;
	      }
	    tube=0;
	  }
	  LOG(DEBUG) << "leading pmt: "<<pmt<<  "  tube: "<<tube<< "  bar: "<< bar<< FairLogger::endl;
	  LOG(DEBUG) << "Multihit  "<<nextChannelStart-curChannelStart<< FairLogger::endl;
	  
//        nextChannelStart=curChannelStart+1;
        
	  for (int j = curChannelStart; j < nextChannelStart; j++)
	    {
	      if (bar == 8)
		{
		  ReadLeadingEdgeChannel(data, tube, bar, j);
		}
	      else
		{
		  if (bar > 1) ReadLeadingEdgeChannel(data, tube, bar-1, j);
		  ReadLeadingEdgeChannel(data, tube, bar, j);
		  if (bar < 6) ReadLeadingEdgeChannel(data, tube, bar+1, j);
		}
	    }
	  
	  curChannelStart = nextChannelStart;
	}
	return kTRUE;
}

Bool_t R3BPtofReader::ReadTrailingEdges(EXT_STR_h101_PTOF_onion *data,
					int t)
{
	 // # of channels with data. not necessarly number
	 // of hits! (b/c multi hit)
	 uint32_t numChannels = data->PTOF_TFTM;
	 LOG(DEBUG) << "mult trailing "<<numChannels<<"---------------------------"<< FairLogger::endl;
	 
	 // loop over channels
	 // index in v for first item of current channel
	 uint32_t curChannelStart = 0;
	 for (int i = 0; i < numChannels; i++)
	   {
	     // or 1..65
	     uint32_t bar;
	     uint32_t tube;
	     uint32_t pmt;
	     // index in v for first item of next channel
	     uint32_t nextChannelStart;
	     nextChannelStart = data->PTOF_TFTME[i];
	     //PTOF_TFTMI is not a bar number but a PMT number.
	     //Now we convert this to a bar number.
	     pmt = data->PTOF_TFTMI[i];
	     LOG(DEBUG) << "trailing pmt  "<<pmt<< FairLogger::endl;
	     Int_t num_cha=4; // for the full PToF this is 32 because the first PMT of the upper side is in channel 33 
	     if(pmt>num_cha) { 
	       bar=(pmt-num_cha)*2;
	       tube=1;
	     }
	     else{
	       bar=pmt*2-1;
	       tube=0;
	     }
	     LOG(DEBUG)<< "trailing pmt: "<< pmt << "  tube: "<<tube<< "  bar: "<< bar<< FairLogger::endl;
	     LOG(DEBUG) << "Multihit  "<<nextChannelStart-curChannelStart<< FairLogger::endl;
	     
	     //        nextChannelStart=curChannelStart+1;
	     
	     for (int j = curChannelStart; j < nextChannelStart; j++)
	       {
		 if (bar>1) ReadTrailingEdgeChannel(data, tube, bar-1, j);			
		 ReadTrailingEdgeChannel(data, tube, bar, j);
		 ReadTrailingEdgeChannel(data, tube, bar+1, j);
	       }
	     
	     curChannelStart=nextChannelStart;
	   }
	 return kTRUE;
}

#define MAX_TIME_DIFF_PADDLE_PMT 10 /* 20 * 5 ns = 100 ns */
Bool_t R3BPtofReader::ReadLeadingEdgeChannel(EXT_STR_h101_PTOF_onion *data,
					     uint32_t tube, uint32_t bar, int ch)
{
	R3BPaddleTamexMappedData* mapped = NULL;
	/*
	 * see if we can find mappedData with
	 * matching PM leading coarse time
	 */
	int n = fArray->GetEntriesFast();
	int coarse = data->PTOF_TCLv[ch];
	LOG(DEBUG) << "coarse: "<<coarse<<" tube "<<tube<< FairLogger::endl;
	
	if(coarse<fCoarseReference)coarse+=2048;
	LOG(DEBUG) << "coarse: "<<coarse<<" tube "<<tube<< FairLogger::endl;
	
	LOG(DEBUG) << "n  "<<n<< FairLogger::endl;
	
	for (int k = 0; k < n; k++){
	  R3BPaddleTamexMappedData* hit = (R3BPaddleTamexMappedData*)fArray->At(k);
	  //
	  // see if other PM has a hit and if leading time is within 
	  // coincidence window
	  //
	  // we need a new hit if we have already registered a hit for this PM
	  
	  //LOG(DEBUG) << "Current bar "<< hit->GetBarId()  << FairLogger::endl;
	  if(hit->GetBarId()!=bar){
	    continue;
	  }
	  if (tube == 0){
	    if(hit->fCoarseTime1LE == -1 && 
	       (abs(hit->fCoarseTime2LE - coarse)<= MAX_TIME_DIFF_PADDLE_PMT)){
	      mapped = hit;
	      //LOG(DEBUG) << "Mapped bar"<<bar<<" tube "<<tube<<" coarse= "<<coarse<<" hit2 " << hit->fCoarseTime2LE  << FairLogger::endl;
	      break;
	    }
	  }
	  if (tube == 1){
	    if ((hit->fCoarseTime2LE == -1) &&
		(abs(hit->fCoarseTime1LE - coarse)<= MAX_TIME_DIFF_PADDLE_PMT)){
	      mapped = hit;
	      //LOG(DEBUG) << "Mapped "<<bar<<" tube "<<tube<<" coarse= "<<coarse<<" hit1 " << hit->fCoarseTime1LE  << FairLogger::endl;
	      break;
	    }
	  }
	}
	
	//
	// Add a new hit to the array, if t == 0 or
    // No corresponding hit in the other PM found	
    //
    if (!mapped) {
		LOG(DEBUG) << "new event in bar "<<bar<<" tube "<<tube<<" coarse= "<<coarse << FairLogger::endl;
	}
	else{
		LOG(DEBUG) << "same event in bar "<<bar<<" tube "<<tube<<" coarse= "<<coarse << FairLogger::endl;		
	}
	
	if (!mapped) mapped = new ((*fArray)[fArray->GetEntriesFast()])
		R3BPaddleTamexMappedData(1, bar); // plane, bar
		
	// Fill leading edge time members 
	if(tube==0){
       // PM1
       mapped->fCoarseTime1LE = coarse;
       mapped->fFineTime1LE   = data->PTOF_TFLv[ch];
    }
    else{
       // PM2
       mapped->fCoarseTime2LE = coarse;
       mapped->fFineTime2LE   = data->PTOF_TFLv[ch]; 
    }
	return kTRUE;
}

#define MAX_TIME_OVER_THRESHOLD 1000 /* 1000 * 5 ns = 5 us */
Bool_t R3BPtofReader::ReadTrailingEdgeChannel(EXT_STR_h101_PTOF_onion *data,
    uint32_t tube, uint32_t bar, int ch)
{
	R3BPaddleTamexMappedData* mapped = NULL;

	// see if we can find mappedData with matching PM data
	int n = fArray->GetEntriesFast();
	int coarse = data->PTOF_TCTv[ch];
    LOG(DEBUG) << "coarse: "<<coarse<<" tube "<<tube<< FairLogger::endl;
    
	if(coarse<fCoarseReference)coarse+=2048;
    LOG(DEBUG) << "coarse: "<<coarse<<" tube "<<tube<< FairLogger::endl;
	 
    LOG(DEBUG) << "n  "<<n<< FairLogger::endl;

	// matching a trailing time is only useful if the corresponding
	// leading time is present. Meaning: no need to check trailing1
	// to leading2 and vice-versa. This reduces the combinations we have to 
	// check to:
	// match trailing2 to leading2
	// match trailing1 to leading1

	// do the if outside to most inner loop for performance reasons
    if (tube == 0) {
	    // PM1
		for (int k = 0; k < n; k++)
		{
			R3BPaddleTamexMappedData* hit =
			    (R3BPaddleTamexMappedData*)fArray->At(k);
			if(hit->GetBarId()!=bar){
				continue;
			}			
			int tot = coarse - hit->fCoarseTime1LE;
		LOG(DEBUG) << "checking tube 1, bar: "<<bar<<" coarse: "<<coarse<<" tot: "<<tot<< FairLogger::endl; 
			if ((tot <= MAX_TIME_OVER_THRESHOLD) && (tot >= 0)
			    && (hit->fCoarseTime1TE == -1) /* no trailing */
			    && (hit->fCoarseTime1LE != -1)) /* has leading */
			{
				mapped = hit;
				LOG(DEBUG) << "matching trailing "<< FairLogger::endl;
				break;
			}			
		}
	}
	else {
		// PM2
		for (int k = 0; k < n; k++)
		{
			R3BPaddleTamexMappedData* hit =
			    (R3BPaddleTamexMappedData*)fArray->At(k);
			if(hit->GetBarId()!=bar){
				continue;
			}			
			int tot = coarse - hit->fCoarseTime2LE;
		LOG(DEBUG) << "checking tube 2, bar: "<<bar<< " coarse: "<<coarse<<" tot: "<<tot<< FairLogger::endl; 
			if ((tot <= MAX_TIME_OVER_THRESHOLD) && (tot >= 0)
			    && (hit->fCoarseTime2TE == -1) // no trailing 
			    && (hit->fCoarseTime2LE != -1)) // has leading
			{
				mapped = hit;
				LOG(DEBUG) << "matching trailing "<< FairLogger::endl;
				break;
			}
		}
    }
    if (!mapped) {
		LOG(DEBUG) << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << FairLogger::endl;   
		LOG(DEBUG) << "Lonely trailing edge without matching leading edge " << FairLogger::endl;  
		LOG(DEBUG) << "IN trailing edge tube "<<tube<<"  bar "<<bar<<"  coarse "<<coarse << FairLogger::endl; 
 		LOG(DEBUG) << "entry "<<n<< FairLogger::endl; 
    }
	if (!mapped) mapped = new ((*fArray)[fArray->GetEntriesFast()])
		R3BPaddleTamexMappedData(1, bar); // plane, bar
    if(tube==0){
		// PM1
		// coarse time leading edge
		mapped->fCoarseTime1TE= coarse;
		// fine time leading edge
		mapped->fFineTime1TE  = data->PTOF_TFTv[ch];
    }
    else{
		// PM2
		// coarse time leading edge
		mapped->fCoarseTime2TE= coarse;
		// fine time leading edge
		mapped->fFineTime2TE  = data->PTOF_TFTv[ch];
    }		
	return kTRUE;
}
Bool_t R3BPtofReader::FindCoarseCounterReference(EXT_STR_h101_PTOF_onion *data)
{
    fCoarseReference=2048;
    Int_t MinCoarse=2048;
    Int_t MaxCoarse=0;
    
 	// # of channels with data. not necessarly number
	// of hits! (b/c multi hit)
	uint32_t numChannels = data->PTOF_TFLM;

	// loop over channels
	// index in v for first item of current channel
	uint32_t curChannelStart = 0;
	for (int i = 0; i < numChannels; i++)
	{
		// index in v for first item of next channel
		uint32_t nextChannelStart;
		nextChannelStart = data->PTOF_TFLME[i];
		Int_t coarse=data->PTOF_TCLv[curChannelStart];
		if(coarse>MaxCoarse) MaxCoarse=coarse;
		if(coarse<MinCoarse) MinCoarse=coarse;
		if(coarse>1024 && coarse<fCoarseReference){
			fCoarseReference=coarse;
		}
		curChannelStart=nextChannelStart;
	}
	if(MaxCoarse-MinCoarse<1024) fCoarseReference=MinCoarse;
		
	
	return kTRUE;
   
}
Bool_t R3BPtofReader::Read()
{

	
	
	// Convert plain raw data to multi-dimensional array
	EXT_STR_h101_PTOF_onion* data = (EXT_STR_h101_PTOF_onion*)fData;
	
	// find out coarse counter reference
	FindCoarseCounterReference(data);
	
	
	ReadLeadingEdges(data, 0);
	ReadTrailingEdges(data, 0);
}

void R3BPtofReader::Reset()
{
	// Reset the output array
	fArray->Clear();
}

ClassImp(R3BPtofReader)

