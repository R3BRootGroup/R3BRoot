// Every CXX-file includes its header-file:
#include "R3BSignalCluster_NoVect.h"

// Constructor definition:
R3BSignalCluster_NoVect::R3BSignalCluster_NoVect()
{
    TheArray = 0;
    ArraySize = 0;
    TradMed_Mark = kFALSE;
}

// Constructor definition to reserve memory from the beginning:
R3BSignalCluster_NoVect::R3BSignalCluster_NoVect(Int_t const MaxSize)
{
    if (MaxSize>0)
    {
        TheArray = new R3BSignal[MaxSize];
        ArraySize = MaxSize;
        TradMed_Mark = kFALSE;
    }
    else
    {
        TheArray = 0;
        ArraySize = 0;
        TradMed_Mark = kFALSE;
    }
}

// Destructor definition:
R3BSignalCluster_NoVect::~R3BSignalCluster_NoVect()
{
    FreeMemory();
}

// Memory management:
void R3BSignalCluster_NoVect::AllocateMemory(Int_t const Size)
{
    if (ArraySize>0)
    {
        FreeMemory();
    }
    
    if (Size>0)
    {
        TheArray = new R3BSignal[Size];
        ArraySize = Size;
    }
}

void R3BSignalCluster_NoVect::FreeMemory()
{
    R3BSignal* DuplicatePointer = TheArray;
    delete[] DuplicatePointer;
    TheArray = 0;
    ArraySize = 0;
}

// member function definitions:
R3BSignal* R3BSignalCluster_NoVect::ObtainPointer(R3BSignal const sig)
{
    Double_t Distance = 0.0;
    Double_t MinDist = 1e99;
    Int_t MinIndex = -1;
    Int_t Counter = 0;
    
    for (Int_t k = 0; k<ArraySize; ++k)
    {
        Distance = 0.0;
        Distance = Distance + (sig.GetPositionX() - TheArray[k].GetPositionX())*(sig.GetPositionX() - TheArray[k].GetPositionX());
        Distance = Distance + (sig.GetPositionY() - TheArray[k].GetPositionY())*(sig.GetPositionY() - TheArray[k].GetPositionY());
        Distance = Distance + (sig.GetPositionZ() - TheArray[k].GetPositionZ())*(sig.GetPositionZ() - TheArray[k].GetPositionZ());
        Distance = Distance + 14.0*14.0*(sig.GetTime() - TheArray[k].GetTime())*(sig.GetTime() - TheArray[k].GetTime());
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
        std::cout << "R3BSignalCluster_NoVect::ObtainPointer() Index was still -1!\n";
        Answer = Atp(0);
    }
    return Answer;
}      

void R3BSignalCluster_NoVect::SetSignal(Int_t const k, R3BSignal const sig)
{
    if ((k>=0)&&(k<ArraySize))
    {
        TheArray[k] = sig;
    }
    else
    {
        std::cout << "R3BSignalCluster_NoVect::SetSignal() You accessed an index that does not exist!\n";
    }
}

void R3BSignalCluster_NoVect::SetSignalp(Int_t const k, R3BSignal* const sig)
{
    if ((k>=0)&&(k<ArraySize))
    {
        TheArray[k].SetPositionX(sig->GetPositionX());
        TheArray[k].SetPositionY(sig->GetPositionY());
        TheArray[k].SetPositionZ(sig->GetPositionZ());
        TheArray[k].SetTime(sig->GetTime());
        TheArray[k].SetEnergy(sig->GetEnergy());
        TheArray[k].SetCrystalIndex(sig->GetCrystalIndex());
    
        if (sig->IsPrimarySim()==kTRUE) {TheArray[k].SetPrimarySim();}
        else {TheArray[k].SetNonPrimarySim();}
    
        if (sig->IsPrimaryExp()==kTRUE) {TheArray[k].SetPrimaryExp();}
        else {TheArray[k].SetNonPrimaryExp();}
    }
    else
    {
        std::cout << "R3BSignalCluster_NoVect::SetSignalp() You accessed an index that does not exist!\n";
    }
}

Double_t R3BSignalCluster_NoVect::GetEnergy()
{
    Double_t Answer = 0.0;
    
    for (Int_t k = 0; k<ArraySize; ++k)
    {
        Answer = Answer + TheArray[k].GetEnergy();
    }
    
    return Answer;
}

Int_t R3BSignalCluster_NoVect::GetSize() {return ArraySize;}

R3BSignal R3BSignalCluster_NoVect::At(Int_t const k)
{
    if ((k>=0)&&(k<ArraySize))
    {
        return TheArray[k];
    }
    else
    {
        R3BSignal stupid;
        return stupid;
    }
}

