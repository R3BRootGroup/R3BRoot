// Start by including the corresponding header:
#include "ObjBoolian.h"

// All other files that should be included, should be included in the header.
// Here, we only define the member functions:

// Default constructor:  
ObjBoolian::ObjBoolian() 
{
    TheContent = kFALSE;
    ThisName = "";
}

// ThisName constructor:
ObjBoolian::ObjBoolian(TString const Key) 
{
    TheContent = kFALSE;
    ThisName = Key;
}

// Content constructor:
ObjBoolian::ObjBoolian(TString const Key, Bool_t const num) 
{
    ThisName = Key;
    TheContent = num;
}

// Destructor definition:
ObjBoolian::~ObjBoolian()
{
    TheContent = kFALSE;
    ThisName = "";
}

// Set-functions:
void ObjBoolian::SetContent(Bool_t const num) {TheContent = num;}
void ObjBoolian::SetTheName(TString const Key) {ThisName = Key;}
    
// Get-functions:
Bool_t ObjBoolian::GetContent() {return TheContent;}
TString ObjBoolian::GetTheName() {return ThisName;}
    
// Printing function:
void ObjBoolian::Print()
{
    if (TheContent==kTRUE)
    {
        cout << "ObjBoolian " << ThisName << " = kTRUE\n";
    }
    else
    {
        cout << "ObjBoolian " << ThisName << " = kFALSE\n";
    }
}

// Writing function:
void ObjBoolian::WriteToFile(TFile* TheFile)
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
                ObjBoolian* temp = new ObjBoolian(ThisName,TheContent);
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
void ObjBoolian::ReadFromFile(TFile* TheFile)
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
                ObjBoolian* temp = (ObjBoolian*) TheFile->Get(ThisName);
                if (temp!=0) {TheContent = temp->GetContent();}
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
ClassImp(ObjBoolian)
