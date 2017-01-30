#include <iomanip>
#include <fstream>
#include <iostream>

void Step0_GenerateInputFiles(const Double_t ExcEn = 0.0001 /*GeV*/, const TString filename = "4He_4n_600AMeV_100keV.dat")
{
    gROOT->Reset();
    if (!gROOT->GetClass("TGenPhaseSpace"))
    {
        gSystem->Load("libPhysics");
    }

    ofstream out;
    out.open(filename);

    Int_t NofEmPart = 5;
    Int_t a_f = 4;
    Int_t z_f = 2;
    Double_t u = 0.931494028;
    Double_t NeutMass = 1.0086649156 * u;
    // Double_t He4Mass = 4*u+2.4249E-3;  // this would be the correct description but Michael uses 128*u in
    // reconstruction so we also use this for the input below; this is a quite small modification
    Double_t He4Mass = 4 * u;
    //Double_t ExcEn = 0.0001; // 100 keV
    Double_t KinE = 0.6;     // GeV/A
    Double_t Gamma = 1 + KinE / u;
    Double_t Beta = sqrt(1 - 1 / pow(Gamma, 2));
    Double_t TotE = He4Mass + 4 * NeutMass + ExcEn;
    TLorentzVector Init(0.0, 0.0, 0.0, TotE);

    Double_t masses[5] = { He4Mass, NeutMass, NeutMass, NeutMass, NeutMass };

    TGenPhaseSpace event;
    event.SetDecay(Init, NofEmPart, masses);

    for (Int_t n = 0; n < 10000; n++)
    {
        Double_t weight = event.Generate();

        TLorentzVector* p4He = event.GetDecay(0);
        TLorentzVector* pNeut1 = event.GetDecay(1);
        TLorentzVector* pNeut2 = event.GetDecay(2);
        TLorentzVector* pNeut3 = event.GetDecay(3);
        TLorentzVector* pNeut4 = event.GetDecay(4);

        Double_t p4HeZ = Beta * Gamma * p4He->E() + Gamma * p4He->Pz();
        Double_t pn1Z = Beta * Gamma * pNeut1->E() + Gamma * pNeut1->Pz();
        Double_t pn2Z = Beta * Gamma * pNeut2->E() + Gamma * pNeut2->Pz();
        Double_t pn3Z = Beta * Gamma * pNeut3->E() + Gamma * pNeut3->Pz();
        Double_t pn4Z = Beta * Gamma * pNeut4->E() + Gamma * pNeut4->Pz();

        // out.width(8);
        // out.precision(7);
        out << setw(8) << n << setw(8) << NofEmPart << setw(8) << "0." << setw(8) << "0." << endl;
        out << setw(8) << "-1" << setw(8) << z_f << setw(8) << a_f << setw(14) << setprecision(5) << p4He->Px()
            << setw(14) << setprecision(5) << p4He->Py() << setw(14) << setprecision(8) << p4HeZ << setw(8) << "0.00"
            << setw(8) << "0.00" << setw(8) << "0.00" << setw(14) << He4Mass << endl;
        out << setw(8) << "1" << setw(8) << "0" << setw(8) << "2112" << setw(14) << setprecision(5) << pNeut1->Px()
            << setw(14) << setprecision(5) << pNeut1->Py() << setw(14) << setprecision(8) << pn1Z << setw(8) << "0.00"
            << setw(8) << "0.00" << setw(8) << "0.00" << setw(14) << NeutMass << endl;
        out << setw(8) << "1" << setw(8) << "0" << setw(8) << "2112" << setw(14) << setprecision(5) << pNeut2->Px()
            << setw(14) << setprecision(5) << pNeut2->Py() << setw(14) << setprecision(8) << pn2Z << setw(8) << "0.00"
            << setw(8) << "0.00" << setw(8) << "0.00" << setw(14) << NeutMass << endl;
        out << setw(8) << "1" << setw(8) << "0" << setw(8) << "2112" << setw(14) << setprecision(5) << pNeut3->Px()
            << setw(14) << setprecision(5) << pNeut3->Py() << setw(14) << setprecision(8) << pn3Z << setw(8) << "0.00"
            << setw(8) << "0.00" << setw(8) << "0.00" << setw(14) << NeutMass << endl;
        out << setw(8) << "1" << setw(8) << "0" << setw(8) << "2112" << setw(14) << setprecision(5) << pNeut4->Px()
            << setw(14) << setprecision(5) << pNeut4->Py() << setw(14) << setprecision(8) << pn4Z << setw(8) << "0.00"
            << setw(8) << "0.00" << setw(8) << "0.00" << setw(14) << NeutMass << endl;
    }
    out.close();
}
