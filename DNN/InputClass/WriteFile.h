void R3BInputClass::WriteFile()
{
    // Test if we are linked:
    if (Linked==kFALSE)
    {
        ErrorMessage("You did not link a .txt-file to the class. Writing is impossible!");
    }
    else
    {
        // Then, test if the class content is not empty:
        if ((Double_Descriptions.size()==0)&&(Integer_Descriptions.size()==0)&&(Boolian_Descriptions.size()==0)&&(String_Descriptions.size()==0))
        {
            ErrorMessage("You tried to write zero content to the linked file. This is not allowed!");
        }
        else
        {
            // This function writes the current class content to the .txt-file.
        
            // Begin by recreating a new .txt-file:
            std::ofstream TheFile (FileName.Data(), std::ofstream::out);
            NLines = 0;
            
            // Next, we will add all class content to this file:
            TString Total = "";
            TString st = "";
            
            for (Int_t k = 0; k<Double_Descriptions.size(); ++k)
            {
                Total = "DESCRIPTION=" + Double_Descriptions.at(k) + "; TYPE=double; UNIT=" + Double_Units.at(k) + "; VALUE=" + RoundOff(Double_Values.at(k),4) + ";\n";
                TheFile << Total.Data();
                NLines = NLines + 1;
            }
            
            for (Int_t k = 0; k<Integer_Descriptions.size(); ++k)
            {
                Total = "DESCRIPTION=" + Integer_Descriptions.at(k) + "; TYPE=int; VALUE=" + st.Itoa(Integer_Values.at(k),10) + ";\n";
                TheFile << Total.Data();
                NLines = NLines + 1;
            }
            
            for (Int_t k = 0; k<Boolian_Descriptions.size(); ++k)
            {
                if (Boolian_Values.at(k)==kTRUE)  {Total = "DESCRIPTION=" + Boolian_Descriptions.at(k) + "; TYPE=bool; VALUE=" + "kTRUE" + ";\n";}
                if (Boolian_Values.at(k)==kFALSE) {Total = "DESCRIPTION=" + Boolian_Descriptions.at(k) + "; TYPE=bool; VALUE=" + "kFALSE" + ";\n";}
                TheFile << Total.Data();
                NLines = NLines + 1;
            }
            
            for (Int_t k = 0; k<String_Descriptions.size(); ++k)
            {
                Total = "DESCRIPTION=" + String_Descriptions.at(k) + "; TYPE=string; VALUE=" + String_Values.at(k) + ";\n";
                TheFile << Total.Data();
                NLines = NLines + 1;
            }
            
            TheFile.close();
            
            // Done.
    }
    }
}
    
