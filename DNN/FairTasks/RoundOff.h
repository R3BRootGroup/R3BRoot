TString R3BRecoTranslator::RoundOff(Double_t const our_input, Int_t const significance)
{
    // converts a Double_t to a TString with a roundoff function.
    // significance should be >= 1 and gives the number of digits that
    // you want to keep. We will use scientific notition by default.
    
    Double_t Lower_Boundary = TMath::Abs(0.5*TMath::Power(10.0,-1.0*((Int_t) (significance-2))));
    if (Lower_Boundary>0.5) {Lower_Boundary = 0.5;}
    Double_t Upper_Boundary = TMath::Abs(100.0 - Lower_Boundary);
    
    // fix in zeros:
    if (our_input<Lower_Boundary)
    {
        TString Answer = "  0";
        
        if (significance>2)
        {
            Answer = Answer + ".0";
            
            if (significance>3)
            {
                for (Int_t k = 3; k<significance; ++k)
                {
                    Answer = Answer + "0";
                }
            }
        }
        
        return Answer;
    }
    
    // fix ones:
    if (our_input>Upper_Boundary)
    {
        TString Answer = "100";
        
        if (significance>2)
        {
            Answer = Answer + ".0";
            
            if (significance>3)
            {
                for (Int_t k = 3; k<significance; ++k)
                {
                    Answer = Answer + "0";
                }
            }
        }
        
        return Answer;
    }
    
    // Before we do anything, extract the sign:
    Bool_t negative = kFALSE;
    Double_t input = our_input;
    if (input<0) {input = -1.0*input; negative = kTRUE;}
    
    // Lets first determine the exponent:
    Int_t kk = 0;
    Int_t exp = 0;
    Bool_t test = kFALSE;
    Double_t low = 0.0;
    Double_t up = 0.0;
    
    while (test == kFALSE)
    {
        // check positive exponents:
        low = TMath::Power(10.0,(Int_t) kk);
        up = TMath::Power(10.0,(Int_t) (kk+1));
        if ((input>=low)&&(input<up)) {exp = kk; test = kTRUE;}
        
        // check negative exponents:
        up = TMath::Power(10.0,-1.0*((Int_t) kk));
        low = TMath::Power(10.0,-1.0*((Int_t) (kk+1)));
        if ((input>=low)&&(input<up)) {exp = -(kk+1); test = kTRUE;}
        
        // increase kk:
        kk = kk+1;
        
        // terminate the loop in case of a really big or small number:
        if (kk>100) {test = kTRUE;}
    }
    
    // in case the loop terminated:
    if ((kk>100)&&(input<1.0)) 
    {
        TString stt = " 0";
        if (significance>2)
        {
            stt = stt + ".0";
            if (significance>3)
            {
                for (Int_t k = 3; k<significance; ++k) {stt = stt + "0";}
            }
        }
        return stt;
    }
    
    if ((kk>100)&&(input>1.0))
    {
        if (negative) {return "-1e99";}
        else {return "+1e99";}
    }
    
    // now continue and test for the significance:
    if (significance<1) {return "--";}
    else
    {
    
    // Now obtain the mantissa of our number:
    Double_t mantissa = input/TMath::Power(10.0,(Int_t) exp);
    
    // cout << mantissa << "\n";
    // cout << exp << "\n";
    
    // Now the next step is to extract all required digits
    // from the mantissa separately:
    Int_t* Digits = new Int_t[significance+1];
    
    Int_t TheDigit = 0;
    Double_t OneDigit = 0.0;
    Double_t TheNumber = mantissa;
    
    // continue one digit further for roundoff-purposes:
    for (Int_t k = 0; k<(significance+1); ++k)
    {
        // cut off the first digit
        // from the mantissa:
        TheDigit = (Int_t) TheNumber;
        
        // write the digit to the array:
        Digits[k] = TheDigit;
        
        // convert the digit back to a double:
        OneDigit = (Int_t) TheDigit;
        
        // compute the difference with the
        // full number:
        TheNumber = TheNumber - OneDigit;
        
        // Then multiply by 10 to re-initialize
        // the loop for the next digit:
        TheNumber = 10.0*TheNumber;
    }
    
    // next, do the roundoff:
    if (Digits[significance]>=5)
    {
        Digits[significance-1] = Digits[significance-1] + 1;
    
        // propagate all the errors through:
        if (significance>1)
        {
            for (Int_t k = 1; k<significance; ++k)
            {
                if (Digits[significance-k]==10)
                {
                    Digits[significance-k] = 0;
                    Digits[significance-k-1] = Digits[significance-k-1] + 1;
                }
            }
        }
        
        // next, if the most significant digit now equals 10, we must
        // pass all the other numbers further:
        if (Digits[0]==10)
        {
            Digits[0] = 1;
            
            if (significance>1)
            {
                for (Int_t k = 1; k<significance; ++k)
                {
                    Digits[significance-k+1] = Digits[significance-k];
                }
                
                Digits[1] = 0;
            }
            
            // and in this rear case, the exponent has to be increased:
            exp = exp + 1;
        }
        
        // now all the roundoff problems are solved and we can
        // compose the answer. Just forget the last element of teh Digits-array.
    }
        
        
    for (Int_t k = 0; k<significance; ++k)
    {
        // cout << "Digit number " << k+1 << " = " << Digits[k] << "\n";
    }
    
    // Finally, compose the string:
    TString st = "";
    TString Answer = st.Itoa(Digits[0],10);
    
    if (input>=10.0)
    {
        Answer = Answer + st.Itoa(Digits[1],10);
        
        if (significance>2)
        {
            Answer = Answer + ".";
            
            for (Int_t k = 2; k<significance; ++k)
            {
                Answer = Answer + st.Itoa(Digits[k],10);
            }
        }
        
        Answer = " " + Answer;
    }
    else if (input>=1.0)
    {
        if (significance>=2)
        {
            Answer = Answer + ".";
            
            for (Int_t k = 1; k<(significance-1); ++k)
            {
                Answer = Answer + st.Itoa(Digits[k],10);
            }
        }
        
        Answer = "  " + Answer;
    }
    else
    {
        Answer = "  0.";
        
        if (significance>2)
        {
            for (Int_t k = 1; k<(significance-1); ++k)
            {
                Answer = Answer + st.Itoa(Digits[k],10);
            }
        }
    }
    
    // Now we have processed all digits:    
    delete Digits;
    
    // Then the next step is now to add a sign:
    if (negative==kTRUE)
    {
        Answer = "-" + Answer;
    }
    else
    {
        //Answer = "+" + Answer;
    }
    
    // Finally, return the answer:
    return Answer;
    
    }
}
