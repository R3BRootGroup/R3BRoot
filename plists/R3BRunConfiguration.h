#ifndef R3BRUNCONFIGURATION_H
#define R3BRUNCONFIGURATION_H


#include "TG4RunConfiguration.h"


class R3BRunConfiguration : public TG4RunConfiguration
{
  public:
    R3BRunConfiguration(const TString& userGeometry,
                          const TString& specialProcess = "stepLimiter" );
    virtual ~R3BRunConfiguration();

    // methods
    virtual G4VUserPhysicsList*  CreatePhysicsList();


};

#endif //R3BRUNCONFIGURATION_H

