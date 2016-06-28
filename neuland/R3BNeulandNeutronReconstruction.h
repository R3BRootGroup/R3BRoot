#ifndef R3BNEULANDNEUTRONRECONSTRUCTION_H
#define R3BNEULANDNEUTRONRECONSTRUCTION_H

/** Neuland Neutron Reconstruction
 * @author Jan Mayer
 *
 * Work in progress
 */

#include "FairTask.h"

class R3BNeulandNeutron2DPar;
class TClonesArray;

class R3BNeulandNeutronReconstruction : public FairTask
{
  public:
    R3BNeulandNeutronReconstruction();
    ~R3BNeulandNeutronReconstruction();

  private:
    // Rule of three/five: If a class requires a user-defined destructor, a user-defined copy
    // constructor, or a user-defined copy assignment operator, it almost certainly requires all three
    // Here no copy and no move is allowed
    R3BNeulandNeutronReconstruction(const R3BNeulandNeutronReconstruction&);            // copy constructor
    R3BNeulandNeutronReconstruction(R3BNeulandNeutronReconstruction&&);                 // move constructor
    R3BNeulandNeutronReconstruction& operator=(const R3BNeulandNeutronReconstruction&); // copy assignment
    R3BNeulandNeutronReconstruction& operator=(R3BNeulandNeutronReconstruction&&);      // move assignment

  protected:
    // TODO: #ROOT6: Declare functions overrriding virtual functions overrride
    InitStatus Init();       // override
    void Finish();           // override
    void SetParContainers(); // override
  public:
    void Exec(Option_t*); // override

  private:
    void Reset();

    // TODO: #ROOT6 Replace raw pointers with std::unique_ptr?
    TClonesArray* fClusters;
    TClonesArray* fNeutrons;

    R3BNeulandNeutron2DPar* fNeulandNeutron2DPar;

  public:
    ClassDef(R3BNeulandNeutronReconstruction, 0);
};

#endif // R3BNEULANDNEUTRONRECONSTRUCTION_H
