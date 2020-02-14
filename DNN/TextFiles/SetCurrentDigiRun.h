void R3BTextFileGenerator::SetCurrentDigiRun(Int_t const ThisRun)
{
    CurrentDigiRun = 0;
    
    if ((ThisRun>=0)&&(ThisRun<nDigiRuns))
    {
        CurrentDigiRun = ThisRun;
    }
}
