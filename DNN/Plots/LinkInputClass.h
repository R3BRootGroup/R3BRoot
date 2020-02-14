void R3B_DNN_Plots::LinkInputClass(R3BInputClass* inp)
{
    // link an input class to this class. but only if that
    // inputsclass is linked to a file:
    
    if (inp->IsLinked()==kTRUE)
    {
        Inputs = inp;
    }
    else
    {
        ErrorMessage("The inputs-class you offered to the ASCII-Generator was not linked to a File!");
    }
}
