// Every CXX-file includes its header-file:
#include "R3BSignalCluster.h"

// Constructor definition:
R3BSignalCluster::R3BSignalCluster()
{
    TheCluster.clear();
    TradMed_Mark = kFALSE;
}

// Constructor definition to reserve memory from the beginning:
R3BSignalCluster::R3BSignalCluster(Int_t const MaxSize)
{
    TheCluster.resize(MaxSize);
    TradMed_Mark = kFALSE;
}

// Destructor definition:
R3BSignalCluster::~R3BSignalCluster()
{
    FreeMemory();
}

// Memory management:
void R3BSignalCluster::AllocateMemory(Int_t const Size)
{
    TheCluster.clear();
    TheCluster.resize(Size);
    TradMed_Mark = kFALSE;
}

void R3BSignalCluster::FreeMemory()
{
    // Create pointer array:
    Int_t Size = TheCluster.size();
    R3BSignal** Array = new R3BSignal*[Size];
    
    for (Int_t k = 0; k<Size; ++k)
    {
        Array[k] = &(TheCluster.at(k));
    }
    
    // Then, swap vector:
    std::vector<R3BSignal>().swap(TheCluster);
    
    // Clear the new vector:
    TheCluster.clear();
    
    // And finally, delete pointer array:
    delete Array;
    
    // Done.
    TradMed_Mark = kFALSE;
}

void R3BSignalCluster::Clear(Option_t *option)
{
    FreeMemory();
}

// member function definitions:
R3BSignal* R3BSignalCluster::ObtainPointer(R3BSignal const sig)
{
    Int_t ClusterSize = TheCluster.size();
    Double_t Distance = 0.0;
    Double_t MinDist = 1e99;
    Int_t MinIndex = -1;
    Int_t Counter = 0;
    
    for (Int_t k = 0; k<ClusterSize; ++k)
    {
        Distance = 0.0;
        Distance = Distance + (sig.GetPositionX() - TheCluster.at(k).GetPositionX())*(sig.GetPositionX() - TheCluster.at(k).GetPositionX());
        Distance = Distance + (sig.GetPositionY() - TheCluster.at(k).GetPositionY())*(sig.GetPositionY() - TheCluster.at(k).GetPositionY());
        Distance = Distance + (sig.GetPositionZ() - TheCluster.at(k).GetPositionZ())*(sig.GetPositionZ() - TheCluster.at(k).GetPositionZ());
        Distance = Distance + 14.0*14.0*(sig.GetTime() - TheCluster.at(k).GetTime())*(sig.GetTime() - TheCluster.at(k).GetTime());
        Distance = TMath::Sqrt(Distance);
        
        if (Distance<MinDist)
        {
            MinDist = Distance;
            MinIndex = k;
            Counter = Counter + 1;
        }
    }
    
    R3BSignal* Answer;    
    if (Counter>0)
    {
        Answer = Atp(MinIndex);
    }
    else
    {
        std::cout << "R3BSignalCluster::ObtainPointer() Index was still -1! Cluster Size = " << ClusterSize << "\n";
        Answer = Atp(0);
    }
    return Answer;
}      

void R3BSignalCluster::SetSignal(Int_t const k, R3BSignal const sig)
{
    if ((k>=0)&&(k<TheCluster.size()))
    {
        TheCluster.at(k) = sig;
    }
    else
    {
        std::cout << "R3BSignalCluster::SetSignal() You accessed an index that does not exist!\n";
    }
}

void R3BSignalCluster::AddSignal(R3BSignal const sig) 
{
    TheCluster.push_back(sig);
}

void R3BSignalCluster::SetSignalp(Int_t const k, R3BSignal* const sig)
{
    if ((k>=0)&&(k<TheCluster.size()))
    {
        TheCluster.at(k).SetPositionX(sig->GetPositionX());
        TheCluster.at(k).SetPositionY(sig->GetPositionY());
        TheCluster.at(k).SetPositionZ(sig->GetPositionZ());
        TheCluster.at(k).SetTime(sig->GetTime());
        TheCluster.at(k).SetEnergy(sig->GetEnergy());
        TheCluster.at(k).SetCrystalIndex(sig->GetCrystalIndex());
    
        if (sig->IsPrimarySim()==kTRUE) {TheCluster.at(k).SetPrimarySim();}
        else {TheCluster.at(k).SetNonPrimarySim();}
    
        if (sig->IsPrimaryExp()==kTRUE) {TheCluster.at(k).SetPrimaryExp();}
        else {TheCluster.at(k).SetNonPrimaryExp();}
    }
    else
    {
        std::cout << "R3BSignalCluster::SetSignalp() You accessed an index that does not exist!\n";
    }
}

