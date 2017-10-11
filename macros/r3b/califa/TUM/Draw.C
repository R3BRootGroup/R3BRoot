#include <iostream>
#include <fstream>

#include <TClonesArray.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TH3I.h>
#include <TCanvas.h>
#include <TCutG.h>
#include <TFile.h>
#include <TVector3.h>
#include <TMath.h>
#include <TEntryList.h>
#include <TF1.h>
#include <TProfile.h>
#include <TRandom3.h>
#include <TLorentzVector.h>

#include <R3BMCTrack.h>
#include <R3BCalifaMappedData.h>
#include <R3BCalifaHitDataSim.h>
#include <R3BCalifaCrystalCalDataSim.h>
#include <R3BCalifaGeometry.h>
#include <R3BDetectorList.h>
#include <startrackerData/R3BSTaRTrackerHit.h>

using namespace std;

class Draw
{
protected:

 int channelMapping[128]; 
 double channelTheta[128];
 double channelPhi[128];

void GenChannelMapping()
{
  int fbxPetalCrystalIdBase[64] =
{
  // Febex 0
  936, 933, 935, 930, 932, 929, 931, 934,         // row 9
  1059, 1062, 1064, 1061, 1063, 1058, 1060, 1057, // row 10
  // Febex 1
  1192, 1189, 1191, 1186, 1188, 1185, 1187, 1190, // row 11
  1315, 1318, 1320, 1317, 1319, 1314, 1316, 1313, // row 12
  // Febex 2
  1448, 1445, 1447, 1442, 1444, 1441, 1443, 1446, // row 13
  1572, 1574, 1575, 1573, 1576, 1570, 1571, 1569, // row 14
  // Febex 3
  1703, 1701, 1704, 1698, 1699, 1697, 1700, 1702, // row 15
  1828, 1830, 1831, 1829, 1832, 1826, 1827, 1825  // row 16
};

  int ptlAlveoliCopyBase[2] = {2, 0};


  
  int crystalId;
  double foo;

  R3BCalifaGeometry *geo = R3BCalifaGeometry::Instance(fGeometryVersion);

  for(int i = 0; i < 128; i++)
  {
    crystalId = fbxPetalCrystalIdBase[i%64] + 4*ptlAlveoliCopyBase[i/64];
    channelMapping[i] = crystalId;

    geo->GetAngles(crystalId, &channelTheta[i], &channelPhi[i], &foo);

//    cout << i << ": " << channelTheta[i] << ", " << channelPhi[i] << endl;
  }
}
  
  
  TClonesArray *hits;
  TClonesArray *rawHits;
  TClonesArray *caloHits;

  TClonesArray *startrackHits;

  TClonesArray *tracks;

  TTree *events;
  Long64_t n;

  void Init()
  {
    n = events->GetEntries();

    cout << "  " << n << endl;

    if(hits)
      delete hits;

    TBranch *b;
    if((b = events->GetBranch("CalifaCrystalCalData")))
    {
      // Experimental data
      hits = new TClonesArray("R3BCalifaCrystalCalData");
    }
    else if((b = events->GetBranch("CalifaCrystalCalDataSim")))
    {
      // Simulated data
      hits = new TClonesArray("R3BCalifaCrystalCalDataSim");
    }
    else
    {
      cerr << "No crystal hit found in tree!" << endl;
//      return;
      hits = NULL;      
    }

//    Note (1): Branch::SetAddress leads to segfault in case of TChain
//    b->SetAddress(&hits);
    if(hits)    
      events->SetBranchAddress(b->GetName(), &hits);

    if((b = events->GetBranch("CalifaMappedData")))
    {
      rawHits = new TClonesArray("R3BCalifaMappedData");
      events->SetBranchAddress(b->GetName(), &rawHits);
    }
    else
      rawHits = NULL;


    if(caloHits)
      delete caloHits;

    if((b = events->GetBranch("CalifaHitData")))
    {
      caloHits = new TClonesArray("R3BCalifaHitData");
//      b->SetAddress(&caloHits);
      events->SetBranchAddress("CalifaHitData", &caloHits);
    }
    else if((b = events->GetBranch("califa_CalifaHitData")))
    {
      caloHits = new TClonesArray("R3BCalifaHitData");
      events->SetBranchAddress("califa_CalifaHitData", &caloHits);
    }
    else if((b = events->GetBranch("CalifaHitDataSim")))
    {
      caloHits = new TClonesArray("R3BCalifaHitDataSim");
//      b->SetAddress(&caloHits);
      events->SetBranchAddress("CalifaHitDataSim", &caloHits);
    }
    else
      caloHits = NULL;

    if(tracks)
      delete tracks;

    if((b = events->GetBranch("MCTrack")))
    {
      tracks = new TClonesArray("R3BMCTrack");
//      b->SetAddress(&tracks);
      events->SetBranchAddress("MCTrack", &tracks);
    }
    else
      tracks = NULL;

    if((b = events->GetBranch("STaRTrackerHit")))
    {
      startrackHits = new TClonesArray("R3BSTaRTrackerHit");
      events->SetBranchAddress("STaRTrackerHit", &startrackHits);
    }
    else
      startrackHits = NULL;
  }

public:

  int fGeometryVersion;

  Draw(TTree *events) : hits(NULL), caloHits(NULL), tracks(NULL)
  {
    fGeometryVersion = 0x438b;

    this->events = events;
    Init();
  }

  ~Draw()
  {
    delete hits;
  }

  void Multiplicity(TH1 *hist = NULL)
  {
    TH1 *h = getTH1(100, 0, 100, "Multiplicity", hist);

    for(Long64_t i = 0; i < n; i++)
    {
      if(i % 1000 == 0)
        cout << i << endl;

      events->GetEntry(i);

      h->Fill(hits->GetEntries());
    }

    h->Draw();
  }

