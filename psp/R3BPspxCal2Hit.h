// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxCal2Hit                          -----
// -----		                created 09-03-2016 by I. Syndikus		           -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BPSPXCAL2HIT_H
#define R3BPSPXCAL2HIT_H

#include "FairTask.h"

class TClonesArray;
class R3BEventHeader;
class R3BPspxHitPar;

/**
 * Class to convert Cal data to Hit data for PSPX detector data.
 * This means: Calculation of total deposited energy, uv (internal) and xy (external) coordinates for every event.
 * For the xy coordinates qualitiy factors (=sigma) is given.
 * Additionally, the multiplicity of each event for the whole detector, the x and the y side is determined.
 * @author Ina Syndikus
 * @since March 9, 2016
 */

class R3BPspxCal2Hit : public FairTask
{
  public:
    /** Default Constructor **/
    R3BPspxCal2Hit();
    /** Standard Constructor **/
    R3BPspxCal2Hit(const char* name, Int_t iVerbose);
    /** Destructor **/
    virtual ~R3BPspxCal2Hit();

    virtual InitStatus Init();
    InitStatus ReInit();
    void SetParContainers();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();
    virtual void FinishTask();

  private:
    R3BEventHeader* fHeader; // do we need that?
    TClonesArray* fCalItems; /**< Array holding input (Cal) data */
    TClonesArray* fHitItems; /**< Array holding output (Hit) data */

    R3BPspxHitPar* fHitPar; /**< Parameter instance holding slopes and offsets */
    std::vector<vector<Float_t>> offset;
    std::vector<vector<Float_t>> slope;
    std::vector<Int_t> sign_x;
    std::vector<Int_t> sign_y;

    // void CreateHistos();
    // void WriteHistos();

  public:
    ClassDef(R3BPspxCal2Hit, 1)
};

#endif
