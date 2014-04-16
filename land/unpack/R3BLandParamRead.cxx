#include "R3BLandParamRead.h"

#include "R3BLandCalPar.h"               // for FairDbTutPar
#include "FairRunAna.h"                 // for FairRunAna
#include "FairRuntimeDb.h"              // for FairRuntimeDb

#include "Riosfwd.h"                    // for ostream

#include <iostream>                     // for operator<<, basic_ostream, etc

using std::cout;
using std::endl;

ClassImp(R3BLandParamRead);

void R3BLandParamRead::SetParContainers()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fCalPar = (R3BLandCalPar*)(rtdb->getContainer("LandCalPar"));
}

InitStatus R3BLandParamRead::Init()
{
  if (!fCalPar) {
    std::cout<<"-E- R3BLandParamRead: fCalPar Container does not exist!"<<std::endl;
    return kERROR;
  }

  if (fCalPar) fCalPar->Print();

  return kSUCCESS;


}

InitStatus R3BLandParamRead::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

void
R3BLandParamRead::Exec(Option_t* opt)
{
  return;
}

void
R3BLandParamRead::Finish()
{
  cout << " -I- R3BLandParamRead::Finish() ***** " << endl;

  fCalPar->Print();

  return;
}
