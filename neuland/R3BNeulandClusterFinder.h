#ifndef R3BNEULANDCLUSTERFINDER_H
#define R3BNEULANDCLUSTERFINDER_H

#include <vector>
#include "FairTask.h"
#include "R3BNeulandDigi.h"

class TClonesArray;

/**
 * NeuLAND cluster finder task
 * @author Jan Mayer
 *
 * For each event, get the R3BNeulandDigis and group them into R3BNeulandClusters using the Neuland Clustering Engine.
 *   Input:  Branch NeulandDigis    = TClonesArray("R3BNeulandDigi")
 *   Output: Branch NeulandClusters = TClonesArray("R3BNeulandCluster")
 *
 * The idea is that once ROOT6 is used by default and ROOT5 is abandned, the Clutering Engine Component can be passed
 * to this task, e.g. R3BNeulandClusterFinder(myClusteringEngine), where all necessary configuration can be done on the
 * engine, such that the task can keep a small footprint. Currently hardcoded.
 */

class R3BNeulandClusterFinder : public FairTask
{
  public:
    R3BNeulandClusterFinder();
    ~R3BNeulandClusterFinder();

  private:
    // Rule of three/five: If a class requires a user-defined destructor, a user-defined copy
    // constructor, or a user-defined copy assignment operator, it almost certainly requires all three
    // Here no copy and no move is allowed
    R3BNeulandClusterFinder(const R3BNeulandClusterFinder&);            // copy constructor
    R3BNeulandClusterFinder(R3BNeulandClusterFinder&&);                 // move constructor
    R3BNeulandClusterFinder& operator=(const R3BNeulandClusterFinder&); // copy assignment
    R3BNeulandClusterFinder& operator=(R3BNeulandClusterFinder&&);      // move assignment

  protected:
    // TODO: #ROOT6: Declare functions overrriding virtual functions overrride
    InitStatus Init(); // override
    void Finish();     // override
                       // void SetParContainers(); // override
  public:
    void Exec(Option_t*); // override

  private:
    void Reset();

  private:
    // TODO: #ROOT6 Replace raw pointers with std::unique_ptr?
    TClonesArray* fNeulandDigis;
    TClonesArray* fNeulandClusters;
    std::vector<R3BNeulandDigi> fVectorDigis;

    // Note that ROOT5 fails hard when dealing with templates, so sadly this currently cannot be a class member
    // TODO: #ROOT6: Promote this to class member
    // Neuland::ClusteringEngine<R3BNeulandDigi>* fClusteringEngine;

  public:
    ClassDef(R3BNeulandClusterFinder, 1);
};

#endif // R3BNEULANDCLUSTERFINDER_H
