TString R3BTextFileGenerator::DoubleIntTotString(Double_t const Input)
{
    // Converts a double that actually should represent an integer
    // to a string. No round-off is made, it is just cutt-off:
    Int_t TheInteger = (Int_t) Input;
    TString st = "";
    TString Answer = st.Itoa(TheInteger,10);
    return Answer;
}