  void CrystalId(TH1 *hist = NULL)
  {
    TH1 *h = getTH1(4000, 0, 4000, "CrystalId", hist);

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        h->Fill(((R3BCalifaCrystalCalData*)hits->At(j))->GetCrystalId());
      }
    }

    h->Draw();
  }

  void CaloEnergy(TH1 *hist = NULL, bool usePid = false)
  {
  if(!caloHits)
  {
    cerr << "No CalifaHitDatas found." << endl;
    return;
  }

  TH1 *h = getTH1(1000, 0, 1000, "Energy (MeV)", hist);

  R3BCalifaHitData *hit;

  for(Long64_t i = 0; i < n; i++)
  {
    events->GetEntry(i);

    for(int j = 0; j < caloHits->GetEntries(); j++)
    {
      hit = dynamic_cast<R3BCalifaHitData*>(caloHits->At(j));
      if(usePid)
        h->Fill((hit->GetNf() + hit->GetNs()) * 1000.);
      else
        h->Fill(hit->GetEnergy() * 1000.0);
    }
  }

  h->Draw();
  }

  void QPIDvsEnergy(TH2 *hist = NULL)
  {
    if(!hits)
    {
      cerr << "No CrystalHits found." << endl;
      return;
    }

    hist = getTH2(500,0,1000,500,0,1000, "QPID Energy vs PeakSensing Energy (MeV)", hist);
    R3BCalifaCrystalCalData *hit;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);
      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = dynamic_cast<R3BCalifaCrystalCalData*>(hits->At(j));
        hist->Fill(hit->GetEnergy() * 1000., (hit->GetNf() + hit->GetNs())*1000.);
      }
    }

    hist->Draw("colz");
  }

  void CaloQPIDvsEnergy(TH2 *hist = NULL)
  {
    if(!caloHits)
    {
      cerr << "No CalifaHitDatas found." << endl;
      return;
    }

    TH2 *h = getTH2(500,0,1000,500,0,1000, "QPID Energy vs PeakSensing Energy (MeV)", hist);

    R3BCalifaHitData *hit;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < caloHits->GetEntries(); j++)
      {
        hit = dynamic_cast<R3BCalifaHitData*>(caloHits->At(j));
        h->Fill(hit->GetEnergy() * 1000., (hit->GetNf() + hit->GetNs())*1000.);
      }
    }

    h->Draw("colz");
  }

  void CaloPidDev(TCutG *cut, TF1 *fnominal, Long64_t nEvents = -1, TH2 *h = NULL)
  {
    if(!caloHits)
    {
      cerr << "No CalifaHitDatas in tree." << endl;
      return;
    }

    R3BCalifaHitData *hit;
    h = getTH2(1000,0,500,1000,0,250, "abs(Ns - f(Nf))", h);

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < caloHits->GetEntries(); j++)
      {
        hit = dynamic_cast<R3BCalifaHitData*>(caloHits->At(j));

        if(!cut->IsInside(1000. * hit->GetNf(), 1000. * hit->GetNs()))
          continue;

        h->Fill(1000. * hit->GetEnergy(), TMath::Abs(1000. * hit->GetNs() - fnominal->Eval(1000. * hit->GetNf())));

        if(nEvents > 0)
          nEvents--;
      }

      if(nEvents == 0)
        break;
    }

    h->Draw("colz");
  }

  void Energy(int crystalId = -1, TH1 *hist = NULL, bool usePid = false)
  {
    if(!hits)
    {
      cerr << "No CrystalHits found. Trying RawHits" << endl;
      RawEnergy(crystalId, hist);
      return;
    }

    TH1 *h = getTH1(1000, 0, 1000, "Energy (MeV)", hist);

    R3BCalifaCrystalCalData *hit;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = (R3BCalifaCrystalCalData*)hits->At(j);
        if(crystalId == -1 || crystalId == hit->GetCrystalId())
        {
          if(usePid)
            h->Fill((hit->GetNf() + hit->GetNs()) * 1000.);
          else
            h->Fill(hit->GetEnergy() * 1000.0);
        }
      }
    }

    h->Draw();
  }

  void RawEnergy(int crystalId = -1, TH1 *hist = NULL)
  {
    if(!rawHits)
    {
      cerr << "No RawHits found. Bailing out!" << endl;
      return;
    }

    TH1 *h = getTH1(1000, 0, 0x8000, "Energy (au)", hist);

    R3BCalifaMappedData *hit;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < rawHits->GetEntries(); j++)
      {
        hit = (R3BCalifaMappedData*)rawHits->At(j);
        if(crystalId == -1 || crystalId == hit->GetCrystalId())
          h->Fill(hit->GetEnergy());
      }
    }

    h->Draw();
  }

  void PID(int crystalId = -1, Long64_t nEvents = -1, TH2 *hist = NULL, TCutG *cut = NULL, TCutG *pidvse = NULL, double thetamin=-1, double thetamax=-1)
  {
    TH2 *h = getTH2(500, 0, 1000, 500, 0, 1000, "Ns vs Nf (MeV)", hist);
    R3BCalifaCrystalCalData *hit;

    double theta, phi, rho;
    R3BCalifaGeometry *geo = NULL;
    if(thetamin >= 0 || thetamax >= 0)
      geo = R3BCalifaGeometry::Instance(fGeometryVersion);

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = (R3BCalifaCrystalCalData*)hits->At(j);

        if(geo)
        {
          geo->GetAngles(hit->GetCrystalId(), &theta, &phi, &rho);
          theta *= 180./TMath::Pi();
        }
        if(thetamin >= 0 && theta < thetamin)
          continue;
        if(thetamax >= 0 && theta > thetamax)
          continue;

        if((crystalId == -1 || crystalId == hit->GetCrystalId()) && (cut == NULL || cut->IsInside(1000. * hit->GetNf(), 1000. * hit->GetNs()))
            && (pidvse == NULL || pidvse->IsInside(1000.*hit->GetEnergy(),1000.*(hit->GetNf()+hit->GetNs()))))
        {
          h->Fill(1000.*hit->GetNf() , 1000.*hit->GetNs());

          if(nEvents > 0)
            nEvents--;
        }
      }

      if(nEvents == 0)
        break;
    }

    h->Draw("colz");
  }

  int getChannelNr(int crystalId)
  {
    for(int i = 0; i < 128; i++)
    {
      if(channelMapping[i] == crystalId)
        return i;
    }

    return -1;
  }

  void PIDArray(int numx = 4, int numy = 4)
  {
    int num = numx * numy;
    int ch;

    GenChannelMapping();

    R3BCalifaCrystalCalData *hit;

    TH2 **hists = new TH2*[num];
    for(int i = 0; i < num; i++)
      hists[i] = getTH2(500,0,250,500,0,250,"QPID",NULL);

    for(Long64_t i = 0; i < n; i++)
    {
      if(i % 1000 == 0)
        cout << i << endl;
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = dynamic_cast<R3BCalifaCrystalCalData*>(hits->At(j));
        ch = getChannelNr(hit->GetCrystalId());
        if(ch < 0 || ch >= num)
          continue;

        hists[ch]->Fill(1000. * hit->GetNf(), 1000. * hit->GetNs());
      }
    }

    TCanvas *c = new TCanvas("cpid", "cpid");
    c->Divide(numx, numy);

    for(int i = 0; i < num; i++)
    {
      c->cd(i+1);
      hists[i]->Draw("colz");
    }
  }

  void PIDCalSlope(int pass = 1)
  {
    int numx = 4, numy = 4, numc = 8;
    int num = numx * numy * numc;
    int ch;

    static double scale[128];
    static double delta[128];

    double nf, ns, y, nfold, nsold, s, d;

    GenChannelMapping();

    R3BCalifaCrystalCalData *hit;

    TH1 **hists = new TH1*[num];
    for(int i = 0; i < num; i++)
      hists[i] = getTH1(200,-.1,.1,"Ratio Nf/Ns @ 90 - 110 MeV",NULL);

    for(Long64_t i = 0; i < n; i++)
    {
      if(i % 1000 == 0)
        cout << i << endl;
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = dynamic_cast<R3BCalifaCrystalCalData*>(hits->At(j));
        ch = getChannelNr(hit->GetCrystalId());
        if(ch < 0 || ch >= num)
          continue;

        nf = hit->GetNf();
        ns = hit->GetNs();

        if(nf + ns < 0.090 || nf + ns > 0.110)
          continue;

        if(pass == 2)
        {
          // Apply normalization
          nfold = nf;
          nsold = ns;

          // Energy dependend scaling + translation (nominal scale + offset at 0.100 GeV)
          s = 1. + (scale[ch] - 1.)*(nf + ns)/.1;
          d = delta[ch]*(nf + ns)/.1;

          nf = nfold*(1. + s)/2. + nsold*(1. - s)/2. - d;
          ns = nfold*(1. - s)/2. + nsold*(1. + s)/2. + d;
        }

        // Projection perpendicular to energy
        hists[ch]->Fill(ns/TMath::Sqrt2() - nf/TMath::Sqrt2());
      }
    }

#if 1
    TCanvas **c = new TCanvas*[numc];
    for(int j = 0; j < numc; j++)
    {
      c[j] = new TCanvas(Form("cpid%d_%d", pass, j), "cpid");
      c[j]->Divide(numx, numy);

      for(int i = 0; i < numx * numy; i++)
      {
        c[j]->cd(i+1);
        hists[i + j * (numx*numy)]->Draw();
      }
    }
