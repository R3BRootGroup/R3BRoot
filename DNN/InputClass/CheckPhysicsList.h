void R3BInputClass::AllowCompositePhysicsList() {Allow_Composite_PlysicsList = kTRUE;}
void R3BInputClass::ForbidCompositePhysicsList() {Allow_Composite_PlysicsList = kFALSE;}
Bool_t R3BInputClass::PhysicsList_IsChecked() {return PhysicsList_Checked;}

void R3BInputClass::CheckPhysicsList()
{
    // Checks whether the physics list input is one of the allowed options.
    
    // Re-set the default:
    PhysicsList_Checked = kFALSE;
    
    // Define the allowed options:
    std::vector<TString> R3BRoot_TGeant4_Known_PhysicsLists;
    R3BRoot_TGeant4_Known_PhysicsLists.clear();
    
    // Jan Mayer physics lists shown in Thesis Fig. 6.4:
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("QGSP_BERT_HP");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("QGSP_BIC_HP");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("QGSP_INCLXX_HP");
    
    // Other options used in neutron cross section example of Jan Mayer:
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("FTFP_BERT");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("FTFP_BERT_HP");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("FTFP_INCLXX");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("FTFP_INCLXX_HP");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("FTF_BIC");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("QBBC");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("QGSP_BERT");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("QGSP_BIC");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("QGSP_FTFP_BERT");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("QGSP_INCLXX");
    
    // Physics lists not Included in Jan Mayers Example:
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("ShieldingLEND");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("Shielding");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("ShieldingM");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("NuBeam");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("FTFP_BERT_TRV");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("QGSP_BIC_AllHP");
    R3BRoot_TGeant4_Known_PhysicsLists.push_back("LBE");
    
    // ===========================================================
    
    // Next, test if the input matches:
    if (Linked==kFALSE)
    {
        ErrorMessage("Without a linked file, we cannot check the physics list!");
    }
    else
    {
        // Then, read the inputs:
        ReadFile();
        
        // Next, retrieve the physics list:
        TString TheList = GetInputString("R3BRoot_MonteCarlo_PhysicsList");
        
        // And next, the checkup begins:
        Int_t EqualityCounter = 0;
        Bool_t EqualToKnownList = kFALSE;
        Bool_t ContainsKnownList = kFALSE;
        
        for (Int_t k = 0; k<R3BRoot_TGeant4_Known_PhysicsLists.size(); ++k)
        {
            if (R3BRoot_TGeant4_Known_PhysicsLists.at(k)==TheList)
            {
                EqualityCounter = EqualityCounter + 1;
                EqualToKnownList = kTRUE;
            }
            
            if (TheList.Contains(R3BRoot_TGeant4_Known_PhysicsLists.at(k))==kTRUE)
            {
                ContainsKnownList = kTRUE;
            }
        }
        
        // next, build the full test:
        if (Allow_Composite_PlysicsList==kFALSE)
        {
            // Then it is easy, the physics list must be equal to exactly one options:
            if ((EqualityCounter==1)&&(EqualToKnownList==kTRUE))
            {
                PhysicsList_Checked = kTRUE;
            }
            else
            {
                ErrorMessage("");
                ErrorMessage("Your suppied Physics List <"+TheList+"> should be equal to one of the following options:");
                
                for (Int_t k = 0; k<R3BRoot_TGeant4_Known_PhysicsLists.size(); ++k)
                {
                    ErrorMessage("=======> " + R3BRoot_TGeant4_Known_PhysicsLists.at(k));
                }
                
                ErrorMessage("");
                
                PhysicsList_Checked = kFALSE;
            }
        }
        else
        {
            // Then, we are happy if the list at least contains one valid variable:
            if (ContainsKnownList==kTRUE)
            {
                PhysicsList_Checked = kTRUE;
            }
            else
            {
                ErrorMessage("");
                ErrorMessage("Your suppied Physics List <"+TheList+"> should contain at least one of the following options:");
                
                for (Int_t k = 0; k<R3BRoot_TGeant4_Known_PhysicsLists.size(); ++k)
                {
                    ErrorMessage("=======> " + R3BRoot_TGeant4_Known_PhysicsLists.at(k));
                }
                
                ErrorMessage("");
                
                PhysicsList_Checked = kFALSE;
            }
        }
    
        // Done.
    }
}
