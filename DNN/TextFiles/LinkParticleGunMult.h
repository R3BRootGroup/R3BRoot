void R3BTextFileGenerator::LinkParticleGunMult(TClonesArray* fArray)
{
    // Links the fArraySignals to an external TClonesArray*.
    
    // Set default value:
    fArrayParticleGunMult_IsLinked = kFALSE;
    
    // Begin by testing whether the fArray is null-pointer:
    if ((fArray==0)||(fArray==nullptr))
    {
        ErrorMessage("You tried to link fArrayParticleGunMult to an empty pointer!");
    }
    else
    {
        // Then, test whether this indee represents signals:
        TString ArrayName = fArray->GetClass()->GetName();
        
        if (ArrayName=="ObjInteger")
        {
            // Then, we are OK:
            fArrayParticleGunMult_IsLinked = kTRUE;
            fArrayParticleGunMult = fArray;
        }
        else
        {
            ErrorMessage("You tried to link fArrayParticleGunMult to an array that does not contain ObjIntegers!");
        }
    }
}
