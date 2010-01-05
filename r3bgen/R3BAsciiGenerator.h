// -------------------------------------------------------------------------
// -----                R3BAsciiGenerator header file                 -----
// -----                Created 27.11.09 <D.Bertini@gsi.de>           -----
// -------------------------------------------------------------------------

/** R3BAsciiGenerator
**/


#ifndef R3BASCIIGENERATOR_H
#define R3BASCIIGENERATOR_H 1


#include "FairGenerator.h"

#include <fstream>
#include <map>

class TDatabasePDG;
class FairPrimaryGenerator;
class FairIon;

class R3BAsciiGenerator : public FairGenerator  
{

 public: 

  /** Default constructor without arguments should not be used. **/
  R3BAsciiGenerator();


  /** Standard constructor. 
   ** @param fileName The input file name
   **/
  R3BAsciiGenerator(const char* fileName);


  /** Destructor. **/
  virtual ~R3BAsciiGenerator();

	
  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the R3BPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);



 private:

  ifstream*      fInputFile;          //! Input file stream
  const Char_t*  fFileName;           //! Input file Name
  TDatabasePDG*  fPDG;                //!  PDG database

	
  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  /** Private method RegisterIons. Goes through the input file and registers
   ** any ion needed. **/
  Int_t RegisterIons();



  /** STL map from ion name to FairIon **/
  std::map<TString, FairIon*> fIonMap;       //!
	
	
  ClassDef(R3BAsciiGenerator,1);

};

#endif
