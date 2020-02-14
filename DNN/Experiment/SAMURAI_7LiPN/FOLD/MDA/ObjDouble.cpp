// Start by including the corresponding header:
#include "ObjDouble.h"

// All other files that should be included, should be included in the header.
// Here, we only define the member functions:

// Default constructor:  
ObjDouble::ObjDouble() 
{
    TheNumber = 0.0;
    ThisName = "";
}

// ThisName constructor:
ObjDouble::ObjDouble(TString const Key) 
{
    TheNumber = 0.0;
    ThisName = Key;
}

// Content constructor:
ObjDouble::ObjDouble(TString const Key, Double_t const num) 
{
    ThisName = Key;
    TheNumber = num;
}

// Set-functions:
void ObjDouble::SetNumber(Double_t const num) {TheNumber = num;}
void ObjDouble::SetTheName(TString const Key) {ThisName = Key;}
    
// Get-functions:
Double_t ObjDouble::GetNumber() {return TheNumber;}
TString ObjDouble::GetTheName() {return ThisName;}
    
// Printing function:
void ObjDouble::Print()
{
    cout << "ObjDouble " << ThisName << " = " << TheNumber << "\n";
}

// Writing function:
void ObjDouble::WriteToFile(TFile* TheFile)
{
    // Writes the object to the file:
    if ((TheFile==0)||(TheFile==nullptr))
    {
        cout << " -FATAL: your pointer does not point a file! Nothing could be written!\n";
    }
    else
    {
        if (TheFile->IsOpen()==kFALSE)
        {
            cout << " -FATAL: Your file is not opened! Nothing could be written\n";
        }
        else
        {
            // Then we must check the proper option:
            TString opt = TheFile->GetOption();
            
            if ((opt=="UPDATE")||(opt=="CREATE"))
            {
                // Then, we are fine. Hence, we could write to the file:
                TheFile->cd();
                ObjDouble* temp = new ObjDouble(ThisName,TheNumber);
                temp->Write(ThisName,2);
                delete temp;
            }
            else
            {
                cout << " -FATAL: Your TFile-option = <" << opt << ">. Only <UPDATE> or <CREATE> are supported for writing!\n";
            }
        }
    }
}

// Reading function:
void ObjDouble::ReadFromFile(TFile* TheFile)
{
    // Reads the object from the file:
    if ((TheFile==0)||(TheFile==nullptr))
    {
        cout << " -FATAL: your pointer does not point a file! Nothing could be read!\n";
    }
    else
    {
        if (TheFile->IsOpen()==kFALSE)
        {
            cout << " -FATAL: Your file is not opened! Nothing could be read\n";
        }
        else
        {
            // Then we must check the proper option:
            TString opt = TheFile->GetOption();
            
            if ((opt=="UPDATE")||(opt=="READ"))
            {
                // Then, we are fine. Hence, we could read from the file:
                TheFile->cd();
                ObjDouble* temp = (ObjDouble*) TheFile->Get(ThisName);
                if (temp!=0) {TheNumber = temp->GetNumber();}
                delete temp;
            }
            else
            {
                cout << " -FATAL: Your TFile-option = <" << opt << ">. Only <UPDATE> or <READ> are supported for reading!\n";
            }
        }
    }
}
    
// Calling of the ClassImp-macro to complete the ROOT-inheritance:
ClassImp(ObjDouble)
