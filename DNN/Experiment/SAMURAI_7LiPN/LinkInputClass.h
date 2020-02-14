void CrossSections_7LiPN::LinkInputClass(R3BInputClass* inp)
{
    // link an input class to this class. but only if that
    // inputsclass is linked to a file:
    Inputs_IsLinked = kFALSE;
    
    if (inp->IsLinked()==kTRUE)
    {
        Inputs = inp;
        Inputs_IsLinked = kTRUE;
    }
    else
    {
        ErrorMessage("The inputs-class you offered to the ASCII-Generator was not linked to a File!");
        Inputs_IsLinked = kFALSE;
    }
}
