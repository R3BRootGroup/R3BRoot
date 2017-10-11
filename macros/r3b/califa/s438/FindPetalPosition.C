#include <iostream>

#include <TH2I.h>
#include <TCanvas.h>
#include <TGeoNode.h>
#include <TGeoMatrix.h>
#include <TGeoManager.h>
#include <TMinuit.h>
#include <TMath.h>
#include <TVector3.h>
#include <TGeoArb8.h>

#include "../../../../califa/R3BCalifaGeometry.h"

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

int fbxCrystalPhiOrder[16] =
{
  14, 8, 4, 6,
  13, 15, 3, 5,
  10, 12, 0, 2,
  9, 11, 7, 1
};

//int ptlAlveoliCopyBase[2] = {12, 28};
int ptlAlveoliCopyBase[2] = {0, 2};

int febexChannelToCrystalId(int _channel)
{
  return fbxPetalCrystalIdBase[(_channel) % 64] + 4 * ptlAlveoliCopyBase[(_channel/64)];
}

TGeoNodeMatrix *nodes[128];
TGeoMatrix *matrixOrg[128];
TGeoCombiTrans matrixPetal;
TGeoRotation rotPetal;

double theta_exp[128], theta_sim, dtmp;

R3BCalifaGeometry *geo_sim;

using namespace std;

void PlotPhiTheta()
{
  TH2I *hsimphi = new TH2I("hsimphi", "Phi Sim Model", 128, 0, 128, 360, -180, 180);
  TH2I *hexpphi = new TH2I("hexpphi", "Phi Experiment", 128, 0, 128, 360, 0, 360);
  TH2I *hsimtheta = new TH2I("hsimtheta", "Theta Sim Model", 128, 0, 128, 180, 0, 180);
  TH2I *hexptheta = new TH2I("hexptheta", "Theta Experiment", 128, 0, 128, 180, 0, 180);
  R3BCalifaGeometry *geo = R3BCalifaGeometry::Instance(17);
  R3BCalifaGeometry *geoe = R3BCalifaGeometry::Instance(0x438b);

  double phi, theta, dtmp;
  int crystalId, fbxChannel, order, row, petal;

  for(int c = 0; c < 4; c++)
  {
    for(int fbx = 7; fbx >= 0; fbx--)
    {
      for(int r = 0; r < 4; r++)
      {
        fbxChannel = fbxCrystalPhiOrder[4*c + r] + 16*fbx;
        crystalId = febexChannelToCrystalId(fbxChannel);

        petal = fbx/4;
        row = r + (3-fbx%4)*4;
        order = petal*64 + c*16 + row;
        cout << "[" << petal << "] (" << c << ", " << row << ") " << order << " " << fbxChannel << " " << crystalId << endl;

        geo->GetAngles(crystalId, &theta, &phi, &dtmp);
        hsimphi->Fill(order, 180.0 * phi / TMath::Pi());
        hsimtheta->Fill(order, 180.0 * theta / TMath::Pi());

        geoe->GetAngles(fbxChannel, &theta, &phi, &dtmp);
        hexpphi->Fill(order, 180.0 * phi / TMath::Pi());
        hexptheta->Fill(order, 180.0 * theta / TMath::Pi());
      }
    }
  }

  TCanvas *c1 = new TCanvas("c1", "c1");
  c1->Divide(2,2);
  c1->cd(1);
  hsimphi->Draw("colz");
  c1->cd(2);
  hsimtheta->Draw("colz");
  c1->cd(3);
  hexpphi->Draw("colz");
  c1->cd(4);
  hexptheta->Draw("colz");
}

// Calculate angle for center, front and back face of crystal and compare to given nominal angle
double GetCrystalChiSqu(int iD, double angleNominal)
{
  const char *path = geo_sim->GetCrystalVolumePath(iD);
  gGeoManager->cd(path);
  TGeoArb8 *vol = dynamic_cast<TGeoArb8*>(gGeoManager->GetCurrentVolume()->GetShape());

  if(!vol)
  {
    cerr << "Could not get crystal volume " << iD << " (" << path << ")" << endl;
    return 0;
  }

  double dz = vol->GetDz();
  double *vertices = vol->GetVertices();

  double chi = 0, x, y;
  double local[3], master[3];

  for(int f = 0; f < 3; f++)
  {
    x = 0; y = 0;
// Center of front face, center, back face
    if(f < 2)
    {
      for(int i = 0; i < 4; i++)
      {
        x += vertices[f*8 + i*2] / 4.;
        y += vertices[f*8 + i*2 + 1] / 4.;
      }
    }
    local[0] = x; local[1] = y;
    if(f == 0)
      local[2] = -dz;
    else if(f == 1)
      local[2] = dz;
    else
      local[2] = 0;

    gGeoManager->LocalToMaster(local, master);
    TVector3 vect(master);
    
    chi += TMath::Power((180.0 * vect.Theta() / TMath::Pi()) - angleNominal, 2);
  }

  return chi;
}

double thetaNominal[2] = {28., 28. + 35.};
//int crystalsEdge[8] = {1827, 1826, 1831, 1830, 931, 929, 935, 933};
int crystalsEdge[] = {1826, 1827, 1825, 1828, 929, 931, 930, 932};

void CalcChiSqu2(int &npar, double *gin, double &f, double *par, int iflag)
{
  double dy = par[0];
  double dz = par[1];
  double theta = par[2];

  matrixPetal.SetTranslation(0.0, dy, dz);
  rotPetal.SetAngles(0.0, theta, 0.0);
  matrixPetal.SetRotation(rotPetal);

  for(int i = 0; i < 64; i++)
  {
    TGeoHMatrix *m = new TGeoHMatrix(matrixPetal);
    m->Multiply(matrixOrg[i]);
    nodes[i]->SetMatrix(m);
  }

  gGeoManager->GetCurrentNavigator()->ResetAll();

  f = 0.;

  for(int dir = 0; dir < 2; dir++)
  {
//      f += GetCrystalChiSqu(crystalsEdge[dir], thetaNominal[dir]);
    for(int c = 0; c < 4; c++)
    {
      f += GetCrystalChiSqu(crystalsEdge[c + 4*dir], thetaNominal[dir]);
    }
  }
}