#endif
    TH1 *hmean, *hrms;

    if(pass == 1)
    {
      hmean = new TH1D("hratio1", "BEFORE Mean: PID @ 90 - 110 MeV", num, 0, num);
      hrms = new TH1D("hrms1", "BEFORE RMS: PID @ 90 - 110 MeV", num, 0, num);
    }
    else
    {
      hmean = new TH1D("hratio2", "AFTER Mean: PID @ 90 - 110 MeV", num, 0, num);
      hrms = new TH1D("hrms2", "AFTER RMS: PID @ 90 - 110 MeV", num, 0, num);
    }

    if(pass == 1)
    {
      double mean0, sigma0, mean, sigma;
      // Calculate parameters
      ofstream outfile("pidscale.txt");
      mean0 = hists[0]->GetMean();
      sigma0 = hists[0]->GetRMS();
      for(int i = 0; i < num; i++)
      {
        mean = hists[i]->GetMean();
        sigma = hists[i]->GetRMS();

        hmean->Fill(i, mean);
        hrms->Fill(i, sigma);

        scale[i] = sigma0/sigma;
        delta[i] = (mean0 - scale[i]*mean)/TMath::Sqrt2();
     
        outfile << dec << i << scientific << " " << scale[i] << " " << delta[i] << endl;
        cout << dec << i << scientific << " " << scale[i] << " " << delta[i] << endl;
      }
      outfile.close();
    }
    else
    {
      // Display only
      for(int i = 0; i < num; i++)
      {
        hmean->Fill(i, hists[i]->GetMean());
        hrms->Fill(i, hists[i]->GetRMS());
      }
    }

    TCanvas *cmean = new TCanvas(Form("cmean%d", pass), "cmean");
    cmean->Divide(1,2);
    cmean->cd(1);
    hmean->Draw();
    cmean->cd(2);
    hrms->Draw();

    if(pass == 1)
      PIDCalSlope(2);
  }

  void CaloPID(TH2 *hist = NULL, TCutG *cut = NULL, Long64_t nEvents = -1, double theta_min = -1, double theta_max = -1)
  {
    if(!caloHits)
    {
      cerr << "No CalifaHitData in tree!" << endl;
      return;
    }

    TH2 *h = getTH2(500, 0, 500, 500, 0, 500, "Ns vs Nf (MeV)", hist);
    R3BCalifaHitData *hit;

    double r_nsnf;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < caloHits->GetEntries(); j++)
      {
        hit = (R3BCalifaHitData*)caloHits->At(j);


        // Normalize PID to energy
        if(hit->GetNf() != 0)
        {
          r_nsnf = hit->GetNs()/hit->GetNf();
          hit->SetNf(hit->GetEnergy()/(r_nsnf + 1.));
          hit->SetNs(hit->GetEnergy()/(1./r_nsnf + 1.));
        }


        if(theta_min >= 0 && hit->GetTheta() < theta_min)
          continue;
        if(theta_max >= 0 && hit->GetTheta() > theta_max)
          continue;

        if(cut != NULL && !cut->IsInside(1000. * hit->GetNf(), 1000. * hit->GetNs()))
          continue;

        h->Fill(1000.*hit->GetNf(), 1000.*hit->GetNs());

//        h->Fill(1000.*(hit->GetNf()*TMath::Cos(-1.09) - hit->GetNs()*TMath::Sin(-1.09)),
//            1000.*(hit->GetNf()*TMath::Sin(-1.09) + hit->GetNs()*TMath::Cos(-1.09)));

        if(nEvents > 0)
          nEvents--;
      }

      if(nEvents == 0)
        break;
    }

    h->Draw("colz");
  }



  void EnergyVsCrystalId(TH2 *hist = NULL)
  {
    TH2 *h = getTH2(500, 0, 4000, 500, 0, 1000, "Energy vs Crystal Id", hist);
    R3BCalifaCrystalCalData *hit;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = (R3BCalifaCrystalCalData*)hits->At(j);
        h->Fill(hit->GetCrystalId(), hit->GetEnergy()*1000.0);
      }
    }

    h->Draw("colz");
  }

  void EnergyVsTheta(TH2 *hist = NULL)
  {
    TH2 *h = getTH2(50, 0, 180, 500, 0, 1000, "Energy vs Theta", hist);
    R3BCalifaCrystalCalData *hit;
    Double_t phi, theta, rho;

    R3BCalifaGeometry *geo = R3BCalifaGeometry::Instance(fGeometryVersion);
  
    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = (R3BCalifaCrystalCalData*)hits->At(j);
        geo->GetAngles(hit->GetCrystalId(), &theta, &phi, &rho);

        h->Fill(theta * 180.0 / TMath::Pi(), hit->GetEnergy()*1000.0);
      }
    }

    h->Draw("colz");
  }

  void CaloEnergyVsTheta(TH2 *hist = NULL)
  {
    if(!caloHits)
    {
      cerr << "No CalifaHitData in tree!" << endl;
      return;
    }

    TH2 *h = getTH2(50, 0, 180, 500, 0, 1000, "Energy vs Theta", hist);
    R3BCalifaHitData *hit;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < caloHits->GetEntries(); j++)
      {
        hit = (R3BCalifaHitData*)caloHits->At(j);
        h->Fill(hit->GetTheta() * 180.0 / TMath::Pi(), hit->GetEnergy() * 1000.0);
      }
    }

    h->Draw("colz");
  }

  // Assumes 1 primary particle at position 0 in stack with start point (0,0,0)
  void ThetaPhiHitVsTrack(TH2 *htheta = NULL, TH2 *hphi = NULL)
  {
    if(!caloHits || !tracks)
    {
      cerr << "No CalifaHitData or MCTrack in tree!" << endl;
      return;
    }

    hphi = getTH2(50, -180, 180, 50, -180, 180, "Phi Hit vs Phi Track", hphi);
    htheta = getTH2(50, 0, 180, 50, 0, 180, "Theta Hit vs Theta Track", htheta);

    R3BMCTrack *t;
    R3BCalifaHitData *h;
    TVector3 v;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      if(tracks->GetEntries() == 0)
        continue;

      t = dynamic_cast<R3BMCTrack*>(tracks->At(0));
      t->GetMomentum(v);

      for(int j = 0; j < caloHits->GetEntries(); j++)
      {
        h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(j));
        htheta->Fill(v.Theta() * 180. / TMath::Pi(), h->GetTheta() * 180.0 / TMath::Pi());
        hphi->Fill(v.Phi() * 180. / TMath::Pi(), h->GetPhi() * 180.0 / TMath::Pi());
      }
    }

    TCanvas *c1 = new TCanvas("c1", "c1");
    c1->Divide(1,2);
    c1->cd(1);
    htheta->Draw("colz");
    c1->cd(2);
    hphi->Draw("colz");
  }

  TCutG *GetIPhosCut()
  {
    TFile *fcut = TFile::Open("qpid_cuts.root", "read");
    return dynamic_cast<TCutG*>(fcut->Get("qpid_iphos_sim"));
  }

  void DrawIphos()
  {
    if(!caloHits)
    {
      cerr << "No Calo Hits!" << endl;
      return;
    }

    if(!tracks)
    {
      cerr << "No tracks!" << endl;
      return;
    }

    GenChannelMapping();

    TCutG *cut_iphos = GetIPhosCut();
    if(!cut_iphos)
    {
      cerr << "Could not load iPhos cut!" << endl;
      return;
    }

    TH2I **hists = new TH2I*[128];

    for(int i = 0; i < 128; i++)
      hists[i] = new TH2I(Form("iphos_%d", i), Form("E vs DE %d", i), 250, 0, 500, 250, 200,1000);

    R3BMCTrack *track;
    R3BCalifaHitData *hit;
    double eprim, phi, theta;
    int j, channel;

    for(Long64_t i = 0; i < n; i++)
    {
      if(i % 1000 == 0)
        cout << i << endl;

      events->GetEntry(i);

      if(tracks->GetEntries() == 0)
      {
        cerr << "No primary track!" << endl;
        continue;
      }

      // Get primary proton
      track = dynamic_cast<R3BMCTrack*>(tracks->At(0));
      // Get primary energy
      eprim = 1000. * (track->GetEnergy() - track->GetMass());

      for(j = 0; j < caloHits->GetEntries(); j++)
      {
        hit = dynamic_cast<R3BCalifaHitData*>(caloHits->At(j));

        if(!cut_iphos->IsInside(hit->GetNf(), hit->GetNs()))
          continue;

        phi = hit->GetPhi();
        theta = hit->GetTheta();

        if((channel = GetCrystal(phi, theta)) == -1)
        {
          cerr << "Invalid position: " << theta << ", " << phi << endl;
          continue;
        }

        hists[channel]->Fill(hit->GetEnergy() * 1000., eprim);
      }
    }

    TCanvas *ciphos = new TCanvas("ciphos", "iPhos");
    ciphos->Divide(12, 11);

    for(int i = 0; i < 128; i++)
    {
      ciphos->cd(i+1);
      hists[i]->Draw("colz");
    }
  }

  void DzVsTheta(Long64_t nEvents = 0)
  {
    if(!caloHits)
    {
      cerr << "No Calo Hits!" << endl;
      return;
    }

    if(!tracks)
    {
      cerr << "No tracks!" << endl;
      return;
    }

    TCutG *cut_iphos = GetIPhosCut();
    if(!cut_iphos)
    {
      cerr << "Could not load iPhos cut!" << endl;
      return;
    }

    R3BCalifaGeometry *geo = R3BCalifaGeometry::Instance(0x438b);
    if(!geo)
    {
      cerr << "Could not load Califa geometry manager" << endl;
      return;
    }

    TH2I *hdztheta = new TH2I("hdztheta", "iPhos", 180, 0, 90, 200, 0, 50);
    TH2I *hdztheta_track = new TH2I("hdztheta_track", "iPhos", 1800, 0, 90, 1000, 0, 50);

    R3BMCTrack *t;
    R3BCalifaHitData *h;
    TVector3 startVertex, momentum, hitPos;

    double eprim, dz;

    int j;
    if(nEvents == 0)
      nEvents = n;

    for(Long64_t i = 0; i < nEvents; i++)
    {
      if(i % 1000 == 0)
        cout << i << endl;

      events->GetEntry(i);

      t = dynamic_cast<R3BMCTrack*>(tracks->At(0));

      if(!t->GetNPoints(kCALIFA) || !caloHits->GetEntries())
        continue;

      t->GetStartVertex(startVertex);
      t->GetMomentum(momentum);
      eprim = 1000. * (t->GetEnergy() - t->GetMass());
      dz = geo->GetDistanceThroughCrystals(startVertex, momentum, &hitPos);

      if(dz == 0)
        continue;

      for(j = 0; j < caloHits->GetEntries(); j++)
      {
        h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(j));
        if(cut_iphos->IsInside(1000. * h->GetNf(), 1000. * h->GetNs()))
        {
          hdztheta->Fill(h->GetTheta()*180./TMath::Pi(), dz);
          hdztheta_track->Fill(hitPos.Theta()*180./TMath::Pi(), dz);
        }
      }
    }

    TCanvas *c_iphos = new TCanvas("c_iphos", "Iphos");
    c_iphos->Divide(1,2);
    c_iphos->cd(1);
    hdztheta->Draw("colz");
    c_iphos->cd(2);
    hdztheta_track->Draw("colz");
  }


  void IPhosEvsDE(Long64_t nEvents = 0)
  {
    if(!caloHits)
    {
      cerr << "No Calo Hits!" << endl;
      return;
    }

    if(!tracks)
    {
      cerr << "No tracks!" << endl;
      return;
    }

    TCutG *cut_iphos = GetIPhosCut();
    if(!cut_iphos)
    {
      cerr << "Could not load iPhos cut!" << endl;
      return;
    }

    TH2I **histos = new TH2I*[3];
    for(int i = 0; i < 3; i++)
      histos[i] = new TH2I(Form("h_%d", i), Form("E vs DE %d", i), 500, 0, 500, 500, 200, 1000);

    double dz_region[3] = {220,180,170};

    R3BCalifaGeometry *geo = R3BCalifaGeometry::Instance(0x438b);
    TVector3 vStart(0,0,0), vDir;

    R3BMCTrack *t;
    R3BCalifaHitData *h;
    
    double eprim, theta;

    int j, region;

//    if(n > 100000)
//      n = 100000;
    if(nEvents == 0)
      nEvents = n;

    for(Long64_t i = 0; i < nEvents; i++)
    {
      if(i % 1000 == 0)
        cout << i << endl;

      events->GetEntry(i);

      t = dynamic_cast<R3BMCTrack*>(tracks->At(0));

      if(!t || !caloHits || !t->GetNPoints(kCALIFA) || !caloHits->GetEntries())
        continue;

      eprim = 1000. * (t->GetEnergy() - t->GetMass());

      for(j = 0; j < caloHits->GetEntries(); j++)
      {
        h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(j));
        if(h->GetNbOfCrystalHits() != 1)
          continue;

        // Normalize PID to energy
        if(h->GetNf() != 0)
        {
          double r_nsnf = h->GetNs()/h->GetNf();
          h->SetNf(h->GetEnergy()/(r_nsnf + 1.));
          h->SetNs(h->GetEnergy()/(1./r_nsnf + 1.));
        }

        t->GetMomentum(vDir);
        double dz = 10.*geo->GetDistanceThroughCrystals(vStart, vDir);

        if(vDir.Theta()*180./TMath::Pi() < 40)
        {
          region = 0;
        }
        else if(vDir.Theta()*180./TMath::Pi() < 55)
        {
          region = 1;
        }
        else
        {
          region = 2;
        }

        if(TMath::Abs(dz - dz_region[region]) > dz_region[region]/100.)
        {
          continue;
        }

        if(cut_iphos->IsInside(1000. * h->GetNf(), 1000. * h->GetNs()) && h->GetEnergy() >= 0.08)
        {
          theta = 180.0 * h->GetTheta() / TMath::Pi();
          if(theta < 40)
          {
            histos[0]->Fill(1000. * h->GetEnergy(), eprim);
            if(h->GetEnergy() < .12)
              cout << "  " << i << endl;
          }
          else if(theta >= 40 && theta < 55)
            histos[1]->Fill(1000. * h->GetEnergy(), eprim);
          else
            histos[2]->Fill(1000. * h->GetEnergy(), eprim);
        }
      }
    }

    TCanvas *c_e_vs_de = new TCanvas("c_e_vs_de");
    c_e_vs_de->Divide(1,3);
    for(int i = 0; i < 3; i++)
    {
      c_e_vs_de->cd(i+1);
      histos[i]->Draw("colz");
    }
  }

