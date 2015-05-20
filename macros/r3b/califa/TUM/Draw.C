#include <iostream>

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

#include <R3BMCTrack.h>
#include <R3BCaloHitSim.h>
#include <R3BCaloCrystalHitSim.h>
#include <R3BCaloGeometry.h>
#include <R3BDetectorList.h>

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

  R3BCaloGeometry *geo = R3BCaloGeometry::Instance(fGeometryVersion);

  for(int i = 0; i < 128; i++)
  {
    crystalId = fbxPetalCrystalIdBase[i%64] + 4*ptlAlveoliCopyBase[i/64];
    channelMapping[i] = crystalId;

    geo->GetAngles(crystalId, &channelTheta[i], &channelPhi[i], &foo);

//    cout << i << ": " << channelTheta[i] << ", " << channelPhi[i] << endl;
  }
}
  
  
  TClonesArray *hits;
  TClonesArray *caloHits;

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
    if((b = events->GetBranch("CaloCrystalHit")))
    {
      // Experimental data
      hits = new TClonesArray("R3BCaloCrystalHit");
    }
    else if((b = events->GetBranch("CaloCrystalHitSim")))
    {
      // Simulated data
      hits = new TClonesArray("R3BCaloCrystalHitSim");
    }
    else
    {
      cerr << "No crystal hit found in tree!" << endl;
      return;
    }

