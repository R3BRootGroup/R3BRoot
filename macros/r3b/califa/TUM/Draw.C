class Draw
{
protected:
  TClonesArray *hits;
  TClonesArray *caloHits;
  TTree *events;
  Long64_t n;

public:
    
  int fGeometryVersion;

  Draw(TTree *events)
  {
    fGeometryVersion = 0x438b;

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

    b->SetAddress(&hits);

    if((b = events->GetBranch("CaloHit")))
    {
      caloHits = new TClonesArray("R3BCaloHit");
      b->SetAddress(&caloHits);
    }
    else if((b = events->GetBranch("CaloHitSim")))
    {
      caloHits = new TClonesArray("R3BCaloHitSim");
      b->SetAddress(&caloHits);
    }
    else
      caloHits = NULL;

    n = events->GetEntriesFast();
    this->events = events;
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

    for(Long64_t i = 0; i < n; i++)
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

  TH1* getTH1(int bins, int min, int max, const char *name, TH1 *h = NULL)
  {
    if(h)
      return h;

    return new TH1I("h", name, bins, min, max);
  }

  TH2* getTH2(int binsx, int minx, int maxx, int binsy, int miny, int maxy, const char *name, TH2 *h = NULL)
  {
    if(h)
      return h;

    return new TH2I("h", name, binsx, minx, maxx, binsy, miny, maxy);
  }
};