#define IPHOS_VAR_Z 0

  void TestIPhos(Long64_t nEvents = 0, const char *fname = NULL, const char *mode = "update")
  {
    if(!caloHits)
    {
      cerr << "No Calo Hits!" << endl;
      return;
    }

    if(!tracks)
    {
      cerr << "No tracks!" << endl;
      return;
    }

    TCutG *cut_iphos = GetIPhosCut();
    if(!cut_iphos)
    {
      cerr << "Could not load iPhos cut!" << endl;
      return;
    }

      double iphos_p[3][6] = 
      {
        {3.68650e+04,9.48732e+01,6.79594e-01,0,0,0},
        {2.94541e+04,7.84205e+01,6.74277e-01,0,0,0},
        {2.88743e+04,7.19554e+01,6.21420e-01,0,0,0}
      };


//  double iphos_a[3] = {147.446, 0.101594, -78.283};
//  double iphos_b[2] = {0.401583, -1.4442};
//  double iphos_c[4] = {-2.18744, -0.238226, 0.000639924, 0.560853};



  R3BCalifaGeometry *geo = R3BCalifaGeometry::Instance(0x438b);
  TVector3 vStart(0,0,0), vDirection;
  
    int region;
    double dz_region[3] = {220,180,170};

    TH2I **histos = new TH2I*[3];
    for(int i = 0; i < 3; i++)
    {
      histos[i] = new TH2I(Form("h_%d", i), Form("E (iPhos) - E (sim) vs E (iPhos) %d", i), 200, 200, 1000, 500, -500, 500);
    }

    TH1*savehists[] = {histos[0], histos[1], histos[2],NULL};

//    hrespfx[3] = new TH1F("hrespfx_total", "Resolution FWHM % total", 200, 200, 1000);

    R3BMCTrack *t;
    R3BCalifaHitData *h;
    
    double eprim, theta, e_iphos, de_hit;
    double nprim;

    int j;
    if(nEvents == 0)
      nEvents = n;

    for(Long64_t i = 0; i < nEvents; i++)
    {
      if(i % 1000 == 0)
        cout << i << endl;

      events->GetEntry(i);

      for(nprim = 0; nprim < tracks->GetEntries(); nprim++)
      {
        if(dynamic_cast<R3BMCTrack*>(tracks->At(nprim))->GetMotherId() != -1)
          break;
      }

      if(nprim != caloHits->GetEntries())
        continue;


      for(j = 0; j < caloHits->GetEntries(); j++)
      {
        t = dynamic_cast<R3BMCTrack*>(tracks->At(j));
  
  //      if(!t->GetNPoints(kCALIFA) || !caloHits->GetEntries())
  //        continue;
  
        eprim = 1000. * (t->GetEnergy() - t->GetMass());

        h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(j));

        if(h->GetNbOfCrystalHits() != 1)
        {
          continue;
        }

        // Normalize PID to energy
        if(h->GetNf() != 0)
        {
          double r_nsnf = h->GetNs()/h->GetNf();
          h->SetNf(h->GetEnergy()/(r_nsnf + 1.));
          h->SetNs(h->GetEnergy()/(1./r_nsnf + 1.));
        }

        de_hit = 1000. * h->GetEnergy();
        if(cut_iphos->IsInside(1000. * h->GetNf(), 1000. * h->GetNs()) && de_hit >= 100)
        {
          theta = 180.0 * h->GetTheta() / TMath::Pi();
          if(theta < 40)
            region = 0;
          else if(theta >= 40 && theta < 55)
            region = 1;
          else
            region = 2;

          t->GetMomentum(vDirection);
          double dz = 10.*geo->GetDistanceThroughCrystals(vStart, vDirection);


//          dz /= 10.;
//          double a = iphos_a[0]*dz*dz + iphos_a[1]*dz + iphos_a[2];
//          double b = iphos_b[0]*(dz - iphos_b[1]);
//          double c = iphos_c[0]/(dz - iphos_c[1]) + iphos_c[2]*dz + iphos_c[3];

//          e_iphos = (a/(de_hit - b) + c*(de_hit - b));
//          cout << "a = " << a << ", b = " << b << ", c = " << c << endl;

          if(TMath::Abs(dz - dz_region[region]) > dz_region[region]/100.)
          {
            continue;
          }

          e_iphos = iphos_p[region][0]/(de_hit - iphos_p[region][1]) + iphos_p[region][2]*(de_hit - iphos_p[region][1]);
//          e_iphos -= (iphos_p[region][3] + iphos_p[region][4]*e_iphos + iphos_p[region][5]*e_iphos*e_iphos);
//          cout << dz << " mm,  " << e_iphos << " MeV" << endl;

          histos[region]->Fill(eprim, e_iphos - eprim);
        }
      }
    }

    TCanvas *c_e_vs_de = new TCanvas("c_e_vs_de");
    c_e_vs_de->Divide(1,3);
    for(int i = 0; i < 3; i++)
    {
      c_e_vs_de->cd(i+1);
      histos[i]->Draw("colz");
    }

    GetErrorHists(histos);

    SaveHists(fname, mode, savehists);
  }

  static void GetErrorHists(TH2I *h0, TH2I *h1, TH2I *h2, TCutG *cut = NULL)
  {
    TH2I *histos[] = {h0, h1, h2};
    GetErrorHists(histos, cut);
  }

  static void GetErrorHists(TH2I** histos, TCutG *cut = NULL)
  {
    TProfile **hsyspfx = new TProfile*[3];
    TProfile **hrespfx = new TProfile*[3];

    TH2I **histos_out = new TH2I*[4];
    histos_out[3] = new TH2I("hout_total", "Out Total", histos[0]->GetXaxis()->GetNbins(), 200, 1000,
        histos[0]->GetYaxis()->GetNbins(),histos[0]->GetYaxis()->GetXmin(), histos[0]->GetYaxis()->GetXmax());

    TProfile *hrespfx_total = new TProfile("hrespfx_total", "Resolution FWHM % total", histos[0]->GetXaxis()->GetNbins(), 200, 1000);
    TProfile *hsyspfx_total = new TProfile("hsyspfx_total", "Systematic deviation % total", histos[0]->GetXaxis()->GetNbins(), 200, 1000);

    double integral;
    for(int i = 0; i < 3; i++)
    {
      hrespfx[i] = new TProfile(Form("hrespfx_%d", i), Form("Resolution FWHM % region %d", i), histos[i]->GetXaxis()->GetNbins(), 200, 1000);
      hsyspfx[i] = new TProfile(Form("hsyspfx_%d", i), Form("Systematic deviation % region %d", i), histos[i]->GetXaxis()->GetNbins(), 200, 1000);

      histos_out[i] = new TH2I(Form("%s_out", histos[i]->GetName()), Form("Out %d", i), histos[i]->GetXaxis()->GetNbins(), 200, 1000,
        histos[i]->GetYaxis()->GetNbins(),histos[i]->GetYaxis()->GetXmin(), histos[i]->GetYaxis()->GetXmax());

      for(int bx = 1; bx <= histos[i]->GetXaxis()->GetNbins(); bx++)
      {
        double e = histos[i]->GetXaxis()->GetBinCenter(bx);
        double mean = 0, mean2 = 0, w = 0, y, v;

        for(int by = 1; by <= histos[i]->GetYaxis()->GetNbins(); by++)
        {
          y = histos[i]->GetYaxis()->GetBinCenter(by);

          if(cut != NULL && !cut->IsInside(e, y))
          {
            continue;
          }

          v = histos[i]->GetBinContent(bx, by);
          mean += y*v;
          mean2 += y*y*v;
          w += v;
        }

        if(w != 0)
        {
          mean /= w;
          mean2 /= w;

          hsyspfx[i]->SetBinContent(bx, w*100.*mean/e);
          hsyspfx[i]->SetBinEntries(bx, w);
          hsyspfx[i]->SetBinError(bx, 100.*TMath::Sqrt(mean2 - mean*mean)/e);

          hrespfx[i]->SetBinContent(bx, w*235.*TMath::Sqrt(mean2-mean*mean)/e);
          hrespfx[i]->SetBinError(bx, 1);
          hrespfx[i]->SetBinEntries(bx, w);

//          hrespfx[i]->Fill(e, 235.*TMath::Sqrt(mean2 - mean*mean)/e, w);

          for(int by = 1; by <= histos[i]->GetYaxis()->GetNbins(); by++)
          {
            y = histos[i]->GetYaxis()->GetBinCenter(by);

            histos_out[i]->Fill(e, y - mean, histos[i]->GetBinContent(bx,by));
            histos_out[3]->Fill(e, y - mean, histos[i]->GetBinContent(bx,by));
          }
        }
      }
    }

    for(int bx = 1; bx <= histos_out[3]->GetXaxis()->GetNbins(); bx++)
    {
      double mean = 0, mean2 = 0, w = 0;
      double e = histos_out[3]->GetXaxis()->GetBinCenter(bx);
      double v, y;

      for(int by = 1; by <= histos_out[3]->GetYaxis()->GetNbins(); by++)
      {
        y = histos_out[3]->GetYaxis()->GetBinCenter(by);
        v = histos_out[3]->GetBinContent(bx, by);

        if(cut != NULL && !cut->IsInside(e, y))
        {
          continue;
        }

        mean += y*v;
        mean2 += y*y*v;
        w += v;
      }

      if(w != 0)
      {
        mean /= w;
        mean2 /= w;

        hsyspfx_total->SetBinContent(bx, w*100.*mean/e);
        hsyspfx_total->SetBinEntries(bx, w);
        hsyspfx_total->SetBinError(bx, 100.*TMath::Sqrt(mean2 - mean*mean)/e);

        hrespfx_total->SetBinContent(bx, w*235.*TMath::Sqrt(mean2-mean*mean)/e);
        hrespfx_total->SetBinError(bx, 1.);
        hrespfx_total->SetBinEntries(bx, w);
      }
    }
  }

  void DrawIPhosZ()
  {
    if(!caloHits)
    {
      cerr << "No Calo Hits!" << endl;
      return;
    }

    if(!tracks)
    {
      cerr << "No tracks!" << endl;
      return;
    }

    TCutG *cut_iphos = GetIPhosCut();
    if(!cut_iphos)
    {
      cerr << "Could not load iPhos cut!" << endl;
      return;
    }

    R3BCalifaGeometry *geo = R3BCalifaGeometry::Instance(0x438b);
    if(!geo)
    {
      cerr << "Could not load Califa geometry manager" << endl;
      return;
    }

    TH3I *hiphos = new TH3I("hiphos", "iPhos", 250, 0, 500, 250, 0, 50, 250, 200, 1000);

    R3BMCTrack *t;
    R3BCalifaHitData *h;
    TVector3 startVertex, momentum;

    double eprim, dz;

    int j;

    for(Long64_t i = 0; i < n; i++)
    {
      if(i % 1000 == 0)
        cout << i << endl;

      events->GetEntry(i);

      t = dynamic_cast<R3BMCTrack*>(tracks->At(0));

      if(!t->GetNPoints(kCALIFA) || !caloHits->GetEntries())
        continue;

      t->GetStartVertex(startVertex);
      t->GetMomentum(momentum);
      eprim = 1000. * (t->GetEnergy() - t->GetMass());
      dz = geo->GetDistanceThroughCrystals(startVertex, momentum);

      if(dz == 0)
        continue;

      for(j = 0; j < caloHits->GetEntries(); j++)
      {
        h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(j));
        if(cut_iphos->IsInside(h->GetNf(), h->GetNs()))
          hiphos->Fill(1000. * h->GetEnergy(), dz, eprim);
      }
    }

    hiphos->Draw("iso");
  }

