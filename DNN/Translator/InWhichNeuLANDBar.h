// Function to identify in which NeuLAND bar a point is:
// ATTENTION: It raises from 1 to 3000, as is the convention in ALL code!
Int_t R3BNeuLANDTranslator::InWhichNeuLANDBar(Double_t const xpoint_c, Double_t const ypoint_c, Double_t const zpoint_c)
{
    // Declare the Answers:
    Int_t FullNeuLANDIndex = -1;
    Bool_t InsideNeuLAND = kFALSE;
    Bool_t ShowOutput = kFALSE;
    if (ShowOutput==kTRUE) {cout << " -> " << xpoint_c << " | " << ypoint_c << " | " << zpoint_c << "\n";}
    
    // Begin by declaring new points & shifting. We will do exactly the reverse of the geometry
    // building file:
    Int_t nModularSkips = nDoublePlanes/NumberOfDoublePlanes_PerModule; // Both integers, so rounded down as it should be!
    Double_t xpoint = xpoint_c - NeuLAND_Center_X; 
    Double_t ypoint = ypoint_c - NeuLAND_Center_Y; 
    Double_t zpoint = zpoint_c - NeuLAND_Front_Z - NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes); 
    if (DoModularSplit==kTRUE) {zpoint = zpoint - ModularDistance*nModularSkips/2.0;}
    if (ShowOutput==kTRUE) {cout << " -> " << xpoint << " | " << ypoint << " | " << zpoint << "\n";}

    // Now, NeuLAND is in the origin. Anti-rotate:
    TVector3 pos;
    pos.SetX(xpoint);
    pos.SetY(ypoint);
    pos.SetZ(zpoint);
    
    pos.RotateZ(-1.0*NeuLAND_Rot_Z*TMath::Pi()/180.0);
    pos.RotateY(-1.0*NeuLAND_Rot_Y*TMath::Pi()/180.0);
    pos.RotateX(-1.0*NeuLAND_Rot_X*TMath::Pi()/180.0);
    
    xpoint = pos.X();
    ypoint = pos.Y();
    zpoint = pos.Z();
    if (ShowOutput==kTRUE) {cout << " -> " << xpoint << " | " << ypoint << " | " << zpoint << "\n";}
    
    // Now, NeuLAND is at the origin. Check that we are indeed inside NeuLAND:
    
    // Next, shift so that z starts at zero:
    zpoint = zpoint + NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes);
    if (DoModularSplit==kTRUE) {zpoint = zpoint + ModularDistance*nModularSkips/2.0;}
    if (ShowOutput==kTRUE) {cout << " -> " << xpoint << " | " << ypoint << " | " << zpoint << "\n";}
    
    // Check that we are inside the proper range:
    if (((zpoint>0.0)&&(zpoint<NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes)*2.0)&&(DoModularSplit==kFALSE))||((zpoint>0.0)&&(zpoint<NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes)*2.0 +  ModularDistance*nModularSkips)&&(DoModularSplit==kTRUE)))
    {
        // Next, find out in which single plane we are located:
        Int_t CurrentSinglePlane = -1;
        Int_t CurrentBarPlane = -1;
    
        while(zpoint>0.0)
        {
            zpoint = zpoint - NeuLAND_TotalBarThicknessZ;
            CurrentSinglePlane = CurrentSinglePlane + 1;
            if ((DoModularSplit==kTRUE)&&(CurrentSinglePlane!=0)&&(CurrentSinglePlane%(NumberOfDoublePlanes_PerModule*2*nPaddlesPerPlane))) {zpoint = zpoint - ModularDistance;}
        }
        
        if (ShowOutput==kTRUE) {cout << " -> " << xpoint << " | " << ypoint << " | " << zpoint << " | " << CurrentSinglePlane << "\n";}
    
        // CurrentSinglePlane now starts at 0 and ranges to n-1.
        // The next question is now, to identify whether we have
        // a horizontal plane or not:
        if (CurrentSinglePlane%2==0)
        {
            // then, we are in a horizontal plane. these indices 
            // start at the bottom (negative y).
            
            // Check that we indeed are in the proper range:
            if ((ypoint>(-0.5*NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane)))&&(ypoint<(0.5*NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane))))
            {
                if ((xpoint>-0.5*NeuLAND_Total_Bar_Length)&&(xpoint<0.5*NeuLAND_Total_Bar_Length))
                {
                    // Shift so y starts at zero:
                    ypoint = ypoint + NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane)/2.0;
                    
                    if (ShowOutput==kTRUE) {cout << " -> " << xpoint << " | " << ypoint << " | " << zpoint << " | " << CurrentSinglePlane << " | " << CurrentBarPlane << "\n";}
        
                    // and now do the same trick:
                    while (ypoint>0.0)
                    {
                        ypoint = ypoint - NeuLAND_TotalBarThicknessXY;
                        CurrentBarPlane = CurrentBarPlane + 1;
                    }
                    
                    if (ShowOutput==kTRUE) {cout << " -> " << xpoint << " | " << ypoint << " | " << zpoint << " | " << CurrentSinglePlane << " | " << CurrentBarPlane << "\n";}
                    
                    // Now CurrentBarPlane & CurrentSinglePlane hold the answers.
                    // We would not have passed the tests if we are not inside
                    // NeuLAND, so:
                    InsideNeuLAND = kTRUE;
                }
            }
        }
        else
        {
            // Then we have a vertical plane. These indices start
            // at negative x.
            
            // Check that we indeed are in the proper range:
            if ((xpoint>(-0.5*NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane)))&&(xpoint<(0.5*NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane))))
            {
                if ((ypoint>-0.5*NeuLAND_Total_Bar_Length)&&(ypoint<0.5*NeuLAND_Total_Bar_Length))
                {
                    // Shift so x starts at zero:
                    xpoint = xpoint + NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane)/2.0;
                    
                    if (ShowOutput==kTRUE) {cout << " -> " << xpoint << " | " << ypoint << " | " << zpoint << " | " << CurrentSinglePlane << " | " << CurrentBarPlane << "\n";}
        
                    // and now do the same trick:
                    while (xpoint>0.0)
                    {
                        xpoint = xpoint - NeuLAND_TotalBarThicknessXY;
                        CurrentBarPlane = CurrentBarPlane + 1;
                    }
                    
                    if (ShowOutput==kTRUE) {cout << " -> " << xpoint << " | " << ypoint << " | " << zpoint << " | " << CurrentSinglePlane << " | " << CurrentBarPlane << "\n";}
                    
                    // Now CurrentBarPlane & CurrentSinglePlane hold the answers.
                    // We would not have passed the tests if we are not inside
                    // NeuLAND, so:
                    InsideNeuLAND = kTRUE;
                }
            }
        }
        
        // Next, define the total index:
        if (InsideNeuLAND==kTRUE)
        {
            FullNeuLANDIndex = CurrentSinglePlane*nPaddlesPerPlane + CurrentBarPlane;
            
            // Done.
        }
    }
    
    // So now return the answer. Outside of NeuLAND, it will remain -1:
    return FullNeuLANDIndex+1; // ATTENTION: This +1 is what makes it range from 1 till 3000.
}
