#include "./NuclearData/Sn122.h"
#include "./NuclearData/Sn116.h"
#include "./NuclearData/Sb122.h"
#include "./NuclearData/Sb116.h"

#include "./NuclearData/C13.h"
#include "./NuclearData/C12.h"
#include "./NuclearData/N13.h"
#include "./NuclearData/N12.h"

#include "./NuclearData/Mg24.h"
#include "./NuclearData/Mg25.h"
#include "./NuclearData/Mg26.h"
#include "./NuclearData/Al24.h"
#include "./NuclearData/Al25.h"
#include "./NuclearData/Al26.h"

#include "./NuclearData/Cd106.h"
#include "./NuclearData/Cd108.h"
#include "./NuclearData/Cd110.h"
#include "./NuclearData/Cd111.h"
#include "./NuclearData/Cd112.h"
#include "./NuclearData/Cd113.h"
#include "./NuclearData/Cd114.h"
#include "./NuclearData/Cd116.h"

#include "./NuclearData/In106.h"
#include "./NuclearData/In108.h"
#include "./NuclearData/In110.h"
#include "./NuclearData/In111.h"
#include "./NuclearData/In112.h"
#include "./NuclearData/In113.h"
#include "./NuclearData/In114.h"
#include "./NuclearData/In116.h"

#include "./NuclearData/Te126.h"
#include "./NuclearData/I126.h"
#include "./NuclearData/He3.h"
#include "./NuclearData/H3.h"
#include "./NuclearData/Au197.h"
#include "./NuclearData/Hg197.h"
#include "./NuclearData/Al27.h"
#include "./NuclearData/Si27.h"
#include "./NuclearData/Fe56.h"

#include "./NuclearData/Zr90.h"
#include "./NuclearData/Nb90.h"
#include "./NuclearData/Sn118.h"
#include "./NuclearData/Sb118.h"

#include "./NuclearData/proton.h"
#include "./NuclearData/neutron.h"
#include "./NuclearData/electron.h"
#include "./NuclearData/pionplus.h"
#include "./NuclearData/muon.h"

#include "./NuclearData/pion0.h"
#include "./NuclearData/gamma.h"

#include "./NuclearData/antiproton.h"
#include "./NuclearData/antineutron.h"
#include "./NuclearData/positron.h"
#include "./NuclearData/pionminus.h"
#include "./NuclearData/antimuon.h"

#include "./NuclearData/Ca48.h"
#include "./NuclearData/Pb208.h"

#include "./NuclearData/Sb132.h"
#include "./NuclearData/In132.h"
#include "./NuclearData/Sn132.h"
#include "./NuclearData/Sn131.h"
#include "./NuclearData/Sn130.h"
#include "./NuclearData/Sn129.h"
#include "./NuclearData/Sn128.h"
#include "./NuclearData/Sn127.h"
#include "./NuclearData/Sn126.h"

#include "./NuclearData/Li7.h"
#include "./NuclearData/Li3.h"
#include "./NuclearData/Be8.h"
#include "./NuclearData/Be7.h"
#include "./NuclearData/Be6.h"
#include "./NuclearData/Be5.h"
#include "./NuclearData/Be4.h"

void Nuclei::BuildDataBase()
{
    // Clean the previous data:
    TheNuclei.clear();
    
    // Then add all nuclei that we want to our DataBase:
    TheNuclei.push_back(Sn122());
    TheNuclei.push_back(Sn116());
    TheNuclei.push_back(Sb122());
    TheNuclei.push_back(Sb116());
    
    TheNuclei.push_back(C13());
    TheNuclei.push_back(N13());
    TheNuclei.push_back(C12());
    TheNuclei.push_back(N12());
    
    TheNuclei.push_back(Mg24());
    TheNuclei.push_back(Mg25());
    TheNuclei.push_back(Mg26());
    TheNuclei.push_back(Al24());
    TheNuclei.push_back(Al25());
    TheNuclei.push_back(Al26());
    
    TheNuclei.push_back(Cd106());
    TheNuclei.push_back(Cd108());
    TheNuclei.push_back(Cd110());
    TheNuclei.push_back(Cd111());
    TheNuclei.push_back(Cd112());
    TheNuclei.push_back(Cd113());
    TheNuclei.push_back(Cd114());
    TheNuclei.push_back(Cd116());
    
    TheNuclei.push_back(In106());
    TheNuclei.push_back(In108());
    TheNuclei.push_back(In110());
    TheNuclei.push_back(In111());
    TheNuclei.push_back(In112());
    TheNuclei.push_back(In113());
    TheNuclei.push_back(In114());
    TheNuclei.push_back(In116());
    
    TheNuclei.push_back(Te126());
    TheNuclei.push_back(I126());
    TheNuclei.push_back(He3());
    TheNuclei.push_back(H3());
    TheNuclei.push_back(Au197());
    TheNuclei.push_back(Hg197());
    TheNuclei.push_back(Al27());
    TheNuclei.push_back(Si27());
    TheNuclei.push_back(Fe56());
    
    TheNuclei.push_back(Zr90());
    TheNuclei.push_back(Nb90());
    TheNuclei.push_back(Sn118());
    TheNuclei.push_back(Sb118());
    
    TheNuclei.push_back(proton());
    TheNuclei.push_back(neutron());
    TheNuclei.push_back(electron());
    TheNuclei.push_back(pionplus());
    TheNuclei.push_back(muon());
    
    TheNuclei.push_back(pion0());
    TheNuclei.push_back(gamma());
    
    TheNuclei.push_back(antiproton());
    TheNuclei.push_back(antineutron());
    TheNuclei.push_back(positron());
    TheNuclei.push_back(pionminus());
    TheNuclei.push_back(antimuon());
    
    TheNuclei.push_back(Ca48());
    TheNuclei.push_back(Pb208());
    
    TheNuclei.push_back(Sb132());
    TheNuclei.push_back(In132());
    TheNuclei.push_back(Sn132());
    TheNuclei.push_back(Sn131());
    TheNuclei.push_back(Sn130());
    TheNuclei.push_back(Sn129());
    TheNuclei.push_back(Sn128());
    TheNuclei.push_back(Sn127());
    TheNuclei.push_back(Sn126());
    
    TheNuclei.push_back(Li7());
    TheNuclei.push_back(Li3());
    TheNuclei.push_back(Be8());
    TheNuclei.push_back(Be7());
    TheNuclei.push_back(Be6());
    TheNuclei.push_back(Be5());
    TheNuclei.push_back(Be4());
    
    // Sort the energy levels:
    for (Int_t k = 0; k<TheNuclei.size(); ++k)
    {
        TheNuclei.at(k).SortEnergyLevels();
    }
    
    // build nuclear PDG codes:
    for (Int_t k = 0; k<TheNuclei.size(); ++k)
    {
        // NOTE: Function is only executed for A>1 (internally programmed):
        TheNuclei.at(k).ComputePDGCode();
    }
    
    // Done!
}
