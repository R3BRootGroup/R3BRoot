#include "R3BNeulandQCalFiller.h"
#include "TClonesArray.h"
#include "R3BEventHeader.h"
#include "R3BNeulandMappedData.h"

R3BNeulandQCalFiller::R3BNeulandQCalFiller()
{
  data = new TClonesArray("R3BNeulandMappedData", 100);
  header = new R3BEventHeader();
}

R3BNeulandQCalFiller::~R3BNeulandQCalFiller()
{
  delete data, header;
}

InitStatus R3BNeulandQCalFiller::Init()
{
  FairRootManager::Instance()->Register ("NeulandMappedData", "NeuLAND", data, kTRUE);
  FairRootManager::Instance()->Register ("R3BEventHeader", "", header, kTRUE);
  return kSUCCESS;
}

void R3BNeulandQCalFiller::Exec(Option_t* option)
{
  Int_t value;
  i++;
  for(Int_t j = 0; j < 100; j++){	
	  value = j + 100;	
	  if(i > 5000)
		  value += std::rand()%10 - 5;			
	  new ((*data)[j]) R3BNeulandMappedData(0, 0, 0, value, 1, j/2 + 1, j%2 + 1);
  }			
  header = new (header) R3BEventHeader();
  header->SetTrigger(2);
}

ClassImp(R3BNeulandQCalFiller)