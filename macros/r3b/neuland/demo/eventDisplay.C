void eventDisplay(const TString input_file, const TString par_file)
{
    FairRunAna *fRun = new FairRunAna();

    FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo  *parIo1 = new FairParRootFileIo();
    parIo1->open(par_file);
    rtdb->setFirstInput(parIo1);
    rtdb->print();

    fRun->SetSource(new FairFileSource(input_file));
    fRun->SetOutputFile("out.root");

    FairEventManager *fMan = new FairEventManager();
    FairMCTracks *Track = new FairMCTracks("Monte-Carlo Tracks");
    fMan->AddTask(Track);

    FairMCPointDraw *LandPoints = new FairMCPointDraw("NeulandPoints", kOrange,  kFullSquare);
    fMan->AddTask(LandPoints);
    
    //gGeoManager->SetVisLevel(6);   

    fMan->Init(1,10,10000);
}
