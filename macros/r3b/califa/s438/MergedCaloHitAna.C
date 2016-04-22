#define CaloHitAna_cxx
#include "MergedCaloHitAna.h"
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TCutG.h>
#include <TF1.h>
#include <TRandom3.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TVector2.h>

#include <iostream>
#include <fstream>

#define USE_PROTON_CALIBRATION 1

#if USE_PROTON_CALIBRATION
#include "readcalib.cpp"
#endif

using namespace std;

void CaloHitAna::DrawPID(bool cut)
{
   if (fChain == 0) return;

   TH2D *hpid = new TH2D("qpid", "QPID", 2500, 0, 1, 2500, 0, 1);
   TCutG *cut_proton = NULL;

   if(cut)
   {
      TFile *f = TFile::Open("qpid_cuts.root", "read");
      cut_proton = dynamic_cast<TCutG*>(f->Get("qpid_proton"));
   }

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   int i;
   for (Long64_t jentry=0; jentry<nentries;jentry++)
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      for(i = 0; i < CaloHit_; i++)
      {
         if(!cut_proton || cut_proton->IsInside(CaloHit_fNf[i]*1000., CaloHit_fNs[i]*1000.))
            hpid->Fill(CaloHit_fNf[i], CaloHit_fNs[i]);
      }

   }

   hpid->Draw("colz");
}

// Ca-48, 550 AMeV
#define E_0 45029.0498 
#define E_Beam (E_0 + 48*550)
#define BETA (sqrt(1-(E_0*E_0)/(E_Beam*E_Beam)))
#define GAMMA (1/(sqrt(1-BETA*BETA)))

inline Double_t Doppler(Double_t E_det, Double_t theta)
{
   return E_det * GAMMA * (1 - BETA*TMath::Cos(theta));
}

#define QUENCHING 1.00

#define USE_PSP 0
#define USE_PID 1
#define USE_IPHOS 1
#define USE_FPROTON 1
#define FPROTON_DELTA (1.25 * (1.4 + 2.2*CaloHit_fEnergy[i]))
#define FIPHOS_DELTA (3.*FPROTON_DELTA)

#define NS_OFFSET_STOPPED 0.
#define NS_OFFSET_PUNCHTHROUGH 0.

#define THETA_SMEARING 2.6
#define PHI_SMEARING 5.6

#define MASS_PROTON 938.2720813
// Ca-48, 550 AMeV
#define MASS_BEAM 45029.0498
#define T_BEAM (550.*48.)
// K-47
#define MASS_RESIDUAL (44099)

#define BETA_BEAM TMath::Sqrt(1-MASS_BEAM*MASS_BEAM/((MASS_BEAM+T_BEAM)*(MASS_BEAM+T_BEAM)))
#define GAMMA_BEAM (1./TMath::Sqrt(1-BETA_BEAM*BETA_BEAM))

#define USE_PSP3 1

#define CENTER_U4 (5.53740e+00/25.)
#define CENTER_U5 (3.60020e+00/25.)

