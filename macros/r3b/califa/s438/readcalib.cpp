//#Total-Energy(MeV)  Deposited-Energy(MeV)  Light-Output(Gamma-MeV)  Ratio  Nf(Gamma-Mev)  Ns(Gamma-MeV)  Ratio(delta electrons)
//1	1	1.3517	1.3517	1.04821	0.303489	0

#include <vector>
#include <map>
#include <fstream>

#include <TString.h>

using namespace std;

typedef struct sLO
{
  double E;
  double dE;
  double L;
  double Q;

  sLO(double E, double dE, double L) : E(E), dE(dE), L(L)
  {
    Q = L/E;
  }
} lo_t;

typedef vector<lo_t> lo_list_t;
typedef map<int,lo_list_t> lo_map_t;

lo_map_t lo_map;

void readLO(const int region, const char *filename)
{
  ifstream fin(filename);

  if(!fin.good())
  {
    cerr << "Could not open calibration file " << filename << " for reading." << endl;
    return;
  }

  cout << "Reading " << filename << "..." << endl;

  lo_list_t &list = lo_map[region];

  // Skip first line
  char linebuf[256];
  fin.getline(linebuf, 256);
  double E, dE, L, Q, Nf, Ns, R;
  while(true)
  {
    fin >> E >> dE >> L >> Q >> Nf >> Ns >> R;
    if(!fin.good())
      break;

    list.push_back(lo_t(E, dE, L));
  }

  cout << "... read " << list.size() << " calibration values" << endl;
}

void readAllLO()
{
  for(int i = 0; i < 3; i++)
    readLO(i, Form("pcalib-r%d.txt", i));
}

double GetE(int region, double L, bool stopped)
{
  double E_l = 0, E_h = 0, L_l = 0, L_h = 0, Q;

  lo_list_t &list = lo_map[region];

  if(stopped)
  {
    for(lo_list_t::iterator i = list.begin(); i != list.end(); i++)
    {
      if(i->dE < i->E)
      {
//        cerr << "GetE(" << region << ", " << L << ", true) failed" << endl;
        break; // punch through
      }

      if(i->L > L)
      {
        E_h = i->E;
        L_h = i->L;
        break;
      }
      Q = i->Q;
      E_l = i->E;
      L_l = i->L;
    }
  }
  else
  {
    // Punched through proton: Iterate over list in reverse order
    for(lo_list_t::reverse_iterator i = list.rbegin(); i != list.rend(); i++)
    {
      if(i->dE >= i->E)
      {
//        cerr << "GetE(" << region << ", " << L << ", false) failed" << endl;
        break;  // stopped
      }

      if(i->L > L)
      {
        E_h = i->dE;
        L_h = i->L;
        break;
      }
      Q = i->Q;
      E_l = i->dE;
      L_l = i->L;
    }
  }

  if(L_l == 0) // L is smaller than first L in list
    return E_h;
  if(L_h == 0) // L is bigger than last value in list
    return L/Q;
  else // linearly interpolate
    return E_l + (L - L_l)*(E_h - E_l)/(L_h - L_l);
}

