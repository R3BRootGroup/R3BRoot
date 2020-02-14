void R3BTextFileGenerator::NormClassSelection(TString const Option)
{
    // Decides which normalization we apply:
    if (Option=="None")
    {
        ExecuteNormalization = kFALSE;
        TheNormalization = DefaultNorms;
        TheNormalization->SkipNormalization();
    }
    else if (Option=="Default")
    {
        TheNormalization = DefaultNorms;
    }
    else if (Option=="Learning")
    {
        TheNormalization = LearnAdvancedNorms;
    }
    else if (Option=="Advanced")
    {
        TheNormalization = ApplyAdvancedNorms;
    }
    else if (Option=="Elena")
    {
        TheNormalization = ElenaNorms;
    }
    else
    {
        TheNormalization = DefaultNorms;
    }
}
