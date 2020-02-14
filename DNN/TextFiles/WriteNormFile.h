void NormalizationClass::WriteNormFile(TString const TheFileName)
{
    // Writes the content from the normalization class to a file:
    TString FileName = TheFileName;
    FileName.ReplaceAll(".txt","_NormParams.txt");
    
    std::ofstream TheFile (FileName.Data(), std::ofstream::out);
    
    Int_t VectorSize = TheNormParameters.size();
    
    for (Int_t k = 0; k<VectorSize; ++k)
    {
        TheFile << "NAME=" << TheNormParameters.at(k).Name << "; ";
        TheFile << "OPTION=" << TheNormParameters.at(k).Normalization_Option << "; ";
        TheFile << "OLD_MEANSHIFT=" << TheNormParameters.at(k).Old_MeanShift << "; ";
        TheFile << "OLD_SCALEFACTOR=" << TheNormParameters.at(k).Old_ScaleFactor << "; ";
        TheFile << "NEW_MEANSHIFT=" << TheNormParameters.at(k).New_MeanShift << "; ";
        TheFile << "NEW_SCALEFACTOR=" << TheNormParameters.at(k).New_ScaleFactor << "; ";
        TheFile << "\n";
    }
    
    TheFile.close();
}
