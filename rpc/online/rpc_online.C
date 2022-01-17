
#include <iostream>
#include <string>
#include <unistd.h>

typedef struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t  unpack;
    EXT_STR_h101_RPC rpc;

} EXT_STR_h101;



 void rpc_online(){


   FairLogger::GetLogger()->SetLogScreenLevel("INFO");
   FairLogger::GetLogger()->SetColoredLog(true);
   FairLogger::GetLogger()->SetLogVerbosityLevel("VERYHIGH");


   TStopwatch timer;

   auto t = std::time(nullptr);
   auto tm = *std::localtime(&t);
   std::ostringstream oss;
   oss << std::put_time(&tm, "%Y%m%d_%H%M%S");

   const Int_t nev = -1; // number of events to read, -1 - until CTRL+C

   TString filename = "/home/manuel/R3Broot_dev/upexps/R3B_odroid/new_data/r3b22010162329.lmd";

   TString outputFileName = "output.root";


   // Online server configuration --------------------------
   Int_t refresh = 1; // Refresh rate for online histograms
   Int_t port = 8888; // Port number for the online visualization, example lxgXXXX:8887


   // UCESB configuration ----------------------------------
   TString ntuple_options = "RAW";
   TString ucesb_dir = getenv("UCESB_DIR");
   TString upexps_dir = ucesb_dir + "../upexps";

   TString ucesb_path;

       ucesb_path =upexps_dir + "/202205_s522/202205_s522 --allow-errors --input-buffer=70Mi";

   ucesb_path.ReplaceAll("//", "/");

   // Load ucesb structure ---------------------------------
   EXT_STR_h101 ucesb_struct;

   // Create online run ------------------------------------
   FairRunOnline* run = new FairRunOnline();
   R3BEventHeader* EvntHeader = new R3BEventHeader();
   run->SetEventHeader(EvntHeader);
   run->SetRunId(1);
   run->SetSink(new FairRootFileSink(outputFileName));
   run->ActivateHttpServer(refresh, port);

   // Create source using ucesb for input ------------------
   R3BUcesbSource* source =
       new R3BUcesbSource(filename, ntuple_options, ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
   source->SetMaxEvents(nev);

   // Definition of reader ---------------------------------
   R3BUnpackReader* unpackreader =
       new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack));

    R3BRpcReader* unpackRPC =
       new R3BRpcReader((EXT_STR_h101_RPC*)&ucesb_struct.rpc, offsetof(EXT_STR_h101,rpc));


      source->AddReader(unpackreader);
      // unpackreader->SetOnline(kFALSE);

      source->AddReader(unpackRPC);
      unpackRPC->SetOnline(kFALSE);

      run->SetSource(source);

       R3BRpcOnlineSpectra * rpcOnline = new R3BRpcOnlineSpectra();
       run->AddTask(rpcOnline);

   timer.Start();

   // Initialize -------------------------------------------
   run->Init();

   // Informations about portnumber and main data stream.
   std::cout << "\n\n" << std::endl;
   std::cout << "Data stream is: " << filename << std::endl;
   std::cout << "Portnumber for Rpc online is: " << port << std::endl;
   std::cout << "\n\n" << std::endl;

   // Run --------------------------------------------------
   run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);

   // -----   Finish   -------------------------------------
   std::cout << std::endl << std::endl;


   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();

   std::cout << "Output file is " << outputFileName << std::endl;
   std::cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << std::endl << std::endl;
   Float_t cpuUsage = ctime / rtime;
   std::cout << "CPU used: " << cpuUsage << std::endl;
   std::cout << "Macro finished successfully." << std::endl;
   std::cout << std::endl;
   gApplication->Terminate();

}
