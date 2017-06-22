#include <fstream>

#include <TClonesArray.h>
#include <FairLogger.h>

#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMappingReader.h"

using namespace std;

R3BCalifaMappingReader::R3BCalifaMappingReader(const char *filename) : fname(filename)
{}

InitStatus R3BCalifaMappingReader::Init()
{
  // Read channel mapping
  ifstream fmap(fname.c_str());
  if(!fmap.good())
  {
    LOG(ERROR) << "R3BCalifaMappingReader::Init(): Could not open mapping file " << fname << " for reading!"
                                       << FairLogger::endl;
    return kFATAL;
  }

  LOG(DEBUG) << "CALIFA Channel Mapping: " << FairLogger::endl;
  int fbxChannel, crystalId;
  while(fmap.good())
  {
    fmap >> fbxChannel >> crystalId;
    channelMap[fbxChannel] = crystalId;

    LOG(DEBUG) << fbxChannel << " - " << crystalId << FairLogger::endl;
  }

  fmap.close();

  // Get CrystalHit array
  fCrystalCalDataCA = dynamic_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("CaloCrystalHit"));
  if(!fCrystalCalDataCA)
  {
    LOG(ERROR) << "R3BCalifaMappingReader::Init(): Could not get CaloCrystalHit" << FairLogger::endl;
    return kFATAL;
  }

  return kSUCCESS;
}

void R3BCalifaMappingReader::Exec(Option_t *option)
{
  R3BCalifaCrystalCalData *hit;
  for(int i = 0; i < fCrystalCalDataCA->GetEntries(); i++)
  {
    if(!(hit = dynamic_cast<R3BCalifaCrystalCalData*>(fCrystalCalDataCA->At(i))))
    {
      LOG(ERROR) << "R3BCalifaMappingReader::Exec(): CalifaCrystalCalData is not of type R3BCalifaCrystalCalData" << FairLogger::endl;
      return;
    }

    if(channelMap.count(hit->GetCrystalId()) == 0)
    {
      LOG(ERROR) << "R3BCalifaMappingReader::Exec(): Invalid channel number " << hit->GetCrystalId() << FairLogger::endl;
      return;
    }

//    LOG(DEBUG) << "R3BCalifaMappingReader::Exec(): Mapping channel " << hit->GetCrystalId() << " to crystal ID " << channelMap[hit->GetCrystalId()] << FairLogger::endl;

    hit->SetCrystalId(channelMap[hit->GetCrystalId()]);
  }
}

ClassImp(R3BCalifaMappingReader)

