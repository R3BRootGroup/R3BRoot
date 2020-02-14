#include "DrawGraphDoumaArray.h"

void MakeMultGraphs_1000MeV()
{
    // NOTE: 1000 MeV:
    
    // Begin by creating the multiplicity graphs:
    DNNDataGraph* DNN_1n = new DNNDataGraph(); DNNDataGraph* DNN_Res_1n = new DNNDataGraph(); DNNDataGraph* TDR_1n = new DNNDataGraph(); DNNDataGraph* TDR_Res_1n = new DNNDataGraph(); DNNDataGraph* Perfect_1n = new DNNDataGraph();
    DNNDataGraph* DNN_2n = new DNNDataGraph(); DNNDataGraph* DNN_Res_2n = new DNNDataGraph(); DNNDataGraph* TDR_2n = new DNNDataGraph(); DNNDataGraph* TDR_Res_2n = new DNNDataGraph(); DNNDataGraph* Perfect_2n = new DNNDataGraph();
    DNNDataGraph* DNN_3n = new DNNDataGraph(); DNNDataGraph* DNN_Res_3n = new DNNDataGraph(); DNNDataGraph* TDR_3n = new DNNDataGraph(); DNNDataGraph* TDR_Res_3n = new DNNDataGraph(); DNNDataGraph* Perfect_3n = new DNNDataGraph();
    DNNDataGraph* DNN_4n = new DNNDataGraph(); DNNDataGraph* DNN_Res_4n = new DNNDataGraph(); DNNDataGraph* TDR_4n = new DNNDataGraph(); DNNDataGraph* TDR_Res_4n = new DNNDataGraph(); DNNDataGraph* Perfect_4n = new DNNDataGraph();
    DNNDataGraph* DNN_5n = new DNNDataGraph(); DNNDataGraph* DNN_Res_5n = new DNNDataGraph(); DNNDataGraph* TDR_5n = new DNNDataGraph(); DNNDataGraph* TDR_Res_5n = new DNNDataGraph(); DNNDataGraph* Perfect_5n = new DNNDataGraph();
    
    Double_t PerfectHeight = 142.0;
    Double_t TDRHeight = 115.0;
    Double_t DNNHeight = 130.0;
    Double_t SqrtN = 0.1;
    
    DNN_1n->SetErrorHeight(DNNHeight); DNN_Res_1n->SetErrorHeight(DNNHeight); TDR_1n->SetErrorHeight(TDRHeight); TDR_Res_1n->SetErrorHeight(TDRHeight); Perfect_1n->SetErrorHeight(PerfectHeight);
    DNN_2n->SetErrorHeight(DNNHeight); DNN_Res_2n->SetErrorHeight(DNNHeight); TDR_2n->SetErrorHeight(TDRHeight); TDR_Res_2n->SetErrorHeight(TDRHeight); Perfect_2n->SetErrorHeight(PerfectHeight);
    DNN_3n->SetErrorHeight(DNNHeight); DNN_Res_3n->SetErrorHeight(DNNHeight); TDR_3n->SetErrorHeight(TDRHeight); TDR_Res_3n->SetErrorHeight(TDRHeight); Perfect_3n->SetErrorHeight(PerfectHeight);
    DNN_4n->SetErrorHeight(DNNHeight); DNN_Res_4n->SetErrorHeight(DNNHeight); TDR_4n->SetErrorHeight(TDRHeight); TDR_Res_4n->SetErrorHeight(TDRHeight); Perfect_4n->SetErrorHeight(PerfectHeight);
    DNN_5n->SetErrorHeight(DNNHeight); DNN_Res_5n->SetErrorHeight(DNNHeight); TDR_5n->SetErrorHeight(TDRHeight); TDR_Res_5n->SetErrorHeight(TDRHeight); Perfect_5n->SetErrorHeight(PerfectHeight);
    
    // ------------------------------------------------------------
    
    // Then, add the 8dp scenario:
    DNN_1n->AddYpoint(0,60.0,59.6,58.6,59.6,58.3); DNN_Res_1n->AddYpoint(0,56.0,55.7,54.8,55.7,54.4); TDR_1n->AddYpoint(0,36.6,37.1,36.0,37.7,37.5); TDR_Res_1n->AddYpoint(0,35.1,35.6,34.5,36.2,35.8); Perfect_1n->AddYpoint(0,65.8,65.7,64.9,65.8,65.1);
    DNN_2n->AddYpoint(0,39.3,38.3,39.0,37.0,37.1); DNN_Res_2n->AddYpoint(0,25.6,24.8,24.8,24.0,23.2); TDR_2n->AddYpoint(0,34.7,34.4,34.4,34.3,34.2); TDR_Res_2n->AddYpoint(0,19.2,19.1,18.8,19.3,18.8); Perfect_2n->AddYpoint(0,44.0,43.8,43.4,43.9,43.1);
    DNN_3n->AddYpoint(0,22.1,23.8,22.5,25.9,26.7); DNN_Res_3n->AddYpoint(0,10.5,11.3,10.3,12.5,12.1); TDR_3n->AddYpoint(0,23.8,23.6,24.0,23.4,23.5); TDR_Res_3n->AddYpoint(0,10.7,10.8,10.7,10.9,10.7); Perfect_3n->AddYpoint(0,29.5,29.6,29.5,29.7,29.4);
    DNN_4n->AddYpoint(0,24.5,21.6,25.5,18.6,20.1); DNN_Res_4n->AddYpoint(0,8.8,7.5,8.8,6.6,6.7);      TDR_4n->AddYpoint(0,16.8,16.6,16.9,16.3,16.6); TDR_Res_4n->AddYpoint(0,6.1,5.9,6.0,5.9,6.0);      Perfect_4n->AddYpoint(0,19.9,19.7,19.8,19.8,19.9);
    DNN_5n->AddYpoint(0,22.2,25.1,24.1,25.5,29.1); DNN_Res_5n->AddYpoint(0,7.5,8.1,8.0,8.2,9.1);      TDR_5n->AddYpoint(0,22.4,22.2,23.1,21.6,22.4); TDR_Res_5n->AddYpoint(0,6.5,6.4,6.7,6.3,6.6);      Perfect_5n->AddYpoint(0,13.3,13.3,13.4,13.4,13.5);
    
    DNN_1n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_1n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_1n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_1n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_1n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_2n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_2n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_2n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_2n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_2n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_3n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_3n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_3n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_3n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_3n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_4n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_4n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_4n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_4n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_4n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_5n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_5n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_5n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_5n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_5n->AddYerror(0,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    
    DNN_1n->AddXpoint(0,8.0); DNN_Res_1n->AddXpoint(0,8.0); TDR_1n->AddXpoint(0,8.0); TDR_Res_1n->AddXpoint(0,8.0); Perfect_1n->AddXpoint(0,8.0);
    DNN_2n->AddXpoint(0,8.0); DNN_Res_2n->AddXpoint(0,8.0); TDR_2n->AddXpoint(0,8.0); TDR_Res_2n->AddXpoint(0,8.0); Perfect_2n->AddXpoint(0,8.0);
    DNN_3n->AddXpoint(0,8.0); DNN_Res_3n->AddXpoint(0,8.0); TDR_3n->AddXpoint(0,8.0); TDR_Res_3n->AddXpoint(0,8.0); Perfect_3n->AddXpoint(0,8.0);
    DNN_4n->AddXpoint(0,8.0); DNN_Res_4n->AddXpoint(0,8.0); TDR_4n->AddXpoint(0,8.0); TDR_Res_4n->AddXpoint(0,8.0); Perfect_4n->AddXpoint(0,8.0);
    DNN_5n->AddXpoint(0,8.0); DNN_Res_5n->AddXpoint(0,8.0); TDR_5n->AddXpoint(0,8.0); TDR_Res_5n->AddXpoint(0,8.0); Perfect_5n->AddXpoint(0,8.0);
    
    // ------------------------------------------------------------
    
    // Then, add the 16dp scenario:
    DNN_1n->AddYpoint(1,76.9,76.4,76.5,78.2,77.2); DNN_Res_1n->AddYpoint(1,75.4,75.0,75.0,76.7,75.6); TDR_1n->AddYpoint(1,56.7,57.4,55.5,61.1,59.7); TDR_Res_1n->AddYpoint(1,56.0,56.7,54.7,60.4,58.9); Perfect_1n->AddYpoint(1,87.9,87.9,87.5,88.1,87.5);
    DNN_2n->AddYpoint(1,57.4,54.8,56.9,54.0,53.7); DNN_Res_2n->AddYpoint(1,51.7,49.0,51.0,48.9,48.2); TDR_2n->AddYpoint(1,47.0,47.7,46.6,47.0,46.8); TDR_Res_2n->AddYpoint(1,40.9,41.8,40.1,41.8,41.3); Perfect_2n->AddYpoint(1,77.2,77.4,76.8,77.3,76.7);
    DNN_3n->AddYpoint(1,37.7,41.5,38.2,39.1,39.2); DNN_Res_3n->AddYpoint(1,31.2,33.4,31.3,32.1,31.5); TDR_3n->AddYpoint(1,38.3,37.8,38.6,37.6,37.6); TDR_Res_3n->AddYpoint(1,31.1,30.9,30.7,31.0,30.6); Perfect_3n->AddYpoint(1,67.6,67.6,67.4,67.4,67.1);
    DNN_4n->AddYpoint(1,35.6,33.5,35.6,31.2,31.7); DNN_Res_4n->AddYpoint(1,25.9,23.9,25.3,22.3,21.9); TDR_4n->AddYpoint(1,30.0,29.7,30.7,29.0,30.0); TDR_Res_4n->AddYpoint(1,22.4,22.4,22.2,22.1,22.3); Perfect_4n->AddYpoint(1,58.7,58.8,58.0,58.6,58.3);
    DNN_5n->AddYpoint(1,46.5,50.9,49.8,50.8,56.0); DNN_Res_5n->AddYpoint(1,32.3,34.6,34.1,34.4,37.0); TDR_5n->AddYpoint(1,40.9,39.5,44.7,37.5,41.0); TDR_Res_5n->AddYpoint(1,27.9,27.0,30.2,25.8,27.9); Perfect_5n->AddYpoint(1,50.3,50.2,50.2,50.3,50.0);
    
    DNN_1n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_1n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_1n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_1n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_1n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_2n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_2n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_2n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_2n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_2n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_3n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_3n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_3n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_3n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_3n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_4n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_4n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_4n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_4n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_4n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_5n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_5n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_5n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_5n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_5n->AddYerror(1,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    
    DNN_1n->AddXpoint(1,16.0); DNN_Res_1n->AddXpoint(1,16.0); TDR_1n->AddXpoint(1,16.0); TDR_Res_1n->AddXpoint(1,16.0); Perfect_1n->AddXpoint(1,16.0);
    DNN_2n->AddXpoint(1,16.0); DNN_Res_2n->AddXpoint(1,16.0); TDR_2n->AddXpoint(1,16.0); TDR_Res_2n->AddXpoint(1,16.0); Perfect_2n->AddXpoint(1,16.0);
    DNN_3n->AddXpoint(1,16.0); DNN_Res_3n->AddXpoint(1,16.0); TDR_3n->AddXpoint(1,16.0); TDR_Res_3n->AddXpoint(1,16.0); Perfect_3n->AddXpoint(1,16.0);
    DNN_4n->AddXpoint(1,16.0); DNN_Res_4n->AddXpoint(1,16.0); TDR_4n->AddXpoint(1,16.0); TDR_Res_4n->AddXpoint(1,16.0); Perfect_4n->AddXpoint(1,16.0);
    DNN_5n->AddXpoint(1,16.0); DNN_Res_5n->AddXpoint(1,16.0); TDR_5n->AddXpoint(1,16.0); TDR_Res_5n->AddXpoint(1,16.0); Perfect_5n->AddXpoint(1,16.0);
    
    // ------------------------------------------------------------
    
    // Then, add the 30dp scenario:
    DNN_1n->AddYpoint(2,89.5,88.9,87.6,91.8,89.7); DNN_Res_1n->AddYpoint(2,89.2,88.6,87.2,91.4,89.3); TDR_1n->AddYpoint(2,67.1,75.4,53.4,80.9,78.0); TDR_Res_1n->AddYpoint(2,66.9,75.2,53.2,80.8,77.8); Perfect_1n->AddYpoint(2,97.2,97.2,97.3,97.3,97.2);
    DNN_2n->AddYpoint(2,70.9,71.9,67.1,74.1,73.9); DNN_Res_2n->AddYpoint(2,69.8,70.7,65.9,73.2,72.8); TDR_2n->AddYpoint(2,64.1,65.2,49.7,66.5,65.2); TDR_Res_2n->AddYpoint(2,62.4,64.0,47.4,65.7,64.2); Perfect_2n->AddYpoint(2,94.1,94.0,93.9,94.0,94.0);
    DNN_3n->AddYpoint(2,64.0,59.0,61.2,60.2,63.7); DNN_Res_3n->AddYpoint(2,61.0,56.3,57.6,58.0,60.7); TDR_3n->AddYpoint(2,57.4,57.4,46.8,52.4,56.7); TDR_Res_3n->AddYpoint(2,54.4,54.7,42.7,50.9,53.9); Perfect_3n->AddYpoint(2,89.7,89.9,89.5,89.9,89.6);
    DNN_4n->AddYpoint(2,52.3,53.4,50.5,48.9,54.7); DNN_Res_4n->AddYpoint(2,47.7,48.2,44.9,45.2,49.7); TDR_4n->AddYpoint(2,48.8,48.6,43.3,40.8,49.2); TDR_Res_4n->AddYpoint(2,44.3,44.2,37.3,37.6,44.4); Perfect_4n->AddYpoint(2,84.7,84.4,84.2,84.5,84.1);
    DNN_5n->AddYpoint(2,66.1,69.1,75.6,49.2,65.7); DNN_Res_5n->AddYpoint(2,56.6,58.8,63.4,42.7,55.9); TDR_5n->AddYpoint(2,58.0,62.3,81.0,43.4,68.8); TDR_Res_5n->AddYpoint(2,49.4,53.0,66.9,37.1,57.9); Perfect_5n->AddYpoint(2,78.2,78.2,77.7,78.2,77.8);
    
    DNN_1n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_1n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_1n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_1n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_1n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_2n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_2n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_2n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_2n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_2n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_3n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_3n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_3n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_3n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_3n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_4n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_4n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_4n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_4n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_4n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    DNN_5n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); DNN_Res_5n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_5n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); TDR_Res_5n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN); Perfect_5n->AddYerror(2,SqrtN,SqrtN,SqrtN,SqrtN,SqrtN);
    
    DNN_1n->AddXpoint(2,30.0); DNN_Res_1n->AddXpoint(2,30.0); TDR_1n->AddXpoint(2,30.0); TDR_Res_1n->AddXpoint(2,30.0); Perfect_1n->AddXpoint(2,30.0);
    DNN_2n->AddXpoint(2,30.0); DNN_Res_2n->AddXpoint(2,30.0); TDR_2n->AddXpoint(2,30.0); TDR_Res_2n->AddXpoint(2,30.0); Perfect_2n->AddXpoint(2,30.0);
    DNN_3n->AddXpoint(2,30.0); DNN_Res_3n->AddXpoint(2,30.0); TDR_3n->AddXpoint(2,30.0); TDR_Res_3n->AddXpoint(2,30.0); Perfect_3n->AddXpoint(2,30.0);
    DNN_4n->AddXpoint(2,30.0); DNN_Res_4n->AddXpoint(2,30.0); TDR_4n->AddXpoint(2,30.0); TDR_Res_4n->AddXpoint(2,30.0); Perfect_4n->AddXpoint(2,30.0);
    DNN_5n->AddXpoint(2,30.0); DNN_Res_5n->AddXpoint(2,30.0); TDR_5n->AddXpoint(2,30.0); TDR_Res_5n->AddXpoint(2,30.0); Perfect_5n->AddXpoint(2,30.0);
    
    // ------------------------------------------------------------
    
    // Next, start building graphs:
    
    DNN_1n->ComputeGraph(); DNN_Res_1n->ComputeGraph(); TDR_1n->ComputeGraph(); TDR_Res_1n->ComputeGraph(); Perfect_1n->ComputeGraph();
    DNN_2n->ComputeGraph(); DNN_Res_2n->ComputeGraph(); TDR_2n->ComputeGraph(); TDR_Res_2n->ComputeGraph(); Perfect_2n->ComputeGraph();
    DNN_3n->ComputeGraph(); DNN_Res_3n->ComputeGraph(); TDR_3n->ComputeGraph(); TDR_Res_3n->ComputeGraph(); Perfect_3n->ComputeGraph();
    DNN_4n->ComputeGraph(); DNN_Res_4n->ComputeGraph(); TDR_4n->ComputeGraph(); TDR_Res_4n->ComputeGraph(); Perfect_4n->ComputeGraph();
    DNN_5n->ComputeGraph(); DNN_Res_5n->ComputeGraph(); TDR_5n->ComputeGraph(); TDR_Res_5n->ComputeGraph(); Perfect_5n->ComputeGraph();
    
    // ------------------------------------------------------------

    // Plot graphs:
    Double_t ScaleFactor = 4.0;
    gROOT->SetBatch(kTRUE);
    
    TGraphErrors** PerfectGraphs = new TGraphErrors*[5];
    PerfectGraphs[0] = Perfect_1n->GetDataGraph();
    PerfectGraphs[1] = Perfect_2n->GetDataGraph();
    PerfectGraphs[2] = Perfect_3n->GetDataGraph();
    PerfectGraphs[3] = Perfect_4n->GetDataGraph();
    PerfectGraphs[4] = Perfect_5n->GetDataGraph();
    
    TString* Titles = new TString[5];
    Titles[0] = "Particle Gun: n=1";
    Titles[1] = "Particle Gun: n=2";
    Titles[2] = "Particle Gun: n=3";
    Titles[3] = "Particle Gun: n=4";
    Titles[4] = "Particle Gun: n=5";
    
    TCanvas* c1 = DrawGraphDoumaArray(5,3,2,PerfectGraphs,Titles,"Nr. of NeuLAND dp","Efficiency [%]",ScaleFactor,0,"AP");
    // DNN_1n->GetDataGraph()->SetLineStyle(1);
    
    // Draw Perfect graphs:
    Perfect_1n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_1n->GetDataGraph()->SetLineColor(kBlack); c1->cd(1); Perfect_1n->GetDataGraph()->Draw("PLsame");
    Perfect_2n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_2n->GetDataGraph()->SetLineColor(kBlack); c1->cd(2); Perfect_2n->GetDataGraph()->Draw("PLsame");
    Perfect_3n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_3n->GetDataGraph()->SetLineColor(kBlack); c1->cd(3); Perfect_3n->GetDataGraph()->Draw("PLsame");
    Perfect_4n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_4n->GetDataGraph()->SetLineColor(kBlack); c1->cd(4); Perfect_4n->GetDataGraph()->Draw("PLsame");
    Perfect_5n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_5n->GetDataGraph()->SetLineColor(kBlack); c1->cd(5); Perfect_5n->GetDataGraph()->Draw("PLsame");
    
    // Draw DNN graphs:
    DNN_1n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); DNN_1n->GetDataGraph()->SetLineColor(kRed);   c1->cd(1); DNN_1n->GetDataGraph()->Draw("PLsame");
    DNN_2n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); DNN_2n->GetDataGraph()->SetLineColor(kRed);   c1->cd(2); DNN_2n->GetDataGraph()->Draw("PLsame");
    DNN_3n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); DNN_3n->GetDataGraph()->SetLineColor(kRed);   c1->cd(3); DNN_3n->GetDataGraph()->Draw("PLsame");
    DNN_4n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); DNN_4n->GetDataGraph()->SetLineColor(kRed);   c1->cd(4); DNN_4n->GetDataGraph()->Draw("PLsame");
    DNN_5n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); DNN_5n->GetDataGraph()->SetLineColor(kRed);   c1->cd(5); DNN_5n->GetDataGraph()->Draw("PLsame");

    // Draw TDR graphs:
    TDR_1n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); TDR_1n->GetDataGraph()->SetLineColor(kBlue);  c1->cd(1); TDR_1n->GetDataGraph()->Draw("PLsame");
    TDR_2n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); TDR_2n->GetDataGraph()->SetLineColor(kBlue);  c1->cd(2); TDR_2n->GetDataGraph()->Draw("PLsame");
    TDR_3n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); TDR_3n->GetDataGraph()->SetLineColor(kBlue);  c1->cd(3); TDR_3n->GetDataGraph()->Draw("PLsame");
    TDR_4n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); TDR_4n->GetDataGraph()->SetLineColor(kBlue);  c1->cd(4); TDR_4n->GetDataGraph()->Draw("PLsame");
    TDR_5n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); TDR_5n->GetDataGraph()->SetLineColor(kBlue);  c1->cd(5); TDR_5n->GetDataGraph()->Draw("PLsame");

    // Draw Perfect Physics Errors:
    Perfect_1n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_1n->GetPhysGraph()->SetLineColor(kBlack); Perfect_1n->GetPhysGraph()->SetFillStyle(3000); Perfect_1n->GetPhysGraph()->SetFillColor(kGray+2); c1->cd(1); Perfect_1n->GetPhysGraph()->Draw("3same");
    Perfect_2n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_2n->GetPhysGraph()->SetLineColor(kBlack); Perfect_2n->GetPhysGraph()->SetFillStyle(3000); Perfect_2n->GetPhysGraph()->SetFillColor(kGray+2); c1->cd(2); Perfect_2n->GetPhysGraph()->Draw("3same");
    Perfect_3n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_3n->GetPhysGraph()->SetLineColor(kBlack); Perfect_3n->GetPhysGraph()->SetFillStyle(3000); Perfect_3n->GetPhysGraph()->SetFillColor(kGray+2); c1->cd(3); Perfect_3n->GetPhysGraph()->Draw("3same");
    Perfect_4n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_4n->GetPhysGraph()->SetLineColor(kBlack); Perfect_4n->GetPhysGraph()->SetFillStyle(3000); Perfect_4n->GetPhysGraph()->SetFillColor(kGray+2); c1->cd(4); Perfect_4n->GetPhysGraph()->Draw("3same");
    Perfect_5n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_5n->GetPhysGraph()->SetLineColor(kBlack); Perfect_5n->GetPhysGraph()->SetFillStyle(3000); Perfect_5n->GetPhysGraph()->SetFillColor(kGray+2); c1->cd(5); Perfect_5n->GetPhysGraph()->Draw("3same");
    
    // Draw DNN Physics Errors:
    DNN_1n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); DNN_1n->GetPhysGraph()->SetLineColor(kRed);   DNN_1n->GetPhysGraph()->SetFillStyle(3000); DNN_1n->GetPhysGraph()->SetFillColor(kRed-7);   c1->cd(1); DNN_1n->GetPhysGraph()->Draw("3same");
    DNN_2n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); DNN_2n->GetPhysGraph()->SetLineColor(kRed);   DNN_2n->GetPhysGraph()->SetFillStyle(3000); DNN_2n->GetPhysGraph()->SetFillColor(kRed-7);   c1->cd(2); DNN_2n->GetPhysGraph()->Draw("3same");
    DNN_3n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); DNN_3n->GetPhysGraph()->SetLineColor(kRed);   DNN_3n->GetPhysGraph()->SetFillStyle(3000); DNN_3n->GetPhysGraph()->SetFillColor(kRed-7);   c1->cd(3); DNN_3n->GetPhysGraph()->Draw("3same");
    DNN_4n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); DNN_4n->GetPhysGraph()->SetLineColor(kRed);   DNN_4n->GetPhysGraph()->SetFillStyle(3000); DNN_4n->GetPhysGraph()->SetFillColor(kRed-7);   c1->cd(4); DNN_4n->GetPhysGraph()->Draw("3same");
    DNN_5n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); DNN_5n->GetPhysGraph()->SetLineColor(kRed);   DNN_5n->GetPhysGraph()->SetFillStyle(3000); DNN_5n->GetPhysGraph()->SetFillColor(kRed-7);   c1->cd(5); DNN_5n->GetPhysGraph()->Draw("3same");

    // Draw TDR Physics Errors:
    TDR_1n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); TDR_1n->GetPhysGraph()->SetLineColor(kBlue);  TDR_1n->GetPhysGraph()->SetFillStyle(3000); TDR_1n->GetPhysGraph()->SetFillColor(kAzure-2); c1->cd(1); TDR_1n->GetPhysGraph()->Draw("3same");
    TDR_2n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); TDR_2n->GetPhysGraph()->SetLineColor(kBlue);  TDR_2n->GetPhysGraph()->SetFillStyle(3000); TDR_2n->GetPhysGraph()->SetFillColor(kAzure-2); c1->cd(2); TDR_2n->GetPhysGraph()->Draw("3same");
    TDR_3n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); TDR_3n->GetPhysGraph()->SetLineColor(kBlue);  TDR_3n->GetPhysGraph()->SetFillStyle(3000); TDR_3n->GetPhysGraph()->SetFillColor(kAzure-2); c1->cd(3); TDR_3n->GetPhysGraph()->Draw("3same");
    TDR_4n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); TDR_4n->GetPhysGraph()->SetLineColor(kBlue);  TDR_4n->GetPhysGraph()->SetFillStyle(3000); TDR_4n->GetPhysGraph()->SetFillColor(kAzure-2); c1->cd(4); TDR_4n->GetPhysGraph()->Draw("3same");
    TDR_5n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); TDR_5n->GetPhysGraph()->SetLineColor(kBlue);  TDR_5n->GetPhysGraph()->SetFillStyle(3000); TDR_5n->GetPhysGraph()->SetFillColor(kAzure-2); c1->cd(5); TDR_5n->GetPhysGraph()->Draw("3same");
    
    // Save & Close:
    c1->SaveAs("./TotalMult_1000MeV.png");
    c1->Close();
    
    // --------------------------------------------------------------
    
    // Make the restricted picture:
    
    TCanvas* c2 = DrawGraphDoumaArray(5,3,2,PerfectGraphs,Titles,"Nr. of NeuLAND dp","Efficiency [%]",ScaleFactor,0,"AP");
    // DNN_1n->GetDataGraph()->SetLineStyle(1);
    
    // Draw Perfect graphs:
    Perfect_1n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_1n->GetDataGraph()->SetLineColor(kBlack); c2->cd(1); Perfect_1n->GetDataGraph()->Draw("PLsame");
    Perfect_2n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_2n->GetDataGraph()->SetLineColor(kBlack); c2->cd(2); Perfect_2n->GetDataGraph()->Draw("PLsame");
    Perfect_3n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_3n->GetDataGraph()->SetLineColor(kBlack); c2->cd(3); Perfect_3n->GetDataGraph()->Draw("PLsame");
    Perfect_4n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_4n->GetDataGraph()->SetLineColor(kBlack); c2->cd(4); Perfect_4n->GetDataGraph()->Draw("PLsame");
    Perfect_5n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_5n->GetDataGraph()->SetLineColor(kBlack); c2->cd(5); Perfect_5n->GetDataGraph()->Draw("PLsame");
    
    // Draw DNN graphs:
    DNN_Res_1n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); DNN_Res_1n->GetDataGraph()->SetLineColor(kRed);   c2->cd(1); DNN_Res_1n->GetDataGraph()->Draw("PLsame");
    DNN_Res_2n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); DNN_Res_2n->GetDataGraph()->SetLineColor(kRed);   c2->cd(2); DNN_Res_2n->GetDataGraph()->Draw("PLsame");
    DNN_Res_3n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); DNN_Res_3n->GetDataGraph()->SetLineColor(kRed);   c2->cd(3); DNN_Res_3n->GetDataGraph()->Draw("PLsame");
    DNN_Res_4n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); DNN_Res_4n->GetDataGraph()->SetLineColor(kRed);   c2->cd(4); DNN_Res_4n->GetDataGraph()->Draw("PLsame");
    DNN_Res_5n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); DNN_Res_5n->GetDataGraph()->SetLineColor(kRed);   c2->cd(5); DNN_Res_5n->GetDataGraph()->Draw("PLsame");

    // Draw TDR graphs:
    TDR_Res_1n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); TDR_Res_1n->GetDataGraph()->SetLineColor(kBlue);  c2->cd(1); TDR_Res_1n->GetDataGraph()->Draw("PLsame");
    TDR_Res_2n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); TDR_Res_2n->GetDataGraph()->SetLineColor(kBlue);  c2->cd(2); TDR_Res_2n->GetDataGraph()->Draw("PLsame");
    TDR_Res_3n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); TDR_Res_3n->GetDataGraph()->SetLineColor(kBlue);  c2->cd(3); TDR_Res_3n->GetDataGraph()->Draw("PLsame");
    TDR_Res_4n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); TDR_Res_4n->GetDataGraph()->SetLineColor(kBlue);  c2->cd(4); TDR_Res_4n->GetDataGraph()->Draw("PLsame");
    TDR_Res_5n->GetDataGraph()->SetLineWidth(ScaleFactor*1.5); TDR_Res_5n->GetDataGraph()->SetLineColor(kBlue);  c2->cd(5); TDR_Res_5n->GetDataGraph()->Draw("PLsame");

    // Draw Perfect Physics Errors:
    Perfect_1n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_1n->GetPhysGraph()->SetLineColor(kBlack); Perfect_1n->GetPhysGraph()->SetFillStyle(3000); Perfect_1n->GetPhysGraph()->SetFillColor(kGray+2); c2->cd(1); Perfect_1n->GetPhysGraph()->Draw("3same");
    Perfect_2n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_2n->GetPhysGraph()->SetLineColor(kBlack); Perfect_2n->GetPhysGraph()->SetFillStyle(3000); Perfect_2n->GetPhysGraph()->SetFillColor(kGray+2); c2->cd(2); Perfect_2n->GetPhysGraph()->Draw("3same");
    Perfect_3n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_3n->GetPhysGraph()->SetLineColor(kBlack); Perfect_3n->GetPhysGraph()->SetFillStyle(3000); Perfect_3n->GetPhysGraph()->SetFillColor(kGray+2); c2->cd(3); Perfect_3n->GetPhysGraph()->Draw("3same");
    Perfect_4n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_4n->GetPhysGraph()->SetLineColor(kBlack); Perfect_4n->GetPhysGraph()->SetFillStyle(3000); Perfect_4n->GetPhysGraph()->SetFillColor(kGray+2); c2->cd(4); Perfect_4n->GetPhysGraph()->Draw("3same");
    Perfect_5n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); Perfect_5n->GetPhysGraph()->SetLineColor(kBlack); Perfect_5n->GetPhysGraph()->SetFillStyle(3000); Perfect_5n->GetPhysGraph()->SetFillColor(kGray+2); c2->cd(5); Perfect_5n->GetPhysGraph()->Draw("3same");
    
    // Draw DNN Physics Errors:
    DNN_Res_1n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); DNN_Res_1n->GetPhysGraph()->SetLineColor(kRed);   DNN_Res_1n->GetPhysGraph()->SetFillStyle(3000); DNN_Res_1n->GetPhysGraph()->SetFillColor(kRed-7);   c2->cd(1); DNN_Res_1n->GetPhysGraph()->Draw("3same");
    DNN_Res_2n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); DNN_Res_2n->GetPhysGraph()->SetLineColor(kRed);   DNN_Res_2n->GetPhysGraph()->SetFillStyle(3000); DNN_Res_2n->GetPhysGraph()->SetFillColor(kRed-7);   c2->cd(2); DNN_Res_2n->GetPhysGraph()->Draw("3same");
    DNN_Res_3n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); DNN_Res_3n->GetPhysGraph()->SetLineColor(kRed);   DNN_Res_3n->GetPhysGraph()->SetFillStyle(3000); DNN_Res_3n->GetPhysGraph()->SetFillColor(kRed-7);   c2->cd(3); DNN_Res_3n->GetPhysGraph()->Draw("3same");
    DNN_Res_4n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); DNN_Res_4n->GetPhysGraph()->SetLineColor(kRed);   DNN_Res_4n->GetPhysGraph()->SetFillStyle(3000); DNN_Res_4n->GetPhysGraph()->SetFillColor(kRed-7);   c2->cd(4); DNN_Res_4n->GetPhysGraph()->Draw("3same");
    DNN_Res_5n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); DNN_Res_5n->GetPhysGraph()->SetLineColor(kRed);   DNN_Res_5n->GetPhysGraph()->SetFillStyle(3000); DNN_Res_5n->GetPhysGraph()->SetFillColor(kRed-7);   c2->cd(5); DNN_Res_5n->GetPhysGraph()->Draw("3same");

    // Draw TDR Physics Errors:
    TDR_Res_1n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); TDR_Res_1n->GetPhysGraph()->SetLineColor(kBlue);  TDR_Res_1n->GetPhysGraph()->SetFillStyle(3000); TDR_Res_1n->GetPhysGraph()->SetFillColor(kAzure-2); c2->cd(1); TDR_Res_1n->GetPhysGraph()->Draw("3same");
    TDR_Res_2n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); TDR_Res_2n->GetPhysGraph()->SetLineColor(kBlue);  TDR_Res_2n->GetPhysGraph()->SetFillStyle(3000); TDR_Res_2n->GetPhysGraph()->SetFillColor(kAzure-2); c2->cd(2); TDR_Res_2n->GetPhysGraph()->Draw("3same");
    TDR_Res_3n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); TDR_Res_3n->GetPhysGraph()->SetLineColor(kBlue);  TDR_Res_3n->GetPhysGraph()->SetFillStyle(3000); TDR_Res_3n->GetPhysGraph()->SetFillColor(kAzure-2); c2->cd(3); TDR_Res_3n->GetPhysGraph()->Draw("3same");
    TDR_Res_4n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); TDR_Res_4n->GetPhysGraph()->SetLineColor(kBlue);  TDR_Res_4n->GetPhysGraph()->SetFillStyle(3000); TDR_Res_4n->GetPhysGraph()->SetFillColor(kAzure-2); c2->cd(4); TDR_Res_4n->GetPhysGraph()->Draw("3same");
    TDR_Res_5n->GetPhysGraph()->SetLineWidth(ScaleFactor*1.5); TDR_Res_5n->GetPhysGraph()->SetLineColor(kBlue);  TDR_Res_5n->GetPhysGraph()->SetFillStyle(3000); TDR_Res_5n->GetPhysGraph()->SetFillColor(kAzure-2); c2->cd(5); TDR_Res_5n->GetPhysGraph()->Draw("3same");
    
    // Save & Close:
    c2->SaveAs("./RestrictedMult_1000MeV.png");
    c2->Close();
}
