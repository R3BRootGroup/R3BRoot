#ifndef R3BCALOGEOMETRY_H
#define R3BCALOGEOMETRY_H

class R3BCaloGeometry : public TObject
{
protected:
    static R3BCaloGeometry *inst;
    R3BCaloGeometry(int version);

    int fGeometryVersion;

public:
  void GetAngles(Int_t iD, Double_t *polar, Double_t *azimuthal, Double_t* rho);

  const char *GetCrystalVolumePath(int iD);

  static R3BCaloGeometry *Instance(int version);

  ClassDef(R3BCaloGeometry, 2);
};

#endif

