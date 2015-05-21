#include <iostream>
#include <TSystem.h>
#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>

#include "R3BMacroCompiler.h"

using namespace std;

#define XSTR(_d) #_d
#define STR(_d) XSTR(_d)

void R3BMacroCompiler::SetIncludeDirectories()
{
  TString dirstr(STR(INCLUDE_DIRECTORIES));
  TObjArray *dirs = dirstr.Tokenize(";");

  for(int i = 0; i < dirs->GetEntries(); i++)
  {
    TObjString *dir = dynamic_cast<TObjString*>(dirs->At(i));
    if(dir)
    {
      gSystem->AddIncludePath("-I" + dir->GetString());
      cout << "  " << dir->GetString() << endl;
    }
  }

  delete dirs;
}
