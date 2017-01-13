static std::map<UInt_t, TCutG*> fCuts;

/*

    |
    |
  ^ 3
  | X X
k | X   X            y = m*x + a
  | X     X      =>  0 = m*x + a
  | X       X    =>  x = -a/m
  v 0         X
    | X         X
    |___1 X X X X 2______________

*/

void SetCut(const UInt_t nNeutrons, const Double_t y0, const Double_t x1, const Double_t x2, const Double_t y3)
{
    if (!fCuts[nNeutrons])
    {
        fCuts[nNeutrons] = new TCutG(TString::UItoa(nNeutrons, 10), 4);
        fCuts.at(nNeutrons)->SetVarX("Total Energy [MeV]");
        fCuts.at(nNeutrons)->SetVarY("Number of Clusters");
    }

    TCutG* cut = fCuts[nNeutrons];
    cut->SetPoint(0, -1, y0);
    cut->SetPoint(1, x1, -1);
    cut->SetPoint(2, x2, -1);
    cut->SetPoint(3, -1, y3);
}

void Step0_FakeCalibr(const TString parFile)
{
    SetCut(0, 0, 0, 0, 0);
    SetCut(1, 0, 0, 0, 0);
    SetCut(2, 0, 0, 0, 0);
    SetCut(3, 0, 0, 0, 0);
    SetCut(4, 1, 1, 10000, 10000);

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();

    FairParRootFileIo* io = new FairParRootFileIo(kTRUE);
    io->open(parFile);
    rtdb->setOutput(io);

    R3BNeulandNeutron2DPar* par = (R3BNeulandNeutron2DPar*)rtdb->getContainer("R3BNeulandNeutron2DPar");

    rtdb->addRun(1);
    par->SetNeutronCuts(fCuts);
    par->setChanged();
    rtdb->writeContainers();

    rtdb->saveOutput();
    rtdb->print();
}
