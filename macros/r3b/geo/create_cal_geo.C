#include <iomanip>
#include <iostream>
#include "TGeoManager.h"
#include "TMath.h"



#define XB_CRYSTAL(no,type,theta,phi,psi,n1,n2,n3,n4,n5,n6) { no, type, theta, phi, psi },
#define XB_CRYSTAL_MOD(nr,mod,delta) { nr, mod, delta },

#define XB_WRAPPING(no,type,theta,phi,psi,n1,n2,n3,n4,n5,n6) {no, type, theta, phi, psi},
#define XB_WRAPPING_MOD(nr,mod,delta) { nr, mod, delta },



struct xb_wrapping
{
  int no;
  int type;
  Double_t theta, phi, psi;
  bool active;
};

struct xb_wrapping_mod
{
  int no;
  int mod;
  Double_t delta;
};

struct xb_crystal
{
  int no;
  int type;
  Double_t theta, phi, psi;
  bool active;
};

TGeoShape* createVolume(Double_t *arbVolumes, int voltype);
void insertWrapping(xb_wrapping *wrapping, TGeoVolume **wrappingVolumes, TGeoVolume *worldVolume, Double_t r);


// Create Matrix Unity
TGeoRotation *fGlobalRot = new TGeoRotation();

// Create a null translation
TGeoTranslation *fGlobalTrans = new TGeoTranslation();
TGeoRotation *fRefRot = NULL;

TGeoManager*   gGeoMan           = NULL;




Double_t fThetaX = 0.;
Double_t fThetaY = 0.;
Double_t fThetaZ = 0.;
Double_t fPhi   = 0.;
Double_t fTheta = 0.;
Double_t fPsi   = 0.;
Double_t fX = 0.;
Double_t fY = 0.;
Double_t fZ = 0.;
Bool_t fLocalTrans = kFALSE;
Bool_t fLabTrans = kFALSE;




