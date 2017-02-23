#include "R3BGammaGenerator.h"

#include <TVector3.h>
#include <TRandom3.h>
#include <TLorentzVector.h>

#include <FairPrimaryGenerator.h>
#include <FairLogger.h>

using namespace std;

Bool_t R3BGammaGenerator::Init()
{
  fRandom.SetSeed(0);
}

Bool_t R3BGammaGenerator::ReadEvent(FairPrimaryGenerator *primGen)
{
  // Pick random initial level
  double weightTotal = 0;
  int lvlSrc = -1, lvlDst = -1;
  bool radiative;

  if(fInitialLevelDistribution.size() == 0)
  {
    return kFALSE;
  }

  for(auto i : fInitialLevelDistribution)
  {
    weightTotal += i.second;
  }
  weightTotal *= fRandom.Rndm();
  for(auto i : fInitialLevelDistribution)
  {
    if(weightTotal <= i.second)
    {
      lvlSrc = i.first;
      break;
    }
    weightTotal -= i.second;
  }

  if(lvlSrc == -1)
  {
    LOG(ERROR) << "R3BGammaGenerator: Could not pick initial level" << FairLogger::endl;
    return kFALSE;
  }

  while((fBranchingRatios.count(lvlSrc)))
  {
    // Pick random destination level
    map<int, std::pair<double, bool> > &dstLevels = fBranchingRatios[lvlSrc];
    if(dstLevels.size() == 0)
    {
      break;
    }
    weightTotal = 0;
    lvlDst = -1;
    for(auto i : dstLevels)
    {
      weightTotal += i.second.first;
    }
    weightTotal *= fRandom.Rndm();
    for(auto i : dstLevels)
    {
      if(weightTotal <= i.second.first)
      {
        lvlDst = i.first;
        radiative = i.second.second;
        break;
      }
      weightTotal -= i.second.first;
    }
    if(lvlDst == -1)
    {
      LOG(ERROR) << "R3BGammaGenerator: Could not pick final level from initial level " << lvlSrc << FairLogger::endl;
      break;
    }

    if(radiative)
    {
      LOG(DEBUG) << "R3BGammaGenerator: Generating transition from level " << lvlSrc
        << " (" << (fEnergyLevels[lvlSrc] * 1000.) << " MeV) to level " << lvlDst
        << " (" << (fEnergyLevels[lvlDst] * 1000.) << " MeV)" << FairLogger::endl;

      GenerateGamma(fEnergyLevels[lvlSrc] - fEnergyLevels[lvlDst], primGen);
    }
    lvlSrc = lvlDst;
  }

  return kTRUE;
}

void R3BGammaGenerator::GenerateGamma(double E, FairPrimaryGenerator *primGen)
{
  if(E <= 0)
  {
    LOG(ERROR) << "R3BGammaGenerator: E < 0!" << FairLogger::endl;
    return;
  }

  double phi = 2.*TMath::Pi()*fRandom.Rndm();
  // For a homogenous distribution over the sphere surface pick random cos(theta) instead of theta
  double theta = TMath::ACos(2.*fRandom.Rndm() - 1.);
  
  TVector3 vMomentum;
  vMomentum.SetMagThetaPhi(E, theta, phi);
  
  TLorentzVector vGamma(vMomentum, E);
  vGamma.Boost(fBeta);

  LOG(DEBUG) << "R3BGammaGenerator: Sending gamma: E_rest = " << (1000.*E) << " MeV, E_lab = " << (1000.*vGamma.E()) << " MeV, Theta_lab = " << (vGamma.Theta()*180./TMath::Pi()) << "�" << FairLogger::endl;

  primGen->AddTrack(22, vGamma.Px(), vGamma.Py(), vGamma.Pz(), 0, 0, 0);
}

void R3BGammaGenerator::SetEnergyLevel(int idLevel, double E)
{
  fEnergyLevels[idLevel] = E;
}

void R3BGammaGenerator::SetBranchingRatio(int idLevelFrom, int idLevelTo, double ratio, bool radiative)
{
  map<int, pair<double, bool> > &dstLevels = fBranchingRatios[idLevelFrom];
  dstLevels[idLevelTo] = pair<double, bool>(ratio, radiative);
}

void R3BGammaGenerator::SetInitialLevel(int idLevel)
{
  fInitialLevelDistribution.clear();
  fInitialLevelDistribution[idLevel] = 1.;
}

void R3BGammaGenerator::SetInitialLevelDistribution(int idLevel, double ratio)
{
  fInitialLevelDistribution[idLevel] = ratio;
}

void R3BGammaGenerator::SetLorentzBoost(TVector3 &beta)
{
  fBeta = beta;
}

ClassImp(R3BGammaGenerator)
