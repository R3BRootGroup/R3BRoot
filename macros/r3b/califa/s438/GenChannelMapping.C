/****************************************
 * Generate channel mapping
 * (Febex channel id to crystal ID)
 * for s438b experiment
 ****************************************/

#include <iostream>
#include <fstream>

using namespace std;

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

void GenChannelMapping(const char *fname = "s438b_califa.map")
{
  ofstream fmap(fname);

  int crystalId;

  for(int i = 0; i < 128; i++)
  {
    crystalId = fbxPetalCrystalIdBase[i%64] + 4*ptlAlveoliCopyBase[i/64];

    fmap << i << " " << crystalId << endl;
    cout << i << " " << crystalId << endl;
  }

  fmap.close();
}