//    Note (1): Branch::SetAddress leads to segfault in case of TChain
//    b->SetAddress(&hits);
    events->SetBranchAddress(b->GetName(), &hits);

    if(caloHits)
      delete caloHits;

    if((b = events->GetBranch("CaloHit")))
    {
      caloHits = new TClonesArray("R3BCaloHit");
//      b->SetAddress(&caloHits);
      events->SetBranchAddress("CaloHit", &caloHits);
    }
    else if((b = events->GetBranch("CaloHitSim")))
    {
      caloHits = new TClonesArray("R3BCaloHitSim");
//      b->SetAddress(&caloHits);
      events->SetBranchAddress("CaloHitSim", &caloHits);
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
        h->Fill(((R3BCaloCrystalHit*)hits->At(j))->GetCrystalId());
      }
    }

    h->Draw();
  }

  void Energy(int crystalId = -1, TH1 *hist = NULL)
  {
    TH1 *h = getTH1(1000, 0, 1000, "Energy (MeV)", hist);
    R3BCaloCrystalHit *hit;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = (R3BCaloCrystalHit*)hits->At(j);
        if(crystalId == -1 || crystalId == hit->GetCrystalId())
          h->Fill(hit->GetEnergy() * 1000.0);
      }
    }

    h->Draw();
  }

  void PID(int crystalId = -1, TH2 *hist = NULL)
  {
    TH2 *h = getTH2(500, 0, 1000, 500, 0, 1000, "Ns vs Nf (MeV)", hist);
    R3BCaloCrystalHit *hit;

    for(Long64_t i = 0; i + 2000 < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = (R3BCaloCrystalHit*)hits->At(j);
        if(crystalId == -1 || crystalId == hit->GetCrystalId())
          h->Fill(hit->GetNf(), hit->GetNs());
      }
    }

    h->Draw("colz");
  }

  void CaloPID(TH2 *hist = NULL)
  {
    if(!caloHits)
    {
      cerr << "No CaloHit in tree!" << endl;
      return;
    }

    TH2 *h = getTH2(500, 0, 500, 500, 0, 500, "Ns vs Nf (MeV)", hist);
    R3BCaloHit *hit;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < caloHits->GetEntries(); j++)
      {
        hit = (R3BCaloHit*)caloHits->At(j);
        h->Fill(hit->GetNf(), hit->GetNs());
      }
    }

    h->Draw("colz");
  }



  void EnergyVsCrystalId(TH2 *hist = NULL)
  {
    TH2 *h = getTH2(500, 0, 4000, 500, 0, 1000, "Energy vs Crystal Id", hist);
    R3BCaloCrystalHit *hit;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = (R3BCaloCrystalHit*)hits->At(j);
        h->Fill(hit->GetCrystalId(), hit->GetEnergy()*1000.0);
      }
    }

    h->Draw("colz");
  }

  void EnergyVsTheta(TH2 *hist = NULL)
  {
    TH2 *h = getTH2(50, 0, 180, 500, 0, 1000, "Energy vs Theta", hist);
    R3BCaloCrystalHit *hit;
    Double_t phi, theta, rho;

    R3BCaloGeometry *geo = R3BCaloGeometry::Instance(fGeometryVersion);
  
    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < hits->GetEntries(); j++)
      {
        hit = (R3BCaloCrystalHit*)hits->At(j);
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
      cerr << "No CaloHit in tree!" << endl;
      return;
    }

    TH2 *h = getTH2(50, 0, 180, 500, 0, 1000, "Energy vs Theta", hist);
    R3BCaloHit *hit;

    for(Long64_t i = 0; i < n; i++)
    {
      events->GetEntry(i);

      for(int j = 0; j < caloHits->GetEntries(); j++)
      {
        hit = (R3BCaloHit*)caloHits->At(j);
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
      cerr << "No CaloHit or MCTrack in tree!" << endl;
      return;
    }

    hphi = getTH2(50, -180, 180, 50, -180, 180, "Phi Hit vs Phi Track", hphi);
    htheta = getTH2(50, 0, 180, 50, 0, 180, "Theta Hit vs Theta Track", htheta);

    R3BMCTrack *t;
    R3BCaloHit *h;
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
        h = dynamic_cast<R3BCaloHit*>(caloHits->At(j));
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
    TFile *fcut = TFile::Open("data/cuts.root", "read");
    return dynamic_cast<TCutG*>(fcut->Get("cut_iphos"));
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
    R3BCaloHit *hit;
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
        hit = dynamic_cast<R3BCaloHit*>(caloHits->At(j));

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

  void DzVsTheta()
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

    R3BCaloGeometry *geo = R3BCaloGeometry::Instance(0x438b);
    if(!geo)
    {
      cerr << "Could not load Califa geometry manager" << endl;
      return;
    }

    TH2I *hdztheta = new TH2I("hdztheta", "iPhos", 180, 0, 90, 200, 0, 50);
    TH2I *hdztheta_track = new TH2I("hdztheta_track", "iPhos", 180, 0, 90, 200, 0, 50);

    R3BMCTrack *t;
    R3BCaloHit *h;
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
        h = dynamic_cast<R3BCaloHit*>(caloHits->At(j));
        if(cut_iphos->IsInside(h->GetNf(), h->GetNs()))
        {
          hdztheta->Fill(h->GetTheta()*180./TMath::Pi(), dz);
          hdztheta_track->Fill(momentum.Theta()*180./TMath::Pi(), dz);
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


  void IPhosEvsDE()
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

    R3BMCTrack *t;
    R3BCaloHit *h;
    
    double eprim, theta;

    int j;

    for(Long64_t i = 0; i < n; i++)
    {
      if(i % 1000 == 0)
        cout << i << endl;

      events->GetEntry(i);

      t = dynamic_cast<R3BMCTrack*>(tracks->At(0));

      if(!t->GetNPoints(kCALIFA) || !caloHits->GetEntries())
        continue;

      eprim = 1000. * (t->GetEnergy() - t->GetMass());

      for(j = 0; j < caloHits->GetEntries(); j++)
      {
        h = dynamic_cast<R3BCaloHit*>(caloHits->At(j));
        if(cut_iphos->IsInside(h->GetNf(), h->GetNs()))
        {
          theta = 180.0 * h->GetTheta() / TMath::Pi();
          if(theta < 40)
            histos[0]->Fill(1000. * h->GetEnergy(), eprim);
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

    R3BCaloGeometry *geo = R3BCaloGeometry::Instance(0x438b);
    if(!geo)
    {
      cerr << "Could not load Califa geometry manager" << endl;
      return;
    }

    TH3I *hiphos = new TH3I("hiphos", "iPhos", 250, 0, 500, 250, 0, 50, 250, 200, 1000);

    R3BMCTrack *t;
    R3BCaloHit *h;
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
        h = dynamic_cast<R3BCaloHit*>(caloHits->At(j));
        if(cut_iphos->IsInside(h->GetNf(), h->GetNs()))
          hiphos->Fill(1000. * h->GetEnergy(), dz, eprim);
      }
    }

    hiphos->Draw("iso");
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

  TH1* getTH1(int bins, int min, int max, const char *name, TH1 *h = NULL)
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
};
