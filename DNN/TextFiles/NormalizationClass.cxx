// Include this class header:
#include "NormalizationClass.h"

// Default Constructor:
NormalizationClass::NormalizationClass()
{
    // Parameter definitions:
    TheNormParameters.clear();
    
    ExecuteNormalization = kTRUE;
    DataDriven_MeanTime = 0.0;
    DataDriven_ScaleTime = 1.0;
    TheScorers = 0;
}
    
// Destructor:
NormalizationClass::~NormalizationClass() 
{
    // Parameter destructions:
    TheNormParameters.clear();
}

// Member function definitions:
#include "WriteNormFile.h"
#include "ReadNormFile.h"
#include "DefaultNorms.h"
#include "ElenaNorms.h"

void NormalizationClass::AddParameter(TString const Name, TString const Opt, Double_t const OldMean, Double_t const OldScale, Double_t const NewMean, Double_t const NewScale)
{
    Int_t Counter = 0;
    Int_t Size = TheNormParameters.size();
    
    for (Int_t k = 0; k<Size; ++k)
    {
        if (Name==TheNormParameters.at(k).Name) {Counter = Counter + 1;}
    }
    
    if (Counter>0)
    {
        cout << "### NORMALIZATION CLASS ERROR: You tried to create a parameter <"+Name+"> that already exists!\n";
    }
    else
    {
        NormUnit ThisUnit;
    
        ThisUnit.Name = Name;
        ThisUnit.Normalization_Option = Opt;
        ThisUnit.Old_MeanShift = OldMean;
        ThisUnit.Old_ScaleFactor = OldScale;
        ThisUnit.New_MeanShift = NewMean;
        ThisUnit.New_ScaleFactor = NewScale;
        ThisUnit.ControlHistogram = 0;
    
        TheNormParameters.push_back(ThisUnit);
    }
}

void NormalizationClass::AddBlankPar(TString const Name)
{
    Int_t Counter = 0;
    Int_t Size = TheNormParameters.size();
    
    for (Int_t k = 0; k<Size; ++k)
    {
        if (Name==TheNormParameters.at(k).Name) {Counter = Counter + 1;}
    }
    
    if (Counter>0)
    {
        cout << "### NORMALIZATION CLASS ERROR: You tried to create a parameter <"+Name+"> that already exists!\n";
    }
    else
    {
        NormUnit ThisUnit;
    
        ThisUnit.Name = Name;
        ThisUnit.Normalization_Option = "Linear";
        ThisUnit.Old_MeanShift = 0.0;
        ThisUnit.Old_ScaleFactor = 1e-98;
        ThisUnit.New_MeanShift = 0.0;
        ThisUnit.New_ScaleFactor = 1e-98;
        ThisUnit.ControlHistogram = 0;
    
        TheNormParameters.push_back(ThisUnit);
    }
}

void NormalizationClass::AdaptOldScale(TString const Name, Double_t const Value)
{
    // Adapts the old boundaries by incorportaing Value. Options are not processed,
    // since we do not know when to compress with arctan & when to learn new 
    // boundaries in this system. That has to be user-specified.
    
    // Find appropriate index:
    Int_t Counter = 0;
    Int_t Index = -1;
    Int_t Size = (Int_t) TheNormParameters.size();
    
    for (Int_t k = 0; k<Size; ++k)
    {
        if (Name==TheNormParameters.at(k).Name)
        {
            Counter = Counter + 1;
            Index = k;
        }
    }
    
    if (Counter==0)
    {
        cout << "### NORMALIZATION CLASS ERROR: The parameter you requested does not exist!\n";
    }
    else if (Counter>=2)
    {
        cout << "### NORMALIZATION CLASS ERROR: The parameter you requested exists multiple times!\n";
    }
    else
    {
        // Now we are in busyness:
        if ((TMath::Abs(TheNormParameters.at(Index).Old_MeanShift)<1e-99)&&
            (TMath::Abs(TheNormParameters.at(Index).Old_ScaleFactor)<1e-97))
        {
            // Then, no previous values have been used so far, hence:
            TheNormParameters.at(Index).Old_MeanShift = Value;
            TheNormParameters.at(Index).Old_ScaleFactor = 1e-98;
        }
        else
        {
            // Then, we have to do the real work:
            Double_t Left = TheNormParameters.at(Index).Old_MeanShift - TheNormParameters.at(Index).Old_ScaleFactor;
            Double_t Right = TheNormParameters.at(Index).Old_MeanShift + TheNormParameters.at(Index).Old_ScaleFactor;
            
            if (Value<Left)
            {
                Left = Value;
            }
            else if (Value>Right)
            {
                Right = Value;
            }
            else if ((Left<=Value)&&(Right>=Value))
            {
                // Do nothing; we are fine.
            }
            else
            {
                cout << "### NORMALIZATION CLASS ERROR: Inconsistency problem! | Value = " << Value << " | Left = " << Left << " | Right = " << Right << "\n";
            }
            
            TheNormParameters.at(Index).Old_MeanShift = 0.5*(Left+Right);
            TheNormParameters.at(Index).Old_ScaleFactor = 0.5*TMath::Abs(Right-Left);
            
            // Scaling has now been performed.
        }
    }
}

