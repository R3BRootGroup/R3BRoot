// General Scoring Class for the neutron reconstruction.
#ifndef ALLSCORERS_H
#define ALLSCORERS_H

// Include C++ headers:
#include <iostream>

// Include ROOT headers:
#include "TObject.h"
#include "TString.h"
#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TRandom3.h"

// Include R3B headers:
#include "ScoringClass.h" 
#include "R3BSignal.h"
#include "R3BSignalCluster.h"
#include "Nuclei.h"
#include "R3BInputClass.h"

using namespace std;

class AllScorers : public TObject
{
    public:
        // Default Constructor:   
        AllScorers();
  
        // Destructor:
        virtual ~AllScorers();

        // Control functions:
        void LinkInputsClass(R3BInputClass* inp) {Inputs = inp;}
        Bool_t Initialize();
        void MakePlots();
        void ReadScorers();
        void WriteScorers();
        void SetDetector(TString const det) {ThisDetector = det;}
        
        // Application of signal scorers:
        Double_t ObtainSignalScore(R3BSignal* ThisSignal);
        Double_t ComputeSignalScoreValue_Energy(R3BSignal* ThisSignal);
        Double_t ComputeSignalScoreValue_TOF(R3BSignal* ThisSignal);
        Double_t ComputeSignalScoreValue_Beta(R3BSignal* ThisSignal);
        Double_t ComputeSignalScoreValue_Rvalue(R3BSignal* ThisSignal);
        Double_t ComputeSignalScoreValue_Ekin(R3BSignal* ThisSignal);
        void UpdateSignalScorers(R3BSignal* ThisSignal);
        
        // Application of cluster scorers:
        R3BSignal* GetSecondaryClusterHit(Int_t const Index, R3BSignalCluster* ThisCluster);
        
        R3BSignal* GetPrimaryClusterHit(R3BSignalCluster* ThisCluster);
        R3BSignal* GetPrimaryClusterHit_TOF(R3BSignalCluster* ThisCluster);
        R3BSignal* GetPrimaryClusterHit_Z(R3BSignalCluster* ThisCluster);
        R3BSignal* GetPrimaryClusterHit_Perfect(R3BSignalCluster* ThisCluster);
        
        R3BSignal* GetFinalClusterHit(R3BSignalCluster* ThisCluster);
        R3BSignal* GetFinalClusterHit_TOF(R3BSignalCluster* ThisCluster);
        R3BSignal* GetFinalClusterHit_Z(R3BSignalCluster* ThisCluster);
        R3BSignal* GetFinalClusterHit_Perfect(R3BSignalCluster* ThisCluster);
        
        Double_t ObtainClusterScore(R3BSignalCluster* ThisCluster);
        Double_t ComputeClusterScoreValue_Energy(R3BSignalCluster* ThisCluster);
        Double_t ComputeClusterScoreValue_Size(R3BSignalCluster* ThisCluster);
        Double_t ComputeClusterScoreValue_TOF(R3BSignalCluster* ThisCluster);
        Double_t ComputeClusterScoreValue_Beta(R3BSignalCluster* ThisCluster);
        Double_t ComputeClusterScoreValue_Ekin(R3BSignalCluster* ThisCluster);
        Double_t ComputeClusterScoreValue_Rvalue(R3BSignalCluster* ThisCluster);
        Double_t ComputeClusterScoreValue_LogR(R3BSignalCluster* ThisCluster);
        Double_t ComputeClusterSpaceRadius(R3BSignalCluster* ThisCluster);
        Double_t ComputeClusterSpaceLength(R3BSignalCluster* ThisCluster);
        Double_t ComputeClusterTimeLength(R3BSignalCluster* ThisCluster);
        void UpdateClusterScorers(R3BSignalCluster* ThisCluster);
        
        // Get-functions:
        ScoringClass* RetrieveSignalScorer(TString const Name);
        ScoringClass* RetrieveClusterScorer(TString const Name);
        Int_t GetNumberOfClusterScores();
        Int_t GetNumberOfSignalScores();
        
        // Error handling:
        void ErrorMessage(TString const Message);
        void PrintAllErrors();
        void DisableErrorPrinting();
        void EnableErrorPrinting();
        Bool_t ContainsNoErrors();
        