//void CaloHitAna::TestPSPX()
//{
//  TH2I *h54 = new TH2I("h54", "5:4 phi vs rho", 100,0,2,100,0,2*TMath::Pi());
//  TH2I *h21 = new TH2I("h21", "2:1 phi vs rho", 100,0,2,100,0,2*TMath::Pi());
//
//  TH2I *hrhorho = new TH2I("hrhorho", "rho 5:4 vs rho 2:1", 100, 0, 2, 100, 0, 2);
//  TH2I *hphiphi = new TH2I("hphiphi", "phi 5:4 vs phi 2:1", 100, 0,2*TMath::Pi(),100,0,2*TMath::Pi());
//
//  TH1I *hdiffphi = new TH1I("hdiffphi", "Phi 5:4 - Phi 2:1", 1000, -2*TMath::Pi(), 2*TMath::Pi());
//
//  TH2I *hxx = new TH2I("hxx", "5:4 X vs 1:2 X", 100, -1, 1, 100, -1, 1);
//  TH2I *hyy = new TH2I("hyy", "5:4 Y vs 1:2 Y", 100, -1, 1, 100, -1, 1);
//
//  TH2I *hxy = new TH2I("hxy", "5:4 X vs 1:2 Y", 100, -1, 1, 100, -1, 1);
//  TH2I *hyx = new TH2I("hyx", "5:4 Y vs 1:2 X", 100, -1, 1, 100, -1, 1);
//
//  TH2I *hxy3 = new TH2I("hxy3", "PSPX 3 x vs y", 1000, -1, 1, 1000, -1, 1);
//
//  double u1, u2, u4, u5;
//  double rho54, rho21, phi54, phi21;
//
//  TVector2 v21, v54, vdiff;
//
//  Long64_t n = fChain->GetEntriesFast();
//  for(Long64_t i = 0; i < n; i++)
//  {
//      Long64_t ientry = LoadTree(i);
//      if (ientry < 0) break;
//      fChain->GetEntry(i);
//
//      u1 = Pspx01u - CENTER_1;
//      u2 = Pspx02u - CENTER_2;
//      u4 = Pspx04u - CENTER_4;
//      u5 = Pspx05u - CENTER_5;
//
//      v21.Set(u1, u2);
////      v21.Set(u2, u1);
//      v54.Set(u4, u5);
//
////      v21 = v21.Rotate(-.85);
//
//      rho21 = v21.Mod();
//      phi21 = v21.Phi();
//      rho54 = v54.Mod();
//      phi54 = v54.Phi();
//
//      h54->Fill(rho54, phi54);
//      h21->Fill(rho21, phi21);
//
//      hrhorho->Fill(rho21, rho54);
//      hphiphi->Fill(phi21, phi54);
//
//      vdiff = v54 - v21;
//      hdiffphi->Fill(vdiff.Phi());
//
//      hxx->Fill(v21.X(), v54.X());
//      hyy->Fill(v21.Y(), v54.Y());
//      hxy->Fill(v21.Y(), v54.X());
//      hyx->Fill(v21.X(), v54.Y());
//
//      double x03 = 0, y03 = 0, wx03 = 0, wy03 = 0, p, wp;
////      cout << Pspx03n << endl;
//      for(Int_t j = 0; j < Pspx03n; j++)
//      {
////        cout << "  " << Pspx03ni[j] << ", " << Pspx03e01[j] << endl;
//        if(Pspx03ni[j] < 32)
//        {
//          // Horizontal strips -> y position
//          y03 += (double)Pspx03e01[j] * ((double)(Pspx03ni[j] / 2) - 7.5)/8.0;
//          wy03 += (double)Pspx03e01[j];
//        }
//        else if(Pspx03ni[j] < 64)
//        {
//          x03 += (double)Pspx03e01[j] * ((double)((Pspx03ni[j] - 32) / 2) - 7.5)/8.0;
//          wx03 += (double)Pspx03e01[j];
//        }
//      }
//
//      if(wx03 != 0)
//        x03 /= wx03;
//      if(wy03 != 0)
//        y03 /= wy03;
//
//      hxy3->Fill(x03, y03);
//  }
//}

