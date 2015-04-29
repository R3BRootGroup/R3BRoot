#include <fstream>

#include <TClonesArray.h>
#include <FairLogger.h>

#include "R3BCaloCrystalHit.h"
#include "R3BCaloMap.h"

using namespace std;

R3BCaloMap::R3BCaloMap(const char *filename) : fname(filename)
{}

InitStatus R3BCaloMap::Init()
{
  // Read channel mapping
  ifstream fmap(fname.c_str());
  if(!fmap.good())
  {
    LOG(ERROR) << "R3BCaloMap::Init(): Could not open mapping file " << fname << " for reading!"
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
  fCrystalHitCA = dynamic_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("CaloCrystalHit"));
  if(!fCrystalHitCA)
  {
    LOG(ERROR) << "R3BCaloMap::Init(): Could not get CaloCrystalHit" << FairLogger::endl;
    return kFATAL;
  }

  return kSUCCESS;
}

void R3BCaloMap::Exec(Option_t *option)
{
  R3BCaloCrystalHit *hit;
  for(int i = 0; i < fCrystalHitCA->GetEntries(); i++)
  {
    if(!(hit = dynamic_cast<R3BCaloCrystalHit*>(fCrystalHitCA->At(i))))
    {
      LOG(ERROR) << "R3BCaloMap::Exec(): CaloCrystalHit is not of type R3BCaloCrystalHit" << FairLogger::endl;
      return;
    }

    if(channelMap.count(hit->GetCrystalId()) == 0)
    {
      LOG(ERROR) << "R3BCaloMap::Exec(): Invalid channel number " << hit->GetCrystalId() << FairLogger::endl;
      return;
    }

//    LOG(DEBUG) << "R3BCaloMap::Exec(): Mapping channel " << hit->GetCrystalId() << " to crystal ID " << channelMap[hit->GetCrystalId()] << FairLogger::endl;

    hit->SetCrystalId(channelMap[hit->GetCrystalId()]);
  }
}

ClassImp(R3BCaloMap)

