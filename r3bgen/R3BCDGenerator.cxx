
#include "R3BCDGenerator.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TString.h"


using namespace std;



R3BCDGenerator::R3BCDGenerator()
  : px1(0.), py1(0.), pz1(0.), E1(0.),
    px2(0.), py2(0.), pz2(0.), E2(0.),
    inputFile(NULL)
{
  //
  // Constructor
  //
  TString WorkDir = gSystem->Getenv("VMCWORKDIR");
  TString kine = WorkDir+"/input/kine/sim_27P.dat";
  inputFile = new ifstream(kine.Data());
}


R3BCDGenerator::R3BCDGenerator(const R3BCDGenerator& right)
  : px1(right.px1), py1(right.py1), pz1(right.pz1), E1(right.E1),
    px2(right.px2), py2(right.py2), pz2(right.pz2), E2(right.E2),
    inputFile(right.inputFile)
{
}


R3BCDGenerator::~R3BCDGenerator()
{
  //
  // Destructor
  //
  delete inputFile;
}


void R3BCDGenerator::ReadNewLine()
{
  //
  // Reads a new line from the CD input file
  //
  
  Char_t * myChar= new Char_t[256];
   (*inputFile).seekg((int)(12100000*gRandom->Rndm()));
  
   (*inputFile).getline(myChar,256);
 
   *inputFile >> E1  >> px1 >> py1 >> pz1 
	      >> E2  >> px2 >> py2 >> pz2;	     
}


ClassImp(R3BCDGenerator)
