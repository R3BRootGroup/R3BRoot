#include <unistd.h>

#include "../../../../build/lib/libtimestitcher.so"

void Stitch(const char *tree1_fNamePattern=NULL, int tree1_first=0, int tree1_last=0, const char *tree2_fName=NULL, const char *merged_treePattern=NULL, const char *merged_chainName = NULL)
{
   // Uhm... I should try to find a more elegent way some time...
   //gSystem->Load("../../../../build/lib/libtimestitcher.so");
   gSystem->CompileMacro("../../../../timestitcher/Timestitch.C", "k,O");

   char *cwd = getcwd(NULL, 0);

   TChain *final = NULL;
   if(merged_chainName)
	   final = new TChain("merged");

   for(int i = tree1_first; i <= tree1_last; i++)
   {
      TString tree1_fname = Form(tree1_fNamePattern, i);
      TString merged_fname = Form(merged_treePattern, i);

      Timestitch(tree1_fname + " " + TString(tree2_fName), merged_fname);

      // Use absolute pathes for TChain
      if(final)
	final->Add(TString(cwd) + "/" + merged_fname);
   }

   free(cwd);
   if(final)
   {
      TFile *fo = TFile::Open(merged_chainName, "RECREATE");
      fo->cd();
      final->Write();
      fo->Close();
   }
}
