// Contains member functions to update and apply the ClusterScorers:

// Function to apply all cluster scores:
Double_t AllScorers::ObtainClusterScore(R3BSignalCluster* ThisCluster)
{
    // Applies all scores and combines them:
    Double_t TotalAdditiveScore = 0.0;
    Double_t TotalMultiplicativeScore = 1.0;
    Double_t TheScore = 0.0;
    Double_t ThisValue = 0.0;
    ScoringClass* ThisScorer;
    
    ThisScorer = RetrieveClusterScorer("Energy");
    ThisValue = ComputeClusterScoreValue_Energy(ThisCluster);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseClusterScore_DepositedEnergy==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseClusterScore_DepositedEnergy==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveClusterScorer("Size");
    ThisValue = ComputeClusterScoreValue_Size(ThisCluster);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseClusterScore_Size==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseClusterScore_Size==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveClusterScorer("TOF");
    ThisValue = ComputeClusterScoreValue_TOF(ThisCluster);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseClusterScore_TOF==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseClusterScore_TOF==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveClusterScorer("Beta");
    ThisValue = ComputeClusterScoreValue_Beta(ThisCluster);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseClusterScore_RelativisticBeta==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseClusterScore_RelativisticBeta==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveClusterScorer("Ekin");
    ThisValue = ComputeClusterScoreValue_Ekin(ThisCluster);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseClusterScore_KineticEnergy==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseClusterScore_KineticEnergy==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveClusterScorer("Rvalue");
    ThisValue = ComputeClusterScoreValue_Rvalue(ThisCluster);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseClusterScore_Rvalue==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseClusterScore_Rvalue==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveClusterScorer("LogR");
    ThisValue = ComputeClusterScoreValue_LogR(ThisCluster);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseClusterScore_LogR==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseClusterScore_LogR==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveClusterScorer("SpaceLength");
    ThisValue = ComputeClusterSpaceLength(ThisCluster);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseClusterScore_SpaceLength==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseClusterScore_SpaceLength==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveClusterScorer("TimeLength");
    ThisValue = ComputeClusterTimeLength(ThisCluster);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseClusterScore_TimeLength==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseClusterScore_TimeLength==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveClusterScorer("SpaceRadius");
    ThisValue = ComputeClusterSpaceRadius(ThisCluster);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseClusterScore_SpaceRadius==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseClusterScore_SpaceRadius==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    Double_t Answer = 0.0;
    TotalMultiplicativeScore = TMath::Power(TotalMultiplicativeScore,1.0/((Int_t) ClusterScorers.size()));
    TotalAdditiveScore = TotalAdditiveScore/((Int_t) ClusterScorers.size());
    if (ScoreCombination=="Multiplication") {Answer = TotalMultiplicativeScore;}
    if (ScoreCombination=="Addition") {Answer = TotalAdditiveScore;}
    return Answer;
}

// Function to update all cluster scores:
void AllScorers::UpdateClusterScorers(R3BSignalCluster* ThisCluster)
{
    // Begin by determining the number of primary hits in the cluster:
    Int_t Nprims = ThisCluster->GetPrimaryMultSim();
    Bool_t IsPrim = kFALSE;
    if (Nprims>0) {IsPrim = kTRUE;}
    
    // Next, process all scorers:
    ScoringClass* ThisScorer;
    Double_t ThisValue = 0.0;
    
    ThisScorer = RetrieveClusterScorer("Energy");
    ThisValue = ComputeClusterScoreValue_Energy(ThisCluster);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveClusterScorer("TOF");
    ThisValue = ComputeClusterScoreValue_TOF(ThisCluster);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveClusterScorer("Size");
    ThisValue = ComputeClusterScoreValue_Size(ThisCluster);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveClusterScorer("Beta");
    ThisValue = ComputeClusterScoreValue_Beta(ThisCluster);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveClusterScorer("Ekin");
    ThisValue = ComputeClusterScoreValue_Ekin(ThisCluster);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveClusterScorer("Rvalue");
    ThisValue = ComputeClusterScoreValue_Rvalue(ThisCluster);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveClusterScorer("LogR");
    ThisValue = ComputeClusterScoreValue_LogR(ThisCluster);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveClusterScorer("SpaceLength");
    ThisValue = ComputeClusterSpaceLength(ThisCluster);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveClusterScorer("TimeLength");
    ThisValue = ComputeClusterTimeLength(ThisCluster);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveClusterScorer("SpaceRadius");
    ThisValue = ComputeClusterSpaceRadius(ThisCluster);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
}