void CaloHitAna::Loop(const char *fout, const char *mode)
{
   if (fChain == 0) return;

   TFile *fcut = TFile::Open("qpid_cuts.root", "read");
   if(!fcut)
   {
      cerr << "Could not open qpid_cuts.root for reading!\n";
      return;
   }
#if USE_PID
#if !USE_FPROTON
   TCutG *cut_proton = dynamic_cast<TCutG*>(fcut->Get("qpid_proton"));
   if(!cut_proton)
   {
      cerr << "Could not find TCutG qpid_proton!\n";
      return;
   }
   TCutG *cut_iphos = dynamic_cast<TCutG*>(fcut->Get("qpid_iphos"));
   if(!cut_iphos)
   {
      cerr << "Could not find TCutG cut_iphos!\n";
      return;
   }
#else
   TF1 *fproton = dynamic_cast<TF1*>(fcut->Get("fproton"));
   if(!fproton)
   {
     cerr << "Coult not find TF1 fproton!\n";
     return;
   }
   fproton->SetParameter(1, .975 * fproton->GetParameter(1));

   double lmax[3] = {316.7,282.9,274.1};
   double nfmax[3];

   TF1 fmax("fmax", "TMath::Power(fproton+x-[0],2)",0,500);
   for(int i = 0; i < 3; i++)
   {
     fmax.SetParameter(0,lmax[i]);
     nfmax[i] = fmax.GetMinimumX();
     cout << "Nf_max[" << i << "] = " << nfmax[i] << endl;
   }

   TF1 **fiphos = new TF1*[3];
   for(int i = 0; i < 3; i++)
   {
//     fiphos[i] = new TF1(Form("fiphos_%d", i), "[0]*exp(-[2]*[3])*(1-exp([2]*x))+[1]*x",0,nfmax[i]);
     fiphos[i] = new TF1(Form("fiphos_%d", i), "([0]*exp(-[2]*[3])*(1-exp([2]*x))+[1]*x)*(x/[3])+([4]*(exp(-[6]*x)-1)+[5]*x)*(1-x/[3])",0,nfmax[i]);
     fiphos[i]->SetParameters(fproton->GetParameter(0),
         fproton->GetParameter(1),
         fproton->GetParameter(2),
         nfmax[i],
         3.2, 1.89, .33); // Bending of gamma line due to trigger walk
   }

   double fproton_Ns, fiphos_Ns, dist_fproton, dist_fiphos;
#endif
#endif

  TCutG *cut_evstheta = dynamic_cast<TCutG*>(fcut->Get("cut_evstheta"));
  if(!cut_evstheta)
  {
    cerr << "Could not find TCutG cut_evstheta" << endl;
    return;
  }

#if USE_PSP
   TCutG *cut_pspx = dynamic_cast<TCutG*>(fcut->Get("pspx_zminus1"));
   if(!cut_pspx)
   {
      cerr << "Could not find TCutG pspx_zminus1\n";
      return;
   }
#endif
   fcut->Close();
   delete fcut;

#if USE_PROTON_CALIBRATION
   // Read proton calibration
   readAllLO();
#endif

   TH2D *hee = new TH2D("evse", "E1 vs E2", 1000, 0, 1000, 1000, 0, 1000);
   TH2D *htt = new TH2D("thetavstheta", "Theta 1 vs Theta 2", 1800, 0, 180, 1800, 0, 180);
   TH2D *hpp = new TH2D("phivsphi", "Phi 1 vs Phi 2", 1800, -180, 180, 1800, -180, 180);
   TH2D *het = new TH2D("evstheta", "E vs Theta", 1800, 0, 180, 1000, 0, 1000);
   TH2D *het2 = new TH2D("evstheta2", "E vs Theta (without cut)", 1800, 0, 180, 1000, 0, 1000);

   TH1D *hsumtheta = new TH1D("hSumTheta", "Sum Theta", 1800, 0, 180);
   TH1D *hdiffphi = new TH1D("hDiffPhi", "Delta Phi", 1800, 90, 270);
   TH1D *hegamma = new TH1D("hEgamma", "Gamma Energy", 1000, 0, 10);
   TH1D *hesum = new TH1D("hESum", "Total Proton Energy", 2000, 0, 2000);

   TH2I *hqpid = new TH2I("hqpid", "QPID", 2500,0,20,2500,0,20);
   TH2I *hqpid_p = new TH2I("hqpid_p", "QPID - Proton", 500, 0, 250, 500, 0, 250);

#define QPID_ROT_ANGLE (TMath::Pi()*(-63.2)/180.)
    TH2I *hqpid_rot = new TH2I("hqpid_rot", "QPID Rotated", 2500, 0, 250, 2000, -20, 20);

   TH1I *hpx = new TH1I("hpx", "Proton Momentum X", 20000, -1000, 1000);
   TH1I *hpy = new TH1I("hpy", "Proton Momentum Y", 20000, -1000, 1000);
   TH1I *hpz = new TH1I("hpz", "Proton Momentum Z", 20000, -1000, 1000);

   TH1I *hpmag = new TH1I("hpmag", "Proton Momentum Magnitude", 10000, 0, 1000);

   TH1I *hM = new TH1I("hM", "Invariant Mass", 100000, 0, 100);
   TH1I *hExc = new TH1I("hExc", "Excitation Energy", 10000, -200, 200);

   TH2I *hpthetavspspxrho = new TH2I("hpthetavspspxrho", "PSPX #Delta#rho vs p_{#Theta}",
       1000, 0, .01, 1000, 0, 2);
   TH2I *hpphivspspxphi = new TH2I("hpphivspspxphi", "PSPX #Delta#Phi vs p_{#Phi}",
       1000, 0, 2*TMath::Pi(), 1000, 0, 2*TMath::Pi());

   TH2I *htrackthetaptheta = new TH2I("htrackthetaptheta", "PSPX #Theta vs p_{#Theta}",
       1000, 0, 10, 1000, 0, 10);
   TH2I *htrackphipphi = new TH2I("htrackphipphi", "PSPX #Phi vs p_{#Phi}",
       1000, -TMath::Pi(), TMath::Pi(), 1000, -TMath::Pi(), TMath::Pi());


   TH2I *hpypx = new TH2I("hpypx", "p_{y} vs p_{x}", 2000, -500, 500, 2000, -500, 500);
   TH2I *hpzpx = new TH2I("hpzpx", "p_{z} vs p_{x}", 2000, -500, 500, 2000, -500, 500);
   TH2I *hpzpy = new TH2I("hpzpy", "p_{z} vs p_{y}", 2000, -500, 500, 2000, -500, 500);

   TH2I *hxy3 = new TH2I("hxy3", "Y:X PSPX 3", 160, -25, 25, 160, -25, 25);
   TH2I *hxypol = new TH2I("hxypol", "Y:X at PSPX 4,5 extrapolated", 1000, -25, 25, 1000, -25, 25);
   TH2I *hxy45 = new TH2I("hxy45", "Y:X PSPX 4,5", 1000, -25, 25, 1000, -25, 25);

   TH1I *hdiffphitrackp = new TH1I("hdiffphitrackp", "#Delta #varphi Q PSPX", 10000, -360, 360);

   TH1 *writeHists[] = {hee, htt, hpp, het, hsumtheta, hdiffphi, hegamma, hesum, htrackthetaptheta, htrackphipphi, NULL};

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   int i;
   int idxEmax1, idxEmax2;
   double Emax1, Emax2;
   Double_t sumTheta, eGamma;

   bool is_proton, tmp_iphos;
   bool is_iphos[2];
   double e_proton;
   double theta;
   int region;

    TRandom3 rnd;
    Long64_t jrnd = 0;
    Long64_t jlast = -1;

   // fitted iPhos parameters for each anglular region / crystal type
   // nominal target position
//    double iphos_p[3][3] =
//    {
//      // a      b       c
//      {3.44151e+04,9.50616e+01,7.72456e-01},  // region 0
//      {2.75582e+04,7.93523e+01,7.39312e-01},  // region 1
//      {2.80111e+04,7.26665e+01,6.83356e-01}   // region 2
//    };

//    // target at z = 2.5cm
//    double iphos_p[3][3] =
//    {
//      // a      b       c
//      {2.66613e+04,1.07166e+02,1.05167e+00},  // region 0
//      {2.51860e+04,7.79525e+01,9.53142e-01},  // region 1
//      {2.78063e+04,6.67780e+01,5.48487e-01}   // region 2
//    };

    // Fit constrained to pure punch through (no nuclear reactions)
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

//   double iphos_p[3][3] =
//    {
//      // a      b       c
//      {3.624e4,9.486e1,7.108e-1},  // region 0
//      {2.965e4,7.732e1,6.739e-1},  // region 1
//      {2.808e4,7.352e1,6.655e-1}   // region 2
//    };

      double iphos_p[3][6] = 
      {
        {3.84671e+04,9.27921e+01,6.29219e-01,0,0,0},
        {3.31084e+04,7.21796e+01,5.29870e-01,0,0,0},
        {2.90035e+04,7.15755e+01,6.16260e-01,0,0,0}
      };


    int nProtons;
    double r_nsnf;

    // Temporary vector
    TVector3 v;
    
    // 4-vector of target proton in projectile rest frame
    TLorentzVector p(0, 0, 0, MASS_PROTON);
    p.Boost(0, 0, -BETA_BEAM);
    cout << "Beta: " << BETA_BEAM << endl;
    cout << "Target proton: E = " << p.Energy() << ", pz = " << p.Pz() << endl;
    // 4-vector of projectile in its rest frame
    TLorentzVector P(0, 0, 0, MASS_BEAM);
    // 4-vectors of outgoing (detected) protons
    TLorentzVector q[2];

    TVector2 v12, v54, vdiff;
    TVector3 vdiff3, vbeamin, vbeamout;
//    double u1, u2, u4, u5;
    double x1, x4, y2, y5, x3, y3, wx3, wy3; 
    double x_target, y_target;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      if(CaloHit_ < 2)
	continue;

#if USE_PSP
      // Only take Z-1 events
      if(!cut_pspx->IsInside(Pspx04e, Pspx05e))
         continue;
#endif

//      cout << ".";

      idxEmax1 = -1;
      idxEmax2 = -1;

      Emax1 = 0;
      Emax2 = 0;

      nProtons = 0;

      for(i = 0; i < CaloHit_; i++)
      {
//          if(CaloHit_fNf[i] != 0)
//          {
//            r_nsnf = CaloHit_fNs[i]/CaloHit_fNf[i];
//            CaloHit_fNs[i] = CaloHit_fEnergy[i]/(1./r_nsnf + 1.);
//            CaloHit_fNf[i] = CaloHit_fEnergy[i]/(r_nsnf + 1.);
//          }

           hqpid->Fill(1000.*CaloHit_fNf[i], 1000.*CaloHit_fNs[i]);

           hqpid_rot->Fill(1000.*(TMath::Cos(QPID_ROT_ANGLE)*CaloHit_fNf[i] - TMath::Sin(QPID_ROT_ANGLE)*CaloHit_fNs[i]),
               1000.*(TMath::Sin(QPID_ROT_ANGLE)*CaloHit_fNf[i] + TMath::Cos(QPID_ROT_ANGLE)*CaloHit_fNs[i]));

           theta = CaloHit_fTheta[i] * 180. / TMath::Pi();

           if(theta < 40)
             region = 0;
           else if(theta < 55)
             region = 1;
           else
             region = 2;

#if USE_PID
#if !USE_FPROTON        
         // Is stopped proton?
         if(cut_proton->IsInside(1000. * CaloHit_fNf[i], 1000. * CaloHit_fNs[i]))
#else
         fproton_Ns = fproton->Eval(1000. * CaloHit_fNf[i]);
         fiphos_Ns = fiphos[region]->Eval(1000. * CaloHit_fNf[i]);
         dist_fproton = TMath::Abs(fproton_Ns - 1000.*CaloHit_fNs[i]);
         dist_fiphos = TMath::Abs(fiphos_Ns - 1000.*CaloHit_fNs[i]);

//         if(dist_fproton <= FPROTON_DELTA && CaloHit_fEnergy[i]*1000. >= 10.)
         if((dist_fproton < dist_fiphos || 1000.*CaloHit_fNf[i] > nfmax[region])
             && dist_fproton  <= FPROTON_DELTA
             && CaloHit_fEnergy[i]*1000.*QUENCHING >= 10.)
#endif
         {
//           cout << "s";
//          if(CaloHit_fEnergy[i] >= 0.05)
//          {
           is_proton = true;
           tmp_iphos = false;
#if USE_PROTON_CALIBRATION
           e_proton = GetE(region, CaloHit_fEnergy[i]*1000., true);
#else
           e_proton = CaloHit_fEnergy[i]*1000.*QUENCHING;
#endif
//          }
         }
      
#if USE_IPHOS         
         // iPhos (punch through proton) ?
#if !USE_FPROTON
         else if(cut_iphos->IsInside(1000. * CaloHit_fNf[i], 1000. * CaloHit_fNs[i]))
#else
//         else if((dist_fiphos < dist_fproton && 1000.*CaloHit_fNf[i] <= nfmax[region]
//             && dist_fiphos  < FIPHOS_DELTA)
//             && CaloHit_fEnergy[i]*1000.*QUENCHING >= 100.)
         else if(((/*dist_fiphos < dist_fproton && */1000.*CaloHit_fNs[i] > fproton_Ns && 1000.*CaloHit_fNf[i] <= nfmax[region]
             && dist_fiphos  < FIPHOS_DELTA)
               || (1000.*CaloHit_fNf[i] > nfmax[2] && 1000.*CaloHit_fNs[i] > fproton_Ns))
             && CaloHit_fEnergy[i]*1000.*QUENCHING >= 100.)
//         else if(1000. * CaloHit_fNs[i] > fproton_Ns && CaloHit_fEnergy[i] * 1000. >= 100.)
#endif
         {
//           cout << "i";
           // Calculate energy by iPhos method
           is_proton = true;
           tmp_iphos = true;

#if USE_PROTON_CALIBRATION
           e_proton = GetE(region, CaloHit_fEnergy[i]*1000., false);
#else
           e_proton = CaloHit_fEnergy[i]*1000.*QUENCHING;
#endif
           e_proton = (iphos_p[region][0] / (e_proton - iphos_p[region][1])
               + iphos_p[region][2]*(e_proton - iphos_p[region][1]));
           e_proton -= (iphos_p[region][3] + iphos_p[region][4]*e_proton + iphos_p[region][5]*e_proton*e_proton);
         }
#endif

#else
         if(CaloHit_fEnergy[i]*1000. > 50.)
         {
           is_proton = true;
           e_proton = 1000. * CaloHit_fEnergy[i] * QUENCHING;
           tmp_iphos = false;
         }

#endif
         else
           is_proton = false;

        

         if(is_proton)
         {
           nProtons++;
           hqpid_p->Fill(1000.*CaloHit_fNf[i], 1000.*CaloHit_fNs[i]);
         }

         if(is_proton && CaloHit_fPhi[i] < 0 /*TMath::Pi()*/ && (idxEmax1 == -1 || e_proton > Emax1))
         {
            // Petal 1
            idxEmax1 = i;
            Emax1 = e_proton;
            is_iphos[0] = tmp_iphos;
//            cout << "1";
         }
         else if(is_proton && CaloHit_fPhi[i] >= 0 /*TMath::Pi()*/ && (idxEmax2 == -1 || e_proton > Emax2))
         {
            // Petal 2
            idxEmax2 = i;
            Emax2 = e_proton;
            is_iphos[1] = tmp_iphos;
//            cout << "2";
         }
//         if(idxEmax1 == -1 || CaloHit_fEnergy[i] > CaloHit_fEnergy[idxEmax1])
//         {
//            idxEmax2 = idxEmax1;
//            idxEmax1 = i;
//         }
//         else if(idxEmax2 == -1 || CaloHit_fEnergy[i] > CaloHit_fEnergy[idxEmax2])
//            idxEmax2 = i;
      }

//      cout << endl;

#if USE_PSP
      // PSP was used to identify Z-1 residual nucleus to tag p,2p reaction
      if(idxEmax1 != -1 && idxEmax2 != -1 /*&& nProtons == 2*/)
#else
      // Identify exactly 2 protons to tag p,2p reaction
      if(idxEmax1 != -1 && idxEmax2 != -1 /*&& nProtons == 2*/)
#endif
      {
        // Discard events with two punch through (misidentification)
//         if(is_iphos[0] && is_iphos[1])
//           continue;

        // Randomly swap protons
        if(rnd.Uniform() >= .5)
        {
          double e_tmp = Emax1;
          int i_tmp = idxEmax1;
          Emax1 = Emax2;
          Emax2 = e_tmp;
          idxEmax1 = idxEmax2;
          idxEmax2 = i_tmp;
        }

        // Get 4-vectors of protons
        v.SetMagThetaPhi(TMath::Sqrt((Emax1+MASS_PROTON)*(Emax1+MASS_PROTON)-MASS_PROTON*MASS_PROTON), CaloHit_fTheta[idxEmax1], CaloHit_fPhi[idxEmax1]);
        q[0] = TLorentzVector(v, Emax1+MASS_PROTON);
        v.SetMagThetaPhi(TMath::Sqrt((Emax2+MASS_PROTON)*(Emax2+MASS_PROTON)-MASS_PROTON*MASS_PROTON), CaloHit_fTheta[idxEmax2], CaloHit_fPhi[idxEmax2]);
        q[1] = TLorentzVector(v, Emax2+MASS_PROTON);

        // Boost to projectile frame
        q[0].Boost(0, 0, -BETA_BEAM);
        q[1].Boost(0, 0, -BETA_BEAM);

        // Get remaining momentum + invariant mass
        TLorentzVector Q = p + P - q[0] - q[1];

        CaloHit_fTheta[idxEmax1] += rnd.Uniform(-THETA_SMEARING/2.,THETA_SMEARING/2.)/180.*TMath::Pi();
        CaloHit_fTheta[idxEmax2] += rnd.Uniform(-THETA_SMEARING/2.,THETA_SMEARING/2.)/180.*TMath::Pi();
        CaloHit_fPhi[idxEmax1] += rnd.Uniform(-PHI_SMEARING/2.,PHI_SMEARING/2.)/180.*TMath::Pi();
        CaloHit_fPhi[idxEmax2] += rnd.Uniform(-PHI_SMEARING/2.,PHI_SMEARING/2.)/180.*TMath::Pi();
         
         het2->Fill(CaloHit_fTheta[idxEmax1] * 180. / TMath::Pi(), Emax1);
         het2->Fill(CaloHit_fTheta[idxEmax2] * 180. / TMath::Pi(), Emax2);

        if(cut_evstheta->IsInside(CaloHit_fTheta[idxEmax1]*180./TMath::Pi(), Emax1)
             && cut_evstheta->IsInside(CaloHit_fTheta[idxEmax2]*180./TMath::Pi(), Emax2))
        {

         sumTheta = (CaloHit_fTheta[idxEmax1] + CaloHit_fTheta[idxEmax2]) * 180.0 / TMath::Pi();
	 htt->Fill(CaloHit_fTheta[idxEmax1] * 180.0 / TMath::Pi(), CaloHit_fTheta[idxEmax2] * 180.0 / TMath::Pi());
         hpp->Fill(CaloHit_fPhi[idxEmax1] * 180.0 / TMath::Pi(), CaloHit_fPhi[idxEmax2] * 180.0 / TMath::Pi());
         hsumtheta->Fill(sumTheta);
         hdiffphi->Fill((CaloHit_fPhi[idxEmax2] - CaloHit_fPhi[idxEmax1]) * 180.0 / TMath::Pi());



         if(sumTheta >= (78.1 - 2.35*2.8) && sumTheta <= (7815 + 2.35*2.8))
         {
	    hee->Fill(Emax1, Emax2);
            hesum->Fill(Emax1 + Emax2);
            het->Fill(CaloHit_fTheta[idxEmax1] * 180. / TMath::Pi(), Emax1);
            het->Fill(CaloHit_fTheta[idxEmax2] * 180. / TMath::Pi(), Emax2);

            hM->Fill(Q.Mag()/1000.);
            hExc->Fill(Q.Mag() - MASS_RESIDUAL);

//            if(Q.Mag() - MASS_RESIDUAL >= 20 && Q.Mag() - MASS_RESIDUAL <= 50)
//            {

             hpx->Fill(-Q.Px());
             hpy->Fill(-Q.Py());
             hpz->Fill(-Q.Pz());
             hpmag->Fill(Q.Vect().Mag());

//            }

            hpypx->Fill(-Q.Px(),-Q.Py());
            hpzpx->Fill(-Q.Px(),-Q.Pz());
            hpzpy->Fill(-Q.Py(),-Q.Pz());

            // Try to find coincident gammas
               for(i = 0; i < CaloHit_; i++)
               {
                  if(i != idxEmax1 && i != idxEmax2 && (eGamma = Doppler(CaloHit_fEnergy[i], CaloHit_fTheta[i])) < 0.02)
                     hegamma->Fill(eGamma * 1000.);
               }


//            x1 = 1.5192 + 25.*1.9725*Pspx01u;
//            y2 = -12.6252 + 25.*2.1316*Pspx02u;
//            x4 = 25.*Pspx04u;
//            y5 = 25.*Pspx05u;

//            // Get uncorrelated event to test PSPX correlation
//            jrnd = TMath::Max(jentry+1, jrnd+1);
//            do
//            {
//              if(jrnd >= nentries)
//                break;
////              jrnd = (Long64_t)(rnd.Rndm() * nentries);
//              LoadTree(jrnd);
//              fChain->GetEntry(jrnd++);
//            }
//            while(Pspx01n == 0 || Pspx02n == 0 || Pspx03n == 0 || Pspx04n == 0 || Pspx05n == 0);
//            cout << jentry << ": " << jrnd << endl;

#if 0
              jrnd = jlast;
              jlast = jentry;
              if(jrnd != -1)
              {
                LoadTree(jrnd);
                fChain->GetEntry(jrnd);

                cout << jentry << ": " << jrnd << endl;
            }
              else
              {
                continue;
              }
#endif

#if USE_PSP3
// Get x/y position of PSPX 3
            x3 = 0; wx3 = 0; y3 = 0; wy3 = 0;
            for(UInt_t j = 0; j < Pspx03n; j++)
            {
              if((Pspx03ni[j] <= 16 && Pspx03e01[j] < 800) || (Pspx03ni[j] > 16 && Pspx03e01[j] < 300))
                continue;
      
              if(Pspx03ni[j] <= 16)
              {
                // y
                wy3 += (double)Pspx03e01[j];
                y3 += (double)Pspx03e01[j]*((double)Pspx03ni[j]-8.5)/8.;
              }
              else if(Pspx03ni[j] <= 32)
              {
                // x
                wx3 += (double)Pspx03e01[j];
                x3 += (double)Pspx03e01[j]*((double)Pspx03ni[j]-16.-8.5)/8.;
         }
      }
      
            if(wx3 > 0 && wy3 > 0)
            {
              x3 /= wx3;
              y3 /= wy3;


#if 0
        double xpol = (.4195*Pspx01u - .1112*Pspx02u + .5033*x3 - .1304*y3)*.503523 + .264788;
        double ypol = (.4070*Pspx01u - .5493*Pspx02u + .2876*x3 - .6361*y3)*.427118 + .207133;
#else
#define Pspx03x x3
#define Pspx03y y3

          double xpol, ypol;
          xpol = 0.247621 + (0.0963818)*Pspx01u + (-0.0251536)*Pspx02u + (0.180171)*Pspx03x + (0.0157061)*Pspx03y;
          ypol = 0.137981 + (0.0352896)*Pspx01u + (-0.00477238)*Pspx02u + (-0.047443)*Pspx03x + (-0.169323)*Pspx03y;
#endif

#else
//        double xpol = (.4195*Pspx01u - .1112*Pspx02u)*.8813 + .2799;
//        double ypol = (.4070*Pspx01u - .5493*Pspx02u)*.8564 + .2711;

        double xpol = CENTER_U4;
        double ypol = CENTER_U5;

#endif
            v12.Set(xpol, ypol);
            // Actual (4,5) position
            v54.Set(Pspx04u, Pspx05u);

            vdiff = v54 - v12;
            vdiff3.SetXYZ(vdiff.X()*25., vdiff.Y()*25., 879.);

            // Back to lab frame
            Q.Boost(0, 0, BETA_BEAM);

//            vdiff3.RotateZ(-134.*TMath::Pi()/180.);

            hpthetavspspxrho->Fill(Q.Theta(), vdiff.Mod());
            hpphivspspxphi->Fill(TMath::Pi() + Q.Phi(), vdiff.Phi());

            htrackthetaptheta->Fill(Q.Theta()*1000., vdiff3.Theta()*1000.);
            htrackphipphi->Fill(Q.Phi(), vdiff3.Phi());

            hdiffphitrackp->Fill(180.*(Q.Phi()-vdiff3.Phi())/TMath::Pi());
#if USE_PSP3
            }
#endif


////            u1 = Pspx01u - CENTER_1;
////            u2 = Pspx02u - CENTER_2;
////            u4 = Pspx04u - CENTER_4;
////            u5 = Pspx05u - CENTER_5;
//
//            // Extrapolated (4,5) position by (1,2) position
//            v12.Set(.2684+.4414*Pspx01u, .2501-.5514*Pspx02u);
//            // Actual (4,5) position
//            v54.Set(Pspx04u, Pspx05u);
//
//            vdiff = v54 - v12;
//            vdiff3.SetXYZ(vdiff.X()*25., vdiff.Y()*25., 879.);
//
//            // Back to lab frame
//            Q.Boost(0, 0, BETA_BEAM);
//
//            hpthetavspspxrho->Fill(Q.Theta(), vdiff.Mod());
//            hpphivspspxphi->Fill(TMath::Pi() + Q.Phi(), vdiff.Phi());
//
//            htrackthetaptheta->Fill(Q.Theta(), vdiff3.Theta());
//            htrackphipphi->Fill(Q.Phi(), vdiff3.Phi());
//
////            hpthetavspspxrho->Fill(Q.Theta(), v54.Mod() - v12.Mod());
////            hpphivspspxphi->Fill(Q.Phi(), v54.Phi() - v12.Phi());


         }
        }
      }

   }

  TCanvas *c1 = new TCanvas("c1", "c1");
  c1->cd();
  hqpid->Draw("colz");
  fproton->Draw("same");
  fproton->SetLineColor(1);
  fproton->SetLineStyle(2);
  for(int i = 0; i < 3; i++)
  {
    fiphos[i]->SetLineColor(1);
    fiphos[i]->Draw("same");
  }

  TCanvas *cqpidrot = new TCanvas("cqpidrot","QPID rot");
  cqpidrot->cd();
  hqpid_rot->Draw("colz");

  TCanvas *c2 = new TCanvas("c2", "c2");
  c2->cd();
  hqpid_p->Draw("colz");

  TCanvas *c3 = new TCanvas("c3", "c3");
  c3->cd();
  hesum->Draw();

  SaveHists(fout, mode, writeHists);
}

void CaloHitAna::SaveHists(const char *fout, const char *mode, TH1 **hists)
{
  if(fout != NULL)
  {
    const char *pwd = gDirectory->GetPath();
    TFile *ffout = new TFile(fout, mode);
    if(!ffout)
    {
      cerr << "Could not create output file " << fout << endl;
      return;
    }

    ffout->cd();
    for(TH1 *h = *hists; h != NULL; h = *(++hists))
      h->Write();
    ffout->Close();

    gDirectory->cd(pwd);
   }
}