void NormalizationClass::AdaptNewScale(TString const Name, Double_t const Value)
{
    // Adapts the old boundaries by incorportaing Value. Options are not processed,
    // since we do not know when to compress with arctan & when to learn new 
    // boundaries in this system. That has to be user-specified.
    
    // Find appropriate index:
    Int_t Counter = 0;
    Int_t Index = -1;
    Int_t Size = TheNormParameters.size();
    
    for (Int_t k = 0; k<Size; ++k)
    {
        if (Name==TheNormParameters.at(k).Name)
        {
            Counter = Counter + 1;
            Index = k;
        }
    }
    
    if (Counter==0)
    {
        cout << "### NORMALIZATION CLASS ERROR: The parameter you requested does not exist!\n";
    }
    else if (Counter>=2)
    {
        cout << "### NORMALIZATION CLASS ERROR: The parameter you requested exists multiple times!\n";
    }
    else
    {
        // Now we are in busyness:
        if ((TMath::Abs(TheNormParameters.at(Index).New_MeanShift)<1e-99)&&
            (TMath::Abs(TheNormParameters.at(Index).New_ScaleFactor)<1e-97))
        {
            // Then, no previous values have been used so far, hence:
            TheNormParameters.at(Index).New_MeanShift = Value;
            TheNormParameters.at(Index).New_ScaleFactor = 1e-98;
        }
        else
        {
            // Then, we have to do the real work:
            Double_t Left = TheNormParameters.at(Index).New_MeanShift - TheNormParameters.at(Index).New_ScaleFactor;
            Double_t Right = TheNormParameters.at(Index).New_MeanShift + TheNormParameters.at(Index).New_ScaleFactor;
            
            if (Value<Left)
            {
                Left = Value;
            }
            
            if (Value>Right)
            {
                Right = Value;
            }
            else if ((Left<=Value)&&(Right>=Value))
            {
                // Do nothing; we are fine.
            }
            else
            {
                cout << "### NORMALIZATION CLASS ERROR: Inconsistency problem! | Value = " << Value << " | Left = " << Left << " | Right = " << Right << "\n";
            }
            
            TheNormParameters.at(Index).New_MeanShift = 0.5*(Left+Right);
            TheNormParameters.at(Index).New_ScaleFactor = 0.5*TMath::Abs(Right-Left);
            
            // Scaling has now been performed.
        }
    }
}

void NormalizationClass::SetOldScale(TString const Name, Double_t const Left, Double_t const Right)
{
    // sets the Old scales to predefined boundaries.
    
    // Find appropriate index:
    Int_t Counter = 0;
    Int_t Index = -1;
    Int_t Size = TheNormParameters.size();
    
    for (Int_t k = 0; k<Size; ++k)
    {
        if (Name==TheNormParameters.at(k).Name)
        {
            Counter = Counter + 1;
            Index = k;
        }
    }
    
    if (Counter==0)
    {
        cout << "### NORMALIZATION CLASS ERROR: The parameter you requested does not exist!\n";
    }
    else if (Counter>=2)
    {
        cout << "### NORMALIZATION CLASS ERROR: The parameter you requested exists multiple times!\n";
    }
    else
    {
        // let's go:
        TheNormParameters.at(Index).Old_MeanShift = 0.5*(Left+Right);
        TheNormParameters.at(Index).Old_ScaleFactor = 0.5*TMath::Abs(Right-Left);
    }
}

void NormalizationClass::SetNewScale(TString const Name, Double_t const Left, Double_t const Right)
{
    // sets the New scales to predefined boundaries.
    
    // Find appropriate index:
    Int_t Counter = 0;
    Int_t Index = -1;
    Int_t Size = TheNormParameters.size();
    
    for (Int_t k = 0; k<Size; ++k)
    {
        if (Name==TheNormParameters.at(k).Name)
        {
            Counter = Counter + 1;
            Index = k;
        }
    }
    
    if (Counter==0)
    {
        cout << "### NORMALIZATION CLASS ERROR: The parameter you requested does not exist!\n";
    }
    else if (Counter>=2)
    {
        cout << "### NORMALIZATION CLASS ERROR: The parameter you requested exists multiple times!\n";
    }
    else
    {
        // let's go:
        TheNormParameters.at(Index).New_MeanShift = 0.5*(Left+Right);
        TheNormParameters.at(Index).New_ScaleFactor = 0.5*TMath::Abs(Right-Left);
    }
}
            
