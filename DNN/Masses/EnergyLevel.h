struct GammaTransition
{
    Double_t E_gamma; // [MeV] Energy of the radiated gamma.
    Double_t I_gamma; // [Dim. less] between 0 & 1 ==> Branching fraction of that gamma.
    Double_t E_final; // [MeV] Final energy level after the transition.
    TString  GamMult; // Designation of transition type.
};

struct AngularMomentum
{
    Double_t J;      // [Double_t] Total Angular momentum quantum number of this state.
    TString Parity;  // "+" or "-" ==> Parity of thev state.
    Bool_t Accuracy; // kTRUE if this angular momentum is well-defined (No brakets in NNDC-database)
                     // kFALSE if there are brakets ==> Hence, not well-defined.
    Bool_t Jdef;     // kTRUE if J is defined for this state and kFALSE if J is fully unknown (Parity might be known in these cases).
};

struct EnergyLevel
{
    Double_t Energy;                     // [MeV] Excitation-energy of this level.
    Double_t LifeTime;                   // [sec] T-1/2 of this energy state (Gamma-decay).
    Double_t BGT;                        // Gamow-Teller strength of this level (NOT NNDC information) through (3He,t)-reaction at 140 MeV/u.
    Double_t BF;                         // Fermi-strength of this level (NOT NNDC information) through (3He,t)-reaction at 140 MeV/u.
    Int_t Key;                           // KeyNumber used to identify this level. No two energy levels may have the same keynumber!
    Bool_t BGT_spec;                     // Keeps track on whether BGT actually was specified yes/no.
    Bool_t BF_spec;                      // Keeps track on whether BF actually was specified yes/no.
    TString References;                  // Holds the ABCD... letters that contain the NNDC reference sources.
    std::vector<AngularMomentum> AngMom; // Angular momentum of this level. Some levels have more then 1 possible J-value, so this has to be a vector.
    std::vector<GammaTransition> Gamma;  // Possible Gamma photons that can be emitted from this state. Multiple photons can be emitted, so this is a vector.
};