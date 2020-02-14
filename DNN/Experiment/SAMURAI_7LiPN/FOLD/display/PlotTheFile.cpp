// include required C++ headers:
#include <iostream>
#include <fstream>

// include required ROOT-headers:
#include "TFile.h"
#include "TSystem.h"
#include "TMath.h"
#include "TString.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TROOT.h"

// include own headers:
#include "DrawFuncDouma.h"
#include "DrawFuncDoumaArray.h"

// Include TObject-headers:
#include "../../../../Objects/ObjDouble.h"

// use standard namespace:
using namespace std;

// declare global variables:
TString FileName = "12cgs.plot";
TString DWHIPATH = "../dwhi_surf/Outputs/";
TString OUTPATH = "./";
Int_t nPlot = 1;
Double_t ScaleFactor = 4.0;
std::vector<Double_t> Xvalues;
std::vector<Double_t> Yvalues;

// plotting function definition:
Double_t PlotFunction(Double_t* x, Double_t* par)
{
    // interpolates the (x,y)-values vector.
    // defined to work with the ROOT TF1-class.
    
    // par defines which part of the function is used.
    
    // begin by figuring out between which elements
    // of the vector x is:
    Int_t Size = Xvalues.size();
    
    if (Size!=Yvalues.size())
    {
        cout << "### FATAL: (x,y) do not have the same size!\n\n";
        return 0.0;
    }
    else
    {
        if (Size<=1)
        {
            cout << "### FATAL: (x,y) are too small to evaluate!\n\n";
            return 0.0;
        }
        else
        {
            Int_t Counter = 0;
            Int_t Index = -1;
            
            // loop over the vectors:
            for (Int_t k = 0; k<(Size-1); ++k)
            {
                // Find where we are in between:
                if ((x[0]>=Xvalues.at(k))&&(x[0]<=Xvalues.at(k+1)))
                {
                    // Now use the parameter:
                    if (Counter<par[0])
                    {
                        // write to the index:
                        Index = k;
                        
                        // And update the counter:
                        Counter = Counter + 1;
                        
                        // This way, if par[0]==1, the first interval where x is,
                        // is marked & kept. But if par[0]==2, this is overwritten 
                        // by the second case, etc.
                    }
                }
            }
            
            if (Index==-1) 
            {
                // then we could not find a suitable value:
                return 0.0;
            }
            
            // Now we can interpolate:
            Double_t Answer = Yvalues.at(Index) + (Yvalues.at(Index+1)-Yvalues.at(Index))*((x[0]-Xvalues.at(Index))/(Xvalues.at(Index+1)-Xvalues.at(Index)));
            
            // then return the answer:
            return Answer;
        }
    }
}