// Function to select the best option for TOF,beta, etc reconstruction:
R3BSignal* AllScorers::GetPrimaryClusterHit(R3BSignalCluster* ThisCluster)
{
         if (SelectPrimHitInCluster=="TOF")     {return GetPrimaryClusterHit_TOF(ThisCluster);}
    else if (SelectPrimHitInCluster=="Z")       {return GetPrimaryClusterHit_Z(ThisCluster);}
    else if (SelectPrimHitInCluster=="Perfect") {return GetPrimaryClusterHit_Perfect(ThisCluster);}
    else
    {
        Int_t ClusterSize = ThisCluster->GetSize();
        Double_t rnd = Generator->Uniform(0.0,(Int_t) ClusterSize);
        Int_t Index = (Int_t) rnd;
        R3BSignal* rnd_Signal = ThisCluster->Atp(Index);
        return rnd_Signal;
    }
}

// Different options:
R3BSignal* AllScorers::GetPrimaryClusterHit_TOF(R3BSignalCluster* ThisCluster)
{
    R3BSignal TOFSignal = ThisCluster->FindFirstSignal();
    R3BSignal* TOFSignalp = ThisCluster->ObtainPointer(TOFSignal);
    return TOFSignalp;
}

R3BSignal* AllScorers::GetPrimaryClusterHit_Z(R3BSignalCluster* ThisCluster)
{
    R3BSignal ZSignal = ThisCluster->FindClosestSignal(NeuLAND_RotX,NeuLAND_RotY,NeuLAND_RotZ);
    R3BSignal* ZSignalp = ThisCluster->ObtainPointer(ZSignal);
    return ZSignalp;
}

R3BSignal* AllScorers::GetPrimaryClusterHit_Perfect(R3BSignalCluster* ThisCluster)
{
    // Begin by looping over the cluster:
    Int_t ClusterSize = ThisCluster->GetSize();
    R3BSignal* ThisSignal;
    Double_t MinTOF = 1e99;
    Double_t ThisTOF = 0.0;
    Int_t MinTOFIndex = -1;
    Int_t Counter = 0;
    
    for (Int_t k = 0; k<ClusterSize; ++k)
    {
        ThisSignal = (R3BSignal*) ThisCluster->Atp(k);
        
        if (ThisSignal->IsPrimarySim()==kTRUE)
        {
            // Only find min. TOF per primary signal:
            ThisTOF = ComputeSignalScoreValue_TOF(ThisSignal);
            
            if (ThisTOF<MinTOF)
            {
                MinTOF = ThisTOF;
                MinTOFIndex = k;
                Counter = Counter + 1;
            }
        }
    }
    
    // Next, if we found at least 1 primary, then that one should be the prim. hit.
    // else, we shall take the largest TOF to make max. possible distinction:
    R3BSignal* Answer;
    
    if (Counter>0)
    {
        Answer = ThisCluster->Atp(MinTOFIndex);
    }
    else
    {
        R3BSignal MaxTOFSignal = ThisCluster->FindFirstSignal();
        Answer = ThisCluster->ObtainPointer(MaxTOFSignal);
    }
        
    // Then, return the answer:
    return Answer;
}