        // Functions to extract the labeling boolians:
        Bool_t ClusterScore_DepositedEnergy_IsUsed()  {return UseClusterScore_DepositedEnergy;}
        Bool_t ClusterScore_Size_IsUsed()             {return UseClusterScore_Size;}
        Bool_t ClusterScore_KineticEnergy_IsUsed()    {return UseClusterScore_KineticEnergy;}
        Bool_t ClusterScore_RelativisticBeta_IsUsed() {return UseClusterScore_RelativisticBeta;}
        Bool_t ClusterScore_Rvalue_IsUsed()           {return UseClusterScore_Rvalue;}
        Bool_t ClusterScore_LogR_IsUsed()             {return UseClusterScore_LogR;}
        Bool_t ClusterScore_TOF_IsUsed()              {return UseClusterScore_TOF;}
        Bool_t ClusterScore_SpaceLength_IsUsed()      {return UseClusterScore_SpaceLength;}
        Bool_t ClusterScore_SpaceRadius_IsUsed()      {return UseClusterScore_SpaceRadius;}
        Bool_t ClusterScore_TimeLength_IsUsed()       {return UseClusterScore_TimeLength;}
        
        Bool_t SignalScore_DepositedEnergy_IsUsed()   {return UseSignalScore_DepositedEnergy;}
        Bool_t SignalScore_RelativisticBeta_IsUsed()  {return UseSignalScore_RelativisticBeta;}
        Bool_t SignalScore_Rvalue_IsUsed()            {return UseSignalScore_Rvalue;}
        Bool_t SignalScore_TOF_IsUsed()               {return UseSignalScore_TOF;}
        Bool_t SignalScore_KineticEnergy_IsUsed()     {return UseSignalScore_KineticEnergy;}
        
        // ATTENTION: we do not yet have a way to compute Rvalues, etc during runtime and 
        // add these to the scorers! or to evaulate these scores and do reconstruction on
        // the basis sof those scores! we still need to do this. And then, we need to 
        // use this class inside a FairTask.
  
        private:
            // Class content:
            std::vector<ScoringClass*> SignalScorers;
            std::vector<ScoringClass*> ClusterScorers;
            Bool_t Scorers_Initialized;
            TString Key;
            TString ThisDetector;
            
            // Required parameters:
            TString OutputPath;
            TString OutputFile;
            Double_t BeamEperN;
            Double_t BeamBeta;
            Double_t ParticleGun_StartX;
            Double_t ParticleGun_StartY;
            Double_t ParticleGun_StartZ;
            Double_t TargetT;
            Double_t TargetX;
            Double_t TargetY;
            Double_t TargetZ;
            Double_t NeuLAND_Center_X;
            Double_t NeuLAND_Center_Y;
            Double_t NeuLAND_Front_Z;
            Double_t NeuLAND_RotX;
            Double_t NeuLAND_RotY;
            Double_t NeuLAND_RotZ;
            Double_t NeuLAND_Start_Time;
            Double_t NeuLAND_Scintillator_Totalthickness;
            Int_t NeuLAND_nBarsPerPlane;
            Int_t NeuLAND_nDoublePlanes;
            Int_t NeuLAND_nBarsTotal;
            Int_t ScoringResolution;
            TString ParticleType_ToBeDetected;
            Double_t ParticleMass;
            TString ScoreCombination;
            TString SelectPrimHitInCluster;
            
            // Decision on which scorers to take along in the total:
            Bool_t UseClusterScore_DepositedEnergy;
            Bool_t UseClusterScore_Size;
            Bool_t UseClusterScore_KineticEnergy;
            Bool_t UseClusterScore_RelativisticBeta;
            Bool_t UseClusterScore_Rvalue;
            Bool_t UseClusterScore_LogR;
            Bool_t UseClusterScore_TOF;
            Bool_t UseClusterScore_SpaceLength;
            Bool_t UseClusterScore_TimeLength;
            Bool_t UseClusterScore_SpaceRadius;
            
            Bool_t UseSignalScore_DepositedEnergy;
            Bool_t UseSignalScore_KineticEnergy;
            Bool_t UseSignalScore_RelativisticBeta;
            Bool_t UseSignalScore_Rvalue;
            Bool_t UseSignalScore_TOF;

            // Error handling:
            std::vector<TString> Errors;
            Bool_t PrintErrors;
            
            // Auxillary classes:
            R3BInputClass* Inputs;
            Nuclei* TheNuclei;
            TRandom3* Generator;
            

        public:
            // generate ROOT dictionary:
            ClassDef(AllScorers, 1)
};

#endif