#define NS_DELTA 1
#define THETA_SMEARING 2.0
#define PHI_SMEARING 4.0

//#define UNIT 931.494061          // Atomic mass unit MeV/c²
#define UNIT 931.4940954          // Atomic mass unit MeV/c²

#define MASS_PROTON 938.272
// Ca-48, 550 AMeV
#define MASS_BEAM (48.0*UNIT-44.2234)
#define T_BEAM (550.*47.952524131)
// K-47
//#define MASS_RESIDUAL (46.961664836*UNIT)
#define MASS_RESIDUAL (47.0*UNIT-35.7089)

#define BETA_BEAM TMath::Sqrt(1-MASS_BEAM*MASS_BEAM/((MASS_BEAM+T_BEAM)*(MASS_BEAM+T_BEAM)))
#define GAMMA_BEAM (1./TMath::Sqrt(1-BETA_BEAM*BETA_BEAM))

#define TRACK_SIM_RES 0
#define TRACK_SIM_RES_THETA .20
#define TRACK_SIM_RES_PHI .027
#define TRACK_SIM_RES_E (.01 / TMath::Sqrt(100))  // 1% @ 100 MeV

//#define USE_TRACK_ANGLES 0
#define USE_TRACK_ENERGY 0
#define USE_STARTRACK_ANGLES 0
#define DIFF_ANGLES_MAX 5.

#define USE_E_VS_THETA_CUT 0

#define REQUIRE_DZ 0

