/** Neuland Digi Monitor
 * @author Jan Mayer
 * @since  10.06.2015
 *
 *  Input:  Digis. Should work both on Digis from Monte Carlo simulations
 *          as well as experimental data on digi level.
 *  Output: Soon: Vairous diagrams.
 *          Currently: A 3D scatterplot for each event.
 */

#ifndef R3BNEULANDDIGIMON_H
#define R3BNEULANDDIGIMON_H 1

#include "FairTask.h"

class TClonesArray;
class TH3D;

class R3BNeulandDigiMon : public FairTask {
public:
   R3BNeulandDigiMon();
   ~R3BNeulandDigiMon();

   InitStatus Init();
   void Exec(Option_t *option);
   void Finish();

private:
   TClonesArray *fDigis;
   TH3D *fh3;

   ClassDef(R3BNeulandDigiMon, 0);
};


#endif //R3BNEULANDDIGIMON_H
