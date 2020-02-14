void R3BTextFileGenerator::SetTheThreads(Int_t const nT, Int_t const cT)
{
    if ((nT>0)&&(cT>=0)&&(cT<nT))
    {
        CurrentThread = cT;
        NumberOfThreads = nT;
    }
}
