void R3BTextFileGenerator::Fill_Network_IO()
{
    // Uses the TClonesArrays to fill the vectors with 
    // network IO-values.
    
    // We decide here, which function we call:
         if (InfoUse=="IO_2to6_TradMed_BabyNetwork") {IO_FillFunction_2to6_TradMed_BabyNetwork();}
    else if (InfoUse=="IO_3to6_TradMed_BabyNetwork") {IO_FillFunction_3to6_TradMed_BabyNetwork();}
    else if (InfoUse=="IO_Signals_StandardNetwork")  {IO_FillFunction_Signals_StandardNetwork();}
    else if (InfoUse=="IO_Signals_MultNetwork")      {IO_FillFunction_Signals_MultNetwork();}
    else if (InfoUse=="IO_Signals_MarkusPolleryd")   {IO_FillFunction_MarkusPolleryd();}
    else if (InfoUse=="ScoringPlus")                 {CutOff_ClusterVector = kTRUE;
                                                      ReShuffle_ClusterVector = kTRUE;
                                                      Kickout_EventsWithLostPrimClusters = kTRUE;
                                                      Step2_UseDetectedMultiplicity = kFALSE;
                                                      IO_FillFunction_ScoringPlus();}
    else if (InfoUse=="IO_Signals_ElenaHoemann")     {Include_HorPaddleCoordinate = kFALSE;
                                                      Include_ZeroMult_Output = kFALSE; 
                                                      Include_ZeroMultEvents = kFALSE; if (ValidationMode==kTRUE) {Include_ZeroMultEvents = kTRUE;}
                                                      Include_Multiplicity_MisAligned_Events = kFALSE; if (ValidationMode==kTRUE) {Include_Multiplicity_MisAligned_Events = kTRUE;}
                                                      Include_SigneHitClusters = kFALSE;
                                                      Include_TotalScores = kFALSE;
                                                      Include_SignalScores = kFALSE;
                                                      Include_SignalBetas = kFALSE;
                                                      Eliminate_Tails_InNormalization = kFALSE;
                                                      IO_FillFunction_ElenaHoemann();}
    else if (InfoUse=="IO_Signals_Elena_6004_5")      {Include_HorPaddleCoordinate = kFALSE;
                                                      Include_ZeroMult_Output = kFALSE;
                                                      Include_ZeroMultEvents = kFALSE; if (ValidationMode==kTRUE) {Include_ZeroMultEvents = kTRUE;}
                                                      Include_Multiplicity_MisAligned_Events = kFALSE; if (ValidationMode==kTRUE) {Include_Multiplicity_MisAligned_Events = kTRUE;}
                                                      Include_SigneHitClusters = kTRUE;
                                                      Include_TotalScores = kTRUE;
                                                      Include_SignalScores = kFALSE;
                                                      Include_SignalBetas = kFALSE;
                                                      Eliminate_Tails_InNormalization = kFALSE;
                                                      IO_FillFunction_ElenaHoemann();}
    else if (InfoUse=="IO_Signals_Elena_9002_5")     {Include_HorPaddleCoordinate = kTRUE;
                                                      Include_ZeroMult_Output = kFALSE;
                                                      Include_ZeroMultEvents = kFALSE; if (ValidationMode==kTRUE) {Include_ZeroMultEvents = kTRUE;}
                                                      Include_Multiplicity_MisAligned_Events = kFALSE; if (ValidationMode==kTRUE) {Include_Multiplicity_MisAligned_Events = kTRUE;}
                                                      Include_SigneHitClusters = kTRUE;
                                                      Include_TotalScores = kFALSE;
                                                      Include_SignalScores = kFALSE;
                                                      Include_SignalBetas = kFALSE;
                                                      Eliminate_Tails_InNormalization = kFALSE;
                                                      IO_FillFunction_ElenaHoemann();}
    else if (InfoUse=="IO_Signals_Elena_9004_5")      {Include_HorPaddleCoordinate = kTRUE;
                                                      Include_ZeroMult_Output = kFALSE;
                                                      Include_ZeroMultEvents = kFALSE; if (ValidationMode==kTRUE) {Include_ZeroMultEvents = kTRUE;}
                                                      Include_Multiplicity_MisAligned_Events = kFALSE; if (ValidationMode==kTRUE) {Include_Multiplicity_MisAligned_Events = kTRUE;}
                                                      Include_SigneHitClusters = kTRUE;
                                                      Include_TotalScores = kTRUE;
                                                      Include_SignalScores = kFALSE;
                                                      Include_SignalBetas = kFALSE;
                                                      Eliminate_Tails_InNormalization = kFALSE;
                                                      IO_FillFunction_ElenaHoemann();}
    else if (InfoUse=="IO_Signals_Elena_12004_5")     {Include_HorPaddleCoordinate = kTRUE;
                                                      Include_ZeroMult_Output = kFALSE;
                                                      Include_ZeroMultEvents = kFALSE; if (ValidationMode==kTRUE) {Include_ZeroMultEvents = kTRUE;}
                                                      Include_Multiplicity_MisAligned_Events = kFALSE; if (ValidationMode==kTRUE) {Include_Multiplicity_MisAligned_Events = kTRUE;}
                                                      Include_SigneHitClusters = kTRUE;
                                                      Include_TotalScores = kTRUE;
                                                      Include_SignalScores = kTRUE;
                                                      Include_SignalBetas = kFALSE;
                                                      Eliminate_Tails_InNormalization = kFALSE;
                                                      IO_FillFunction_ElenaHoemann();}
    else {IO_Default_Fillfunction();}
}