// Function to select the best option for TOF,beta, etc reconstruction:
R3BSignal* AllScorers::GetFinalClusterHit(R3BSignalCluster* ThisCluster)
{
         if (SelectPrimHitInCluster=="TOF")     {return GetFinalClusterHit_TOF(ThisCluster);}
    else if (SelectPrimHitInCluster=="Z")       {return GetFinalClusterHit_Z(ThisCluster);}
    else if (SelectPrimHitInCluster=="Perfect") {return GetFinalClusterHit_Perfect(ThisCluster);}
    else
    {
        Int_t ClusterSize = ThisCluster->GetSize();
        Double_t rnd = Generator->Uniform(0.0,(Int_t) ClusterSize);
        Int_t Index = (Int_t) rnd;
        R3BSignal* rnd_Signal = ThisCluster->Atp(Index);
        return rnd_Signal;
    }
}

// Function to select secondary hits in clusters:
R3BSignal* AllScorers::GetSecondaryClusterHit(Int_t const Index, R3BSignalCluster* ThisCluster)
{
    // Fail safe for index out of bounds:
    Int_t TheIndex = Index;
    if (TheIndex<0) {TheIndex = 0;}
    if (TheIndex>=ThisCluster->GetSize()) {TheIndex = ThisCluster->GetSize() - 1;}
    
    // Next, determine how to sort the cluster:
         if (SelectPrimHitInCluster=="TOF")     {ThisCluster->SortClusterTOF();}
    else if (SelectPrimHitInCluster=="Z")       {ThisCluster->SortClusterZ();}
    else if (SelectPrimHitInCluster=="Perfect") {ThisCluster->SortClusterPerfect();}
    else
    {
        // Simply do not do any sorting.
    }
    
    // Then, request the appropriate member:
    return ThisCluster->Atp(TheIndex);
}

// Different options:
R3BSignal* AllScorers::GetFinalClusterHit_TOF(R3BSignalCluster* ThisCluster)
{
    R3BSignal TOFSignal = ThisCluster->FindLastSignal();
    R3BSignal* TOFSignalp = ThisCluster->ObtainPointer(TOFSignal);
    return TOFSignalp;
}

R3BSignal* AllScorers::GetFinalClusterHit_Z(R3BSignalCluster* ThisCluster)
{
    R3BSignal ZSignal = ThisCluster->FindFarestSignal(NeuLAND_RotX,NeuLAND_RotY,NeuLAND_RotZ);
    R3BSignal* ZSignalp = ThisCluster->ObtainPointer(ZSignal);
    return ZSignalp;
}

R3BSignal* AllScorers::GetFinalClusterHit_Perfect(R3BSignalCluster* ThisCluster)
{
    // Begin by looping over the cluster:
    Int_t ClusterSize = ThisCluster->GetSize();
    R3BSignal* ThisSignal;
    Double_t MaxTOF = 0.0;
    Double_t ThisTOF = 0.0;
    Int_t MaxTOFIndex = -1;
    Int_t Counter = 0;
    
    for (Int_t k = 0; k<ClusterSize; ++k)
    {
        ThisSignal = (R3BSignal*) ThisCluster->Atp(k);
        
        if (ThisSignal->IsPrimarySim()==kTRUE)
        {
            // Only find min. TOF per primary signal:
            ThisTOF = ComputeSignalScoreValue_TOF(ThisSignal);
            
            if (ThisTOF>MaxTOF)
            {
                MaxTOF = ThisTOF;
                MaxTOFIndex = k;
                Counter = Counter + 1;
            }
        }
    }
    
    // Next, if we found at least 1 primary, then that one should be the prim. hit.
    // else, we shall take the largest TOF to make max. possible distinction:
    R3BSignal* Answer;
    
    if (Counter>0)
    {
        Answer = ThisCluster->Atp(MaxTOFIndex);
    }
    else
    {
        R3BSignal MaxTOFSignal = ThisCluster->FindLastSignal();
        Answer = ThisCluster->ObtainPointer(MaxTOFSignal);
    }
        
    // Then, return the answer:
    return Answer;
}
        
// Functions to compute the different cluster score values:
Double_t AllScorers::ComputeClusterScoreValue_Energy(R3BSignalCluster* ThisCluster)
{
    // Retrieve the Deposited Energy in this cluster:
    Double_t Edep = ThisCluster->GetEnergy();
    return Edep;
}