void R3BSignalCluster::AddSignalp(R3BSignal* const sig)
{
    Int_t ClusterSize = TheCluster.size();
    TheCluster.resize(ClusterSize+1);
    TheCluster.at(ClusterSize).SetPositionX(sig->GetPositionX());
    TheCluster.at(ClusterSize).SetPositionY(sig->GetPositionY());
    TheCluster.at(ClusterSize).SetPositionZ(sig->GetPositionZ());
    TheCluster.at(ClusterSize).SetTime(sig->GetTime());
    TheCluster.at(ClusterSize).SetEnergy(sig->GetEnergy());
    TheCluster.at(ClusterSize).SetCrystalIndex(sig->GetCrystalIndex());
    
    if (sig->IsPrimarySim()==kTRUE) {TheCluster.at(ClusterSize).SetPrimarySim();}
    else {TheCluster.at(ClusterSize).SetNonPrimarySim();}
    
    if (sig->IsPrimaryExp()==kTRUE) {TheCluster.at(ClusterSize).SetPrimaryExp();}
    else {TheCluster.at(ClusterSize).SetNonPrimaryExp();}
}    

Double_t R3BSignalCluster::GetEnergy()
{
    Double_t Answer = 0.0;
    Int_t Size = TheCluster.size();
    
    for (Int_t k = 0; k<Size; ++k)
    {
        Answer = Answer + TheCluster.at(k).GetEnergy();
    }
    
    return Answer;
}

Int_t R3BSignalCluster::GetPrimaryMultSim()
{
    // Returns number of primaries in the cluster based on the MC simulation data.
    Int_t Size = TheCluster.size();
    Int_t MultSim = 0;
    Bool_t IsPrimary = kFALSE;
    
    for (Int_t k = 0; k<Size; ++k)
    {
        IsPrimary = TheCluster.at(k).IsPrimarySim();
        if (IsPrimary==kTRUE) {MultSim = MultSim + 1;}
    }
    
    return MultSim;
}
 
Int_t R3BSignalCluster::GetPrimaryMultExp()
{
    // Returns number of primaries in the cluster based on the Exp. data.
    Int_t Size = TheCluster.size();
    Int_t MultExp = 0;
    Bool_t IsPrimary = kFALSE;
    
    for (Int_t k = 0; k<Size; ++k)
    {
        IsPrimary = TheCluster.at(k).IsPrimaryExp();
        if (IsPrimary==kTRUE) {MultExp = MultExp + 1;}
    }
    
    return MultExp;
}

R3BSignal R3BSignalCluster::FindFirstSignal()
{
    // Finds the first signal (Based on TOF) in the cluster.
    Double_t MinTime = 1e99;
    Int_t MinIndex = -1;
    Int_t Size = TheCluster.size();
    
    for (Int_t k = 0; k<Size; ++k)
    {
        if (TheCluster.at(k).GetTime()<MinTime)
        {
            MinTime = TheCluster.at(k).GetTime();
            MinIndex = k;
        }
    }
    
    if (MinIndex==-1)
    {
        std::cout << "R3BSignalCluster::FindFirstSignal() Index was still -1! Cluster Size = " << Size << "\n";
        return TheCluster.at(0);
    }
    else
    {
        return TheCluster.at(MinIndex);
    }
}

void R3BSignalCluster::StartPosition(TVector3 &pos)
{
    R3BSignal First = FindFirstSignal();
    pos.SetX(First.GetPositionX());
    pos.SetY(First.GetPositionY());
    pos.SetZ(First.GetPositionZ());
}

Double_t R3BSignalCluster::GetStartT()
{
    R3BSignal First = FindFirstSignal();
    return First.GetTime();
}

Double_t R3BSignalCluster::GetStartX()
{
    R3BSignal First = FindFirstSignal();
    return First.GetPositionX();
}

Double_t R3BSignalCluster::GetStartY()
{
    R3BSignal First = FindFirstSignal();
    return First.GetPositionY();
}

Double_t R3BSignalCluster::GetStartZ()
{
    R3BSignal First = FindFirstSignal();
    return First.GetPositionZ();
}

R3BSignal R3BSignalCluster::FindLastSignal()
{
    // Finds the Last signal (Based on TOF) in the cluster.
    Double_t MaxTime = -1.0*1e99;
    Int_t MaxIndex = -1;
    Int_t Size = TheCluster.size();
    
    for (Int_t k = 0; k<Size; ++k)
    {
        if (TheCluster.at(k).GetTime()>MaxTime)
        {
            MaxTime = TheCluster.at(k).GetTime();
            MaxIndex = k;
        }
    }
    
    if (MaxIndex==-1)
    {
        std::cout << "R3BSignalCluster::FindLastSignal() Index was still -1! Cluster Size = " << Size << "\n";
        return TheCluster.at(0);
    }
    else
    {
        return TheCluster.at(MaxIndex);
    }
}
    
