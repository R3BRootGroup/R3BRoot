// Include this class header:
#include "R3BInputClass.h"

// Default Constructor:
R3BInputClass::R3BInputClass()
{
    Linked = kFALSE;
    NLines = 0;
    FileName = "";
    Errors.clear();
    PrintErrors = kTRUE;
    PhysicsList_Checked = kFALSE;
    Allow_Composite_PlysicsList = kFALSE;
        
    Double_Descriptions.clear();
    Double_Units.clear();
    Double_Values.clear();
    Integer_Descriptions.clear();
    Integer_Values.clear();
    String_Descriptions.clear();
    String_Values.clear();
    Boolian_Descriptions.clear();
    Boolian_Values.clear();
    
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
}
    
// Destructor:
R3BInputClass::~R3BInputClass() 
{
    Errors.clear();
       
    Double_Descriptions.clear();
    Double_Units.clear();
    Double_Values.clear();
    Integer_Descriptions.clear();
    Integer_Values.clear();
    String_Descriptions.clear();
    String_Values.clear();
    Boolian_Descriptions.clear();
    Boolian_Values.clear();
}

// Member function definitions:
#include "LinkFile.h"
#include "Print.h"
#include "AddInput.h"
#include "ErrorMessage.h"
#include "RoundOff.h"
#include "CreateFile.h"
#include "CreateElenaFile.h"
#include "CreateSAMURAIFile.h"
#include "ReadFile.h"
#include "GetInput.h"
#include "WriteFile.h"
#include "Modify.h"
#include "EraseInput.h"
#include "BeamProperties.h"
#include "CheckPhysicsList.h"

// Generate ROOT dictionary:
ClassImp(R3BInputClass);
