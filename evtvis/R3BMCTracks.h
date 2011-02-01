#ifndef R3BMCTRACKS_H
#define R3BMCTRACKS_H

#include "FairMCTracks.h"



class R3BMCTracks : public FairMCTracks {

public:

  R3BMCTracks();
  R3BMCTracks(const char* name, Int_t iVerbose = 1);
  virtual ~R3BMCTracks(){;}

  virtual InitStatus Init();
  virtual void Exec(Option_t* option);

 ClassDef(R3BMCTracks,1);
};
#endif

