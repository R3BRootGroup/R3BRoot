// This header contains the functions to determine the
// multiplicity of the traditional method reconstruction
// using the DNN outcome.

// Function to extract the multiplicity inside the Exec-function:
Int_t R3B_TradMed_NeutronTracker::GetDNNMultiplicity()
{
    ObjInteger* ThisObjInt = (ObjInteger*) fArrayMult->At(0);
    Int_t TheMultiplicity = ThisObjInt->GetNumber();
    return TheMultiplicity;
}
    