void R3BSignalCluster::StopPosition(TVector3 &pos)
{
    R3BSignal Last = FindLastSignal();
    pos.SetX(Last.GetPositionX());
    pos.SetY(Last.GetPositionY());
    pos.SetZ(Last.GetPositionZ());
}

Double_t R3BSignalCluster::GetStopT()
{
    R3BSignal Last = FindLastSignal();
    return Last.GetTime();
}    

Double_t R3BSignalCluster::GetStopX()
{
    R3BSignal Last = FindLastSignal();
    return Last.GetPositionX();
} 

Double_t R3BSignalCluster::GetStopY()
{
    R3BSignal Last = FindLastSignal();
    return Last.GetPositionY();
} 

Double_t R3BSignalCluster::GetStopZ()
{
    R3BSignal Last = FindLastSignal();
    return Last.GetPositionZ();
} 

R3BSignal R3BSignalCluster::FindClosestSignal(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    // Finds the signal with the smallest z-coordinate. We correct for the rotation angles of NeuLAND.
    Double_t MinZ = 1e99;
    Double_t ThisZ = 0.0;
    Int_t MinIndex = -1;
    Int_t Size = TheCluster.size();
    TVector3 pos;
    
    for (Int_t k = 0; k<Size; ++k)
    {
        // Begin by loading the position coordinates:
        pos.SetX(TheCluster.at(k).GetPositionX());
        pos.SetY(TheCluster.at(k).GetPositionY());
        pos.SetZ(TheCluster.at(k).GetPositionZ());
        
        // rotate back:
        pos.RotateZ(-1.0*RotZ*TMath::Pi()/180.0); // NOTE: RotX, RotY & RotZ are in degrees!
        pos.RotateY(-1.0*RotY*TMath::Pi()/180.0); // NOTE: RotX, RotY & RotZ are in degrees!
        pos.RotateX(-1.0*RotX*TMath::Pi()/180.0); // NOTE: RotX, RotY & RotZ are in degrees!
        
        // Then, extract the current Z:
        ThisZ = pos.Z();
        
        // Then, find the minimal Z:
        if (ThisZ<MinZ)
        {
            MinZ = ThisZ;
            MinIndex = k;
        }
    }
    
    if (MinIndex==-1)
    {
        std::cout << "R3BSignalCluster::FindClosestSignal() Index was still -1! Cluster Size = " << Size << "\n";
        return TheCluster.at(0);
    }
    else
    {
        return TheCluster.at(MinIndex);
    }
}

Double_t R3BSignalCluster::GetClosestT(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindClosestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetTime();
}

Double_t R3BSignalCluster::GetClosestX(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindClosestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionX();
}

Double_t R3BSignalCluster::GetClosestY(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindClosestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionY();
}

Double_t R3BSignalCluster::GetClosestZ(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindClosestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionZ();
}

R3BSignal R3BSignalCluster::FindFarestSignal(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    // Finds the signal with the smallest z-coordinate. We correct for the rotation angles of NeuLAND.
    Double_t MaxZ = -1.0*1e99;
    Double_t ThisZ = 0.0;
    Int_t MaxIndex = -1;
    Int_t Size = TheCluster.size();
    TVector3 pos;
    
    for (Int_t k = 0; k<Size; ++k)
    {
        // Begin by loading the position coordinates:
        pos.SetX(TheCluster.at(k).GetPositionX());
        pos.SetY(TheCluster.at(k).GetPositionY());
        pos.SetZ(TheCluster.at(k).GetPositionZ());
        
        // rotate back:
        pos.RotateZ(-1.0*RotZ*TMath::Pi()/180.0); // NOTE: RotX, RotY & RotZ are in degrees!
        pos.RotateY(-1.0*RotY*TMath::Pi()/180.0); // NOTE: RotX, RotY & RotZ are in degrees!
        pos.RotateX(-1.0*RotX*TMath::Pi()/180.0); // NOTE: RotX, RotY & RotZ are in degrees!
        
        // Then, extract the current Z:
        ThisZ = pos.Z();
        
        // Then, find the minimal Z:
        if (ThisZ>MaxZ)
        {
            MaxZ = ThisZ;
            MaxIndex = k;
        }
    }
    
    if (MaxIndex==-1)
    {
        std::cout << "R3BSignalCluster::FindFarestSignal() Index was still -1! Cluster Size = " << Size << "\n";
        return TheCluster.at(0);
    }
    else
    {
        return TheCluster.at(MaxIndex);
    }
}

