// Every CXX-file includes its header-file:
#include "R3BSimpleCluster.h"

// Constructor definition:
R3BSimpleCluster::R3BSimpleCluster()
{
    nPrimaries_Sim = 0;
    nPrimaries_Exp = 0;
    
    nSignals = 0;
    Total_Energy = 0.0;
}

// Copy constructor definition:
R3BSimpleCluster::R3BSimpleCluster(R3BSimpleCluster* Cluster)
{
    nPrimaries_Sim = Cluster->nPrimaries_Sim;
    nPrimaries_Exp = Cluster->nPrimaries_Exp;
    
    nSignals = Cluster->nSignals;
    Total_Energy = Cluster->Total_Energy;
}

// Destructor definition:
R3BSimpleCluster::~R3BSimpleCluster() {}

// member function definitions:
void R3BSimpleCluster::AddSignal(R3BSignal const sig)
{
    // Adds one signal to the cluster:
    nSignals = nSignals + 1;
    Total_Energy = Total_Energy + sig.GetEnergy();
    
    // Increment labels:
    if (sig.IsPrimarySim()==kTRUE) {AddSimulatedPrimary();}
    if (sig.IsPrimaryExp()==kTRUE) {AddExperimentalPrimary();}
}

void R3BSimpleCluster::AddSignalp(R3BSignal* const sig)
{
    // Adds one signal to the cluster:
    nSignals = nSignals + 1;
    Total_Energy = Total_Energy + sig->GetEnergy();
    
    // Increment labels:
    if (sig->IsPrimarySim()==kTRUE) {AddSimulatedPrimary();}
    if (sig->IsPrimaryExp()==kTRUE) {AddExperimentalPrimary();}
}

void R3BSimpleCluster::MakeEmpty()
{
    nPrimaries_Sim = 0;
    nPrimaries_Exp = 0;
    
    nSignals = 0;
    Total_Energy = 0.0;
}

// Generate ROOT-dictionary:
ClassImp(R3BSimpleCluster)
