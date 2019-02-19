// -------------------------------------------------------------------------
// -----         R3BCalifaMapped2CrystalCalPar source file             -----
// -----             Created 22/07/14  by H.Alvarez                    -----
// -----            Modified 20/03/17  by P.Cabanelas                  -----
// -----            Modified 11/12/17  by E.Galiana                    -----
// -----            Modified 27/11/18  by J.L. Rodriguez               -----
// -------------------------------------------------------------------------

//ROOT headers
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TSpectrum.h"
#include "TGraph.h"
#include "TF1.h"

//Fair headers
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "TGeoManager.h"

//Califa headers
#include "R3BCalifaMappedData.h"
#include "R3BCalifaMapped2CrystalCalPar.h"
#include "R3BEventHeader.h"
#include "R3BCalifaCrystalCalPar.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

//R3BCalifaMapped2CrystalCalPar: Default Constructor --------------------------
R3BCalifaMapped2CrystalCalPar::R3BCalifaMapped2CrystalCalPar() :
  FairTask("R3B CALIFA Calibration Parameters Finder ",1),
  fCal_Par(NULL),
  fCalifaMappedDataCA(NULL),
  fNumCrystals(0),
  fNumParam(0),
  fMinStadistics(0),
  fMapHistos_left(0),
  fMapHistos_right(0),
  fMapHistos_bins(0),
  fMapHistos_leftp(0),
  fMapHistos_rightp(0),
  fMapHistos_binsp(0),
  fNumPeaks(0),
  fSigma(0),
  fThreshold(0),
  fEnergyPeaks(NULL),
  fOutputFile(NULL),
  fDebugMode(0)
{
  
}

//R3BCalifaMapped2CrystalCalPar: Standard Constructor --------------------------
R3BCalifaMapped2CrystalCalPar::R3BCalifaMapped2CrystalCalPar(const char* name, Int_t iVerbose) :
  FairTask(name, iVerbose),
  fCal_Par(NULL),
  fCalifaMappedDataCA(NULL),
  fNumCrystals(0),
  fNumParam(0),
  fMinStadistics(0),
  fMapHistos_left(0),
  fMapHistos_right(0),
  fMapHistos_bins(0),
  fMapHistos_leftp(0),
  fMapHistos_rightp(0),
  fMapHistos_binsp(0),
  fNumPeaks(0),
  fSigma(0),
  fThreshold(0),
  fEnergyPeaks(NULL),
  fOutputFile(NULL),
  fDebugMode(0)
{
  
}

