//
// Taken from R3BLandParamRead.cxx
//
#include "R3BStarTrackParamRead.h"

#include "R3BStarTrackCalPar.h"               // for FairDbTutPar
#include "FairRunAna.h"                 // for FairRunAna
#include "FairRuntimeDb.h"              // for FairRuntimeDb

#include "Riosfwd.h"                    // for ostream

#include <iostream>                     // for operator<<, basic_ostream, etc

using std::cout;
using std::endl;

ClassImp(R3BStarTrackParamRead);

void R3BStarTrackParamRead::SetParContainers()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fCalPar = (R3BStarTrackCalPar*)(rtdb->getContainer("StarTrackCalPar"));
}

InitStatus R3BStarTrackParamRead::Init()
{
  if (!fCalPar) {
    std::cout<<"-E- R3BStarTrackParamRead: fCalPar Container does not exist!"<<std::endl;
    return kERROR;
  }

  if (fCalPar) fCalPar->Print();

  return kSUCCESS;


}

InitStatus R3BStarTrackParamRead::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

void
R3BStarTrackParamRead::Exec(Option_t* opt)
{
  return;
}

void
R3BStarTrackParamRead::Finish()
{
  cout << " -I- R3BStarTrackParamRead::Finish() ***** " << endl;

  fCalPar->Print();

  return;
}
