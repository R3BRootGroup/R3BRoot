#ifndef R3BCALIFACLUSTERMATCHER_H
#define R3BCALIFACLUSTERMATCHER_H

#include <TObject.h>

#include "R3BCalifaClusterData.h"
#include "R3BCalifaCrystalCalData.h"

#include <CLHEP/Units/SystemOfUnits.h>
using namespace CLHEP;

class R3BCalifaClusterMatcher : public TObject
{
 public:
  virtual bool Match(const R3BCalifaClusterData&, const R3BCalifaCrystalCalData& )=0;

  // Does our matching only depend on the position of the position of the hit
  // spawning the cluster and the other hit (so we could precalculate them),
  // or does it depend on energy, cluster composition so far etc?
  virtual bool IsPurePositional() { return 1; }
  
  bool DoMatch(const R3BCalifaClusterData& cl, const R3BCalifaCrystalCalData& cand)
  {
    auto res=cl.fClusterId==cand.fCrystalId || Match(cl, cand);
    LLOG(debug) << "Candidate "<<cand.fCrystalId<<" is "<<(res?"":"NOT ")<< "part of cluster " <<cl.fClusterId;
    return res;
  }
  
  ClassDefOverride(R3BCalifaClusterMatcher, 1);
};


class R3BCalifaCone: public R3BCalifaClusterMatcher
{
  double fCosHalfAngle;
public:
  R3BCalifaCone(double halfAngle // angle between axis of cone and mantle, in radians 
		)
    : fCosHalfAngle(std::cos(halfAngle))
  {
  }

  bool Match(const R3BCalifaClusterData& cl, const R3BCalifaCrystalCalData& cand) override
  {
    // note the sign: cosine is monotonically falling. 
    return cl.fPos.Unit().Dot((cand.fPos.Unit())) > fCosHalfAngle; 
  }
  ClassDefOverride(R3BCalifaCone, 1);
};

class R3BCalifaRectangle: public R3BCalifaClusterMatcher
{
  double fDTheta, fDPhi;
public:
  // "Rectangle" here is short for latitude-longitude rectangle
  R3BCalifaRectangle(double dTheta, // all radians 
		     double dPhi
                )
    : fDTheta(dTheta)
    , fDPhi(dPhi)
  {
  }
  
  bool Match(const R3BCalifaClusterData& cl, const R3BCalifaCrystalCalData& cand) override
  {
    return std::abs(cl.fPos.Theta()-cand.fPos.Theta()) < fDTheta
	 && std::abs(cl.fPos.Phi()-cand.fPos.Phi()) < fDPhi ;
  }
  ClassDefOverride(R3BCalifaRectangle, 1);
};

class R3BCalifaSphere: public R3BCalifaClusterMatcher
{
  double fRadius{};
public:
  // For now, we only consider spheres around the center of a crystal
  
  R3BCalifaSphere(double radius // IN CLHEP units, e.g. 12*mm
		  )
    :fRadius(radius)
  {}

  bool Match(const R3BCalifaClusterData& cl, const R3BCalifaCrystalCalData& cand) override
  {
    const double ROOT_LENGTH_UNIT=cm; // for now, we are using cm in our geometry
    return (cl.fPos-cand.fPos).R()*ROOT_LENGTH_UNIT<=fRadius;
  }
  ClassDefOverride(R3BCalifaSphere, 1);
};

#endif
