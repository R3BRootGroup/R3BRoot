// Define plotting structure:
struct PlotInfo
{
    TString HistName;
    TString LegendaText;
    Int_t TheColor;
    Bool_t IncludePlot;
};

void R3B_DNN_Plots::ErelSpect(Int_t const Multiplicity, TString const CompMethod = "Gun")
{
    // Plots the relative energy spectra with legenda.
    
    // Definition of the plotting information:
    TString st = "";
    TString MultStr = st.Itoa(Multiplicity,10);
    
    std::vector<PlotInfo> ThePlots; ThePlots.clear();
    PlotInfo ThisPlot;
    
    TString ClusterHead = Inputs->GetInputString("NeuLAND_Scoring_HowToFind_PrimSignalWithinCluster");
    
    Bool_t IncludeGunPlots = kFALSE; if (CompMethod=="Gun") {IncludeGunPlots = kTRUE;}
    Bool_t IncludeDetPlots = kFALSE; if (CompMethod=="Det") {IncludeDetPlots = kTRUE;}
    Bool_t IncludeExpPlots = kFALSE; if (CompMethod=="Exp") {IncludeExpPlots = kTRUE;}
    
    // -----------------
    
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_MCData; ThisPlot.HistName = "MCNeutronTracks_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Gun)> & Rec.=<MCtracks>"; ThisPlot.TheColor = kBlack; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_MCData; ThisPlot.HistName = "MCNeutronTracks_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Gun)> & Rec.=<MCtracks>"; ThisPlot.TheColor = kBlack; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_MCData; ThisPlot.HistName = "MCNeutronTracks_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Gun)> & Rec.=<MCtracks>"; ThisPlot.TheColor = kBlack; ThePlots.push_back(ThisPlot);
    
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_Perfect_Signals; ThisPlot.HistName = "NeutronTracks_PerfectMethod_Signals_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<Sim>/Signals"; ThisPlot.TheColor = kRed; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_Perfect_Signals; ThisPlot.HistName = "NeutronTracks_PerfectMethod_Signals_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<Sim>/Signals"; ThisPlot.TheColor = kRed; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_Perfect_Signals; ThisPlot.HistName = "NeutronTracks_PerfectMethod_Signals_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<Sim>/Signals"; ThisPlot.TheColor = kRed; ThePlots.push_back(ThisPlot);
    
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_Perfect_Clusters; ThisPlot.HistName = "NeutronTracks_PerfectMethod_Clusters_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<Sim>/Clusters("+ClusterHead+")"; ThisPlot.TheColor = kGreen+2; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_Perfect_Clusters; ThisPlot.HistName = "NeutronTracks_PerfectMethod_Clusters_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<Sim>/Clusters("+ClusterHead+")"; ThisPlot.TheColor = kGreen+2; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_Perfect_Clusters; ThisPlot.HistName = "NeutronTracks_PerfectMethod_Clusters_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<Sim>/Clusters("+ClusterHead+")"; ThisPlot.TheColor = kGreen+2; ThePlots.push_back(ThisPlot);
    
    // -----------------
    
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_TradMed_Clusters_CutsMult; ThisPlot.HistName = "NeutronTracks_TradMed_Clusters_CutsMult_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Cuts> & Rec.=<TDR>/Clusters"; ThisPlot.TheColor = kBlue; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_TradMed_Clusters_DNNMult; ThisPlot.HistName = "NeutronTracks_TradMed_Clusters_DNNMult_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<TDR>/Clusters"; ThisPlot.TheColor = kCyan+1; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_TradMed_Clusters_PerfectMult; ThisPlot.HistName = "NeutronTracks_TradMed_Clusters_PerfectMult_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<TDR>/Clusters"; ThisPlot.TheColor = kCyan-1; ThePlots.push_back(ThisPlot);
    
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_ScoringPlus_Clusters_CutsMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Clusters_CutsMult_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Cuts> & Rec.=<Scoring+>/Clusters"; ThisPlot.TheColor = kMagenta; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_ScoringPlus_Clusters_DNNMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Clusters_DNNMult_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<Scoring+>/Clusters"; ThisPlot.TheColor = kMagenta+2; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_ScoringPlus_Clusters_PerfectMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Clusters_PerfectMult_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<Scoring+>/Clusters"; ThisPlot.TheColor = kMagenta-1; ThePlots.push_back(ThisPlot);
    
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_ScoringPlus_Signals_CutsMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Signals_CutsMult_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Cuts> & Rec.=<Scoring+>/Signals"; ThisPlot.TheColor = kOrange; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_ScoringPlus_Signals_DNNMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Signals_DNNMult_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<<Scoring+>/Signals"; ThisPlot.TheColor = kOrange-1; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_ScoringPlus_Signals_PerfectMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Signals_PerfectMult_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & <Scoring+>/Signals"; ThisPlot.TheColor = kOrange-3; ThePlots.push_back(ThisPlot);
     
    // -----------------

    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_TradMed_Clusters_CutsMult; ThisPlot.HistName = "NeutronTracks_TradMed_Clusters_CutsMult_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Cuts> & Rec.=<TDR>/Clusters"; ThisPlot.TheColor = kBlue; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_TradMed_Clusters_DNNMult; ThisPlot.HistName = "NeutronTracks_TradMed_Clusters_DNNMult_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<TDR>/Clusters"; ThisPlot.TheColor = kCyan+1; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_TradMed_Clusters_PerfectMult; ThisPlot.HistName = "NeutronTracks_TradMed_Clusters_PerfectMult_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<TDR>/Clusters"; ThisPlot.TheColor = kCyan-1; ThePlots.push_back(ThisPlot);
    
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_ScoringPlus_Clusters_CutsMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Clusters_CutsMult_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Cuts> & Rec.=<Scoring+>/Clusters"; ThisPlot.TheColor = kMagenta; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_ScoringPlus_Clusters_DNNMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Clusters_DNNMult_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<Scoring+>/Clusters"; ThisPlot.TheColor = kMagenta+2; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_ScoringPlus_Clusters_PerfectMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Clusters_PerfectMult_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<Scoring+>/Clusters"; ThisPlot.TheColor = kMagenta-1; ThePlots.push_back(ThisPlot);
    
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_ScoringPlus_Signals_CutsMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Signals_CutsMult_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Cuts> & Rec.=<Scoring+>/Signals"; ThisPlot.TheColor = kOrange; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_ScoringPlus_Signals_DNNMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Signals_DNNMult_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<<Scoring+>/Signals"; ThisPlot.TheColor = kOrange-1; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_ScoringPlus_Signals_PerfectMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Signals_PerfectMult_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & <Scoring+>/Signals"; ThisPlot.TheColor = kOrange-3; ThePlots.push_back(ThisPlot);
      
    // -----------------
    
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_TradMed_Clusters_CutsMult; ThisPlot.HistName = "NeutronTracks_TradMed_Clusters_CutsMult_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Cuts> & Rec.=<TDR>/Clusters"; ThisPlot.TheColor = kBlue; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_TradMed_Clusters_DNNMult; ThisPlot.HistName = "NeutronTracks_TradMed_Clusters_DNNMult_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<TDR>/Clusters"; ThisPlot.TheColor = kCyan+1; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_TradMed_Clusters_PerfectMult; ThisPlot.HistName = "NeutronTracks_TradMed_Clusters_PerfectMult_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<TDR>/Clusters"; ThisPlot.TheColor = kCyan-1; ThePlots.push_back(ThisPlot);
    
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_ScoringPlus_Clusters_CutsMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Clusters_CutsMult_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Cuts> & Rec.=<Scoring+>/Clusters"; ThisPlot.TheColor = kMagenta; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_ScoringPlus_Clusters_DNNMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Clusters_DNNMult_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<Scoring+>/Clusters"; ThisPlot.TheColor = kMagenta+2; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_ScoringPlus_Clusters_PerfectMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Clusters_PerfectMult_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & Rec.=<Scoring+>/Clusters"; ThisPlot.TheColor = kMagenta-1; ThePlots.push_back(ThisPlot);
    
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_ScoringPlus_Signals_CutsMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Signals_CutsMult_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Cuts> & Rec.=<Scoring+>/Signals"; ThisPlot.TheColor = kOrange; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_ScoringPlus_Signals_DNNMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Signals_DNNMult_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<<Scoring+>/Signals"; ThisPlot.TheColor = kOrange-1; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_ScoringPlus_Signals_PerfectMult; ThisPlot.HistName = "NeutronTracks_ScoringPlus_Signals_PerfectMult_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Sim(Det.)> & <Scoring+>/Signals"; ThisPlot.TheColor = kOrange-3; ThePlots.push_back(ThisPlot);
    
    // -----------------
    
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_DNNScoringPlus; ThisPlot.HistName = "NeutronTracks_DNNScoringPlus_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<DNNScoring+>"; ThisPlot.TheColor = kYellow+2; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_DNNScoringPlus; ThisPlot.HistName = "NeutronTracks_DNNScoringPlus_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<DNNScoring+>"; ThisPlot.TheColor = kYellow+2; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_DNNScoringPlus; ThisPlot.HistName = "NeutronTracks_DNNScoringPlus_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<DNNScoring+>"; ThisPlot.TheColor = kYellow+2; ThePlots.push_back(ThisPlot);
    
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_DNNScoringPlus_Max; ThisPlot.HistName = "NeutronTracks_DNNScoringPlus_Max_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<Max. DNNScoring+>"; ThisPlot.TheColor = kYellow+3; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_DNNScoringPlus_Max; ThisPlot.HistName = "NeutronTracks_DNNScoringPlus_Max_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<Max. DNNScoring+>"; ThisPlot.TheColor = kYellow+3; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_DNNScoringPlus_Max; ThisPlot.HistName = "NeutronTracks_DNNScoringPlus_Max_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<DNN> & Rec.=<Max. DNNScoring+>"; ThisPlot.TheColor = kYellow+3; ThePlots.push_back(ThisPlot);
    
    // -----------------
    
    ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_BetaReconstruction_NeuLAND; ThisPlot.HistName = "NeutronTracks_BetaReconstruction_NeuLAND_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Beta> & Rec.=<NeuLAND>"; ThisPlot.TheColor = kGray+0; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_BetaReconstruction_NeuLAND; ThisPlot.HistName = "NeutronTracks_BetaReconstruction_NeuLAND_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Beta> & Rec.=<NeuLAND>"; ThisPlot.TheColor = kGray+1; ThePlots.push_back(ThisPlot);
    ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_BetaReconstruction_NeuLAND; ThisPlot.HistName = "NeutronTracks_BetaReconstruction_NeuLAND_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Beta> & Rec.=<NeuLAND>"; ThisPlot.TheColor = kGray+2; ThePlots.push_back(ThisPlot);
    
    if (UseNEBULA==kTRUE)
    {
        ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_BetaReconstruction_NEBULA; ThisPlot.HistName = "NeutronTracks_BetaReconstruction_NEBULA_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Beta> & Rec.=<NeuLAND>"; ThisPlot.TheColor = kGray+0; ThePlots.push_back(ThisPlot);
        ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_BetaReconstruction_NEBULA; ThisPlot.HistName = "NeutronTracks_BetaReconstruction_NEBULA_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Beta> & Rec.=<NeuLAND>"; ThisPlot.TheColor = kGray+1; ThePlots.push_back(ThisPlot);
        ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_BetaReconstruction_NEBULA; ThisPlot.HistName = "NeutronTracks_BetaReconstruction_NEBULA_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Beta> & Rec.=<NeuLAND>"; ThisPlot.TheColor = kGray+2; ThePlots.push_back(ThisPlot);
   
        ThisPlot.IncludePlot = IncludeExpPlots&&Include_ErelSpectrum_BetaReconstruction_Combined; ThisPlot.HistName = "NeutronTracks_BetaReconstruction_Combined_Erel_Spectrum_ExpMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Beta> & Rec.=<NeuLAND>"; ThisPlot.TheColor = kGray+0; ThePlots.push_back(ThisPlot);
        ThisPlot.IncludePlot = IncludeDetPlots&&Include_ErelSpectrum_BetaReconstruction_Combined; ThisPlot.HistName = "NeutronTracks_BetaReconstruction_Combined_Erel_Spectrum_DetMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Beta> & Rec.=<NeuLAND>"; ThisPlot.TheColor = kGray+1; ThePlots.push_back(ThisPlot);
        ThisPlot.IncludePlot = IncludeGunPlots&&Include_ErelSpectrum_BetaReconstruction_Combined; ThisPlot.HistName = "NeutronTracks_BetaReconstruction_Combined_Erel_Spectrum_GunMult_n="+MultStr; ThisPlot.LegendaText = "Mult.=<Beta> & Rec.=<NeuLAND>"; ThisPlot.TheColor = kGray+2; ThePlots.push_back(ThisPlot);
    }
   
    // ============================================================================
    
    
    
    // Search for the first plot:
    Bool_t Succes = kFALSE;
    Int_t VectSize = ThePlots.size();
    TH1D* FirstPlot;
    
    for (Int_t k = 0; k<VectSize; ++k)
    {
        if ((Succes==kFALSE)&&(ThePlots.at(k).IncludePlot==kTRUE))
        {
            Succes = FindFirstPlot(FirstPlot,ThePlots.at(k).HistName);
        }
    }
    
    // Next, generate the canvas:
    if (Succes==kTRUE)
    {
        TString Title = "Relative Energy Spectrum for n="+MultStr;
             if ((IncludeGunPlots==kTRUE)&&(IncludeDetPlots==kFALSE)&&(IncludeExpPlots==kFALSE)) {Title = Title + " (Gun. Mult)";}
        else if ((IncludeGunPlots==kFALSE)&&(IncludeDetPlots==kTRUE)&&(IncludeExpPlots==kFALSE)) {Title = Title + " (Det. Mult)";}
        else if ((IncludeGunPlots==kFALSE)&&(IncludeDetPlots==kFALSE)&&(IncludeExpPlots==kTRUE)) {Title = Title + " (Exp. Mult)";}
        else {Title = Title + " (???? Mult)";}
    
        TString Xlabel = "Relative Energy [MeV]";
        TString Ylabel = "Counts";
    
        gROOT->SetBatch(kTRUE);
        LogScaleMinimum = 20.0;
        Bool_t UseLogScale = kFALSE;
        TCanvas* TheCanvas = DrawHistDouma(FirstPlot,Title,Xlabel,Ylabel,0,UseLogScale);
        
        // Extract the plot size:
        Double_t Xmin = FirstPlot->GetXaxis()->GetXmin();
        Double_t Xmax = FirstPlot->GetXaxis()->GetXmax();
        Double_t Ymin = FirstPlot->GetMinimum();
        Double_t Ymax = FirstPlot->GetMaximum();
        Double_t LegendaHeight = 0.96;
        
        // Then, add all plots:
        for (Int_t k = 0; k<VectSize; ++k)
        {
            if (ThePlots.at(k).IncludePlot==kTRUE)
            {
                AddErelPlot(TheCanvas,ThePlots.at(k).HistName,ThePlots.at(k).LegendaText,ThePlots.at(k).TheColor,Xmin,Xmax,Ymin,Ymax,LegendaHeight,UseLogScale);
            }
        }
    
        // And then, save the plot:
        TheCanvas->SaveAs(OutPutPath + "/ErelSpectra_n"+MultStr+"_"+CompMethod+".png");
        TheCanvas->Close();
    }

    // Done.
}
