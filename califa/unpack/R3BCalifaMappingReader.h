#ifndef _R3BCALIFAMAPPINGREADER_H_
#define _R3BCALIFAMAPPINGREADER_H_

#include <map>
#include <string>

#include <FairTask.h>

class TClonesArray;

class R3BCalifaMappingReader: public FairTask
{
public:

  R3BCalifaMappingReader(const char *filename);

  virtual void Exec(Option_t* option);

protected:

  virtual InitStatus Init();

  std::string fname;
  TClonesArray *fCrystalCalDataCA;
  std::map<int,int> channelMap;

public:
  ClassDef(R3BCalifaMappingReader, 0)

};

#endif

