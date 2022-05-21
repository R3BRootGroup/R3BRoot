#include <boost/format.hpp>

#include <TH2I.h>
#include <TFolder.h>
#include <TVector3.h>
#include <R3BCalifaHitData.h>


#include <math.h>
#define CONV_DEG (180/M_PI)


enum sides_t
  {
    MES=0,
    WIX=1,
  };


#define SIDES 2

const std::string SIDE_NAMES[]={ "Mes", "Wix"};


class ProtonCutHists
{
public:
  TH1I* en[SIDES]{};
  TH2I* theta_phi[SIDES]{};
  TH2I* e_vs_e{};
  TH2I* theta_theta{};
  TH2I* phi_phi{};
  TH1I* op_ang{};

  ProtonCutHists() {}  // placeholder object
  
  ProtonCutHists(std::string name, TFolder& parent)
  {
    double thetaLowers[]={0.39, 0.45, 0.5,  0.55, 0.59, 0.63, 0.67, 0.72, 0.76,
                                0.8,  0.84, 0.87, 0.91, 0.95, 1.0,  1.04, 1.08, 1.12,
                                1.16, 1.21, 1.26, 1.30, 1.35, 1.4,  1.45, 1.5};

    static const int thetaBins=sizeof(thetaLowers)/sizeof(double)-1;
    for(int i=0; i<thetaBins+1; i++)
      thetaLowers[i]*=CONV_DEG;
    
    double phiLowers[65]{};
    static const int phiBins=64; // last upper edge does not count. 
    for (int i=0; i<phiBins+1; i++)
      {
        phiLowers[i]=i*M_PI/32-M_PI-M_PI/128;
        phiLowers[i]*=CONV_DEG;
      }

    auto me=new TFolder(name.c_str(), name.c_str());

    
    decltype(boost::format("foo")) fname;
    auto sname=[&fname](){ return strdup(fname.str().c_str()); /*sue me*/ };


    auto add=[me](TH1* h)
      { 
        if (dynamic_cast<TH2*>(h))
          {
            // h->SetDrawOption("colz"); //<-- does not work.
            auto s=(boost::format("%s_cv")%h->GetName()).str();
            auto cv=new TCanvas(strdup(s.c_str()));
            h->Draw("colz");
            me->Add(cv);
          }
        else
          me->Add(h);
      };
    
    for (int i=0; i<SIDES; i++)
      {
        fname=boost::format("%s/en_%s")%name%SIDE_NAMES[i];
        add(en[i]=new TH1I(sname(), sname(), 5000, 0, 300e3));
        
        fname=boost::format("%s/theta_phi_%s")%name%SIDE_NAMES[i];
        add(theta_phi[i]=new TH2I(sname(), sname(),
                                      thetaBins, thetaLowers,
                                      phiBins, phiLowers));
      }

    fname=boost::format("%s/e_vs_e")%name;
    add(e_vs_e=new TH2I(sname(), sname(), 300, 0, 300e3, 300, 0, 300e3));

    
    fname=boost::format("%s/theta_theta")%name;
    add(theta_theta=new TH2I(sname(), sname(),
                                 thetaBins, thetaLowers,
                                 thetaBins, thetaLowers));
    
    fname=boost::format("%s/phi_phi")%name;
    add(phi_phi=new TH2I(sname(), sname(),
                             phiBins, phiLowers,
                             phiBins, phiLowers));
    
    fname=boost::format("%s/op_ang")%name;
    add(op_ang=new TH1I(sname(), sname(),
                            150., 0., 150.));

    parent.Add(me);
  }
  
  void Fill(R3BCalifaHitData* max[SIDES]) // takes
  {
    TVector3 vec[SIDES];
    for (int i=0; i<SIDES; i++)
      {
        auto h=max[i];
        if (!h)
          continue;
        en[i]->Fill(h->GetEnergy());
        theta_phi[i]->Fill( h->GetTheta() * CONV_DEG, h->GetPhi()* CONV_DEG);
        vec[i].SetMagThetaPhi(1, h->GetTheta(), h->GetPhi());
      }

    if (max[MES] && max[WIX])
      {
        e_vs_e->Fill(max[MES]->GetEnergy(), max[WIX]->GetEnergy());
        theta_theta->Fill(max[MES]->GetTheta()*CONV_DEG, max[WIX]->GetTheta()*CONV_DEG);
        phi_phi->Fill(max[MES]->GetPhi()*CONV_DEG, max[WIX]->GetPhi()*CONV_DEG);
        op_ang->Fill(vec[MES].Angle(vec[WIX])*CONV_DEG);
      }
  }
};