Double_t R3BSignalCluster::GetFarestT(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindFarestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetTime();
}

Double_t R3BSignalCluster::GetFarestX(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindFarestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionX();
}

Double_t R3BSignalCluster::GetFarestY(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindFarestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionY();
}

Double_t R3BSignalCluster::GetFarestZ(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindFarestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionZ();
}

// ----------------------------------------------------------

// Sorting function definition by time:
Bool_t ClusterSortTimeFunction(const R3BSignal &First, const R3BSignal &Second)
{
    // Sort by acending time:
    
    // Declare the default:
    Bool_t Answer = kFALSE;
    
    // Now compute:
    if (Second.GetTime() > First.GetTime()) {Answer = kTRUE;}
     
    // return the answer: 
    return Answer;
}

// Sorting function definition by Z:
Bool_t ClusterSortZFunction(const R3BSignal &First, const R3BSignal &Second)
{
    // Sort by acending Z:
    
    // Declare the default:
    Bool_t Answer = kFALSE;
    
    // Now compute:
    if (Second.GetPositionZ() > First.GetPositionZ()) {Answer = kTRUE;}
     
    // return the answer: 
    return Answer;
}

// Sorting function definition by primary marks:
Bool_t ClusterSortPerfectFunction(const R3BSignal &First, const R3BSignal &Second)
{
    // Sort by primary marks:
    
    // Declare the default:
    Bool_t Answer = kFALSE;
    
    // Now compute:
    if ((Second.IsPrimarySim()==kTRUE)&&(First.IsPrimarySim()==kFALSE)) 
    {
        Answer = kFALSE;
    }
    else if ((Second.IsPrimarySim()==kFALSE)&&(First.IsPrimarySim()==kTRUE)) 
    {
        Answer = kTRUE;
    }
    else if ((Second.IsPrimarySim()==kTRUE)&&(First.IsPrimarySim()==kTRUE))
    {
        if (Second.GetTime() > First.GetTime()) {Answer = kTRUE;}
        else {Answer = kFALSE;}
    }
    else if ((Second.IsPrimarySim()==kFALSE)&&(First.IsPrimarySim()==kFALSE))
    {
        if (Second.GetTime() > First.GetTime()) {Answer = kTRUE;}
        else {Answer = kFALSE;}
    }    
    else 
    {
        Answer = kFALSE;
    }
     
    // return the answer: 
    return Answer;
}

void R3BSignalCluster::SortClusterTOF()
{
    // This function will sort the cluster with smallest TOF first:
    std::sort(TheCluster.begin(), TheCluster.end(), ClusterSortTimeFunction);
    
    // Loop over the cluster to see if something went wrong:
    Int_t Size = TheCluster.size();
    
    if (Size>1)
    {
        for (Int_t k = 0; k<(Size-1); ++k)
        {
            if (TheCluster.at(k).GetTime() > TheCluster.at(k+1).GetTime())
            {
                std::cout << "### R3BSignalCluster ERROR: Sorting went wrong!\n";
            }
        }
    }
}

void R3BSignalCluster::SortClusterZ()
{
    // This function will sort the cluster with smallest TOF first:
    std::sort(TheCluster.begin(), TheCluster.end(), ClusterSortZFunction);
    
    // Loop over the cluster to see if something went wrong:
    Int_t Size = TheCluster.size();
    
    if (Size>1)
    {
        for (Int_t k = 0; k<(Size-1); ++k)
        {
            if (TheCluster.at(k).GetPositionZ() > TheCluster.at(k+1).GetPositionZ())
            {
                std::cout << "### R3BSignalCluster ERROR: Sorting went wrong!\n";
            }
        }
    }
}

void R3BSignalCluster::SortClusterPerfect()
{
    // This function will sort the cluster with smallest TOF first:
    std::sort(TheCluster.begin(), TheCluster.end(), ClusterSortPerfectFunction);
    
    // Loop over the cluster to see if something went wrong:
    Int_t Size = TheCluster.size();
    
    if (Size>1)
    {
        for (Int_t k = 0; k<(Size-1); ++k)
        {
            if ((TheCluster.at(k+1).IsPrimarySim()==kTRUE)&&(TheCluster.at(k).IsPrimarySim()==kFALSE))
            {
                std::cout << "### R3BSignalCluster ERROR: Sorting went wrong!\n";
            }
        }
    }
}


// Generate ROOT-dictionary:
ClassImp(R3BSignalCluster)
