// -------------------------------------------------------------------------
// -----                FairAsciiGenerator header file                  -----
// -----          Created 09/06/04  by V. Friese / D.Bertini           -----
// -------------------------------------------------------------------------

/**  FairAsciiGenerator.h 
 *@author V.Friese  <v.friese@gsi.de>
 *@author D.Bertini <d.bertini@gsi.de>
 *@author M.Al-Turany<M.Al-Turany@gsi.de>
 *
 The FairAsciiGenerator reads an Ascii input file. The file must contain
 for each event a header line of the format  NTRACKS  IEV  VX VY VZ,
 where NTRACKS is the number of tracks in the event, IEV the event number,
 and VX, VY, VZ the event vertex coordinates in cm. The header line is
 followed by NTRACKS lines of the format G3PID, PX, PY, PZ, where
 G3PID is the GEANT3 particle code, and PX, PY, PZ the cartesian 
 momentum coordinates in GeV.
 Derived from FairGenerator.
**/


#ifndef R3BASCIIURQMDGEN_H
#define R3BASCIIURQMDGEN_H


#include "FairGenerator.h"

#include <fstream>

class TDatabasePDG;
class FairPrimaryGenerator;



class R3BAsciiUrQMDGen : public FairGenerator  
{

 public: 

  /** Default constructor without arguments should not be used. **/
  R3BAsciiUrQMDGen();


  /** Standard constructor. 
   ** @param fileName The input file name
   **/
  R3BAsciiUrQMDGen(const char* filename);


  /** Destructor. **/
  virtual ~R3BAsciiUrQMDGen();

	
  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);




 private:

 
  FILE* furqmd;
  const Char_t * fname;           //! Input file Name
 

  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();
	
	
  /** PDG data base */
  
//  TDatabasePDG *fPDG; //!

  ClassDef(R3BAsciiUrQMDGen,1);

};

#endif
