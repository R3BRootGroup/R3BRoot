// Define auxillary structures:
struct MarkusHit
{
    Int_t PlaneNumber;
    Int_t BarNumber_InPlane;
    Double_t PMT1_TDC; // [ns]
    Double_t PMT2_TDC; // [ns]
    Double_t PMT1_QDC; // [MeV]
    Double_t PMT2_QDC; // [MeV]
};

struct ElenaHit
{
    Int_t nPaddle;
    Double_t HitMeanTime; // [ns]
    Double_t HitMeanEnergy; // [MeV]
    Double_t HitMeanLocation; // [cm]
    Double_t SignalScore; // [dim. less]
    Double_t Beta;
    Bool_t PaddleFired; // containes a signal yes/no.
};

// Define sorting function for Markus format:
Bool_t MarkusHit_SortingFunction(const MarkusHit &First, const MarkusHit &Second)
{
    // Sorting function for sorting a vector of MarkusHits to ascending
    // plane number and within there, to ascending bar number.
    
    // Define the answer:
    Bool_t Answer = kFALSE;
    
    // Sort by ascending plane number:
    if (Second.PlaneNumber > First.PlaneNumber) {Answer = kTRUE;}
    else if (Second.PlaneNumber < First.PlaneNumber) {Answer = kFALSE;}
    else
    {
        // meaning that the bar number ase equal. Then, sort by 
        // acending plane number:
        if (Second.BarNumber_InPlane > First.BarNumber_InPlane) {Answer = kTRUE;}
        else if (Second.BarNumber_InPlane < First.BarNumber_InPlane) {Answer = kFALSE;}
        else
        {
            // Then, bar numbers are equal too. Hence, sort by mean TDC:
            Double_t FirstMeanTDC = 0.5*(First.PMT1_TDC + First.PMT2_TDC);
            Double_t SecondMeanTDC = 0.5*(Second.PMT1_TDC + Second.PMT2_TDC);
            
            if (SecondMeanTDC > FirstMeanTDC) {Answer = kTRUE;}
            else {Answer = kFALSE;}
        }
    }
    
    // return answer:
    return Answer;
}

struct ClusterSortStructure
{
    R3BSignalCluster* TheCluster;
    Double_t TheScore;
};

Bool_t ClusterSortFunction_TxT(const ClusterSortStructure &First, const ClusterSortStructure &Second)
{
    // Sort by decreasing cluster score.
    
    // Declare the default:
    Bool_t Answer = kFALSE;
    
    // Do the sorting:
    if (Second.TheScore < First.TheScore) {Answer = kTRUE;}
     
    // return the answer: 
    return Answer;
}

Int_t RandomSortFunction(Int_t const k)
{
    return TheGenerator->Integer(k);
}
