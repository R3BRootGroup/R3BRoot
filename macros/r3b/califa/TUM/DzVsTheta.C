#include <iostream>

#include <TH2I.h>
#include <TVector3.h>
#include <TMath.h>

#include <R3BCalifaGeometry.h>

void DzVsTheta()
{
  double phi, theta, dz;
  TVector3 vStart(0,0,0), vDir;

  R3BCalifaGeometry *geo = R3BCalifaGeometry::Instance(17);

  TH2I *dztheta = new TH2I("dztheta", "dz vs Theta", 360, 0, 180, 5000, 0, 25);

  for(int t = 0; t < 360; t++)
  {
      theta = (double)t * 180./360.;
  
      for(int p = 0; p < 720; p++)
      {
        phi = -180. + ((double)p * 360./720.);

        vDir.SetMagThetaPhi(1., theta*TMath::Pi()/180., phi*TMath::Pi()/180.);
        dz = geo->GetDistanceThroughCrystals(vStart, vDir);

        dztheta->Fill(theta, dz);
      }
  }

  dztheta->Draw("colz");
}