Double_t AllScorers::ComputeClusterScoreValue_Size(R3BSignalCluster* ThisCluster)
{
    // Retrieve the Size of this cluster:
    Int_t Size = ThisCluster->GetSize();
    Double_t Size_d = (Int_t) Size;
    return Size_d;
}

Double_t AllScorers::ComputeClusterScoreValue_TOF(R3BSignalCluster* ThisCluster)
{
    // Retrieve the TOF of this cluster. Begin by selecting which hit in this
    // cluster we should base it on:
    R3BSignal* PrimHit = GetPrimaryClusterHit(ThisCluster);
    
    // Then, extract the TOF:
    Double_t TOF = ComputeSignalScoreValue_TOF(PrimHit);
    
    // and return it:
    return TOF;
}

Double_t AllScorers::ComputeClusterScoreValue_Beta(R3BSignalCluster* ThisCluster)
{
    // Retrieve the Beta of this cluster. Begin by selecting which hit in this
    // cluster we should base it on:
    R3BSignal* PrimHit = GetPrimaryClusterHit(ThisCluster);
    
    // Then, extract the Beta:
    Double_t Beta = ComputeSignalScoreValue_Beta(PrimHit);
    
    // and return it:
    return Beta;
}

Double_t AllScorers::ComputeClusterScoreValue_Ekin(R3BSignalCluster* ThisCluster)
{
    // Retrieve the Ekin of this cluster. Begin by selecting which hit in this
    // cluster we should base it on:
    R3BSignal* PrimHit = GetPrimaryClusterHit(ThisCluster);
    
    // Then, extract the Ekin:
    Double_t Ekin = ComputeSignalScoreValue_Ekin(PrimHit);
    
    // and return it:
    return Ekin;
}

Double_t AllScorers::ComputeClusterScoreValue_Rvalue(R3BSignalCluster* ThisCluster)
{
    // Retrieve the Rvalue of this cluster. Begin by selecting which hit in this
    // cluster we should base it on:
    R3BSignal* PrimHit = GetPrimaryClusterHit(ThisCluster);
    
    // Then, extract the Beta of that signal:
    Double_t Beta = ComputeSignalScoreValue_Beta(PrimHit);
    
    // Next, obtain the total energy deposition of the cluster:
    Double_t Edep = ThisCluster->GetEnergy();
    
    // Then, compute the R-value of the full cluster:
    Double_t Rvalue = TMath::Abs(Beta-BeamBeta)/Edep;
    
    // and return it:
    return Rvalue;
}

Double_t AllScorers::ComputeClusterScoreValue_LogR(R3BSignalCluster* ThisCluster)
{
    Double_t Rvalue = ComputeClusterScoreValue_Rvalue(ThisCluster);
    Double_t LogR = -1.0*TMath::Log(Rvalue)/TMath::Log(10.0);
    return LogR;
}

Double_t AllScorers::ComputeClusterSpaceLength(R3BSignalCluster* ThisCluster)
{
    R3BSignal* FirstHit = GetPrimaryClusterHit(ThisCluster);
    R3BSignal* LastHit = GetFinalClusterHit(ThisCluster);
    
    Double_t Distance = 0.0;
    Distance = Distance + (FirstHit->GetPositionX() - LastHit->GetPositionX())*(FirstHit->GetPositionX() - LastHit->GetPositionX());
    Distance = Distance + (FirstHit->GetPositionY() - LastHit->GetPositionY())*(FirstHit->GetPositionY() - LastHit->GetPositionY());
    Distance = Distance + (FirstHit->GetPositionZ() - LastHit->GetPositionZ())*(FirstHit->GetPositionZ() - LastHit->GetPositionZ());
    Distance = TMath::Sqrt(Distance);
    
    return Distance;
}

