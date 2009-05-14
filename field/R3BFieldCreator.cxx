#include "R3BFieldCreator.h"

#include "R3BFieldConst.h"
#include "R3BFieldMap.h"
#include "R3BFieldMapSym2.h"
#include "R3BFieldMapSym3.h"
#include "R3BBsField.h"
#include "R3BFieldMapDistorted.h"
#include "R3BFieldMapSym1.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"

using std::cout;
using std::cerr;
using std::endl;

static R3BFieldCreator gR3BFieldCreator;

R3BFieldCreator::R3BFieldCreator()
	:FairFieldFactory()
{
	fCreator=this;
        fFieldPar=0;
}
R3BFieldCreator::~R3BFieldCreator()
{
}
void R3BFieldCreator::SetParm()
{
  FairRunAna *Run = FairRunAna::Instance();
  FairRuntimeDb *RunDB = Run->GetRuntimeDb();
  fFieldPar = (R3BFieldPar*) RunDB->getContainer("R3BFieldPar");

}

FairField* R3BFieldCreator::createFairField()
{ 
  FairField *fMagneticField=0;

  if ( ! fFieldPar ) {
    cerr << "-E-  No field parameters available!"
	 << endl;
  }else{
	// Instantiate correct field type
	Int_t fType = fFieldPar->GetType();
	if      ( fType == 0 ) fMagneticField = new R3BFieldConst(fFieldPar);
	else if ( fType == 1 ) fMagneticField = new R3BFieldMap(fFieldPar);
	else if ( fType == 2 ) fMagneticField = new R3BFieldMapSym2(fFieldPar);
	else if ( fType == 3 ) fMagneticField = new R3BFieldMapSym3(fFieldPar);
	else if ( fType == kTypeDistorted ) fMagneticField = new R3BFieldMapDistorted(fFieldPar); 
	else if ( fType == 5 ) fMagneticField = new R3BFieldMapSym1(fFieldPar);
	else if ( fType == 6 ) fMagneticField = new R3BBsField(fFieldPar);
	else cerr << "-W- FairRunAna::GetField: Unknown field type " << fType
		<< endl;
	cout << "New field at " << fMagneticField << ", type " << fType << endl;
	// Initialise field
	if ( fMagneticField ) {
		fMagneticField->Init();
		fMagneticField->Print();
	}
  }
  return fMagneticField;
}


ClassImp(R3BFieldCreator)