//R3BCalifaMapped2CrystalCalPar: Destructor ----------------------------------------
R3BCalifaMapped2CrystalCalPar::~R3BCalifaMapped2CrystalCalPar() {
  if(fEnergyPeaks)
    delete fEnergyPeaks;
  
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BCalifaMapped2CrystalCalPar::Init() {

  LOG(INFO)<<"R3BCalifaMapped2CrystalCalPar::Init"<<FairLogger::endl;

  if(!fEnergyPeaks){
    fEnergyPeaks = new TArrayF;
    fEnergyPeaks->Set(fNumPeaks);
  }
  
  char name[100];
  Int_t fright, fleft, fbins; 
  
  fh_Map_energy_crystal = new TH1F*[fNumCrystals];
  for(Int_t i=0;i<fNumCrystals;i++){
    sprintf(name,"fh_Map_energy_crystal_%i",i+1);
    if(Searchrange(i+1)){
       fright = fMapHistos_right;
       fleft = fMapHistos_left;
       fbins = fMapHistos_bins;
    }else{
       fright = fMapHistos_rightp;
       fleft = fMapHistos_leftp;
       fbins = fMapHistos_binsp;
    }
    fh_Map_energy_crystal[i] = new TH1F(name,name,fbins,fleft,fright);
  }
  
  FairRootManager* rootManager = FairRootManager::Instance();
  if (!rootManager) { return kFATAL;}
  
  fCalifaMappedDataCA = (TClonesArray*)rootManager->GetObject("CalifaMappedData");
  if (!fCalifaMappedDataCA) { return kFATAL;}
  
  FairRuntimeDb* rtdb = FairRuntimeDb::instance();
  if (!rtdb) { return kFATAL;}
  
  fCal_Par=(R3BCalifaCrystalCalPar*)rtdb->getContainer("califaCrystalCalPar");
  if (!fCal_Par) {
    LOG(ERROR)<<"R3BCalifaMapped2CrystalCalPar::Init() Couldn't get handle on califaCrystalCalPar container"<<FairLogger::endl;
    return kFATAL;
  }
  
  return kSUCCESS;
}

// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCalifaMapped2CrystalCalPar::ReInit() {
  
  // MOVE PAR SETTINGS IN INIT TO SETPARCONTAINERS AND CALL ALSO IT HERE
  // SetParContainers();
  
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void R3BCalifaMapped2CrystalCalPar::Exec(Option_t* opt) {
  
  Int_t nHits = fCalifaMappedDataCA->GetEntries();
  if(!nHits) return;
  
  R3BCalifaMappedData* MapHit;
  Int_t crystalId;
  
  for(Int_t i = 0; i < nHits; i++) {
    MapHit = (R3BCalifaMappedData*)(fCalifaMappedDataCA->At(i));
    crystalId = MapHit->GetCrystalId();
    //Fill Histos
    fh_Map_energy_crystal[crystalId-1]->Fill(MapHit->GetEnergy());
  }
}

// ---- Public method Reset   --------------------------------------------------
void R3BCalifaMapped2CrystalCalPar::Reset() {
  
}

void R3BCalifaMapped2CrystalCalPar::FinishEvent() {
}

// ---- Public method Finish   --------------------------------------------------
void R3BCalifaMapped2CrystalCalPar::FinishTask() {
  
  SearchPeaks();
  
  //obtain an output file
  /*FILE *fout = NULL;
    if(fOutputFile)
    {
    fout = fopen(fOutputFile, "w");
    if(!fout)
    {
    cerr << "Could not open " << fOutputFile << " for writing!\n";
    }
    else
    fprintf(fout, "# CrystalId Ratio NumEvents\n");
    }*/
  
  
}

//------------------
void R3BCalifaMapped2CrystalCalPar::SearchPeaks(){
  
  Int_t nfound=0;
  
  Int_t numPars;
  if (fNumParam){numPars=fNumParam;}
  else {numPars=2;}//by default! number of parameters=2
  
  fCal_Par->SetNumCrystals(fNumCrystals);
  fCal_Par->SetNumParametersFit(fNumParam);
  fCal_Par->GetCryCalParams()->Set(numPars*fNumCrystals);

  TSpectrum *ss= new TSpectrum(fNumPeaks); 
  
  Int_t fright, fleft; 
  
  for (Int_t i=0;i<fNumCrystals;i++){
    
    if (fh_Map_energy_crystal[i]->GetEntries()>fMinStadistics){
      
      if(fDebugMode) nfound = ss->Search(fh_Map_energy_crystal[i],fSigma,"",fThreshold);
      else  nfound = ss->Search(fh_Map_energy_crystal[i],fSigma,"goff",fThreshold);
      //	std::cout<< i << " " << nfound <<" "<< fThreshold << std::endl;
      fChannelPeaks = (Double_t*) ss->GetPositionX();

      Int_t idx[nfound];
      TMath::Sort(nfound, fChannelPeaks, idx, kTRUE);

      //Calibrated Spectrum
      Double_t X[nfound+1];
      Double_t Y[nfound+1];
      
      for (Int_t j=0;j<nfound;j++){
	X[j]=fChannelPeaks[idx[nfound-j-1]];
	Y[j]=fEnergyPeaks->GetAt(nfound-j-1);
	//std::cout<<"CrystalId="<<i+1<<" "<< j+1  <<" "<< X[j+1]  << std::endl;
      }
      X[nfound]=0.;
      Y[nfound]=0.;

      if(Searchrange(i+1)){
      fright = fMapHistos_right;
      fleft = fMapHistos_left;
      }else{
      fright = fMapHistos_rightp;
      fleft = fMapHistos_leftp;
      }     

      TF1 *f1;
      if (fNumParam){
	
	if (fNumParam==1){
	  f1 = new TF1 ("f1", "[0]*x", fleft, fright);
	}
	if (fNumParam==2){
	  f1 = new TF1 ("f1", "[0]+[1]*x", fleft, fright);
	}
	if (fNumParam==3){
	  f1 = new TF1 ("f1", "[0]+[1]*x+[2]*pow(x,2)", fleft, fright);
	}
	if (fNumParam==4){
	  f1 = new TF1 ("f1", "[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)", fleft, fright);
	}
	if (fNumParam==5){
	  f1 = new TF1 ("f1", "[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)+[4]*pow(x,4)", fleft, fright);
	}
	if (fNumParam>5){
	  LOG(WARNING)<<"R3BCalifaMapped2CrystalCalPar:: The number of fit parameters can not be higher than 5"<<FairLogger::endl;
	}
      }else{
	  LOG(INFO)<<"R3BCalifaMapped2CrystalCalPar:: No imput number of fit parameters, therefore, by default NumberParameters=2"<<FairLogger::endl;
	  f1 = new TF1 ("f1", "[0]+[1]*x", fleft, fright);
      }
      
      TGraph* graph = new TGraph (fNumPeaks+1, X, Y);
      graph->Fit("f1","Q");//Quiet mode (minimum printing)

      //if(i<5000){
      for(Int_t h=0; h<numPars;h++){
	fCal_Par->SetCryCalParams(f1->GetParameter(h),numPars*i+h);
	//Double_t par;
	//par=f1->GetParameter(h);
      }
     /* }else{
      for(Int_t h=0; h<numPars;h++){
	if(h==0)fCal_Par->SetCryCalParams(f1->GetParameter(h),numPars*i+h);
         else fCal_Par->SetCryCalParams(f1->GetParameter(h),numPars*i+h);
	//Double_t par;
	//par=f1->GetParameter(h);
      }
      }  */

      if(fDebugMode) fh_Map_energy_crystal[i]->Write();
      
    }else{
          LOG(WARNING)<<"R3BCalifaMapped2CrystalCalPar::Histogram NO Fitted number "<<i<<FairLogger::endl;
         }
  }
  
  delete ss;
  fCal_Par->setChanged();
  return;
}

bool R3BCalifaMapped2CrystalCalPar::Searchrange(Int_t val){
  // With this mapping all the code can remain unchanged
  // with the new software mapping for s444 (after 12 Feb 2019).
  // This is only valid for the s444 experiment
  // (but this is the case for all the code in this class!!)
  Int_t id_1[512]={1142, 1144, 1138, 1140, 1141, 1143, 1137, 1139,
                   1014, 1016, 1010, 1012, 1013, 1015, 1009, 1011,
                   1398, 1400, 1394, 1396, 1397, 1399, 1393, 1395,
                   1270, 1272, 1266, 1268, 1269, 1271, 1265, 1267,
                   1654, 1655, 1650, 1651, 1653, 1656, 1649, 1652,
                   1526, 1528, 1522, 1524, 1525, 1527, 1521, 1523,
                   6654, 6655, 6650, 6651, 6653, 6656, 6649, 6652,
                   6526, 6528, 6522, 6524, 6525, 6527, 6521, 6523,
                   1910, 1911, 1906, 1907, 1909, 1912, 1905, 1908,
                   1782, 1783, 1778, 1779, 1781, 1784, 1777, 1780,
                   6910, 6911, 6906, 6907, 6909, 6912, 6905, 6908,
                   6782, 6783, 6778, 6779, 6781, 6784, 6777, 6780,
                   1126, 1128, 1122, 1124, 1125, 1127, 1121, 1123,
                   998, 1000, 994, 996, 997, 999, 993, 995,
                   1382, 1384, 1378, 1380, 1381, 1383, 1377, 1379,
                   1254, 1256, 1250, 1252, 1253, 1255, 1249, 1251,
                   1638, 1639, 1634, 1635, 1637, 1640, 1633, 1636,
                   1510, 1512, 1506, 1508, 1509, 1511, 1505, 1507,
                   1894, 1895, 1890, 1891, 1893, 1896, 1889, 1892,
                   1766, 1767, 1762, 1763, 1765, 1768, 1761, 1764,
                   1102, 1104, 1098, 1100, 1101, 1103, 1097, 1099,
                   974, 976, 970, 972, 973, 975, 969, 971,
                   1358, 1360, 1354, 1356, 1357, 1359, 1353, 1355,
                   1230, 1232, 1226, 1228, 1229, 1231, 1225, 1227,
                   1614, 1615, 1610, 1611, 1613, 1616, 1609, 1612,
                   1486, 1488, 1482, 1484, 1485, 1487, 1481, 1483,
                   1870, 1871, 1866, 1867, 1869, 1872, 1865, 1868,
                   1742, 1743, 1738, 1739, 1741, 1744, 1737, 1740,
                   1094, 1096, 1090, 1092, 1093, 1095, 1089, 1091,
                   966, 968, 962, 964, 965, 967, 961, 963,
                   1350, 1352, 1346, 1348, 1349, 1351, 1345, 1347,
                   1222, 1224, 1218, 1220, 1221, 1223, 1217, 1219,
                   1606, 1607, 1602, 1603, 1605, 1608, 1601, 1604,
                   1478, 1480, 1474, 1476, 1477, 1479, 1473, 1475,
                   6606, 6607, 6602, 6603, 6605, 6608, 6601, 6604,
                   6478, 6480, 6474, 6476, 6477, 6479, 6473, 6475,
                   1862, 1863, 1858, 1859, 1861, 1864, 1857, 1860,
                   1734, 1735, 1730, 1731, 1733, 1736, 1729, 1732,
                   6862, 6863, 6858, 6859, 6861, 6864, 6857, 6860,
                   6734, 6735, 6730, 6731, 6733, 6736, 6729, 6732,
                   1078, 1080, 1074, 1076, 1077, 1079, 1073, 1075,
                   950, 952, 946, 948, 949, 951, 945, 947,
                   1334, 1336, 1330, 1332, 1333, 1335, 1329, 1331,
                   1206, 1208, 1202, 1204, 1205, 1207, 1201, 1203,
                   1590, 1591, 1586, 1587, 1589, 1592, 1585, 1588,
                   1462, 1464, 1458, 1460, 1461, 1463, 1457, 1459,
                   1846, 1847, 1842, 1843, 1845, 1848, 1841, 1844,
                   1718, 1719, 1714, 1715, 1717, 1720, 1713, 1716,
                   1070, 1072, 1066, 1068, 1069, 1071, 1065, 1067,
                   942, 944, 938, 940, 941, 943, 937, 939,
                   1326, 1328, 1322, 1324, 1325, 1327, 1321, 1323,
                   1198, 1200, 1194, 1196, 1197, 1199, 1193, 1195,
                   1582, 1583, 1578, 1579, 1581, 1584, 1577, 1580,
                   1454, 1456, 1450, 1452, 1453, 1455, 1449, 1451,
                   1838, 1839, 1834, 1835, 1837, 1840, 1833, 1836,
                   1710, 1711, 1706, 1707, 1709, 1712, 1705, 1708,
                   1062, 1064, 1058, 1060, 1061, 1063, 1057, 1059,
                   934, 936, 930, 932, 933, 935, 929, 931,
                   1318, 1320, 1314, 1316, 1317, 1319, 1313, 1315,
                   1190, 1192, 1186, 1188, 1189, 1191, 1185, 1187,
                   1574, 1575, 1570, 1571, 1573, 1576, 1569, 1572,
                   1446, 1448, 1442, 1444, 1445, 1447, 1441, 1443,
                   1830, 1831, 1826, 1827, 1829, 1832, 1825, 1828,
                   1702, 1703, 1698, 1699, 1701, 1704, 1697, 1700};
  Int_t id_2[512]={ 246, 245, 244, 243, 242, 241, 240, 247,
                    248, 255, 254, 253, 252, 251, 250, 249,
                    230, 229, 228, 227, 226, 225, 224, 231,
                    232, 239, 238, 237, 236, 235, 234, 233,
                    214, 213, 212, 211, 210, 209, 208, 215,
                    216, 223, 222, 221, 220, 219, 218, 217,
                    198, 197, 196, 195, 194, 193, 192, 199,
                    200, 207, 206, 205, 204, 203, 202, 201,
                    182, 181, 180, 179, 178, 177, 176, 183,
                    184, 191, 190, 189, 188, 187, 186, 185,
                    166, 165, 164, 163, 162, 161, 160, 167,
                    168, 175, 174, 173, 172, 171, 170, 169,
                    502, 501, 500, 499, 498, 497, 496, 503,
                    504, 511, 510, 509, 508, 507, 506, 505,
                    486, 485, 484, 483, 482, 481, 480, 487,
                    488, 495, 494, 493, 492, 491, 490, 489,
                    470, 469, 468, 467, 466, 465, 464, 471,
                    472, 479, 478, 477, 476, 475, 474, 473,
                    454, 453, 452, 451, 450, 449, 448, 455,
                    456, 463, 462, 461, 460, 459, 458, 457,
                    54, 53, 52, 51, 50, 49, 48, 55,
                    56, 63, 62, 61, 60, 59, 58, 57,
                    38, 37, 36, 35, 34, 33, 32, 39,
                    40, 47, 46, 45, 44, 43, 42, 41,
                    22, 21, 20, 19, 18, 17, 16, 23,
                    24, 31, 30, 29, 28, 27, 26, 25,
                    6, 5, 4, 3, 2, 1, 0, 7,
                    8, 15, 14, 13, 12, 11, 10, 9,
                    150, 149, 148, 147, 146, 145, 144, 151,
                    152, 159, 158, 157, 156, 155, 154, 153,
                    134, 133, 132, 131, 130, 129, 128, 135,
                    136, 143, 142, 141, 140, 139, 138, 137,
                    118, 117, 116, 115, 114, 113, 112, 119,
                    120, 127, 126, 125, 124, 123, 122, 121,
                    102, 101, 100, 99, 98, 97, 96, 103,
                    104, 111, 110, 109, 108, 107, 106, 105,
                    86, 85, 84, 83, 82, 81, 80, 87,
                    88, 95, 94, 93, 92, 91, 90, 89,
                    70, 69, 68, 67, 66, 65, 64, 71,
                    72, 79, 78, 77, 76, 75, 74, 73,
                    310, 309, 308, 307, 306, 305, 304, 311,
                    312, 319, 318, 317, 316, 315, 314, 313,
                    294, 293, 292, 291, 290, 289, 288, 295,
                    296, 303, 302, 301, 300, 299, 298, 297,
                    278, 277, 276, 275, 274, 273, 272, 279,
                    280, 287, 286, 285, 284, 283, 282, 281,
                    262, 261, 260, 259, 258, 257, 256, 263,
                    264, 271, 270, 269, 268, 267, 266, 265,
                    374, 373, 372, 371, 370, 369, 368, 375,
                    376, 383, 382, 381, 380, 379, 378, 377,
                    358, 357, 356, 355, 354, 353, 352, 359,
                    360, 367, 366, 365, 364, 363, 362, 361,
                    342, 341, 340, 339, 338, 337, 336, 343,
                    344, 351, 350, 349, 348, 347, 346, 345,
                    326, 325, 324, 323, 322, 321, 320, 327,
                    328, 335, 334, 333, 332, 331, 330, 329,
                    438, 437, 436, 435, 434, 433, 432, 439,
                    440, 447, 446, 445, 444, 443, 442, 441,
                    422, 421, 420, 419, 418, 417, 416, 423,
                    424, 431, 430, 429, 428, 427, 426, 425,
                    406, 405, 404, 403, 402, 401, 400, 407,
                    408, 415, 414, 413, 412, 411, 410, 409,
                    390, 389, 388, 387, 386, 385, 384, 391,
                    392, 399, 398, 397, 396, 395, 394, 393};
  Int_t nbcry=0;
  for(Int_t i=0; i<512;i++){
    if(id_1[i]==val)nbcry = id_2[i];
  }

  //New arrangement due to dual preamps
  if(nbcry>=0&&nbcry<64){
    nbcry=nbcry+128;//USC1: petal 3
  }
  else if(nbcry>63&&nbcry<80){
    nbcry=nbcry-64+448+32;//USC2: proton range, preamp 1, section 1
  }
  else if(nbcry>79&&nbcry<96){
    nbcry=nbcry-80+192;//USC2: gamma range, preamp 1, section 1
  }
  else if(nbcry>95&&nbcry<112){
    nbcry=nbcry-96+448+48;//USC2: proton range, preamp 1, section 2
  }
  else if(nbcry>111&&nbcry<128){
    nbcry=nbcry-112+192+16;//USC2: gamma range, preamp 1, section 2
  }
  else if(nbcry>127&&nbcry<144){
    nbcry=nbcry-128+192+32;//USC2: preamp 2, section 1
  }
  else if(nbcry>143&&nbcry<160){
    nbcry=nbcry-144+192+48;//USC2: preamp 2, section 2
  }
  else if(nbcry>159&&nbcry<176){
    nbcry=nbcry-160+448;//TUDA1: proton range, preamp 1, section 1
  }
  else if(nbcry>175&&nbcry<192){
    nbcry=nbcry-176;//TUDA1: gamma range, preamp 1, section 1
  }
  else if(nbcry>191&&nbcry<208){
    nbcry=nbcry-192+448+16;//TUDA1: proton range, preamp 1, section 2
  }
  else if(nbcry>207&&nbcry<224){
    nbcry=nbcry-208+16;//TUDA1: gamma range, preamp 1, section 2
  }
  else if(nbcry>223&&nbcry<240){
    nbcry=nbcry-224+32;//TUDA1: preamp 2, section 1
  }
  else if(nbcry>239&&nbcry<256){
    nbcry=nbcry-240+48;//TUDA1: preamp 2, section 2
  }
  else if(nbcry>447){
    nbcry=nbcry-448+64;//TUDA2: petal 2
  }


 if(nbcry<32)return kTRUE;
 else if(nbcry>191&&nbcry<224)return kTRUE;
 else if(nbcry>255&&nbcry<448)return kTRUE;
 else return kFALSE;

}

/*
  void R3BCalifaMapped2CrystalCalPar::SetOutputFile(const char *outFile)
  {
  fOutputFile = const_cast<char*>(outFile);
  }
*/
ClassImp(R3BCalifaMapped2CrystalCalPar)