R3BSignal* R3BSignalCluster_NoVect::Atp(Int_t const k)
{
    if ((k>=0)&&(k<ArraySize))
    {
        return &TheArray[k];
    }
    else
    {
        R3BSignal* stupid = 0;
        return stupid;
    }
}

Int_t R3BSignalCluster_NoVect::GetPrimaryMultSim()
{
    // Returns number of primaries in the cluster based on the MC simulation data.
    Int_t MultSim = 0;
    Bool_t IsPrimary = kFALSE;
    
    for (Int_t k = 0; k<ArraySize; ++k)
    {
        IsPrimary = TheArray[k].IsPrimarySim();
        if (IsPrimary==kTRUE) {MultSim = MultSim + 1;}
    }
    
    return MultSim;
}
 
Int_t R3BSignalCluster_NoVect::GetPrimaryMultExp()
{
    // Returns number of primaries in the cluster based on the Exp. data.
    Int_t MultExp = 0;
    Bool_t IsPrimary = kFALSE;
    
    for (Int_t k = 0; k<ArraySize; ++k)
    {
        IsPrimary = TheArray[k].IsPrimaryExp();
        if (IsPrimary==kTRUE) {MultExp = MultExp + 1;}
    }
    
    return MultExp;
}

R3BSignal R3BSignalCluster_NoVect::FindFirstSignal()
{
    // Finds the first signal (Based on TOF) in the cluster.
    Double_t MinTime = 1e99;
    Int_t MinIndex = -1;
    
    for (Int_t k = 0; k<ArraySize; ++k)
    {
        if (TheArray[k].GetTime()<MinTime)
        {
            MinTime = TheArray[k].GetTime();
            MinIndex = k;
        }
    }
    
    if (MinIndex==-1)
    {
        std::cout << "R3BSignalCluster_NoVect::FindFirstSignal() Index was still -1!\n";
        R3BSignal Stupid;
        return Stupid;
    }
    else
    {
        return TheArray[MinIndex];
    }
}

void R3BSignalCluster_NoVect::StartPosition(TVector3 &pos)
{
    R3BSignal First = FindFirstSignal();
    pos.SetX(First.GetPositionX());
    pos.SetY(First.GetPositionY());
    pos.SetZ(First.GetPositionZ());
}

Double_t R3BSignalCluster_NoVect::GetStartT()
{
    R3BSignal First = FindFirstSignal();
    return First.GetTime();
}

Double_t R3BSignalCluster_NoVect::GetStartX()
{
    R3BSignal First = FindFirstSignal();
    return First.GetPositionX();
}

Double_t R3BSignalCluster_NoVect::GetStartY()
{
    R3BSignal First = FindFirstSignal();
    return First.GetPositionY();
}

Double_t R3BSignalCluster_NoVect::GetStartZ()
{
    R3BSignal First = FindFirstSignal();
    return First.GetPositionZ();
}

R3BSignal R3BSignalCluster_NoVect::FindLastSignal()
{
    // Finds the Last signal (Based on TOF) in the cluster.
    Double_t MaxTime = -1.0*1e99;
    Int_t MaxIndex = -1;
    
    for (Int_t k = 0; k<ArraySize; ++k)
    {
        if (TheArray[k].GetTime()>MaxTime)
        {
            MaxTime = TheArray[k].GetTime();
            MaxIndex = k;
        }
    }
    
    if (MaxIndex==-1)
    {
        std::cout << "R3BSignalCluster_NoVect::FindLastSignal() Index was still -1!\n";
        R3BSignal Stupid;
        return Stupid;
    }
    else
    {
        return TheArray[MaxIndex];
    }
}
    
void R3BSignalCluster_NoVect::StopPosition(TVector3 &pos)
{
    R3BSignal Last = FindLastSignal();
    pos.SetX(Last.GetPositionX());
    pos.SetY(Last.GetPositionY());
    pos.SetZ(Last.GetPositionZ());
}

Double_t R3BSignalCluster_NoVect::GetStopT()
{
    R3BSignal Last = FindLastSignal();
    return Last.GetTime();
}    

Double_t R3BSignalCluster_NoVect::GetStopX()
{
    R3BSignal Last = FindLastSignal();
    return Last.GetPositionX();
} 

Double_t R3BSignalCluster_NoVect::GetStopY()
{
    R3BSignal Last = FindLastSignal();
    return Last.GetPositionY();
} 

Double_t R3BSignalCluster_NoVect::GetStopZ()
{
    R3BSignal Last = FindLastSignal();
    return Last.GetPositionZ();
} 