Double_t NormalizationClass::Normalize(TString const Name, Double_t const Value)
{
    // Decide if we actually do the normalization:
    if (ExecuteNormalization==kFALSE)
    {
        return Value;
    }
    else
    {
    
    // Normalizes Value from the old range to the new one. We have different normalization options:
    // Linear, Arctan, Cutoff.
    Double_t Answer = -1.0*1e99;
    
    // Find appropriate index:
    Int_t Counter = 0;
    Int_t Index = -1;
    Int_t Size = TheNormParameters.size();
    
    for (Int_t k = 0; k<Size; ++k)
    {
        if (Name==TheNormParameters.at(k).Name)
        {
            Counter = Counter + 1;
            Index = k;
        }
    }
    
    if (Counter==0)
    {
        cout << "### NORMALIZATION CLASS ERROR: The parameter you requested does not exist!\n";
    }
    else if (Counter>=2)
    {
        cout << "### NORMALIZATION CLASS ERROR: The parameter you requested exists multiple times!\n";
    }
    else
    {
        // Begin by creating the control histogram if it was not yet there:
        if ((TheNormParameters.at(Index).ControlHistogram==0)||(TheNormParameters.at(Index).ControlHistogram==nullptr))
        {
            Double_t Nbins = 200;
            Double_t Xmin = TheNormParameters.at(Index).New_MeanShift - 2.0*TheNormParameters.at(Index).New_ScaleFactor;
            Double_t Xmax = TheNormParameters.at(Index).New_MeanShift + 4.0*TheNormParameters.at(Index).New_ScaleFactor;
            
            TheNormParameters.at(Index).ControlHistogram = new TH1D(TheNormParameters.at(Index).Name+"_ControlHistogram",TheNormParameters.at(Index).Name+"_ControlHistogram",Nbins,Xmin,Xmax);
        }
        
        // Now we have to do the normalization:
        Double_t NewValue = Value;
        
        // Subtract the shift:
        NewValue = NewValue - TheNormParameters.at(Index).Old_MeanShift;
        
        // Scale to [-1,1]:
        NewValue = NewValue/TheNormParameters.at(Index).Old_ScaleFactor;
        
        // Next, process the options:
        if (TheNormParameters.at(Index).Normalization_Option=="Linear")
        {
            // No problem. Do not do anything.
        }
        else if (TheNormParameters.at(Index).Normalization_Option=="DoubleArctan")
        {
            // Then, suppress the upp and lower values in the last 5 %:
            if (NewValue>0.95)
            {
                Double_t CompressedValue = 0.95 + (0.1/TMath::Pi())*TMath::ATan(NewValue-0.95);
                NewValue = CompressedValue;
            }
            else if (NewValue<-0.95)
            {
                Double_t CompressedValue = -0.95 + (0.1/TMath::Pi())*TMath::ATan(NewValue+0.95);
                NewValue = CompressedValue;
            }
        }
        else if (TheNormParameters.at(Index).Normalization_Option=="UpperArctan")
        {
            // Then, suppress the upp and lower values in the last 5 %:
            if (NewValue>0.95)
            {
                Double_t CompressedValue = 0.95 + (0.1/TMath::Pi())*TMath::ATan(NewValue-0.95);
                NewValue = CompressedValue;
            }
        }
        else if (TheNormParameters.at(Index).Normalization_Option=="LowerArctan")
        {
            // Then, suppress the upp and lower values in the last 5 %:
            if (NewValue<-0.95)
            {
                Double_t CompressedValue = -0.95 + (0.1/TMath::Pi())*TMath::ATan(NewValue+0.95);
                NewValue = CompressedValue;
            }
        }
        else if (TheNormParameters.at(Index).Normalization_Option=="DoubleCutoff")
        {
            if (NewValue<-1.0) {NewValue = -1.0;}
            else if (NewValue>1.0) {NewValue = 1.0;}
        }
        else if (TheNormParameters.at(Index).Normalization_Option=="UpperCutoff")
        {
            if (NewValue>1.0) {NewValue = 1.0;}
        }
        else if (TheNormParameters.at(Index).Normalization_Option=="LowerCutoff")
        {
            if (NewValue<-1.0) {NewValue = -1.0;}
        }
        else if (TheNormParameters.at(Index).Normalization_Option=="UpperReset")
        {
            if (NewValue>1.0) {NewValue = -1.0;}
        }
        else if (TheNormParameters.at(Index).Normalization_Option=="LowerCutoff_UpperReset")
        {
            if (NewValue<-1.0) {NewValue = -1.0;}
            else if (NewValue>1.0) {NewValue = -1.0;}
        }
        else
        {
            cout << "### NORMALIZATION CLASS ERROR: The Normalization option <"+TheNormParameters.at(Index).Normalization_Option+"> you requested does not exist!\n";
        }
        
        // Next, rescale to the new range:
        NewValue = NewValue*TheNormParameters.at(Index).New_ScaleFactor;
        
        // And shift:
        NewValue = NewValue + TheNormParameters.at(Index).New_MeanShift;
        
        // Give asnwer:
        TheNormParameters.at(Index).ControlHistogram->Fill(NewValue);
        Answer = NewValue;
    }
    
    // return Answer:
    return Answer;
    }
}

NormUnit* NormalizationClass::GetNormPar(Int_t const Index)
{
    if ((Index<0)||(Index>=TheNormParameters.size())) {return 0;}
    else
    {
        return &(TheNormParameters.at(Index));
    }
}
        
// Generate ROOT dictionary:
ClassImp(NormalizationClass);
