#ifndef _R3BCALOMAP_H_
#define _R3BCALOMAP_H_

#include <map>
#include <string>

#include <FairTask.h>

class TClonesArray;

class R3BCaloMap: public FairTask
{
public:

  R3BCaloMap(const char *filename);

  virtual void Exec(Option_t* option);

protected:

  virtual InitStatus Init();

  std::string fname;
  TClonesArray *fCrystalHitCA;
  std::map<int,int> channelMap;

public:
  ClassDef(R3BCaloMap, 0)

};

#endif

