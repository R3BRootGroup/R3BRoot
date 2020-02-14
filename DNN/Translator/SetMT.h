void R3BNeuLANDTranslator::SetMT(Int_t const nT, Int_t const cT)
{
    if (nT>0)
    {
        if (cT>=0)
        {
            if (cT<nT)
            {
                TotalNumberOfThreads = nT;
                CurrentThread = cT;
            }
        }
    }
}