Double_t AllScorers::ComputeClusterTimeLength(R3BSignalCluster* ThisCluster)
{
    R3BSignal* FirstHit = GetPrimaryClusterHit(ThisCluster);
    R3BSignal* LastHit = GetFinalClusterHit(ThisCluster);
    
    Double_t Distance = TMath::Abs(FirstHit->GetTime() - LastHit->GetTime());
    return Distance;
}

Double_t AllScorers::ComputeClusterSpaceRadius(R3BSignalCluster* ThisCluster)
{
    // Obtain first and last hits:
    R3BSignal* FirstHit = GetPrimaryClusterHit(ThisCluster);
    R3BSignal* LastHit = GetFinalClusterHit(ThisCluster);
    
    // Fill a TVector3 with position & direction:
    TVector3 Position;
    Position.SetX(FirstHit->GetPositionX());
    Position.SetY(FirstHit->GetPositionY());
    Position.SetZ(FirstHit->GetPositionZ());
    
    TVector3 Direction;
    Direction.SetX(LastHit->GetPositionX() - FirstHit->GetPositionX());
    Direction.SetY(LastHit->GetPositionY() - FirstHit->GetPositionY());
    Direction.SetZ(LastHit->GetPositionZ() - FirstHit->GetPositionZ());
    Double_t Theta = Direction.Theta();
    Double_t Phi = Direction.Phi();
    
    // Now, create a new cluster and rotate it such that FirstHit is 
    // at the origin & lasthit is at the positive z-axis:
    Int_t ClusterSize = ThisCluster->GetSize();
    R3BSignalCluster* TransformedCluster = new R3BSignalCluster(ClusterSize);
    R3BSignal* ThisSignal;
    TVector3 ThisPos;
    
    for (Int_t k = 0; k<ClusterSize; ++k)
    {
        // Put in the signal:
        ThisSignal = ThisCluster->Atp(k);
        TransformedCluster->SetSignalp(k,ThisSignal);
        
        // Transform the new signal:
        ThisSignal = TransformedCluster->Atp(k);
        ThisPos.SetX(ThisSignal->GetPositionX());
        ThisPos.SetY(ThisSignal->GetPositionY());
        ThisPos.SetZ(ThisSignal->GetPositionZ());
        
        // Shift:
        ThisPos.SetX(ThisPos.X() - Position.X());
        ThisPos.SetY(ThisPos.Y() - Position.Y());
        ThisPos.SetZ(ThisPos.Z() - Position.Z());
        
        // Rotate:
        ThisPos.RotateZ(-1.0*Phi);
        ThisPos.RotateY(-1.0>Theta);
        
        // Put back:
        ThisSignal->SetPositionX(ThisPos.X());
        ThisSignal->SetPositionY(ThisPos.Y());
        ThisSignal->SetPositionZ(ThisPos.Z());
        
        // Done.
    }
    
    // Now, find the largest xy-radius in the new cluster:
    Double_t MaxRadius = 0.0;
    Double_t ThisRadius = 0.0;
    Double_t MaxRadIndex = -1;
    Double_t MaxRadCounter = 0;
    
    for (Int_t k = 0; k<ClusterSize; ++k)
    {
        // Retrieve current signal:
        ThisSignal = TransformedCluster->Atp(k);
        
        // Compute radius:
        ThisRadius = 0.0;
        ThisRadius = ThisRadius + (ThisSignal->GetPositionX())*(ThisSignal->GetPositionX());
        ThisRadius = ThisRadius + (ThisSignal->GetPositionY())*(ThisSignal->GetPositionY());
        ThisRadius = TMath::Sqrt(ThisRadius);
        
        // Find maximum:
        if (ThisRadius>MaxRadius)
        {
            MaxRadius = ThisRadius;
            MaxRadIndex = k;
            MaxRadCounter = MaxRadCounter + 1;
        }
    }
    
    // Now that we are done, delete the transformed cluster:
    TransformedCluster->FreeMemory();
    delete TransformedCluster;
    
    // And return the answer:
    return MaxRadius;
    
    // Done.
}
