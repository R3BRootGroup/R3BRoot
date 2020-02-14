// Every CXX-file includes its own header file:
#include "R3BNeutronConvertor.h"

// Default constructor definition:
R3BNeutronConvertor::R3BNeutronConvertor() : FairTask("R3BNeutronConvertor")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create the single TConesArray:
    fArrayClusters = new TClonesArray("R3BNeulandCADCluster");
    fArrayNeutrons = new TClonesArray("R3BNeulandCADNeutron");
    TheOutputFile = 0;
    nEvents = 1;
    EventCounter = 0;
    
    // Inputs:
    nPaddlesPerPlane = 0;
    nDoublePlanes = 0;
    NeuLAND_Center_X = 0.0;
    NeuLAND_Center_Y = 0.0;
    NeuLAND_Front_Z = 1400.0;
    NeuLAND_Rot_X = 0.0;
    NeuLAND_Rot_Y = 0.0;
    NeuLAND_Rot_Z = 0.0;
    NeuLAND_Active_Bar_Thickness = 0.0;
    NeuLAND_Total_Bar_Length = 0.0;
    NeuLAND_TotalBarThicknessXY = 0.0;
    NeuLAND_TotalBarThicknessZ = 0.0;
    
    // Auxillary classes:
    Inputs = 0;
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    OutputNameTag = "";
}

// Destructor definition:
R3BNeutronConvertor::~R3BNeutronConvertor()
{
    // Delete all arrays:
    if (fArrayClusters) {fArrayClusters->Clear(); delete fArrayClusters;}
    if (fArrayNeutrons) {fArrayNeutrons->Clear(); delete fArrayNeutrons;}
    
    // Delete rest:
    delete TheNuclei;
}

// FairTask initialization function:
InitStatus R3BNeutronConvertor::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BNeutronConvertor::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs-class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: The Inputs-class of the R3BNeutronConvertor is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain the required parameters:
    nPaddlesPerPlane = Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane");
    nDoublePlanes = Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes");
    NeuLAND_Center_X = Inputs->GetInputDouble("NeuLAND_center_x_position","cm");
    NeuLAND_Center_Y = Inputs->GetInputDouble("NeuLAND_center_y_position","cm");
    NeuLAND_Front_Z = Inputs->GetInputDouble("NeuLAND_front_z_position","cm");
    NeuLAND_Rot_X = Inputs->GetInputDouble("NeuLAND_x_rotation_angle","degree");
    NeuLAND_Rot_Y = Inputs->GetInputDouble("NeuLAND_y_rotation_angle","degree");
    NeuLAND_Rot_Z = Inputs->GetInputDouble("NeuLAND_z_rotation_angle","degree");
    NeuLAND_Active_Bar_Thickness = Inputs->GetInputDouble("NeuLAND_BC408_BarThickness","cm");
    NeuLAND_Total_Bar_Length = Inputs->GetInputDouble("NeuLAND_Total_BarLength","cm");
    NeuLAND_TotalBarThicknessXY = Inputs->GetTotalPaddleThicknessXY();
    NeuLAND_TotalBarThicknessZ = Inputs->GetTotalPaddleThicknessZ();
    
    // Obtain R3BNeulandCADClusters:
    if ((TClonesArray*)ioman->GetObject("NeulandPrimaryClustersMultiple"+OutputNameTag) == nullptr)
    {
        cout << "I/O-manager FATAL: R3BNeulandNeutronConvertor::Init No NeulandPrimaryClustersMultiple!\n\n";
        return kFATAL;
    }
    fArrayClusters = (TClonesArray*)ioman->GetObject("NeulandPrimaryClustersMultiple"+OutputNameTag);
    
    // Register the neutrons:
    ioman->Register("NeulandNeutronsCheat"+OutputNameTag,"R3BNeulandCADNeutron",fArrayNeutrons,kTRUE);
    
    // Finish up:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BNeutronConvertor::Exec(Option_t *option)
{
    // NOTE: FairTask will automatically load the content of the fArrayDigis.
    // we need to clean the output-array:
    fArrayNeutrons->Clear();
    
    // Begin by declaring what we need:
    Int_t nClusters = fArrayClusters->GetEntries();
    R3BNeulandCADHit ThisHit; // No pointer!
    R3BNeulandCADCluster* ThisCluster;
    R3BNeulandCADNeutron* ThisNeutron;
    Int_t PaddleID;
    Double_t Time;
    TVector3 ThePos;
    TVector3 ThePixel;
    
    // Next, loop over all clusters and transfer the first hit to the new class:
    for (Int_t k = 0; k<nClusters; ++k)
    {
        // Begin by extracting the current cluster:
        ThisCluster = (R3BNeulandCADCluster*) fArrayClusters->At(k);
        
        // Then, extract the TOF cluster head:
        ThisHit = ThisCluster->GetFirstHit();
        
        // Next, extract the required quantities from the hit:
        PaddleID = ThisHit.GetPaddle();
        Time = ThisHit.GetT();
        ThePos = ThisHit.GetPosition();
        ThePixel = ObtainPixel(ThePos.X(),ThePos.Y(),ThePos.Z());
        
        // Then, generate the neutron:
        new ((*fArrayNeutrons)[k]) R3BNeulandCADNeutron(PaddleID,Time,ThePos,ThePixel);
        
        // Done.
    }
    
    // Log progress:
    EventCounter = EventCounter + 1;
}
        
