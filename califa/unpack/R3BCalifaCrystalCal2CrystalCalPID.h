/**
 * Calibrate/Normalize PID information to energy (such that E = Nf + Ns)
 *
 * @author Max Winkel <max.winkel@ph.tum.de>
 *
 * @modified Pablo Cabanelas
 */

#ifndef R3BCalifaCrystalCal2CrystalCalPID_H
#define R3BCalifaCrystalCal2CrystalCalPID_H

#include "FairTask.h"
#include "R3BCalifaCrystalCalData.h"

class TClonesArray;

/**
 * Calibrate/Normalize PID information to energy (such that E = Nf + Ns)
 *
 * @remark Needs a file called qpidscale.txt with structure
 *  channel_nr slope delta
 * with E[channel_nr] = slope*(Nf[channel_nr] + Ns[channel_nr]) + delta
 *
 * @warning Currently only 128 channels (s438b demonstrator) supported.
 *
 * @todo Support full CALIFA
 */
class R3BCalifaCrystalCal2CrystalCalPID : public FairTask {
    
 public:
  /** Default constructor **/
  R3BCalifaCrystalCal2CrystalCalPID();
  
  /** Destructor **/
  virtual ~R3BCalifaCrystalCal2CrystalCalPID();
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* option);
    
  /** Virtual method Reset **/
  virtual void Reset();

protected:
  //Fair specific
  /** Virtual method Init **/
  virtual InitStatus Init();
    
  /** Virtual method ReInit **/
  virtual InitStatus ReInit();
    
  /** Virtual method SetParContainers **/
  virtual void SetParContainers();
    
  /** Virtual method Finish **/
  virtual void Finish();

  virtual void Register();

  Long64_t nEvents;

  double scale[128];
  double delta[128];

 private:
    
  TClonesArray *fCrystalCalDataCA;

 public:
  //Class definition
  ClassDef(R3BCalifaCrystalCal2CrystalCalPID, 0)
};

#endif