void CalcChiSqu(int &npar, double *gin, double &f, double *par, int iflag)
{
  double dy = par[0];
  double dz = par[1];
  double theta = par[2];

  matrixPetal.SetTranslation(0.0, dy, dz);
  rotPetal.SetAngles(0.0, theta, 0.0);
  matrixPetal.SetRotation(rotPetal);

  for(int i = 0; i < 64; i++)
  {
    TGeoHMatrix *m = new TGeoHMatrix(matrixPetal);
    m->Multiply(matrixOrg[i]);
    nodes[i]->SetMatrix(m);
  }

  gGeoManager->GetCurrentNavigator()->ResetAll();

  f = 0;

  for(int i = 0; i < 64; i++)
  {
    geo_sim->GetAngles(febexChannelToCrystalId(i), &theta_sim, &dtmp, &dtmp);

    f += TMath::Power((theta_sim - theta_exp[i]) * 180.0 / TMath::Pi(), 2);
  }
}

// Note: Execute with Alveoli copies 0, 1 to get transformation matrix
void Fit()
{
  geo_sim = R3BCalifaGeometry::Instance(0x438b);

  for(int i = 0; i < 64; i++)
  {
    if(!gGeoManager->cd(geo_sim->GetCrystalVolumePath(febexChannelToCrystalId(i))))
    {
      cerr << "Could not cd to volume " << i << endl;
      return;
    }
    
//    gGeoManager->LocalToMaster(local, master);
//    cout << "Original Pos " << i << ": " << master[0] << ", " << master[1] << ", " << master[2] << endl;

    gGeoManager->CdUp();  // Crystal with Wrapping
    gGeoManager->CdUp();  // Alveolus Inner
    gGeoManager->CdUp();  // Alveolus
//    cout << gGeoManager->GetPath() << endl;

    nodes[i] = (TGeoNodeMatrix*)gGeoManager->GetCurrentNode();
    matrixOrg[i] = nodes[i]->GetMatrix()->MakeClone();

    geo_sim->GetAngles(febexChannelToCrystalId(i), &theta_sim, &dtmp, &dtmp);

    cout << i << ": " << 180.0 * theta_sim / TMath::Pi() << endl; //" (" << 180.0 * theta_exp[i] / TMath::Pi() << ")" << endl;
  }

  cout << "-------------------------------------------------" << endl;

  TMinuit min(3);
  min.SetFCN(CalcChiSqu2);

  int ieflag = 0;
  min.mnparm(0, TString("DY (cm)"), 0., 0.01, -50., 50., ieflag);
  min.mnparm(1, TString("DZ (cm)"), 0., 0.01, -50., 50., ieflag);
  min.mnparm(2, TString("Theta (deg)"), 20., 0.01, -45., 45., ieflag);

  min.SetPrintLevel(1);
  min.SetErrorDef(1); // Chi2

  min.Migrad();

  double v, e;
  min.GetParameter(0, v, e);
  cout << "DY: " << v << " (" << e << ")" << endl;
  min.GetParameter(1, v, e);
  cout << "DZ: " << v << " (" << e << ")" << endl;
  min.GetParameter(2, v, e);
  cout << "Theta: " << v << " (" << e << ")" << endl;

  cout << "-------------------------------------------------" << endl;

  for(int i = 0; i < 64; i++)
  {
    geo_sim->GetAngles(febexChannelToCrystalId(i), &theta_sim, &dtmp, &dtmp);

    cout << i << ": " << 180.0 * theta_sim / TMath::Pi() << endl; //" (" << 180.0 * theta_exp[i] / TMath::Pi() << ")" << endl;
  }
}

void Plot()
{
  TH2I *hphi[2], *htheta[2];

  for(int i = 0; i < 2; i++)
  {
    hphi[i] = new TH2I(Form("phi%d", i), Form("Phi %d", i), 360, 0, 360, 360, 0, 360);
    htheta[i] = new TH2I(Form("theta%d", i), Form("Theta %d", i), 180, 0, 180, 180, 0, 180);
  }

  R3BCalifaGeometry *gsim = R3BCalifaGeometry::Instance(17);
  R3BCalifaGeometry *gexp = R3BCalifaGeometry::Instance(0x438b);

  double phi_exp, theta_exp, rho_exp, phi_sim, theta_sim, rho_sim;

  for(int i = 0; i < 128; i++)
  {
    gsim->GetAngles(febexChannelToCrystalId(i), &theta_sim, &phi_sim, &rho_sim);
    gexp->GetAngles(i, &theta_exp, &phi_exp, &rho_exp);

    hphi[i/64]->Fill(180.0 * phi_exp / TMath::Pi(), 180.0 * phi_sim / TMath::Pi());
    htheta[i/64]->Fill(180.0 * theta_exp / TMath::Pi(), 180.0 * theta_sim / TMath::Pi());
  }

  TCanvas *c = new TCanvas("c1", "c1");
  c->Divide(2,2);
  c->cd(1);
  hphi[0]->Draw("colz");
  c->cd(2);
  hphi[1]->Draw("colz");
  c->cd(3);
  htheta[0]->Draw("colz");
  c->cd(4);
  htheta[1]->Draw("colz");
}

// TODO: TMinuit to minimize Chi**2 between Angles angles and sim angles with transformation matrix T
