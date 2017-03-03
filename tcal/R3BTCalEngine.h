
#ifndef _R3BTCAL_ENGINE_
#define _R3BTCAL_ENGINE_

#define MAX_TACQUILA_SAM 7     // 0 .. 7
#define MAX_TACQUILA_GTB 1     // 0 and 1
#define MAX_TACQUILA_MODULE 20 // 0 .. 20
#define TACQUILA_NUM_GEOM ((MAX_TACQUILA_SAM + 1) * (MAX_TACQUILA_GTB + 1) * (MAX_TACQUILA_MODULE + 1))

#define CLOCK_TDC_MHZ 250
#define TACQUILA_CLOCK_MHZ 40.002903
#define VFTX_CLOCK_MHZ 200

#include "R3BTCalPar.h"
#include "TObject.h"

class TH1F;

/**
 * Class with implementation of TCAL time calibration.
 * Currently supported electronics: Clock TDC, Tacquila, and VFTX.
 * Clock frequency is set as a constant in MHz and the
 * clock cycle in ns is calculated from it.
 * Recommended value of minimum statistics per module is
 * 10000 entries.
 * @author D. Kresan
 * @since September 4, 2015
 */
class R3BTCalEngine : public TObject
{
  public:
    /**
     * Standard constructor.
     * Creates instance of TCAL engine. To be used in
     * analysis task for specific detector.
     * @param param a pointer to parameter container.
     * @param nModules a number of detector modules.
     * @param minStats a minimum number of entries per module.
     */
    R3BTCalEngine(R3BTCalPar *param, Int_t minStats = 10000);

    /**
     * Destructor.
     * Releases memory used by the object.
     */
    virtual ~R3BTCalEngine();

    /**
     * A method to fill TDC distribution for a specific module.
     * To be called from event loop of an analysis task.
     * @param iModule an index of a module.
     * @param tdc a raw TDC value.
     */
    void Fill(Int_t plane, Int_t paddle, Int_t side, Int_t tdc);

    /**
     * A method to calculate calibration parameters for clock TDC
     * electronics. Parameters will be automatically stored.
     * To be called from FinishTask() method of an analysis task.
     */
    void CalculateParamClockTDC();

    /**
     * A method to calculate calibration parameters for Tacquila
     * electronics. Parameters will be automatically stored.
     * To be called from FinishTask() method of an analysis task.
     */
    void CalculateParamTacquila();

    /**
     * A method to calculate calibration parameters for VFTX
     * electronics. Parameters will be automatically stored.
     * To be called from FinishTask() method of an analysis task.
     */
    void CalculateParamVFTX();

  protected:
    /**
     * A method to determine the range of a TDC distribution.
     * @param h1 a pointer to the histogram with data.
     * @param ic output: center of distribution.
     * @param iMin output: lower bound.
     * @param iMax output: upper bound.
     */
    void FindRange(TH1F* h1, Int_t& ic, Int_t& iMin, Int_t& iMax);

    /**
     * A method to interpolate a section of the raw TDC distribution
     * starting from the middle towards the lower bound.
     * @param h1 a pointer to the histogram with data.
     * @param iMin a lower bound.
     * @param iMax an upper bound.
     * @param il an initial value and output of a lower bound of the section.
     * @param ih an initial value and output of an upper bound of the section.
     * @param slope output: a slope of linear interpolation.
     * @param offset output: an offset of linear interpolation (value at il).
     */
    void LinearUp(TH1F* h1, Int_t iMin, Int_t iMax, Int_t& il, Int_t& ih, Double_t& slope, Double_t& offset);

    /**
     * A method to interpolate a section of the raw TDC distribution
     * starting from the middle towards the upper bound.
     * @param h1 a pointer to the histogram with data.
     * @param iMin a lower bound.
     * @param iMax an upper bound.
     * @param il an initial value and output of a lower bound of the section.
     * @param ih an initial value and output of an upper bound of the section.
     * @param slope output: a slope of linear interpolation.
     * @param offset output: an offset of linear interpolation (value at il).
     */
    void LinearDown(TH1F* h1, Int_t iMin, Int_t iMax, Int_t& il, Int_t& ih, Double_t& slope, Double_t& offset);

  private:
    Int_t fMinStats; /**< Minimum number of entries in raw TDC distribution per module */
    TH1F* fhData[N_PLANE_MAX][N_PADDLE_MAX][N_SIDE_MAX]; /**< An array of histograms to store raw TDC distributions. */
    TH1F* fhTime[N_PLANE_MAX][N_PADDLE_MAX]
                [N_SIDE_MAX]; /**< An array of histograms to store unparametrized bin-by-bin calibration. */
    R3BTCalPar* fCal_Par;     /**< A pointer to the parameter container. */
    Double_t fClockFreq;      /**< A clock cycle in [ns]. */

  public:
    ClassDef(R3BTCalEngine, 1)
};

#endif