void create_cal_geo(const char* geoTag)
{
  fGlobalTrans->SetTranslation(0.0,0.0,0.0);

  // -------   Load media from media file   -----------------------------------
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString medFile = geoPath + "/geometry/media_r3b.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  gGeoMan = gGeoManager;
  // --------------------------------------------------------------------------



  // -------   Geometry file name (output)   ----------------------------------
  TString geoFileName = geoPath + "/geometry/cal_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------



  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* mAir      = geoMedia->getMedium("Air");
  if ( ! mAir ) Fatal("Main", "FairMedium Air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* pMed2 = gGeoMan->GetMedium("Air");
  if ( ! pMed2 ) Fatal("Main", "Medium Air not found");

  FairGeoMedium* mNaI      = geoMedia->getMedium("NaI");
  if ( ! mNaI ) Fatal("Main", "FairMedium NaI not found");
  geoBuild->createMedium(mNaI);
  TGeoMedium* pMed10 = gGeoMan->GetMedium("NaI");
  if ( ! pMed10 ) Fatal("Main", "Medium NaI not found");

  FairGeoMedium* mAlu      = geoMedia->getMedium("aluminium");
  if ( ! mAlu ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAlu);
  TGeoMedium* pMed21 = gGeoMan->GetMedium("aluminium");
  if ( ! pMed21 ) Fatal("Main", "Medium aluminium not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("CALgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------




  Double_t dx,dy,dz;
  Double_t rmin, rmax, rmin1, rmax1, rmin2, rmax2;
  Double_t theta1, theta2, phi1, phi2, dphi;
  Double_t crystalPosRad =35;  //Radius of the center of the crystals [cm]
  Double_t wrappingPosRad=35;  //Radius of the center of the wrappings [cm]

  // --- Aluminum Wrappings

  // Definitions for the Al wrapping types from "xb_wrapping_loc.hh"
  int XB_WTYPE_A=0, XB_WTYPE_B=1, XB_WTYPE_C=2, XB_WTYPE_D=3, WDISABLE=0, WMOVE_R=1;


  // Get wrapping position information
  xb_wrapping xb_wrappings[] = {
{1, XB_WTYPE_D,   90.000, 180.000, -90.000}, // D    0.000
{2, XB_WTYPE_B,   90.000,-163.743,   0.000}, // B    0.051
{3, XB_WTYPE_C,   73.928,-171.645, -32.656}, // C    0.107
{4, XB_WTYPE_C,   73.928, 171.645, 212.656}, // C    0.107
{5, XB_WTYPE_B,   90.000, 163.743, 180.000}, // B    0.051
{6, XB_WTYPE_C,  106.072, 171.645, 147.344}, // C    0.107
{7, XB_WTYPE_C,  106.072,-171.645,  32.656}, // C    0.107
{8, XB_WTYPE_B,  104.686,-153.168,  71.370}, // B    0.019
{9, XB_WTYPE_A,   90.000,-148.283,   0.000}, // A    0.003
{10, XB_WTYPE_B,   75.314,-153.168, -71.370}, // B    0.019
{11, XB_WTYPE_D,   60.000,-159.095,  20.905}, // D    0.017
{12, XB_WTYPE_C,   59.817, 180.000,  90.000}, // C    0.064
{13, XB_WTYPE_D,   60.000, 159.095, -20.905}, // D    0.017
{14, XB_WTYPE_B,   75.314, 153.168, 251.370}, // B    0.019
{15, XB_WTYPE_A,   90.000, 148.283, 180.000}, // A    0.003
{16, XB_WTYPE_B,  104.686, 153.168, 108.630}, // B    0.019
{17, XB_WTYPE_D,  120.000, 159.095,-159.095}, // D    0.017
{18, XB_WTYPE_C,  120.183, 180.000, 270.000}, // C    0.064
{19, XB_WTYPE_D,  120.000,-159.095, 159.095}, // D    0.017
{20, XB_WTYPE_C,  116.612,-138.914, 110.220}, // C    0.022
{21, XB_WTYPE_B,   99.014,-135.670, 144.998}, // B    0.013
{22, XB_WTYPE_B,   80.986,-135.670, 215.002}, // B    0.013
{23, XB_WTYPE_C,   63.388,-138.914, 249.780}, // C    0.014
{24, XB_WTYPE_C,   47.634,-142.679, 132.588}, // C    0.018
{25, XB_WTYPE_B,   45.050,-167.209, 115.890}, // B    0.024
{26, XB_WTYPE_B,   45.050, 167.209,  64.110}, // B    0.024
{27, XB_WTYPE_C,   47.634, 142.679,  47.412}, // C    0.018
{28, XB_WTYPE_C,   63.388, 138.914, -69.780}, // C    0.014
{29, XB_WTYPE_B,   80.986, 135.670, -35.002}, // B    0.013
{30, XB_WTYPE_B,   99.014, 135.670,  35.002}, // B    0.013
{31, XB_WTYPE_C,  116.612, 138.914,  69.780}, // C    0.022
{32, XB_WTYPE_C,  132.366, 142.679, -47.412}, // C    0.018
{33, XB_WTYPE_B,  134.950, 167.209, -64.110}, // B    0.024
{34, XB_WTYPE_B,  134.950,-167.209, 244.110}, // B    0.024
{35, XB_WTYPE_C,  132.366,-142.679, 227.412}, // C    0.018
{36, XB_WTYPE_C,  125.985,-123.613,  -1.114}, // C    0.020
{37, XB_WTYPE_D,  108.000,-121.717,  58.283}, // D    0.001
{38, XB_WTYPE_C,   90.000,-120.183, 180.000}, // C    0.013
{39, XB_WTYPE_D,   72.000,-121.717, 121.717}, // D    0.001
{40, XB_WTYPE_C,   54.015,-123.613,   1.114}, // C    0.022
{41, XB_WTYPE_B,   30.325,-149.860, 172.011}, // B    0.018
{42, XB_WTYPE_A,   31.717, 180.000, -90.000}, // A    0.003
{43, XB_WTYPE_B,   30.325, 149.860,   7.989}, // B    0.018
{44, XB_WTYPE_C,   54.015, 123.613, 178.886}, // C    0.022
{45, XB_WTYPE_D,   72.000, 121.717,-121.717}, // D    0.001
{46, XB_WTYPE_C,   90.000, 120.183,   0.000}, // C    0.013
{47, XB_WTYPE_D,  108.000, 121.717, -58.283}, // D    0.001
{48, XB_WTYPE_C,  125.985, 123.613, 181.114}, // C    0.022
{49, XB_WTYPE_B,  149.675, 149.860,  -7.989}, // B    0.018
{50, XB_WTYPE_A,  148.283, 180.000,  90.000}, // A    0.003
{51, XB_WTYPE_B,  149.675,-149.860, 187.989}, // B    0.018
{52, XB_WTYPE_D,  144.000,-121.717, -58.283}, // D    0.004
{53, XB_WTYPE_B,  133.643,-102.504,  46.296}, // B    0.021
{54, XB_WTYPE_B,  115.890,-106.368, -25.938}, // B    0.016
{55, XB_WTYPE_C,   98.026,-106.235, -59.672}, // C    0.114
{56, XB_WTYPE_C,   81.974,-106.235,  59.672}, // C    0.114
{57, XB_WTYPE_B,   64.110,-106.368,  25.938}, // B    0.016
{58, XB_WTYPE_B,   46.357,-102.504, -46.296}, // B    0.021
{59, XB_WTYPE_D,   36.000,-121.717,-121.717}, // D    0.004
{60, XB_WTYPE_C,   18.063,-116.764, 175.290}, // C    0.114
{61, XB_WTYPE_B,   16.257, 180.000, -90.000}, // B    0.012
{62, XB_WTYPE_C,   18.063, 116.764,   4.710}, // C    0.114
{63, XB_WTYPE_D,   36.000, 121.717, 121.717}, // D    0.004
{64, XB_WTYPE_B,   46.357, 102.504, 226.296}, // B    0.021
{65, XB_WTYPE_B,   64.110, 106.368, 154.062}, // B    0.016
{66, XB_WTYPE_C,   81.974, 106.235, 120.328}, // C    0.114
{67, XB_WTYPE_C,   98.026, 106.235, 239.672}, // C    0.114
{68, XB_WTYPE_B,  115.890, 106.368, 205.938}, // B    0.016
{69, XB_WTYPE_B,  133.643, 102.504, 133.704}, // B    0.021
{70, XB_WTYPE_D,  144.000, 121.717,  58.283}, // D    0.004
{71, XB_WTYPE_C,  161.937, 116.764,  -4.710}, // C    0.114
{72, XB_WTYPE_B,  163.743, 180.000,  90.000}, // B    0.012
{73, XB_WTYPE_C,  161.937,-116.764, 184.710}, // C    0.114
{74, XB_WTYPE_C,  149.817, -90.000,  90.000}, // C    0.013
{75, XB_WTYPE_A,  121.717, -90.000, -90.000}, // A    0.003
{76, XB_WTYPE_B,  106.257, -90.000, -90.000}, // B    0.051
{77, XB_WTYPE_D,   90.000, -90.000, 180.000}, // D    0.000
{78, XB_WTYPE_B,   73.743, -90.000,  90.000}, // B    0.051
{79, XB_WTYPE_A,   58.283, -90.000,  90.000}, // A    0.003
{80, XB_WTYPE_C,   30.183, -90.000, 270.000}, // C    0.013
{81, XB_WTYPE_D,    0.000,   0.000,   0.000}, // D    0.000
{82, XB_WTYPE_D,  180.000,   0.000, 180.000}, // D    0.000
{83, XB_WTYPE_C,  149.817,  90.000,  90.000}, // C    0.013
{84, XB_WTYPE_A,  121.717,  90.000, 270.000}, // A    0.003
{85, XB_WTYPE_B,  106.257,  90.000, 270.000}, // B    0.051
{86, XB_WTYPE_D,   90.000,  90.000,-180.000}, // D    0.000
{87, XB_WTYPE_B,   73.743,  90.000,  90.000}, // B    0.051
{88, XB_WTYPE_A,   58.283,  90.000,  90.000}, // A    0.003
{89, XB_WTYPE_C,   30.183,  90.000, -90.000}, // C    0.013
{90, XB_WTYPE_C,   18.063,  63.236, 175.290}, // C    0.114
{91, XB_WTYPE_B,   16.257,   0.000, 270.000}, // B    0.012
{92, XB_WTYPE_C,   18.063, -63.236,   4.710}, // C    0.114
{93, XB_WTYPE_D,   36.000, -58.283, 121.717}, // D    0.004
{94, XB_WTYPE_B,   46.357, -77.496, 226.296}, // B    0.021
{95, XB_WTYPE_B,   64.110, -73.632, 154.062}, // B    0.016
{96, XB_WTYPE_C,   81.974, -73.765, 120.328}, // C    0.114
{97, XB_WTYPE_C,   98.026, -73.765, 239.672}, // C    0.114
{98, XB_WTYPE_B,  115.890, -73.632, 205.938}, // B    0.016
{99, XB_WTYPE_B,  133.643, -77.496, 133.704}, // B    0.021
{100, XB_WTYPE_D,  144.000, -58.283,  58.283}, // D    0.004
{101, XB_WTYPE_C,  161.937, -63.236,  -4.710}, // C    0.114
{102, XB_WTYPE_B,  163.743,   0.000,  90.000}, // B    0.012
{103, XB_WTYPE_C,  161.937,  63.236, 184.710}, // C    0.114
{104, XB_WTYPE_D,  144.000,  58.283, -58.283}, // D    0.004
{105, XB_WTYPE_B,  133.643,  77.496,  46.296}, // B    0.021
{106, XB_WTYPE_B,  115.890,  73.632, -25.938}, // B    0.016
{107, XB_WTYPE_C,   98.026,  73.765, -59.672}, // C    0.114
{108, XB_WTYPE_C,   81.974,  73.765,  59.672}, // C    0.114
{109, XB_WTYPE_B,   64.110,  73.632,  25.938}, // B    0.016
{110, XB_WTYPE_B,   46.357,  77.496, -46.296}, // B    0.021
{111, XB_WTYPE_D,   36.000,  58.283,-121.717}, // D    0.004
{112, XB_WTYPE_B,   30.325,  30.140, 172.011}, // B    0.018
{113, XB_WTYPE_A,   31.717,   0.000, 270.000}, // A    0.003
{114, XB_WTYPE_B,   30.325, -30.140,   7.989}, // B    0.018
{115, XB_WTYPE_C,   54.015, -56.387, 178.886}, // C    0.022
{116, XB_WTYPE_D,   72.000, -58.283,-121.717}, // D    0.001
{117, XB_WTYPE_C,   90.000, -59.817,   0.000}, // C    0.013
{118, XB_WTYPE_D,  108.000, -58.283, -58.283}, // D    0.001
{119, XB_WTYPE_C,  125.985, -56.387, 181.114}, // C    0.022
{120, XB_WTYPE_B,  149.675, -30.140,  -7.989}, // B    0.018
{121, XB_WTYPE_A,  148.283,   0.000,  90.000}, // A    0.003
{122, XB_WTYPE_B,  149.675,  30.140, 187.989}, // B    0.018
{123, XB_WTYPE_C,  125.985,  56.387,  -1.114}, // C    0.022
{124, XB_WTYPE_D,  108.000,  58.283,  58.283}, // D    0.001
{125, XB_WTYPE_C,   90.000,  59.817, 180.000}, // C    0.013
{126, XB_WTYPE_D,   72.000,  58.283, 121.717}, // D    0.001
{127, XB_WTYPE_C,   54.015,  56.387,   1.114}, // C    0.034
{128, XB_WTYPE_C,   47.634,  37.321, 132.588}, // C    0.018
{129, XB_WTYPE_B,   45.050,  12.791, 115.890}, // B    0.024
{130, XB_WTYPE_B,   45.050, -12.791,  64.110}, // B    0.024
{131, XB_WTYPE_C,   47.634, -37.321,  47.412}, // C    0.018
{132, XB_WTYPE_C,   63.388, -41.086, -69.780}, // C    0.022
{133, XB_WTYPE_B,   80.986, -44.330, -35.002}, // B    0.013
{134, XB_WTYPE_B,   99.014, -44.330,  35.002}, // B    0.013
{135, XB_WTYPE_C,  116.612, -41.086,  69.780}, // C    0.014
{136, XB_WTYPE_C,  132.366, -37.321, -47.412}, // C    0.018
{137, XB_WTYPE_B,  134.950, -12.791, -64.110}, // B    0.024
{138, XB_WTYPE_B,  134.950,  12.791, 244.110}, // B    0.024
{139, XB_WTYPE_C,  132.366,  37.321, 227.412}, // C    0.018
{140, XB_WTYPE_C,  116.612,  41.086, 110.220}, // C    0.014
{141, XB_WTYPE_B,   99.014,  44.330, 144.998}, // B    0.013
{142, XB_WTYPE_B,   80.986,  44.330, 215.002}, // B    0.013
{143, XB_WTYPE_C,   63.388,  41.086, 249.780}, // C    0.022
{144, XB_WTYPE_D,   60.000,  20.905,  20.905}, // D    0.017
{145, XB_WTYPE_C,   59.817,   0.000,  90.000}, // C    0.064
{146, XB_WTYPE_D,   60.000, -20.905, -20.905}, // D    0.017
{147, XB_WTYPE_B,   75.314, -26.832, 251.370}, // B    0.019
{148, XB_WTYPE_A,   90.000, -31.717, 180.000}, // A    0.003
{149, XB_WTYPE_B,  104.686, -26.832, 108.630}, // B    0.019
{150, XB_WTYPE_D,  120.000, -20.905,-159.095}, // D    0.017
{151, XB_WTYPE_C,  120.183,   0.000, 270.000}, // C    0.064
{152, XB_WTYPE_D,  120.000,  20.905, 159.095}, // D    0.017
{153, XB_WTYPE_B,  104.686,  26.832,  71.370}, // B    0.019
{154, XB_WTYPE_A,   90.000,  31.717,   0.000}, // A    0.003
{155, XB_WTYPE_B,   75.314,  26.832, -71.370}, // B    0.019
{156, XB_WTYPE_C,   73.928,   8.355, -32.656}, // C    0.107
{157, XB_WTYPE_C,   73.928,  -8.355, 212.656}, // C    0.107
{158, XB_WTYPE_B,   90.000, -16.257, 180.000}, // B    0.051
{159, XB_WTYPE_C,  106.072,  -8.355, 147.344}, // C    0.107
{160, XB_WTYPE_C,  106.072,   8.355,  32.656}, // C    0.107
{161, XB_WTYPE_B,   90.000,  16.257,   0.000}, // B    0.051
{162, XB_WTYPE_D,   90.000,   0.000,  90.000}  // D    0.000
  };
  
  int nrWrappings = (Double_t)sizeof( xb_wrappings ) / (Double_t)sizeof( xb_wrappings[0] );
  for (int i = 0; i < nrWrappings; i++) {
    xb_wrappings[i].active = true;
  }

  // Get corner information for the wrappings
  Double_t xb_wrapping_parts[][16] =
    {
0.0,       9.06038,  8.61693,  2.79981,  5.32555, -7.33,     0.0,      -7.33,
0.0,      -7.33,    -5.32555, -7.33,    -8.61693,  2.79981,  0.0,       9.06038,
-9.60862,   0.0,     -5.31000,  6.18700,  5.31000,  6.18700,  9.60862,   0.0,
9.60862,   0.0,      4.33050, -7.42200, -4.33050, -7.42200, -9.60862,   0.0,
0.0,      -8.11689, -7.15389, -3.93231, -8.51298,  4.85925,  0.0,       8.09558,
0.0,       8.09558,  8.51298,  4.85925,  7.15389, -3.93231,  0.0,      -8.11689,
0.0,     -10.08625, -6.80000, -4.3305,  -6.80000,  4.3305,   0.0,      10.08625,
0.0,      10.08625,  6.80000,  4.3305,   6.80000, -4.3305,   0.0,     -10.08625
    };

  // Get modification information for wrappings
  xb_wrapping_mod xb_wrappings_mod[] = {
{77, WDISABLE, 0.0 },
{81, WDISABLE, 0.0 },
{82, WDISABLE, 0.0 }
    };

  // --- NaI Crystals
  // Get corner information for the crystals
  Double_t xb_crystal_parts[][16] =
    {
0.0,       9.06038,  8.61693,  2.79981,  5.32555, -7.33,     0.0,      -7.33,
0.0,      -7.33,    -5.32555, -7.33,    -8.61693,  2.79981,  0.0,       9.06038,
-9.60862,   0.0,     -5.31000,  6.18700,  5.31000,  6.18700,  9.60862,   0.0,
9.60862,   0.0,      4.33050, -7.42200, -4.33050, -7.42200, -9.60862,   0.0,
0.0,      -8.11689, -7.15389, -3.93231, -8.51298,  4.85925,  0.0,       8.09558,
0.0,       8.09558,  8.51298,  4.85925,  7.15389, -3.93231,  0.0,      -8.11689,
0.0,     -10.08625, -6.80000, -4.3305,  -6.80000,  4.3305,   0.0,      10.08625,
0.0,      10.08625,  6.80000,  4.3305,   6.80000, -4.3305,   0.0,     -10.08625
    };



  // --- Create Volumes: Aluminum Wrappings
  // Create volume: wrappingLogNAJA
cout << 1 << endl;
  TGeoShape *pWTRA = createVolume(xb_wrapping_parts[0],0);
cout << 2 << endl;
  pWTRA->SetTitle("WTRA1+WTRA2");
  TGeoVolume* pwrappingLogNAJA = new TGeoVolume("wrappingLogNAJA",pWTRA, pMed21);
  pwrappingLogNAJA->SetLineColor(34);
  pwrappingLogNAJA->VisibleDaughters(kTRUE);
cout << 3 << endl;

  // Create volume: wrappingLogNAJB
  TGeoShape *pWTRB = createVolume(xb_wrapping_parts[1],0);
  pWTRB->SetTitle("WTRB1+WTRB2");
  TGeoVolume* pwrappingLogNAJB = new TGeoVolume("wrappingLogNAJB",pWTRB, pMed21);
  pwrappingLogNAJB->SetLineColor(35);
  pwrappingLogNAJB->VisibleDaughters(kTRUE);

  // Create volume: wrappingLogNAJC
  TGeoShape *pWTRC = createVolume(xb_wrapping_parts[2],0);
  pWTRC->SetTitle("WTRC1+WTRC2");
  TGeoVolume* pwrappingLogNAJC = new TGeoVolume("wrappingLogNAJC",pWTRC, pMed21);
  pwrappingLogNAJC->SetLineColor(36);
  pwrappingLogNAJC->VisibleDaughters(kTRUE);

  // Create volume: wrappingLogNAJC
  TGeoShape *pWTRD = createVolume(xb_wrapping_parts[3],0);
  pWTRD->SetTitle("WTRD1+WTRD2");
  TGeoVolume* pwrappingLogNAJD = new TGeoVolume("wrappingLogNAJD",pWTRD, pMed21);
  pwrappingLogNAJD->SetLineColor(37);
  pwrappingLogNAJD->VisibleDaughters(kTRUE);

  TGeoVolume* wrappingVolumes[]={pwrappingLogNAJA,
				 pwrappingLogNAJB,
				 pwrappingLogNAJC,
				 pwrappingLogNAJD};

  // --- Create Volumes: NaI Volumes

  // Create volume: crystalLogNAJA
  TGeoShape *pTRA = createVolume(xb_crystal_parts[0],1);
  pTRA->SetTitle("TRA1+TRA2");
  TGeoVolume* pcrystalLogNAJA = new TGeoVolume("crystalLogNAJA",pTRA, pMed10);
  pcrystalLogNAJA->SetLineColor(41);

  // Create volume: crystalLogNAJB
  TGeoShape *pTRB = createVolume(xb_crystal_parts[1],1);
  pTRB->SetTitle("TRB1+TRB2");
  TGeoVolume* pcrystalLogNAJB = new TGeoVolume("crystalLogNAJB",pTRB, pMed10);
  pcrystalLogNAJB->SetLineColor(42);

  // Create volume: crystalLogNAJC
  TGeoShape *pTRC = createVolume(xb_crystal_parts[2],1);
  pTRC->SetTitle("TRC1+TRC2");
  TGeoVolume* pcrystalLogNAJC = new TGeoVolume("crystalLogNAJC",pTRC, pMed10);
  pcrystalLogNAJC->SetLineColor(43);

  // Create volume: crystalLogNAJC
  TGeoShape *pTRD = createVolume(xb_crystal_parts[3],1);
  pTRD->SetTitle("TRD1+TRD2");
  TGeoVolume* pcrystalLogNAJD = new TGeoVolume("crystalLogNAJD",pTRD, pMed10);
  pcrystalLogNAJD->SetLineColor(44);

  TGeoVolume* crystalVolumes[]={pcrystalLogNAJA,
                                pcrystalLogNAJB,
                                pcrystalLogNAJC,
                                pcrystalLogNAJD};

  // ---- Geometry Definition


  // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  // Shape: CBSphereWorld type: TGeoSphere
  rmin  = 24.250000;
  rmax  = 70.000000;
  theta1 = 0.000000;
  theta2 = 180.000000;
  phi1  = 0.000000;
  phi2  = 360.000000;
  TGeoShape *pCBSphereWorld = new TGeoSphere("CBSphereWorld",rmin,rmax,theta1, theta2,phi1,phi2);
  // Volume: CBLogWorld
  TGeoVolume*
    pCBLogWorld = new TGeoVolume("CBLogWorld",pCBSphereWorld, pMed2);
  pCBLogWorld->SetVisLeaves(kTRUE);

  TGeoRotation *zeroRot = new TGeoRotation;
  TGeoTranslation *zeroTrans = new TGeoTranslation;
  TGeoCombiTrans *zeroRotTrans = new TGeoCombiTrans(*zeroTrans, *zeroRot);

  // add the sphere as Mother Volume
  top->AddNode(pCBLogWorld, 0, zeroRotTrans);

  // --- Definition of the Reaction Chamber
  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 39.100000;
  // Rotation: 
  TGeoCombiTrans* pCombTrans1 = new TGeoCombiTrans("", dx,dy,dz, zeroRot);
  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = -39.100000;
  // Rotation:
  TGeoRotation *pRot1 = new TGeoRotation("", 0, 180, 0);
  TGeoCombiTrans* pCombTrans2 = new TGeoCombiTrans("", dx,dy,dz, pRot1);
  // Combi transformation:
  dx = 0.000000; 
  dy = -39.100000;
  dz = 0.000000;
  // Rotation: 
  TGeoRotation *pRot2 = new TGeoRotation("", 0, 90, 0);
  TGeoCombiTrans* pCombTrans3 = new TGeoCombiTrans("", dx,dy,dz, pRot2);

  // Shape: CBReactionChamber type: TGeoSphere
  rmin = 24.250000;
  rmax = 24.500000;
  theta1 = 6.423161;
  theta2 = 173.576839;
  phi1 = 0.000000;
  phi2 = 360.000000;
  TGeoShape *pCBReactionChamber = new TGeoSphere("CBReactionChamber",rmin,rmax,theta1, theta2,phi1,phi2);
  // Volume: CBChamberLog
  TGeoVolume* pCBChamberLog = new TGeoVolume("CBChamberLog",pCBReactionChamber, pMed21);
  pCBChamberLog->SetVisLeaves(kTRUE);
  pCBChamberLog->SetLineColor(18);
    TGeoCombiTrans* zeroRotTrans_1 = new TGeoCombiTrans(0., 0., 0., zeroRot);
  pCBLogWorld->AddNode(pCBChamberLog, 0, zeroRotTrans_1);

  // Shape: CBConicalTube type: TGeoConeSeg
  dz = 14.600000;
  rmin1 = 2.730000;
  rmax1 = 2.855000;
  rmin2 = 5.125000;
  rmax2 = 5.250000;
  phi1 = 0.000000;
  phi2 = 360.000000;
  TGeoShape *pCBConicalTube = new TGeoConeSeg("CBConicalTube", dz,rmin1,rmax1,rmin2,rmax2,phi1,phi2);
  // Volume: CBConicalTubeLog
  TGeoVolume* pCBConicalTubeLog = new TGeoVolume("CBConicalTubeLog",pCBConicalTube, pMed21);
  pCBConicalTubeLog->SetVisLeaves(kTRUE);
  pCBConicalTubeLog->SetLineColor(33);
  pCBLogWorld->AddNode(pCBConicalTubeLog, 0, pCombTrans1);
  pCBLogWorld->AddNode(pCBConicalTubeLog, 1, pCombTrans2);
  pCBLogWorld->AddNode(pCBConicalTubeLog, 2, pCombTrans3);

  // --- Add Wrappings to mother volume (CBWorld)

  // Add modified wrappings to the crystal ball, defined in "xb_wrapping_mod.hh"
  int nrModifiedWrappings = sizeof( xb_wrappings_mod ) / sizeof( xb_wrappings_mod[0] );
  for (int i = 0; i < nrModifiedWrappings; i++)
    {
      xb_wrappings[xb_wrappings_mod[i].no - 1].active = false;
      if (xb_wrappings_mod[i].mod == WMOVE_R)
	insertWrapping(&xb_wrappings[xb_wrappings_mod[i].no - 1], wrappingVolumes, pCBLogWorld, wrappingPosRad + xb_wrappings_mod[i].delta);
    }

  // Add all enabled wrappings to the wrapping ball
  for (int i=0; i < nrWrappings; i++){
    if (xb_wrappings[i].active) 
      insertWrapping(&xb_wrappings[i], wrappingVolumes, pCBLogWorld, wrappingPosRad);
  }

  // --- Add crystals as wrappings' daughters
  pwrappingLogNAJA->AddNode(pcrystalLogNAJA, 0);
  pwrappingLogNAJB->AddNode(pcrystalLogNAJB, 0);
  pwrappingLogNAJC->AddNode(pcrystalLogNAJC, 0);
  pwrappingLogNAJD->AddNode(pcrystalLogNAJD, 0);

  // --- End of Cal Crystal definition

  // --- Define Sensitive Modules
  //   AddSensitiveVolume(pcrystalLogNAJA);
  //   AddSensitiveVolume(pcrystalLogNAJB);
  //   AddSensitiveVolume(pcrystalLogNAJC);
  //   AddSensitiveVolume(pcrystalLogNAJD);




  // ---------------   Finish   -----------------------------------------------
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* geoFile = new TFile(geoFileName, "RECREATE");
  top->Write();
  geoFile->Close();
  // --------------------------------------------------------------------------
}




// -----   Private method insertWrapping   ----------------------------------
void insertWrapping(xb_wrapping *wrapping, TGeoVolume **wrappingVolumes, TGeoVolume *worldVolume, Double_t r)
{
  Double_t conv=TMath::DegToRad();
  Double_t dx = r*sin(wrapping->theta*conv)*cos(wrapping->phi*conv);
  Double_t dy = r*sin(wrapping->theta*conv)*sin(wrapping->phi*conv);
  Double_t dz = r*cos(wrapping->theta*conv);

  TGeoTranslation *trans = new TGeoTranslation(dx,dy,dz);
  TGeoRotation *pRot = new TGeoRotation();
  pRot->RotateZ(wrapping->psi);
  pRot->RotateY(wrapping->theta);
  pRot->RotateZ(wrapping->phi);

  TGeoCombiTrans *combi = new TGeoCombiTrans(*trans,*pRot);
 
  worldVolume->AddNode(wrappingVolumes[wrapping->type], wrapping->no, combi);
}


// -----   Private method createVolume   ----------------------------------
// Create TGeoShape from two TGeoArb8 for both crystals and wrappings
//   using the information in "xb_crysta_part.hh" and "xb_wrapping_part.hh"
TGeoShape* createVolume(Double_t *arbVolumes, int voltype)
{

  Double_t innerR = -1.;
  Double_t dZ = -1.;

  Double_t thick = 0.06;   // Al wrapping thickness in cm (600microns)

  if(voltype == 0) {          //wrappings
    innerR=25.;  //[cm]
    dZ=10.;      //[cm]
  } else if(voltype == 1) {   //crystals
    innerR=25.-thick;  //[cm]
    dZ=10.-thick;      //[cm]
  } else { cerr << " ******** Unknown volume type ******** " << endl; }

  // frameR is the radious of the frame which was the radious used during cornar definitions
  Double_t frameR = 49.86932985;  // [cm]
  Double_t scaleOuter=(innerR+(2*dZ))/frameR;
  Double_t scaleInner=innerR/frameR;

  Double_t volume1[]={
    arbVolumes[0]*scaleInner, arbVolumes[1]*scaleInner,
    arbVolumes[2]*scaleInner, arbVolumes[3]*scaleInner,
    arbVolumes[4]*scaleInner, arbVolumes[5]*scaleInner,
    arbVolumes[6]*scaleInner, arbVolumes[7]*scaleInner,
    arbVolumes[0]*scaleOuter, arbVolumes[1]*scaleOuter,
    arbVolumes[2]*scaleOuter, arbVolumes[3]*scaleOuter,
    arbVolumes[4]*scaleOuter, arbVolumes[5]*scaleOuter,
    arbVolumes[6]*scaleOuter, arbVolumes[7]*scaleOuter};

  Double_t volume2[]={
    arbVolumes[8]*scaleInner, arbVolumes[9]*scaleInner,
    arbVolumes[10]*scaleInner, arbVolumes[11]*scaleInner,
    arbVolumes[12]*scaleInner, arbVolumes[13]*scaleInner,
    arbVolumes[14]*scaleInner, arbVolumes[15]*scaleInner,
    arbVolumes[8]*scaleOuter,  arbVolumes[9]*scaleOuter,
    arbVolumes[10]*scaleOuter, arbVolumes[11]*scaleOuter,
    arbVolumes[12]*scaleOuter, arbVolumes[13]*scaleOuter,
    arbVolumes[14]*scaleOuter, arbVolumes[15]*scaleOuter};

  TGeoArb8 *pTRA1 = new TGeoArb8("PARB1", dZ, volume1);
  TGeoArb8 *pTRA2 = new TGeoArb8("PARB2", dZ, volume2);
  TGeoBoolNode *pBoolNode = new TGeoUnion(pTRA1, pTRA2, 0, 0);
  TGeoShape *pTRA = new TGeoCompositeShape("", pBoolNode);
  return pTRA;
}





TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef)
{
  if (fLocalTrans == kTRUE ) {
    
    if ( ( fThetaX == 0 )  && ( fThetaY==0 )  && ( fThetaZ == 0 )
	 &&
	 ( fX == 0 ) && ( fY == 0 ) && ( fZ == 0 )
	 )  return fRef;
    
    
    // X axis
    Double_t xAxis[3] = { 1. , 0. , 0. };
    Double_t yAxis[3] = { 0. , 1. , 0. };
    Double_t zAxis[3] = { 0. , 0. , 1. };
    // Reference Rotation
    fRefRot = fRef->GetRotation();
    
    if (fRefRot) {
      Double_t mX[3] = {0.,0.,0.};
      Double_t mY[3] = {0.,0.,0.};
      Double_t mZ[3] = {0.,0.,0.};
      
      fRefRot->LocalToMasterVect(xAxis,mX);
      fRefRot->LocalToMasterVect(yAxis,mY);
      fRefRot->LocalToMasterVect(zAxis,mZ);
      
      Double_t a[4]={ mX[0],mX[1],mX[2], fThetaX };
      Double_t b[4]={ mY[0],mY[1],mY[2], fThetaY };
      Double_t c[4]={ mZ[0],mZ[1],mZ[2], fThetaZ };
      
      ROOT::Math::AxisAngle aX(a,a+4);
      ROOT::Math::AxisAngle aY(b,b+4);
      ROOT::Math::AxisAngle aZ(c,c+4);
      
      ROOT::Math::Rotation3D fMatX( aX );
      ROOT::Math::Rotation3D fMatY( aY );
      ROOT::Math::Rotation3D fMatZ( aZ );
      
      ROOT::Math::Rotation3D  fRotXYZ = (fMatZ * (fMatY * fMatX));
      
      //cout << fRotXYZ << endl;
      
      Double_t fRotable[9]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
      fRotXYZ.GetComponents(
			    fRotable[0],fRotable[3],fRotable[6],
			    fRotable[1],fRotable[4],fRotable[7],
			    fRotable[2],fRotable[5],fRotable[8]
			    );
      TGeoRotation *pRot = new TGeoRotation();
      pRot->SetMatrix(fRotable);
      TGeoCombiTrans *pTmp = new TGeoCombiTrans(*fGlobalTrans,*pRot);
      
      // ne peut pas etre applique ici
      // il faut differencier trans et rot dans la multi.
      TGeoRotation rot_id;
      rot_id.SetAngles(0.0,0.0,0.0);
      
      TGeoCombiTrans c1;
      c1.SetRotation(rot_id);
      const Double_t *t = pTmp->GetTranslation();
      c1.SetTranslation(t[0],t[1],t[2]);
      
      TGeoCombiTrans c2;
      c2.SetRotation(rot_id);
      const Double_t *tt = fRefRot->GetTranslation();
      c2.SetTranslation(tt[0],tt[1],tt[2]);
      
      TGeoCombiTrans cc = c1 * c2 ;
      
      TGeoCombiTrans c3;
      c3.SetRotation(pTmp->GetRotation());
      TGeoCombiTrans c4;
      c4.SetRotation(fRefRot);
      
      TGeoCombiTrans ccc = c3 * c4;
      
      TGeoCombiTrans pGlobal;
      pGlobal.SetRotation(ccc.GetRotation());
      const Double_t *allt = cc.GetTranslation();
      pGlobal.SetTranslation(allt[0],allt[1],allt[2]);
      
      return  ( new TGeoCombiTrans( pGlobal ) );
      
    }else{
      
      cout << "-E- R3BDetector::GetGlobalPosition() \
	      No. Ref. Transformation defined ! " << endl;
      cout << "-E- R3BDetector::GetGlobalPosition() \
	      cannot create Local Transformation " << endl;
      return NULL;
    } //! fRefRot
    
  } else {
    // Lab Transf.
    if ( ( fPhi == 0 )  && ( fTheta==0 )  && ( fPsi == 0 )
	 &&
	 ( fX == 0 ) && ( fY == 0 ) && ( fZ == 0 )
	 )  return fRef;
    
    
    return ( new TGeoCombiTrans(*fGlobalTrans,*fGlobalRot) );
    
  }
}