void P2P(Long64_t nMax = 0, bool usetracks = true, bool useiphos = false, bool use_track_angles = false, bool cut_proton = true, const char *fout = NULL, const char *mode = "update")
  {
    const char *pwd = gDirectory->GetPath();

    if(!caloHits)
    {
      cerr << "No CalifaHitData in tree" << endl;
      return;
    }

    if(usetracks && !tracks)
    {
      cerr << "No tracks in tree! Switching off >usetracks<" << endl;
      usetracks = false;
    }

    TF1 *fproton = NULL;
    TF1 **fiphos = NULL;
#if USE_E_VS_THETA_CUT
    TCutG *cut_evstheta = NULL;
#endif
//    double iphos_nfmax[3] = {133.4, 121.5, 116.6};
    double iphos_nfmax[3] = {153, 142, 135};
    if(!usetracks && (useiphos || cut_proton))
    {
      TFile *fcut = new TFile("qpid_cuts.root", "read");
      if(!fcut)
      {
        cerr << "Could not open file qpid_cuts.root" << endl;
        return;
      }
      fproton = dynamic_cast<TF1*>(fcut->Get("fproton"));
      if(!fproton)
      {
        cerr << "Could not load TF1 fproton from qpid_cuts.root" << endl;
        return;
      }

      fiphos = new TF1*[3];
      for(int i = 0; i < 3; i++)
      {
        fiphos[i] = new TF1(Form("fiphos_%d", i), "[0]*exp(-[1]*[3])*(1-exp([1]*x))+[2]*x",0,iphos_nfmax[i]);
        fiphos[i]->SetParameters(fproton->GetParameter(0),
            fproton->GetParameter(1),
            fproton->GetParameter(2),
            iphos_nfmax[i]);
      }

#if USE_E_VS_THETA_CUT
      cut_evstheta = dynamic_cast<TCutG*>(fcut->Get("cut_evstheta"));
      if(!cut_evstheta)
      {
        cerr << "Could not load TCutG cut_evstheta from qpid_cuts.root" << endl;
        return;
      }
#endif
    }

    gDirectory->cd(pwd);

    TH2 *hthetavstheta = getTH2(1800, 0, 180, 1800, 0, 180, "hthetavstheta");
    TH2 *hphivsphi = getTH2(3600, -180, 180, 3600, -180, 180, "hphivsphi");
    TH2 *hevse = getTH2(1000, 0, 1000, 1000, 0, 1000, "hevse");
    TH2 *hevstheta = getTH2(1800, 0, 180, 1000, 0, 1000, "hevstheta");

    TH1 *hthetasum = getTH1(3600, 0, 360, "hthetasum");
    TH1 *hphidiff = getTH1(3600, 0, 360, "hphidiff");
    TH1 *hesum = getTH1(2000, 0, 2000, "hesum");

    TH1 *hpx_cm = getTH1(10000,-500,500,"hpx_cm");
    TH1 *hpy_cm = getTH1(10000,-500,500,"hpy_cm");
    TH1 *hpz_cm = getTH1(10000,-500,500,"hpz_cm");
    TH1 *hpmag_cm = getTH1(20000,0,2000,"hpmag_cm");
    TH1 *he_cm = getTH1(100000,-20,20,"he_cm");
    TH1 *hM = getTH1(1000000, 0, 100, "hM");

    TH1 *hpx_cm_iphos = getTH1(10000,-500,500,"hpx_cm_iphos");
    TH1 *hpy_cm_iphos = getTH1(10000,-500,500,"hpy_cm_iphos");
    TH1 *hpz_cm_iphos = getTH1(10000,-500,500,"hpz_cm_iphos");
    TH1 *hpmag_cm_iphos = getTH1(20000,0,2000,"hpmag_cm_iphos");
    TH1 *he_cm_iphos = getTH1(100000,-20,20,"he_cm_iphos");
    TH1 *hM_iphos = getTH1(1000000, 0, 100, "hM_iphos");

    TH1 *hpx_cm_stopped = getTH1(10000,-500,500,"hpx_cm_stopped");
    TH1 *hpy_cm_stopped = getTH1(10000,-500,500,"hpy_cm_stopped");
    TH1 *hpz_cm_stopped = getTH1(10000,-500,500,"hpz_cm_stopped");
    TH1 *hpmag_cm_stopped = getTH1(20000,0,2000,"hpmag_cm_stopped");
    TH1 *he_cm_stopped = getTH1(100000,-20,20,"he_cm_stopped");
    TH1 *hM_stopped = getTH1(1000000, 0, 100, "hM_stopped");

    TH2 *hpypx = getTH2(2000, -500, 500, 2000,-500,500, "hpypx");
    TH2 *hpzpx = getTH2(2000, -500, 500, 2000,-500,500, "hpzpx");
    TH2 *hpzpy = getTH2(2000, -500, 500, 2000,-500,500, "hpzpy");

    TH2I *hqpid = new TH2I("hqpid", "QPID", 500,0,250,500,0,250);
    TH2I *hqpid_p = new TH2I("hqpid_p", "QPID proton", 500, 0, 250, 500, 0, 250);
    TH2I *hqpid_iphos = new TH2I("hqpid_iphos", "QPID iPhos", 500, 0, 250, 500, 0, 250);

    TH2I *hqpid_false = new TH2I("hqpid_false", "QPID false", 500,0,250,500,0,250);

    TH1 *hdtheta_good = getTH1(1000,-10,10, "#Delta #Theta Good");
    TH1 *hdtheta_bad = getTH1(1000,-10,10, "#Delta #Theta Bad");
    TH1 *hdphi_good = getTH1(1000,-10,10, "#Delta #Phi Good");
    TH1 *hdphi_bad = getTH1(1000,-10,10, "#Delta #Phi Bad");
    TH1 *hdangle_good = getTH1(1000,0,30, "#Delta Total Good");
    TH1 *hdangle_bad = getTH1(1000,0,30, "#Delta Total Bad");

    TH1 *savehists[] = {hthetavstheta, hphivsphi, hevse, hevstheta, hthetasum, hphidiff, hesum, hpx_cm, hpy_cm, hpz_cm, hpmag_cm, he_cm,
      hM, hpypx, hpzpx, hpzpy, hqpid, hqpid_p, hqpid_iphos, 
      hpx_cm_iphos, hpy_cm_iphos, hpz_cm_iphos, hpmag_cm_iphos, he_cm_iphos, hM_iphos,
      hpx_cm_stopped, hpy_cm_stopped, hpz_cm_stopped, hpmag_cm_stopped, he_cm_stopped, hM_stopped,
      NULL};

    TRandom3 rnd;

    R3BMCTrack *t;
    R3BCalifaHitData *h;
    TVector3 v;

    Long64_t j;

    double theta[2], phi[2], e[2], eMax[2];
    Long64_t idxMax[2];
    double fproton_Ns, fiphos_Ns, dist_fproton, dist_fiphos;

    bool is_proton;
    bool tmp_iphos;
    bool is_iphos[2];
    double e_proton, e_theta;

   int region;

   // fitted iPhos parameters for each anglular region / crystal type
//    double iphos_p[3][3] =
//    {
//      // a      b       c
//      {3.44151e+04,9.50616e+01,7.72456e-01},  // region 0
//      {2.75582e+04,7.93523e+01,7.39312e-01},  // region 1
//      {2.80111e+04,7.26665e+01,6.83356e-01}   // region 2
//    };


//    double iphos_p[3][6] =
//    {
//      // a      b       c       alpha  beta gamma
//      {3.624e4,9.486e1,7.108e-1,-107.3,.45,-3.83e-4},  // region 0
//      {2.965e4,7.732e1,6.739e-1,-98.0,.46,-4.27e-4},  // region 1
//      {2.808e4,7.352e1,6.655e-1,-74.6,.36,-3.31e-4}   // region 2
//    };

//    double iphos_p[3][6] =
//    {
//      {4.10208e4,8.60752e1,5.96453e-1,0,0,0},
//      {3.19005e4,7.45934e1,5.68374e-1,0,0,0},
//      {2.78814e4,7.77798e1,6.75553e-1,0,0,0}
//    };


#if !IPHOS_VAR_Z
//    double iphos_p[3][6] =
//    {
//      {3.60439e+04,9.16985e+01,7.83688e-01,0,0,0},
//      {3.00288e+04,7.51223e+01,6.60387e-01,0,0,0},
//      {2.96317e+04,7.03858e+01,5.95214e-01,0,0,0}
//    };

//      double iphos_p[3][6] = 
//      {
//        {3.84671e+04,9.27921e+01,6.29219e-01,0,0,0},
//        {3.31084e+04,7.21796e+01,5.29870e-01,0,0,0},
//        {2.90035e+04,7.15755e+01,6.16260e-01,0,0,0}
//      };

      double iphos_p[3][6] = 
      {
        {3.68650e+04,9.48732e+01,6.79594e-01,0,0,0},
        {2.94541e+04,7.84205e+01,6.74277e-01,0,0,0},
        {2.88743e+04,7.19554e+01,6.21420e-01,0,0,0}
      };


#else
//  double iphos_a[3] = {90464.9,-737.49,2.23384};
//  double iphos_b[2] = {-1.29684,0.42529};
//  double iphos_c[3] = {4.82288,-0.00875979,-464.413};

  double iphos_a[3] = {147.446, 0.101594, -78.283};
  double iphos_b[2] = {0.401583, -1.4442};
  double iphos_c[4] = {-2.18744, -0.238226, 0.000639924, 0.560853};
#endif

#if REQUIRE_DZ
  R3BCalifaGeometry *geo = R3BCalifaGeometry::Instance(0x438b);
  TVector3 vStart(0,0,0), vDirection;

    double dz_region[3] = {220,180,170};
#endif

    // 4-vector of target proton in projectile rest frame
    TLorentzVector p(0, 0, 0, MASS_PROTON);
    p.Boost(0, 0, -BETA_BEAM);
    cout << "Beta: " << BETA_BEAM << endl;
    cout << "Target proton: E = " << p.Energy() << ", pz = " << p.Pz() << endl;
    // 4-vector of projectile in its rest frame
    TLorentzVector P(0, 0, 0, MASS_BEAM);
    // 4-vectors of outgoing protons
    TLorentzVector q[2];

    double r_nsnf;

    if(nMax == 0)
    {
      nMax = n;
    }

    for(Long64_t i = 0; i < nMax; i++)
    {
      if(i % 1000 == 0)
        cout << i << endl;

      events->GetEntry(i);

      if(usetracks)
      {
        if(tracks->GetEntries() < 2)
          continue;

        for(j = 0; j < 2; j++)
        {
          t = dynamic_cast<R3BMCTrack*>(tracks->At(j));
          if(t->GetMotherId() != -1)
            break;

          t->GetMomentum(v);
          theta[j] = v.Theta() * 180. / TMath::Pi();
          phi[j] = v.Phi() * 180. / TMath::Pi();
          e[j] = 1000. * (t->GetEnergy() - t->GetMass());

#if TRACK_SIM_RES
          theta[j] = (double)TMath::Nint(theta[j]/TRACK_SIM_RES_THETA)*TRACK_SIM_RES_THETA;
          phi[j] = (double)TMath::Nint(phi[j]/TRACK_SIM_RES_PHI)*TRACK_SIM_RES_PHI;
          e[j] += rnd.Gaus(0,TRACK_SIM_RES_E * TMath::Sqrt(e[j]) / 2.35); 
#endif

          v.SetMagThetaPhi(
              TMath::Sqrt((e[j]+MASS_PROTON)*(e[j]+MASS_PROTON)-MASS_PROTON*MASS_PROTON),
              theta[j]*TMath::Pi()/180., phi[j]*TMath::Pi()/180.);
          q[j] = TLorentzVector(v, e[j] + MASS_PROTON);

          hevstheta->Fill(theta[j], e[j]);
        }
        if(j != 2)
          continue;
      }
      else
      {
        idxMax[0] = -1; idxMax[1] = -1;
        eMax[0] = 0; eMax[1] = 0;
        for(j = 0; j < caloHits->GetEntries(); j++)
        {
          h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(j));
#if REQUIRE_DZ
          if(h->GetNbOfCrystalHits() != 1)
          {
            continue;
          }
#endif

          if(use_track_angles)
          {
          // Try to corelate tracks to hits
          bool found_matching_track = false;
            for(int it = 0; it < 2; it++)
            {
              t = dynamic_cast<R3BMCTrack*>(tracks->At(it));
              t->GetMomentum(v);
              if(TMath::Sqrt(TMath::Power(v.Theta() - h->GetTheta(), 2)
                    + TMath::Power(v.Phi() - h->GetPhi(), 2))*180./TMath::Pi() <= DIFF_ANGLES_MAX)
              {

#if TRACK_SIM_RES
                h->SetTheta((double)TMath::Nint(v.Theta()*180./TMath::Pi()/TRACK_SIM_RES_THETA)
                    *TRACK_SIM_RES_THETA*TMath::Pi()/180.);
                h->SetPhi((double)TMath::Nint(v.Phi()*180./TMath::Pi()/TRACK_SIM_RES_PHI)
                    *TRACK_SIM_RES_PHI*TMath::Pi()/180.);
#else
                h->SetTheta(v.Theta());
                h->SetPhi(v.Phi());
#endif

                found_matching_track = true;
                break;
                }
            }
            if(!found_matching_track)
              continue;

          }

          if(h->GetNf() != 0)
          {
            r_nsnf = h->GetNs()/h->GetNf();
            h->SetNf(1000.*h->GetEnergy()/(r_nsnf + 1.));
            h->SetNs(1000.*h->GetEnergy()/(1./r_nsnf + 1.));
          }
          else
          {
            h->SetNs(1000.*h->GetEnergy());
          }

          e_theta = h->GetTheta() * 180. / TMath::Pi();

          if(e_theta < 40)
            region = 0;
          else if(e_theta < 55)
            region = 1;
          else
            region = 2;

#if REQUIRE_DZ
          vDirection.SetMagThetaPhi(1., h->GetTheta(), h->GetPhi());
          double dz = 10.*geo->GetDistanceThroughCrystals(vStart, vDirection);

          if(TMath::Abs(dz - dz_region[region]) > dz_region[region]/100.)
          {
            continue;
          }
#endif

          hqpid->Fill(h->GetNf(),h->GetNs());

          if(useiphos || cut_proton)
          {
            fproton_Ns = fproton->Eval(h->GetNf());
            fiphos_Ns = fiphos[region]->Eval(h->GetNf());
            dist_fproton = TMath::Abs(fproton_Ns - h->GetNs());
            dist_fiphos = TMath::Abs(fiphos_Ns - h->GetNs());
            if(((dist_fproton < dist_fiphos || !useiphos) || h->GetNf() > iphos_nfmax[region]) && dist_fproton < NS_DELTA && h->GetEnergy() >= 0.01)
//            if(dist_fproton < NS_DELTA && h->GetEnergy() >= .01)            
            {
              // Stopped proton
              is_proton = true;
              e_proton = h->GetEnergy();
              tmp_iphos = false;
            }
            else if(((dist_fiphos < dist_fproton && h->GetNf() <= iphos_nfmax[region] && dist_fiphos < NS_DELTA)
              || (h->GetNf() < iphos_nfmax[2] && h->GetNs() > fiphos_Ns)) && h->GetEnergy() >= .1 && useiphos)
//            else if(h->GetNs()  > fproton_Ns && h->GetEnergy() > .1)
            {
              // Punched through -> iPhos energy reconstruction

#if IPHOS_VAR_Z
          if(dz >= 160)
          {
              is_proton = true;
              tmp_iphos = true;

//          double a = iphos_a[0] + iphos_a[1]*dz + iphos_a[2]*dz*dz;
//          double b = iphos_b[0] + iphos_b[1]*dz;
//          double c = iphos_c[0] + iphos_c[1]*dz + iphos_c[2]/dz;

          double a = iphos_a[0]*dz*dz + iphos_a[1]*dz + iphos_a[2];
          double b = iphos_b[0]*(dz - iphos_b[1]);
          double c = iphos_c[0]/(dz - iphos_c[1]) + iphos_c[2]*dz + iphos_c[3];

          e_proton = (a/(h->GetEnergy()*1000. - b) + c*(h->GetEnergy()*1000. - b))/1000.;

          cout << dz << " mm,  " << e_proton << " GeV" << endl;
          }
          else
          {
            is_proton = false;
          }
#else

#if REQUIRE_DZ
              if(TMath::Abs(dz - dz_region[region]) > dz_region[region]/100.)
              {
                is_proton = false;
                continue;
              }
#endif

              is_proton = true;
              tmp_iphos = true;


              e_proton = (iphos_p[region][0] / (h->GetEnergy()*1000. - iphos_p[region][1])
               + iphos_p[region][2]*(h->GetEnergy()*1000. - iphos_p[region][1]));
              e_proton -= (iphos_p[region][3] + iphos_p[region][4]*e_proton + iphos_p[region][5]*e_proton*e_proton);
              e_proton /= 1000.;
#endif
            }
            else
              is_proton = false;
          }
          else
          {
            is_proton = true;
            e_proton = h->GetEnergy();
            tmp_iphos = false;
          }

          if(is_proton && !tmp_iphos)
            hqpid_p->Fill(h->GetNf(),h->GetNs());
          else if(is_proton)
            hqpid_iphos->Fill(h->GetNf(),h->GetNs());

          if(is_proton && (h->GetPhi() < 0 && (idxMax[0] == -1 || e_proton > eMax[0])))
          {
            idxMax[0] = j;
            eMax[0] = e_proton;
            is_iphos[0] = tmp_iphos;
          }
          else if(is_proton && (h->GetPhi() >= 0 && (idxMax[1] == -1 || e_proton > eMax[1])))
          {
            idxMax[1] = j;
            eMax[1] = e_proton;
            is_iphos[1] = tmp_iphos;
          }
        }

        if(idxMax[0] == -1 || idxMax[1] == -1)
          continue;

//        // Discard if both protons are punch through (= misidentification)
//        if(is_iphos[0] && is_iphos[1])
//          continue;

//#if USE_TRACK_ANGLES
#if !USE_STARTRACK_ANGLES

#if USE_TRACK_ENERGY
        // Try to corelate tracks to hits
        bool found_matching_track;
        for(int ih = 0; ih < 2; ih++)
        {
          h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(idxMax[ih]));
          found_matching_track = false;
          for(int it = 0; it < 2; it++)
          {
            t = dynamic_cast<R3BMCTrack*>(tracks->At(it));
            t->GetMomentum(v);
            if(TMath::Sqrt(TMath::Power(v.Theta() - h->GetTheta(), 2)
                  + TMath::Power(v.Phi() - h->GetPhi(), 2))*180./TMath::Pi() <= DIFF_ANGLES_MAX)
            {

              double et = t->GetEnergy() - t->GetMass();
              if(TMath::Abs((eMax[ih] - et)/et) > .1)
              {
                cout << "--> " << i << ": " << endl
                  << " E track: " << (t->GetEnergy() - t->GetMass())*1000. << "MeV, hit: " << (h->GetEnergy()*1000.) << " MeV, reconstructed: " << (eMax[ih]*1000.) << " MeV" << endl
                  << " iPhos: " << (is_iphos[ih] ? "yes" : "no") << endl
                  << " Theta: " << (h->GetTheta()*180./TMath::Pi()) << ", Phi: " << (h->GetPhi()*180./TMath::Pi()) << endl
                  << " Track: " << it << ", Mother: " << t->GetMotherId() << endl;

                hdtheta_bad->Fill((h->GetTheta() - v.Theta())*180./TMath::Pi());
                hdphi_bad->Fill((h->GetPhi() - v.Phi())*180./TMath::Pi());
                hdangle_bad->Fill(TMath::Sqrt(TMath::Power((h->GetTheta() - v.Theta())*180./TMath::Pi(), 2.) + TMath::Power((h->GetPhi() - v.Phi())*180./TMath::Pi(),2.)));
              }
              else
              {
                hdtheta_good->Fill((h->GetTheta() - v.Theta())*180./TMath::Pi());
                hdphi_good->Fill((h->GetPhi() - v.Phi())*180./TMath::Pi());
                hdangle_good->Fill(TMath::Sqrt(TMath::Power((h->GetTheta() - v.Theta())*180./TMath::Pi(), 2.) + TMath::Power((h->GetPhi() - v.Phi())*180./TMath::Pi(),2.)));
              }

              eMax[ih] = t->GetEnergy() - t->GetMass();

              found_matching_track = true;
              break;
            }
          }
          if(!found_matching_track)
            break;
        }

        if(!found_matching_track)
          continue;

#endif

#else
//#elif USE_STARTRACK_ANGLES

        if(startrackHits == NULL)
        {
          cerr << "No StarTracker hits in tree!" << endl;
          return;
        }

        // Try to corelate tracks to hits
        bool found_matching_track;
        for(int ih = 0; ih < 2; ih++)
        {
          h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(idxMax[ih]));
          found_matching_track = false;
          for(int it = 0; it < startrackHits->GetEntries(); it++)
          {
            R3BSTaRTrackerHit* st = dynamic_cast<R3BSTaRTrackerHit*>(startrackHits->At(it));
            v.SetXYZ(st->GetPx(),st->GetPy(),st->GetPz());
            if(TMath::Sqrt(TMath::Power(v.Theta() - h->GetTheta(), 2)
                  + TMath::Power(v.Phi() - h->GetPhi(), 2))*180./TMath::Pi() <= DIFF_ANGLES_MAX)
            {
              h->SetTheta(v.Theta());
              h->SetPhi(v.Phi());
              found_matching_track = true;
              break;
            }
          }
          if(!found_matching_track)
            break;
        }

        if(!found_matching_track)
          continue;