// Definition of Finish-function:
void R3BNeutronConvertor::Finish()
{
    // Select TheOutputFile:
    TheOutputFile->cd();
}

// Function to extract the NeuLAND pixels. This idea is taken from Jan Mayers original code. It should do
// nothing more then map x & y to 0:1:49 (integers) and z to 0:1:59 (integer).
TVector3 R3BNeutronConvertor::ObtainPixel(Double_t const xpoint_c, Double_t const ypoint_c, Double_t const zpoint_c)
{
    // Begin by declaring new points & shifting. We will do exactly the reverse of the geometry
    // building file:
    Double_t xpoint = xpoint_c - NeuLAND_Center_X; 
    Double_t ypoint = ypoint_c - NeuLAND_Center_Y; 
    Double_t zpoint = zpoint_c - NeuLAND_Front_Z - NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes); 

    // Now, NeuLAND is in the origin. Anti-rotate:
    TVector3 pos;
    pos.SetX(xpoint);
    pos.SetY(ypoint);
    pos.SetZ(zpoint);
    
    pos.RotateZ(-1.0*NeuLAND_Rot_Z*TMath::Pi()/180.0);
    pos.RotateY(-1.0*NeuLAND_Rot_Y*TMath::Pi()/180.0);
    pos.RotateX(-1.0*NeuLAND_Rot_X*TMath::Pi()/180.0);
    
    xpoint = pos.X();
    ypoint = pos.Y();
    zpoint = pos.Z();
    
    // Now, NeuLAND is at the origin and is de-rotated. Next, shift
    // so that x, y and x all start at zero:
    xpoint = xpoint + 0.5*NeuLAND_Total_Bar_Length;
    ypoint = ypoint + 0.5*NeuLAND_Total_Bar_Length;
    zpoint = zpoint + NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes);
    
    // Now, map them to pixels one-by-one and make sure that they are 
    // all in the proper range:
    Int_t Pixel_X = 0;
         if (xpoint<0.0) {Pixel_X = 0;}
    else if (xpoint>(NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane))) {Pixel_X = nPaddlesPerPlane - 1;}
    else
    {
        // Map to the range [0,1]:
        xpoint = xpoint/(NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane));
        
        // Map to the range [0,nPaddlesPerPlane-1]:
        xpoint = xpoint*((Int_t) (nPaddlesPerPlane-1));
        
        // Round it off:
        xpoint = xpoint + 0.5;
        Pixel_X = (Int_t) xpoint;
    }
    
    // Same for Y:
    Int_t Pixel_Y = 0;
         if (ypoint<0.0) {Pixel_Y = 0;}
    else if (ypoint>(NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane))) {Pixel_Y = nPaddlesPerPlane - 1;}
    else
    {
        // Map to the range [0,1]:
        ypoint = ypoint/(NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane));
        
        // Map to the range [0,nPaddlesPerPlane-1]:
        ypoint = ypoint*((Int_t) (nPaddlesPerPlane-1));
        
        // Round it off:
        ypoint = ypoint + 0.5;
        Pixel_Y = (Int_t) ypoint;
    }
    
    // And slightly different for Z:
    Int_t Pixel_Z = 0;
         if (zpoint<0.0) {Pixel_Z = 0;}
    else if (zpoint>(2.0*NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes))) {Pixel_Z = 2*nDoublePlanes - 1;}
    else
    {
        // Map to the range [0,1]:
        zpoint = zpoint/(2.0*NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes));
        
        // Map to the range [0,nPaddlesPerPlane-1]:
        zpoint = zpoint*((Int_t) (2*nDoublePlanes-1));
        
        // Round it off:
        zpoint = zpoint + 0.5;
        Pixel_Z = (Int_t) zpoint;
    }
        
    // Next, compose the final vector:
    TVector3 Answer;
    Answer.SetX((Int_t) Pixel_X);
    Answer.SetY((Int_t) Pixel_Y);
    Answer.SetZ((Int_t) Pixel_Z);
    
    // and, return it.
    return Answer;
}

// Generate ROOT dictionary:
ClassImp(R3BNeutronConvertor);
