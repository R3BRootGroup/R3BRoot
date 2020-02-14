struct PhysicsConstants
{
    Double_t Mp_MeV;
    Double_t Mn_MeV;
    Double_t Me_MeV;
    Double_t Mp_kg;
    Double_t Mn_kg;
    Double_t Me_kg;
    Double_t Mp_uu;
    Double_t Mn_uu;
    Double_t Me_uu;
    Double_t Mu_kg;
    Double_t cc_ms;
    Double_t q_eVJ;
    Double_t q_MeVJ;
};

// NOTE: inline will remove the function from the code, but replaces the
// function calls with these tasks. Should only be done if the number of
// calls is limited. But it will prevent multiple-definition-errors.
inline PhysicsConstants DefineConstants()
{
    // Declare output:
    PhysicsConstants Answer;
    
    // Define output:
    Answer.Mp_kg = 1.672621898e-27;
    Answer.Mn_kg = 1.674927471e-27;
    Answer.Me_kg = 9.109383561e-31;

    Answer.Mu_kg = 1.6605390402e-27;
    Answer.cc_ms = 2.99792458e8;     
    Answer.q_eVJ = 1.60217662e-19; 
    
    Answer.q_MeVJ = Answer.q_eVJ*1e6;
    
    Answer.Mp_uu = Answer.Mp_kg/Answer.Mu_kg;
    Answer.Mn_uu = Answer.Mn_kg/Answer.Mu_kg;
    Answer.Me_uu = Answer.Me_kg/Answer.Mu_kg;
    
    Double_t Conversion = Answer.cc_ms*Answer.cc_ms; // Convert kg to J
    Conversion = Conversion/Answer.q_eVJ;            // Converts T to eV.
    Conversion = Conversion*1e-6;                    // Converts eV to MeV.
    
    Answer.Mp_MeV = Answer.Mp_kg*Conversion;
    Answer.Mn_MeV = Answer.Mn_kg*Conversion;
    Answer.Me_MeV = Answer.Me_kg*Conversion;
    
    // cout << Answer.Mp_MeV - Answer.Mn_MeV << "<======\n";
    
    // Return output:
    return Answer;
}
