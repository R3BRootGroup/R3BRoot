
#ifndef R3BEVENTMANAGER_H
#define R3BEVENTMANAGER_H

#include "FairEventManager.h"

class R3BIonName;

class R3BEventManager : public FairEventManager
{
 public:
  static R3BEventManager *Instance();
  R3BEventManager();
  virtual ~R3BEventManager(){;}
  virtual void AddParticlesToPdgDataBase(Int_t pdgCode);
  virtual void SetScaleByEnergy(Bool_t scale) {fScaleByEnergy = scale;}
  virtual Bool_t IsScaleByEnergy() {return fScaleByEnergy;}

 protected:
  Bool_t fScaleByEnergy; //!

  R3BIonName *fIonName;

    
 private:
  static R3BEventManager*    fgRinstance; //!
  ClassDef(R3BEventManager,1);

};


#endif
