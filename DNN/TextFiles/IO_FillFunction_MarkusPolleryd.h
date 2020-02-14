void R3BTextFileGenerator::IO_FillFunction_MarkusPolleryd()
{
    // fills vectors InputNeurons & OutputNeurons on a specific 
    // method with values of the TClonesArrays.
    
    // we need the digitizer array to make this work. Hence:
    if ((Digis_IsLinked[CurrentDigiRun]==kFALSE)||(Signals_IsLinked[CurrentDigiRun]==kFALSE))
    {
        if (Digis_IsLinked[CurrentDigiRun]==kFALSE) {ErrorMessage("The IO MarkusPolleryd fillfunction needs fArrayDigis to work!");}
        if (Signals_IsLinked[CurrentDigiRun]==kFALSE) {ErrorMessage("The IO MarkusPolleryd fillfunction needs fArraySignals to work!");}
    }
    else
    {
        // The first thing we need to do, is to convert the digis to MarkusHits.
        // So we declare what we need:
        Int_t nDigis = fArrayDigis[CurrentDigiRun]->GetEntries();
        R3BNeulandDigi* ThisDigi;
        MarkusHit ThisHit;
        std::vector<MarkusHit> MarkusVector;
        MarkusVector.clear();
        
        // Next, loop over all digis:
        for (Int_t k = 0; k<nDigis; ++k)
        {
            // Retrieve current Digi:
            ThisDigi = (R3BNeulandDigi*) fArrayDigis[CurrentDigiRun]->At(k);
            
            // Transfer information to MarkusHit:
            ThisHit.PMT1_TDC = ThisDigi->GetTdcL();
            ThisHit.PMT2_TDC = ThisDigi->GetTdcR();
            ThisHit.PMT1_QDC = ThisDigi->GetQdcL();
            ThisHit.PMT2_QDC = ThisDigi->GetQdcR();
            
            // Extract plane & bar number:
            ThisHit.BarNumber_InPlane = ThisDigi->GetPaddle() + 1; // To make it start at 1 instead of zero.
            ThisHit.PlaneNumber = 1; // This also has to start at 1, not zero.
            
            while (ThisHit.BarNumber_InPlane > 0)
            {
                ThisHit.BarNumber_InPlane = ThisHit.BarNumber_InPlane - NbarsPerPlane;
                ThisHit.PlaneNumber = ThisHit.PlaneNumber + 1;
            }
            
            // Obviously, this will take it one step too far:
            ThisHit.BarNumber_InPlane = ThisHit.BarNumber_InPlane + NbarsPerPlane;
            ThisHit.PlaneNumber = ThisHit.PlaneNumber - 1;
            
            // Now, the correctly assigned MarkusHit has to be added to the vector:
            MarkusVector.push_back(ThisHit);
        }
        
        // The next step is now to properly sort the vector:
        std::sort(MarkusVector.begin(), MarkusVector.end(), MarkusHit_SortingFunction);
        
        // Then, we have to transfer the information orderly to the InputNeurons-vector:
        for (Int_t k = 0; k<nDigis; ++k)
        {
            // With this order, the structure matches the Hits.txt-file of Markus Polleryd.
            // All we then have to do is put the proper text in between and skip a line 
            // after each hit.
        
            // Decide upon normalization of the other inputs:
            if (ExecuteNormalization==kFALSE)
            {
                InputNeurons.push_back((Int_t) MarkusVector.at(k).PlaneNumber);
                InputNeurons.push_back((Int_t) MarkusVector.at(k).BarNumber_InPlane);
                InputNeurons.push_back(MarkusVector.at(k).PMT1_TDC);
                InputNeurons.push_back(MarkusVector.at(k).PMT2_TDC);
                InputNeurons.push_back(MarkusVector.at(k).PMT1_QDC);
                InputNeurons.push_back(MarkusVector.at(k).PMT2_QDC);
            }
            else
            {
                // learn normalizations:
                LearnAdvancedNorms->AdaptOldScale("SinglePlaneNumber",(Int_t) MarkusVector.at(k).PlaneNumber);
                LearnAdvancedNorms->AdaptOldScale("BarPerPlane",(Int_t) MarkusVector.at(k).BarNumber_InPlane);
                LearnAdvancedNorms->AdaptOldScale(TimeNormalization_NormName.Data(),MarkusVector.at(k).PMT1_TDC);
                LearnAdvancedNorms->AdaptOldScale(TimeNormalization_NormName.Data(),MarkusVector.at(k).PMT2_TDC);
                LearnAdvancedNorms->AdaptOldScale("SignalEnergy",MarkusVector.at(k).PMT1_QDC);
                LearnAdvancedNorms->AdaptOldScale("SignalEnergy",MarkusVector.at(k).PMT2_QDC);
                
                // Next, normalize inputs:
                InputNeurons.push_back(TheNormalization->Normalize("SinglePlaneNumber",(Int_t) MarkusVector.at(k).PlaneNumber));
                InputNeurons.push_back(TheNormalization->Normalize("BarPerPlane",(Int_t) MarkusVector.at(k).BarNumber_InPlane));
                InputNeurons.push_back(TheNormalization->Normalize(TimeNormalization_NormName.Data(),MarkusVector.at(k).PMT1_TDC));
                InputNeurons.push_back(TheNormalization->Normalize(TimeNormalization_NormName.Data(),MarkusVector.at(k).PMT2_TDC));
                InputNeurons.push_back(TheNormalization->Normalize("SignalEnergy",MarkusVector.at(k).PMT1_QDC));
                InputNeurons.push_back(TheNormalization->Normalize("SignalEnergy",MarkusVector.at(k).PMT2_QDC));
            }
        }
        
        // NOTE: The InputNeurons-vector will have a variable length per event. This 
        // is not a problem for the CNN of Markus Polleryd, becuase it works with 
        // pictures that are based on this #hits. we just have to take this into 
        // account during the writing-phase.
        
        // The next phase will be the definition of the output neurons. For this,
        // we need to reconstruct the neutron momenta from the primary signals.
        
        // Declare what we need:
        std::vector<TLorentzVector> NeutronMomenta; NeutronMomenta.clear();
        TLorentzVector ThisMomentum;
        TVector3 PosDiff;
        Int_t nSignals = fArraySignals[CurrentDigiRun]->GetEntries();
        R3BSignal* ThisSignal;
        Double_t TravelTime = 0.0;
        Double_t Distance = 0.0;
        Double_t Theta = 0.0;
        Double_t Phi = 0.0;
        Double_t beta = 0.0;
        Double_t gamma = 0.0;
        Double_t cLight = 29.9792458;
        
        // Then, loop over all signals:
        for (Int_t k = 0; k<nSignals; ++k)
        {
            // Retrieve current signal:
            ThisSignal = (R3BSignal*) fArraySignals[CurrentDigiRun]->At(k);
            
            // Test if it is a primary:
            if (ThisSignal->IsPrimarySim()==kTRUE)
            {
                // Then, reconstruct a neutron momentum from it:
                PosDiff.SetX(ThisSignal->GetPositionX() - Target_X);
                PosDiff.SetY(ThisSignal->GetPositionY() - Target_Y);
                PosDiff.SetZ(ThisSignal->GetPositionZ() - Target_Z);
                
                // Get Theta & Phi:
                Theta = PosDiff.Theta();
                Phi = PosDiff.Phi();
                
                // Obtain the Travel Distance:
                Distance = TMath::Sqrt(PosDiff.X()*PosDiff.X() + PosDiff.Y()*PosDiff.Y() + PosDiff.Z()*PosDiff.Z());
                
                // Obtain the travel time:
                TravelTime = TMath::Abs(ThisSignal->GetTime() - Target_T);
                
                // Obtain relativistic beta:
                beta = Distance/TravelTime;
                beta = beta/cLight;
                
                // Obtain gamma:
                gamma = 1.0/TMath::Sqrt(1.0 - beta*beta);
                
                // Then, build 4-momentum of the neutron:
                ThisMomentum.SetT(NeutronMass*gamma);
                ThisMomentum.SetX(NeutronMass*gamma*TMath::Sin(Theta)*TMath::Cos(Phi));
                ThisMomentum.SetY(NeutronMass*gamma*TMath::Sin(Theta)*TMath::Sin(Phi));
                ThisMomentum.SetZ(NeutronMass*gamma*TMath::Cos(Theta));
                
                // And finally, add it to the vector:
                NeutronMomenta.push_back(ThisMomentum);
                
                // Done.
            }
        }
        
        // Next, enlarge or shrink the vector to the proper size:
        Int_t MomentumSize = NeutronMomenta.size();
        
        if (MomentumSize>MaxMultiplicity) 
        {
            ErrorMessage("Too many signals were labeled as primary hits! This is an inconsistency in the code!");
            NeutronMomenta.resize(MaxMultiplicity);
        }
        else if (MomentumSize<MaxMultiplicity)
        {
            // Then, add pointless neutron vectors untill we reach the maximum.
            // For this purpose, we will use neutrons at rest, but only if this
            // option is enabled:
            if (MarkusFormat_AddRestNeutrons==kTRUE)
            {
                ThisMomentum.SetT(NeutronMass);
                ThisMomentum.SetX(0.0);
                ThisMomentum.SetY(0.0);
                ThisMomentum.SetZ(0.0);
            
                // Then, add them:
                for (Int_t k = 0; k<TMath::Abs(MomentumSize-MaxMultiplicity); ++k)
                {
                    // add neutrons at rest:
                    NeutronMomenta.push_back(ThisMomentum);
                }
            }
        }
        
        // Next, update the MomentumSize:
        MomentumSize = NeutronMomenta.size();
        
        // Now, we will have to fill OutputNeurons with the neutron momenta.
        // the size will always be 4*MaxMultiplicity because we enforced that.
        // Now, fill the vector in the order Z,Y,X,T as was used by Markus Polleryd:
        for (Int_t k = 0; k<MomentumSize; ++k)
        {
            OutputNeurons.push_back(NeutronMomenta.at(k).Z());
            OutputNeurons.push_back(NeutronMomenta.at(k).Y());
            OutputNeurons.push_back(NeutronMomenta.at(k).X());
            OutputNeurons.push_back(NeutronMomenta.at(k).T());
        }
        
        // Done.
    }
}
