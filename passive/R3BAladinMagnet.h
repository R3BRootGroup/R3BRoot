#ifndef R3BALADINMAGNET_H
#define R3BALADINMAGNET_H

#include "R3BModule.h"

class R3BAladinMagnet : public R3BModule
{
  public:
    // NOTE:
    // - There is no scenario where name and title should be set to something different than "Aladin Magnet"
    // - Aladin should not be moved or rotated from its given position
    // ... thus don't offer the option to.
    // TODO: Consider if the default constructor should be given at all, as the only valid operation is setting the
    // geometry file name
    R3BAladinMagnet();
    R3BAladinMagnet(const TString& geoFile);

    void SetPosition(const TGeoTranslation&); // override;
    void SetRotation(const TGeoRotation&); // override;

  private:
    ClassDef(R3BAladinMagnet, 3)
    // ClassDefOverride(R3BAladinMagnet, 3)
};

#endif // R3BALADINMAGNET_H
