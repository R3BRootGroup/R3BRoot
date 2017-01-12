#include <iomanip>
#include <fstream>
#include <iostream>

void Step0_GenerateInputFiles(const Int_t nNeutrons = 4,
                              const Double_t EKin = 600. /* MeV/A */,
                              const Double_t ERel = 100. /* keV */,
                              const TString filename = "4He_4n_600AMeV_100keV.dat")
{
    const Double_t KinE = EKin / 1000.;            // MeV/A -> GeV/A
    const Double_t ExcEn = ERel / (1000. * 1000.); // keV -> GeV

    gROOT->Reset();
    if (!gROOT->GetClass("TGenPhaseSpace"))
    {
        gSystem->Load("libPhysics");
    }

    const Double_t u = 0.931494028;
    const Double_t NeutMass = 1.0086649156 * u;

    const Int_t NofEmPart = nNeutrons + 1;

    const Int_t a_f = 4;
    const Int_t z_f = 2;
    const Double_t He4Mass = 4 * u;
    // Double_t He4Mass = 4*u+2.4249E-3;  // this would be the correct description but Michael uses 128*u in
    // reconstruction so we also use this for the input below; this is a quite small modification

    const Double_t Gamma = 1 + KinE / u;
    const Double_t Beta = sqrt(1 - 1 / pow(Gamma, 2));
    const Double_t TotE = He4Mass + nNeutrons * NeutMass + ExcEn;
    TLorentzVector Init(0.0, 0.0, 0.0, TotE);

    Double_t masses[NofEmPart];
    masses[0] = He4Mass;
    for (Int_t i = 1; i <= nNeutrons; i++)
    {
        masses[i] = NeutMass;
    }

    TGenPhaseSpace event;
    event.SetDecay(Init, NofEmPart, masses);

    ofstream out;
    out.open(filename);
    for (Int_t n = 0; n < 100000; n++)
    {
        event.Generate();

        out << setw(8) << n << setw(8) << NofEmPart << setw(8) << "0." << setw(8) << "0." << endl;

        TLorentzVector* p4He = event.GetDecay(0);
        Double_t p4HeZ = Beta * Gamma * p4He->E() + Gamma * p4He->Pz();
        out << setw(8) << "-1" << setw(8) << z_f << setw(8) << a_f << setw(14) << setprecision(5) << p4He->Px()
            << setw(14) << setprecision(5) << p4He->Py() << setw(14) << setprecision(8) << p4HeZ << setw(8) << "0.00"
            << setw(8) << "0.00" << setw(8) << "0.00" << setw(14) << He4Mass << endl;

        for (Int_t i = 1; i <= nNeutrons; i++)
        {
            TLorentzVector* pNeut = event.GetDecay(i);
            Double_t pnZ = Beta * Gamma * pNeut->E() + Gamma * pNeut->Pz();
            out << setw(8) << "1" << setw(8) << "0" << setw(8) << "2112" << setw(14) << setprecision(5) << pNeut->Px()
                << setw(14) << setprecision(5) << pNeut->Py() << setw(14) << setprecision(8) << pnZ << setw(8) << "0.00"
                << setw(8) << "0.00" << setw(8) << "0.00" << setw(14) << NeutMass << endl;
        }
    }
    out.close();
}
