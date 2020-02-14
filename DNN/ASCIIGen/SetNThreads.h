void R3BASCIIFileGenerator::SetNThreads(Int_t const nT_tot, Int_t const nT_cur)
{
    // Set the number of threads. A separate file has to be generated for
    // each of the threads:
    
    if ((nT_tot>0)&&(nT_cur>=0)&&(nT_cur<nT_tot))
    {
        TotalNumberOfThreads = nT_tot;
        CurrentThread = nT_cur;
    }
    else
    {
        ErrorMessage("You need to specify a positive number of threads and the current one starts at zero!");
    }
}