// Main function definition:
void PlotTheFile()
{
    // Suppress pictures coming up:
    gROOT->SetBatch(kTRUE);
    
    // begin by opening the file and connecting to it.
    // The purpose is to make a plot of the plot-file.
    TString FullName = DWHIPATH + FileName;
    ifstream PlotFile;
    PlotFile.open(FullName.Data());
    
    // check if it worked:
    if (!(PlotFile.is_open()))
    {
        cout << "### FATAL: The plot-file <" << FullName.Data() << "> could not be found!\n\n";
    }
    else
    {
        // Now that we know that the file is there, start with
        // Determining the total number of lines:
        Int_t NLines = 0;
        std::string ThisLine;
        while (std::getline(PlotFile,ThisLine)) {NLines = NLines + 1;}
        
        // Re-open the file, to start from the beginning again:
        PlotFile.close();
        PlotFile.open(FullName.Data());
        
        // Now declare the numbers that we need:
        Double_t ThisX;
        Double_t ThisY;
        
        // clear the global vectors:
        Xvalues.clear();
        Yvalues.clear();
        
        // Loop over the file:
        for (Int_t k = 2; k<NLines; ++k)
        {
            // Get the numbers from the line:
            PlotFile >> ThisX;
            PlotFile >> ThisY;
            
            // Get the new line:
            std::getline(PlotFile,ThisLine);
            
            // Add it to the global vectors:
            Xvalues.push_back(ThisX);
            Yvalues.push_back(ThisY);
            
            // Done. Close the blocks:
        }
        
        // And the file:
        PlotFile.close();
        
        // next, we must create the plots:
        Int_t Size = Xvalues.size();
        
        if ((Size>1)&&(Size==Yvalues.size()))
        {
            // Create the plotting function:
            Int_t nFunc = nPlot;
            TString st = "";
            TString kstr = "";
            Double_t par0;
            
            if (nFunc>0)
            {
                TF1** PlotFunc = new TF1*[nFunc];
                TString* Legenda = new TString[nFunc];
                
                for (Int_t k = 0 ; k<nFunc; ++k)
                {
                    kstr = st.Itoa(k,10);
                    PlotFunc[k] = new TF1("PlotFunction_"+kstr,PlotFunction,Xvalues.at(0),Xvalues.at(Size-1),1);
                    par0 = (Int_t) k;
                    par0 = par0 + 0.9;
                    PlotFunc[k]->SetParameter(0,par0);
                    Legenda[k] = "Output Function "+kstr;
                }
            
                // Create the plot:
                Double_t Res_d = (Int_t) Size;
                Res_d = Res_d - 1.0;
                Res_d = Res_d/((Int_t) nFunc);
                Res_d = Res_d*10;
                Int_t Npx = (Int_t) Res_d;
                TCanvas* c1 = DrawFuncDoumaArray(nFunc,PlotFunc,Legenda,"Result of the Fold-package DWBA calculation","Scattering Angle [deg]","Differential Cross Section [mb/sr]",Npx,ScaleFactor,kTRUE);
                
                // then save & close:
                FileName.ReplaceAll(".plot",".png");
                c1->SaveAs(OUTPATH+"/picture/"+FileName);
                c1->Close();
                
                // Next, we must save the results:
                TH1D** PlotHist = new TH1D*[nFunc];
                FileName.ReplaceAll(".png",".root");
                TFile* f = new TFile(OUTPATH+"/ROOT/"+FileName,"recreate");
                f->cd();
                
                for (Int_t k = 0; k<nFunc; ++k)
                {
                    kstr = st.Itoa(k,10);
                    PlotHist[k] = (TH1D*) PlotFunc[k]->GetHistogram();
                    PlotHist[k]->Write("DWBA_Histogram_"+kstr,2);
                    PlotFunc[k]->Write("DWBA_Function_"+kstr,2);
                }
                
                ObjDouble* ZeroPoint = new ObjDouble("Theoretical_ZeroPoint");
                ZeroPoint->SetNumber(Yvalues.at(0));
                ZeroPoint->WriteToFile(f);
                
                // Close up:
                f->Close();
                
                delete f;
                delete c1;
                delete PlotFunc;
                delete PlotHist;
                delete ZeroPoint;
            
                // Close all function blocks:
            }
        }
    }
}

// Define the main for stand-alone compilation:
#ifndef __CINT__
#include <TApplication.h>
int main(int argc, char** argv)
{
  // Extract inputs. The first input is the filename and the second is
  // the number of plotting functions that you want:
  TString Number = argv[2];
  FileName = argv[1];
  
  // Test if this is the case:
  if (!Number.IsDigit())
  {
      cout << "### ERROR: The second input argument should be a number!\n"
           << "### ERROR: It should be the number of plots that you want.\n\n";
  }
  else
  {
      // Check if the file exists:
      FileStat_t stom;
      Int_t test = gSystem->GetPathInfo(DWHIPATH+FileName,stom);
      
      if (test!=0)
      {
          cout << "### ERROR: The first input argument should be an existing!\n"
               << "### ERROR: The .plot-file in " << DWHIPATH << " is not there!\n\n";
      }
      else
      {
          // Now we can safely run the program:

          // Process the number of plotting functions:
          nPlot = Number.Atoi();

          // And execute:
          TApplication* TheApp = new TApplication("PlotTheFile",&argc,argv);
          PlotTheFile();
          // TheApp->Run();
         
          // Close all blocks:
      }
  }
  
  // End main function:
  return 0;
}
#endif
