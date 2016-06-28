#ifndef R3BNEULANDNEUTRONRECONSTRUCTIONEVALUATION_H
#define R3BNEULANDNEUTRONRECONSTRUCTIONEVALUATION_H

/** Neuland Neutron Reconstruction Evaluation
 * @author Jan Mayer
 *
 * Work in progress
 */

#include "FairTask.h"

class TClonesArray;
class TH1D;

class R3BNeulandNeutronReconstructionEvaluation : public FairTask
{
  public:
    R3BNeulandNeutronReconstructionEvaluation();
    ~R3BNeulandNeutronReconstructionEvaluation();

  private:
    // Rule of three/five: If a class requires a user-defined destructor, a user-defined copy
    // constructor, or a user-defined copy assignment operator, it almost certainly requires all three
    // Here no copy and no move is allowed
    R3BNeulandNeutronReconstructionEvaluation(const R3BNeulandNeutronReconstructionEvaluation&); // copy constructor
    R3BNeulandNeutronReconstructionEvaluation(R3BNeulandNeutronReconstructionEvaluation&&);      // move constructor
    R3BNeulandNeutronReconstructionEvaluation& operator=(
        const R3BNeulandNeutronReconstructionEvaluation&); // copy assignment
    R3BNeulandNeutronReconstructionEvaluation& operator=(
        R3BNeulandNeutronReconstructionEvaluation&&); // move assignment

  protected:
    // TODO: #ROOT6: Declare functions overrriding virtual functions overrride
    InitStatus Init();       // override
    void Finish();           // override
  public:
    void Exec(Option_t*); // override

  private:
    TH1D* fhCount;
    TH1D* fhScore;
    TH1D* fhEdiff;

    // TODO: #ROOT6 Replace raw pointers with std::unique_ptr?
    TClonesArray* fPrimaryNeutronInteractionPoints;
    TClonesArray* fReconstructedNeutrons;

  public:
    ClassDef(R3BNeulandNeutronReconstructionEvaluation, 0);
};

#endif // R3BNEULANDNEUTRONRECONSTRUCTIONEVALUATION_H
