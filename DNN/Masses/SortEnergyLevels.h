Bool_t EnergySortFunction(const EnergyLevel &First, const EnergyLevel &Second)
{
    // Sort by acending energy levels:
    
    // Declare the default:
    Bool_t Answer = kFALSE;
    
    // now do the sorting:
    if (Second.Energy > First.Energy) {Answer = kTRUE;}
    else {Answer = kFALSE;}
         
    // return the answer: 
    return Answer;
}

void Nucleus::SortEnergyLevels()
{
    // sorts the energy levels from the nucleus according to ascending energy levels:
    std::sort(Levels.begin(), Levels.end(), EnergySortFunction);
}