R3BSignal R3BSignalCluster_NoVect::FindClosestSignal(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    // Finds the signal with the smallest z-coordinate. We correct for the rotation angles of NeuLAND.
    Double_t MinZ = 1e99;
    Double_t ThisZ = 0.0;
    Int_t MinIndex = -1;
    TVector3 pos;
    
    for (Int_t k = 0; k<ArraySize; ++k)
    {
        // Begin by loading the position coordinates:
        pos.SetX(TheArray[k].GetPositionX());
        pos.SetY(TheArray[k].GetPositionY());
        pos.SetZ(TheArray[k].GetPositionZ());
        
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
        std::cout << "R3BSignalCluster_NoVect::FindClosestSignal() Index was still -1!\n";
        R3BSignal Stupid;
        return Stupid;
    }
    else
    {
        return TheArray[MinIndex];
    }
}

Double_t R3BSignalCluster_NoVect::GetClosestT(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindClosestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetTime();
}

Double_t R3BSignalCluster_NoVect::GetClosestX(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindClosestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionX();
}

Double_t R3BSignalCluster_NoVect::GetClosestY(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindClosestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionY();
}

Double_t R3BSignalCluster_NoVect::GetClosestZ(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindClosestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionZ();
}

R3BSignal R3BSignalCluster_NoVect::FindFarestSignal(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    // Finds the signal with the smallest z-coordinate. We correct for the rotation angles of NeuLAND.
    Double_t MaxZ = -1.0*1e99;
    Double_t ThisZ = 0.0;
    Int_t MaxIndex = -1;
    TVector3 pos;
    
    for (Int_t k = 0; k<ArraySize; ++k)
    {
        // Begin by loading the position coordinates:
        pos.SetX(TheArray[k].GetPositionX());
        pos.SetY(TheArray[k].GetPositionY());
        pos.SetZ(TheArray[k].GetPositionZ());
        
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
        std::cout << "R3BSignalCluster_NoVect::FindFarestSignal() Index was still -1!\n";
        R3BSignal Stupid;
        return Stupid;
    }
    else
    {
        return TheArray[MaxIndex];
    }
}

Double_t R3BSignalCluster_NoVect::GetFarestT(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindFarestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetTime();
}

Double_t R3BSignalCluster_NoVect::GetFarestX(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindFarestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionX();
}

Double_t R3BSignalCluster_NoVect::GetFarestY(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindFarestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionY();
}

Double_t R3BSignalCluster_NoVect::GetFarestZ(Double_t const RotX, Double_t const RotY, Double_t const RotZ)
{
    R3BSignal ThisSignal = FindFarestSignal(RotX,RotY,RotZ);
    return ThisSignal.GetPositionZ();
}

// ----------------------------------------------------------

// Sorting function definition by time:
Bool_t ClusterSortTimeFunction(R3BSignal* First, R3BSignal* Second)
{
    // Sort by acending time:
    
    // Declare the default:
    Bool_t Answer = kFALSE;
    
    // Now compute:
    if (Second->GetTime() > First->GetTime()) {Answer = kTRUE;}
     
    // return the answer: 
    return Answer;
}

void R3BSignalCluster_NoVect::SortClusterTOF()
{
    if (ArraySize>0)
    {
        // First, fill a vector with pointers to the arrays:
        std::vector<R3BSignal*> ThePointers;
        ThePointers.resize(ArraySize);
    
        for (Int_t k = 0; k<ArraySize; ++k)
        {
            ThePointers.at(k) = &TheArray[k];
        }
    
        // Next, sort the vector:
        std::sort(ThePointers.begin(), ThePointers.end(), ClusterSortTimeFunction);
    
        // Then, generate a new array:
        R3BSignal* NewArray = new R3BSignal[ArraySize];
    
        // Duplicate the cluster:
        for (Int_t k = 0; k<ArraySize; ++k)
        {
            NewArray[k].Duplicate(ThePointers.at(k));
        }
    
        // Now, delete the old cluster:
        R3BSignal* DuplicatePointer = TheArray;
        delete[] DuplicatePointer;
        TheArray = NewArray;
    
        // Loop over the cluster to see if something went wrong:
        for (Int_t k = 0; k<(ArraySize-1); ++k)
        {
            if (TheArray[k].GetTime() > TheArray[k+1].GetTime())
            {
                std::cout << "### R3BSignalCluster_NoVect ERROR: Sorting went wrong!\n";
            }
        }
        
        // Done.
    }
}

// Generate ROOT-dictionary:
ClassImp(R3BSignalCluster_NoVect)
