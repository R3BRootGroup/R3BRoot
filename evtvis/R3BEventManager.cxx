#include "R3BEventManager.h"
#include "TEveManager.h"
#include "TEveGeoNode.h"
#include "TGeoManager.h"
#include "TDatabasePDG.h"


R3BEventManager *R3BEventManager::fgRinstance= 0;

R3BEventManager * R3BEventManager::Instance(){
        return fgRinstance;
}


R3BEventManager::R3BEventManager()
  :FairEventManager()
{
   fgRinstance=this;
}

void R3BEventManager::AddParticlesToPdgDataBase(Int_t pdgCode){

  TDatabasePDG *pdgDB = TDatabasePDG::Instance();
 
 if ( !pdgDB->GetParticle(pdgCode))
  {
    int temp=pdgCode/1E8;
    temp = pdgCode - temp*1E8;
    int element = temp/1E4;
    int mass = (temp - element*1E4)/10;

    bool particleRecognised=true;
    char name[20];
    switch (element)
    {
    case 2 : sprintf(name, "He"); break;
    case 3 : sprintf(name, "Li"); break;
    case 4 : sprintf(name, "Be"); break;
    case 5 : sprintf(name, "B"); break;
    case 6 : sprintf(name, "C"); break;
    case 7 : sprintf(name, "N"); break;
    case 8 : sprintf(name, "O"); break;
    case 9 : sprintf(name, "F"); break;
    case 10 : sprintf(name, "Ne"); break;
    case 11 : sprintf(name, "Na"); break;
    case 12 : sprintf(name, "Mg"); break;
    case 13 : sprintf(name, "Al"); break;
    case 14 : sprintf(name, "Si"); break;     
    case 15 : sprintf(name, "P"); break;
    case 16 : sprintf(name, "S"); break;
    case 17 : sprintf(name, "Cl"); break;
    case 18 : sprintf(name, "Ar"); break;
    case 19 : sprintf(name, "K"); break;
    case 20 : sprintf(name, "Ca"); break;
    case 21 : sprintf(name, "Sc"); break;
    case 22 : sprintf(name, "Ti"); break;
    case 23 : sprintf(name, "V"); break;
    case 24 : sprintf(name, "Cr"); break;
    case 25 : sprintf(name, "Mn"); break;
    case 26 : sprintf(name, "Fe"); break;
    case 27 : sprintf(name, "Co"); break;
    case 28 : sprintf(name, "Ni"); break;
    case 29 : sprintf(name, "Cu"); break;
    case 30 : sprintf(name, "Zn"); break;
    // ...
    default :
      particleRecognised=false;
      break;
    }

    if (particleRecognised)
    {
      char title[20];
      sprintf(title, "%s%i",name, mass);
      pdgDB->AddParticle(name, title, mass, kTRUE,
                         0, 0, "ION", pdgCode);
    }
  }  
}





ClassImp(R3BEventManager)
