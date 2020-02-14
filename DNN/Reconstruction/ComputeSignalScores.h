// Contains member functions to update and apply the SignalScorers:

// General function to apply all signal scores:
Double_t AllScorers::ObtainSignalScore(R3BSignal* ThisSignal)
{
    // Applies all scores and combines them:
    Double_t TotalAdditiveScore = 0.0;
    Double_t TotalMultiplicativeScore = 1.0;
    Double_t TheScore = 0.0;
    Double_t ThisValue = 0.0;
    ScoringClass* ThisScorer;
    
    ThisScorer = RetrieveSignalScorer("Energy");
    ThisValue = ComputeSignalScoreValue_Energy(ThisSignal);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseSignalScore_DepositedEnergy==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseSignalScore_DepositedEnergy==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveSignalScorer("TOF");
    ThisValue = ComputeSignalScoreValue_TOF(ThisSignal);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseSignalScore_TOF==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseSignalScore_TOF==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveSignalScorer("Beta");
    ThisValue = ComputeSignalScoreValue_Beta(ThisSignal);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseSignalScore_RelativisticBeta==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseSignalScore_RelativisticBeta==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveSignalScorer("Ekin");
    ThisValue = ComputeSignalScoreValue_Ekin(ThisSignal);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseSignalScore_KineticEnergy==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseSignalScore_KineticEnergy==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    ThisScorer = RetrieveSignalScorer("Rvalue");
    ThisValue = ComputeSignalScoreValue_Rvalue(ThisSignal);
    TheScore = ThisScorer->GetScoreDivision(ThisValue);
    if (UseSignalScore_Rvalue==kTRUE) {TotalAdditiveScore = TotalAdditiveScore + TheScore;}
    if (UseSignalScore_Rvalue==kTRUE) {TotalMultiplicativeScore = TotalMultiplicativeScore*TheScore;}
    
    Double_t Answer = 0.0;
    TotalMultiplicativeScore = TMath::Power(TotalMultiplicativeScore,1.0/((Int_t) SignalScorers.size()));
    TotalAdditiveScore = TotalAdditiveScore/((Int_t) SignalScorers.size());
    if (ScoreCombination=="Multiplication") {Answer = TotalMultiplicativeScore;}
    if (ScoreCombination=="Addition") {Answer = TotalAdditiveScore;}
    return Answer;
}
 
// General update function for all scores:
void AllScorers::UpdateSignalScorers(R3BSignal* ThisSignal)
{
    // Begin by determining if the signal is a primary:
    Bool_t IsPrim = ThisSignal->IsPrimarySim();
    
    // Next, pass all scorers:
    ScoringClass* ThisScorer;
    Double_t ThisValue = 0.0;
    
    ThisScorer = RetrieveSignalScorer("Energy");
    ThisValue = ComputeSignalScoreValue_Energy(ThisSignal);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveSignalScorer("TOF");
    ThisValue = ComputeSignalScoreValue_TOF(ThisSignal);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveSignalScorer("Beta");
    ThisValue = ComputeSignalScoreValue_Beta(ThisSignal);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveSignalScorer("Ekin");
    ThisValue = ComputeSignalScoreValue_Ekin(ThisSignal);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
    
    ThisScorer = RetrieveSignalScorer("Rvalue");
    ThisValue = ComputeSignalScoreValue_Rvalue(ThisSignal);
    ThisScorer->UpdateScorer(ThisValue,IsPrim);
}

// Functions to compute the signal scorers:
Double_t AllScorers::ComputeSignalScoreValue_Energy(R3BSignal* ThisSignal)
{
    // Retrieve the Deposited Energy of this single signal:
    Double_t Edep = ThisSignal->GetEnergy();
    return Edep;
}

Double_t AllScorers::ComputeSignalScoreValue_TOF(R3BSignal* ThisSignal)
{
    // Retrieve the TOF of this single signal:
    Double_t Tsignal = ThisSignal->GetTime();
    Double_t TOF = TMath::Abs(Tsignal - TargetT);
    return TOF;
}

Double_t AllScorers::ComputeSignalScoreValue_Beta(R3BSignal* ThisSignal)
{
    // Retrieve the relativistic beta of this single signal:
    Double_t SignalT = ThisSignal->GetTime();
    Double_t SignalX = ThisSignal->GetPositionX();
    Double_t SignalY = ThisSignal->GetPositionY();
    Double_t SignalZ = ThisSignal->GetPositionZ();
    
    Double_t Distance = 0.0;
    Distance = Distance + (SignalX - TargetX)*(SignalX - TargetX);
    Distance = Distance + (SignalY - TargetY)*(SignalY - TargetY);
    Distance = Distance + (SignalZ - TargetZ)*(SignalZ - TargetZ);
    Distance = TMath::Sqrt(Distance);
    
    Double_t cvac = 29.9792458;
    Double_t TOF = TMath::Abs(SignalT - TargetT);
    Double_t TheBeta = Distance/(cvac*TOF);
    
    return TheBeta;
}

Double_t AllScorers::ComputeSignalScoreValue_Rvalue(R3BSignal* ThisSignal)
{
    // Retrieve the R-value of this single signal:
    Double_t beta = ComputeSignalScoreValue_Beta(ThisSignal);
    Double_t Edep = ThisSignal->GetEnergy();
    Double_t Rvalue = TMath::Abs(beta-BeamBeta)/Edep;
    return Rvalue;
}

Double_t AllScorers::ComputeSignalScoreValue_Ekin(R3BSignal* ThisSignal)
{
    // Retrieve the reconstructed kinetic energy for this signal:
    Double_t beta = ComputeSignalScoreValue_Beta(ThisSignal);
    Double_t gamma = 1.0/TMath::Sqrt(1.0 - beta*beta);
    Double_t Etot = gamma*ParticleMass;
    Double_t Ekin = Etot - ParticleMass;
    return Ekin;
}
