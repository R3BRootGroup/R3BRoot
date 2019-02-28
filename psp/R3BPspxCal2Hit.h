// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxCal2Hit                          -----
// -----             created 09-03-2016 by I. Syndikus		           -----
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
 * Additionally, the multiplicity of each event for the whole detector, the x, and the y side is determined.
 * The total deposited energy is only calculated for an energy readout (X1 type detector) or the energy on the front and
 * on the back of the detector are the same within a certain uncertainty (X5 type detector).
 * @author Ina Syndikus
 * @since March 9, 2016
 */

class R3BPspxCal2Hit : public FairTask
{
  public:
    /** Default Constructor **/
    R3BPspxCal2Hit();
    /**
     * Standard Constructor
     * @param range  Parameter to check whether energy recorded on front and back are the same, in case that no cathode
     * energy is available. Sets the rangeE private variable.
     **/
    R3BPspxCal2Hit(const char* name, Int_t iVerbose, Float_t range);
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

    R3BPspxHitPar* fHitPar; /**< Parameter instance holding slopes, offsets and signs */
    std::vector<std::vector<Float_t>> offset;
    std::vector<std::vector<Float_t>> slope;
    std::vector<Short_t> sign_pos_x;
    std::vector<Short_t> sign_pos_y;
    std::vector<Short_t> sign_strip_x;
    std::vector<Short_t> sign_strip_y;

    /** Parameter to check whether energy recorded on front and back are the same, in case that no cathode energy is
     * available. A value of 0 corresponds to the energy have to be exactly the same, a value of 0.1 corresponds to a
     * difference of up to 10%, etc. **/
    Float_t rangeE;

    // void CreateHistos();
    // void WriteHistos();

  public:
    ClassDef(R3BPspxCal2Hit, 3)
};

#endif
