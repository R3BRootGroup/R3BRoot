// -------------------------------------------------------------------------

#ifndef R3BElasticppGenerator_H
#define R3BElasticppGenerator_H

#include "FairGenerator.h"

class TDatabasePDG;
class FairPrimaryGenerator;

class R3BElasticppGenerator : public FairGenerator  
{

 public: 

  /** Default constructor without arguments should not be used. **/
  R3BElasticppGenerator();

  /** Standard constructor. 
   ** @param Ekin, kinetic energy 
   **/
  R3BElasticppGenerator(const Double_t Ekin);
  
  void kinematics ( );
  //void loop ();
  /** Destructor. **/
  virtual ~R3BElasticppGenerator();

	
  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);


 private:

	double fEkin;
	double p3x;
	double p3y;
	double p3z;
	double p4x;
	double p4y;
	double p4z;

  ClassDef(R3BElasticppGenerator,1);

};

#endif