#endif

        // Randomly assign indices
        int _[2];
        _[0] = TMath::Nint(rnd.Uniform());
        _[1] = 1 - _[0];
        for(j = 0; j < 2; j++)
        {
          h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(idxMax[j]));
          theta[_[j]] = h->GetTheta() * 180. / TMath::Pi() + rnd.Uniform(-THETA_SMEARING/2., THETA_SMEARING/2.);
          phi[_[j]] = h->GetPhi() * 180. / TMath::Pi() + rnd.Uniform(-PHI_SMEARING/2.,PHI_SMEARING/2.);
          e[_[j]] = 1000. * eMax[j];

          v.SetMagThetaPhi(TMath::Sqrt((e[_[j]] + MASS_PROTON)*(e[_[j]]+MASS_PROTON) - MASS_PROTON*MASS_PROTON), h->GetTheta(), h->GetPhi());
          q[_[j]] = TLorentzVector(v, e[_[j]] + MASS_PROTON);

          hevstheta->Fill(theta[_[j]], e[_[j]]);
        }

      }

      hthetavstheta->Fill(theta[0], theta[1]);
      hphivsphi->Fill(phi[0], phi[1]);

      if((theta[0] + theta[1] >= 70. && theta[0] + theta[1] <= 90.)
#if USE_E_VS_THETA_CUT
        && (cut_evstheta == NULL || 
          (cut_evstheta->IsInside(theta[0], e[0]) && cut_evstheta->IsInside(theta[1], e[1])))
#endif
        )
      {
        hevse->Fill(e[0], e[1]);
        hesum->Fill(e[0] + e[1]);

//        hevstheta->Fill(theta[0], e[0]);
//        hevstheta->Fill(theta[1], e[1]);

        // Detectet protons in projectile rest frame
        q[0].Boost(0, 0, -BETA_BEAM);
        q[1].Boost(0, 0, -BETA_BEAM);

        TLorentzVector Q = p + P - q[0] - q[1];
        hpx_cm->Fill(-Q.Px());
        hpy_cm->Fill(-Q.Py());
        hpz_cm->Fill(-Q.Pz());
        hpmag_cm->Fill(Q.Vect().Mag());
        // Invariant mass spectrum
        hM->Fill(Q.Mag()/1000.);
        // Invariant mass should be rest mass + excitation energy, so
        // magnitute of Q (invariant mass) - rest mass should give excitation energy, right?
        he_cm->Fill(Q.Mag() - MASS_RESIDUAL);

        if(is_iphos[0] || is_iphos[1])
        {
          // At least one proton energy was reconstructed using iPhos
          hpx_cm_iphos->Fill(-Q.Px());
          hpy_cm_iphos->Fill(-Q.Py());
          hpz_cm_iphos->Fill(-Q.Pz());
          hpmag_cm_iphos->Fill(Q.Vect().Mag());
          // Invariant mass spectrum
          hM_iphos->Fill(Q.Mag()/1000.);
          // Invariant mass should be rest mass + excitation energy, so
          // magnitute of Q (invariant mass) - rest mass should give excitation energy, right?
          he_cm_iphos->Fill(Q.Mag() - MASS_RESIDUAL);
        }
        else
        {
          // Both protons were stopped
          hpx_cm_stopped->Fill(-Q.Px());
          hpy_cm_stopped->Fill(-Q.Py());
          hpz_cm_stopped->Fill(-Q.Pz());
          hpmag_cm_stopped->Fill(Q.Vect().Mag());
          // Invariant mass spectrum
          hM_stopped->Fill(Q.Mag()/1000.);
          // Invariant mass should be rest mass + excitation energy, so
          // magnitute of Q (invariant mass) - rest mass should give excitation energy, right?
          he_cm_stopped->Fill(Q.Mag() - MASS_RESIDUAL);
        }

        hpypx->Fill(-Q.Px(), -Q.Py());
        hpzpx->Fill(-Q.Pz(), -Q.Px());
        hpzpy->Fill(-Q.Pz(), -Q.Py());

        if(Q.Mag() - MASS_RESIDUAL < -.5 && !usetracks)
        {
          h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(idxMax[0]));
          hqpid_false->Fill(h->GetNf(), h->GetNs());
          h = dynamic_cast<R3BCalifaHitData*>(caloHits->At(idxMax[1]));
          hqpid_false->Fill(h->GetNf(), h->GetNs());

          cout << i << ": " << e[0] << " MeV, " << e[1] << " MeV" << endl;
        }
      }

      hthetasum->Fill(theta[0] + theta[1]);
      hphidiff->Fill(TMath::Abs(phi[0] - phi[1]));

    }

    TCanvas *c1 = new TCanvas();
    c1->cd();
    hqpid->Draw("colz");
    if(!usetracks && useiphos)
    {
      fproton->Draw("same");
      for(int i = 0; i < 3; i++)
        fiphos[i]->Draw("same");
    }

    TCanvas *c2 = new TCanvas();
    c2->cd();
    hqpid_p->Draw("colz");

    TCanvas *c3 = new TCanvas();
    c3->cd();
    hqpid_iphos->Draw("colz");

    TFile *ffout = SaveHists(fout, mode, savehists, false);

    if(ffout != NULL && useiphos)
    {
      ffout->cd();
      for(int i = 0; i < 3; i++)
        fiphos[i]->Write();

      ffout->Close();
    }

    gDirectory->cd(pwd);
  }

  int GetCrystal(double phi, double theta)
  {

    for(int i = 0; i < 128; i++)
    {
      if(phi == channelPhi[i] && theta == channelTheta[i])
        return i;
    }

    return -1;
  }

  TH1* getTH1(int bins, double min, double max, const char *name, TH1 *h = NULL)
  {
    static int i = 0;

    if(h)
      return h;

    return new TH1I(Form("h1_%d", ++i), name, bins, min, max);
  }

  TH2* getTH2(int binsx, int minx, int maxx, int binsy, int miny, int maxy, const char *name, TH2 *h = NULL)
  {
    static int i = 0;

    if(h)
      return h;

    return new TH2I(Form("h2_%d", ++i), name, binsx, minx, maxx, binsy, miny, maxy);
  }

  TFile* SaveHists(const char *fout, const char *mode, TH1 **hists, bool closeFile = true)
  {
    if(fout != NULL)
    {
      const char *pwd = gDirectory->GetPath();
      TFile *ffout = new TFile(fout, mode);
      if(!ffout)
      {
        cerr << "Could not create output file " << fout << endl;
        return NULL;
      }
  
      ffout->cd();
      for(TH1 *h = *hists; h != NULL; h = *(++hists))
        h->Write();

      if(closeFile)
      {
        ffout->Close();
      }
  
      gDirectory->cd(pwd);

      return ffout;
    }

    return NULL;
  }

static TProfile* ProfileX(TH2 *h, const char *name, TCutG *cut = NULL, bool square = false)
  {
    Int_t nx = h->GetNbinsX();
    Int_t ny = h->GetNbinsY();
    Double_t bwx = h->GetXaxis()->GetBinWidth(1);

    TProfile *pfx = new TProfile(name, h->GetTitle(), nx, h->GetXaxis()->GetBinCenter(1)-bwx/2., h->GetXaxis()->GetBinCenter(nx)+bwx/2.);

    Double_t x, y, c;
    for(Int_t ix = 1; ix <= nx; ix++)
    {
      x = h->GetXaxis()->GetBinCenter(ix);
      for(Int_t iy = 1; iy < ny; iy++)
      {
        y = h->GetYaxis()->GetBinCenter(iy);
        c = h->GetBinContent(ix, iy);
        if(c > 0 && (cut == NULL || cut->IsInside(x, y)))
          pfx->Fill(x,square ? y*y : y,c);
      }
    }

    pfx->Draw();
    return pfx;
  }

};
