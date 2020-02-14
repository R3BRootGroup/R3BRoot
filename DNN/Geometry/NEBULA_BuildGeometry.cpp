// Declaration of Global variables:
Double_t NEBULA_gAlumniniumStrip_Width;
Double_t NEBULA_gAlumniniumStrip_Thickness;
Double_t NEBULA_gPaddleDistance_xy;
Double_t NEBULA_gPaddleDistance_z;
Double_t NEBULA_gPaddleBaseLength;
Double_t NEBULA_gPaddleConeLength;
Double_t NEBULA_gBC408ConeRadius;
Double_t NEBULA_gBC408Thickness;
Double_t NEBULA_gAlThickness;
Double_t NEBULA_gTapeThickness;
Double_t NEBULA_gAirGabThickness;
Double_t NEBULA_gModuleDistance;
Int_t NEBULA_gPaddlesPerPlane;
Int_t NEBULA_gNumberOfDoublePlanes;
Int_t NEBULA_gFrameColor;
Int_t NEBULA_gDetectorColor;
Bool_t NEBULA_gVisualizeOutput;

// Function to retrieve building materials:
TGeoMedium* NEBULA_BuildMaterial(TString const material, FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild)
{
    FairGeoMedium *fairMedium = geoMedia->getMedium(material);
    if (!fairMedium) {
        std::cout << "FairGeoMedium " << material << " not found" << std::endl;
    }

    geoBuild->createMedium(fairMedium);
    TGeoMedium *med = gGeoManager->GetMedium(material);
    if (!med) {
        std::cout << "TGeoMedium " << material << " not found" << std::endl;
    }
    
    return med;
}

// Function to build a single NeuLAND paddle shape: a bar + 2 cones.
TGeoShape* NEBULA_BuildPaddleShape(TString const &name, Double_t const length, Double_t const width, Double_t const coneRadius, Double_t const coneLength)
{
    new TGeoBBox(name + "Box", length, width, width);
    new TGeoCone(name + "Cone", coneLength + 0.001, 0., coneRadius, 0., width * TMath::Sqrt(2.));
    new TGeoBBox(name + "Conebox", width, width, coneLength);
    
    TGeoShape *shape = new TGeoCompositeShape(name,name + "Box + ((" + name + "Conebox*" + name + "Cone):trc1) + ((" + name + "Conebox*" + name + "Cone):trc2)");
    return shape;
}

// Function to build a single NeuLAND bar. It consists of 3 paddle shapes: Inner: BC408 scintillator. 
// middle: Al reflection foil and outer: wrapping tape. It also contaisn Aluminium frame holders.
TGeoVolume* NEBULA_BuildPaddleVolume()
{
    // Load Interfaces to build materials
    FairGeoLoader    *geoLoad = FairGeoLoader::Instance();
    FairGeoInterface *geoFace = geoLoad->getGeoInterface();
    geoFace->setMediaFile(TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/media_r3b.geo");
    geoFace->readMedia();
    FairGeoBuilder  *geoBuild = geoLoad->getGeoBuilder();
    FairGeoMedia    *geoMedia = geoFace->getMedia();
    const TGeoMedium *medBC408 = NEBULA_BuildMaterial("BC408", geoMedia, geoBuild);
    const TGeoMedium *medCH2   = NEBULA_BuildMaterial("polyethylene", geoMedia, geoBuild); // ATTENTION polyethylene
    const TGeoMedium *medAl    = NEBULA_BuildMaterial("aluminium", geoMedia, geoBuild);

    // Prepare Transformations for cones
    TGeoRotation *r1 = new TGeoRotation();
    r1->RotateY(90);
    TGeoCombiTrans *trc1 = new TGeoCombiTrans(TGeoTranslation(-(NEBULA_gPaddleBaseLength + NEBULA_gPaddleConeLength), 0., 0.), *r1);
    trc1->SetName("trc1");
    trc1->RegisterYourself();

    TGeoRotation *r2 = new TGeoRotation();
    r2->RotateY(-90);
    TGeoCombiTrans *trc2 = new TGeoCombiTrans(TGeoTranslation(+(NEBULA_gPaddleBaseLength + NEBULA_gPaddleConeLength), 0., 0.), *r2);
    trc2->SetName("trc2");
    trc2->RegisterYourself();

    // Build shapes
    const TGeoShape *shapeBC408 = NEBULA_BuildPaddleShape("NEBULAshapeBC408",
                                  NEBULA_gPaddleBaseLength,
                                  NEBULA_gBC408Thickness,
                                  NEBULA_gBC408ConeRadius,
                                  NEBULA_gPaddleConeLength);

    NEBULA_BuildPaddleShape("NEBULAshapeAlWrappingSolid",
                     NEBULA_gPaddleBaseLength,
                     NEBULA_gBC408Thickness + NEBULA_gAlThickness,
                     NEBULA_gBC408ConeRadius +  NEBULA_gAlThickness,
                     NEBULA_gPaddleConeLength);
    const TGeoShape *shapeAlWrapping = new TGeoCompositeShape("NEBULAshapeAlWrapping", "NEBULAshapeAlWrappingSolid - NEBULAshapeBC408");

    NEBULA_BuildPaddleShape("NEBULAshapeTapeWrappingSolid",
                     NEBULA_gPaddleBaseLength,
                     NEBULA_gBC408Thickness + NEBULA_gAlThickness + NEBULA_gTapeThickness,
                     NEBULA_gBC408ConeRadius +  NEBULA_gAlThickness + NEBULA_gTapeThickness,
                     NEBULA_gPaddleConeLength);
    const TGeoShape *shapeTapeWrapping = new TGeoCompositeShape("NEBULAshapeTapeWrapping", "NEBULAshapeTapeWrappingSolid - NEBULAshapeAlWrappingSolid");
    
    // Build the components of the Aluminium frame:
    TGeoRotation* rotplus = new TGeoRotation("NEBULArotplus");
    rotplus->RotateX(0.0);
    rotplus->RotateY(0.0);
    rotplus->RotateZ(0.0);
    TGeoCombiTrans* transfplus = new TGeoCombiTrans("NEBULAtransfplus",NEBULA_gPaddleBaseLength/3.0,0.0,0.0,rotplus);
    transfplus->RegisterYourself();
    
    TGeoRotation* rotmin = new TGeoRotation("NEBULArotmin");
    rotmin->RotateX(0.0);
    rotmin->RotateY(0.0);
    rotmin->RotateZ(0.0);
    TGeoCombiTrans* transfmin = new TGeoCombiTrans("NEBULAtransfmin",-1.0*NEBULA_gPaddleBaseLength/3.0,0.0,0.0,rotmin);
    transfmin->RegisterYourself();
    
    new TGeoBBox("NEBULAAlbox1",NEBULA_gAlumniniumStrip_Width/2.0,NEBULA_gPaddleDistance_xy,NEBULA_gPaddleDistance_z);
    new TGeoBBox("NEBULAAlbox2",NEBULA_gAlumniniumStrip_Width/1.0,NEBULA_gBC408Thickness+NEBULA_gAlThickness+NEBULA_gTapeThickness,NEBULA_gBC408Thickness+NEBULA_gAlThickness+NEBULA_gTapeThickness);
      
    TGeoCompositeShape* AlStrip1 = new TGeoCompositeShape("NEBULAAlStrip1","NEBULAAlbox1-NEBULAAlbox2:");
    TGeoVolume* AlStrip1Vol = new TGeoVolume("NEBULAAlStrip1Vol",AlStrip1,medAl);
    AlStrip1Vol->SetLineColor(NEBULA_gFrameColor);
    
    TGeoCompositeShape* AlStrip2 = new TGeoCompositeShape("NEBULAAlStrip2","NEBULAAlbox1-NEBULAAlbox2");
    TGeoVolume* AlStrip2Vol = new TGeoVolume("NEBULAAlStrip2Vol",AlStrip2,medAl);
    AlStrip2Vol->SetLineColor(NEBULA_gFrameColor);

    // Build total Volumes. Note that the volume names are important and reappear in R3BNeuland()
    TGeoVolume *volBC408 = new TGeoVolume("NEBULAvolBC408", shapeBC408, medBC408);
    volBC408->SetLineColor(NEBULA_gDetectorColor); 
    TGeoVolume *volAlWrapping = new TGeoVolume("NEBULAvolAlWrapping", shapeAlWrapping, medAl);
    volAlWrapping->SetLineColor(NEBULA_gDetectorColor);
    TGeoVolume *volTapeWrapping = new TGeoVolume("NEBULAvolTapeWrapping", shapeTapeWrapping, medCH2);
    volTapeWrapping->SetLineColor(NEBULA_gDetectorColor);

    // Make the elementary assembly
    TGeoVolume *volPaddle = new TGeoVolumeAssembly("NEBULAvolPaddle");
    volPaddle->AddNode(volBC408, 1);
    volPaddle->AddNode(volAlWrapping, 1);
    volPaddle->AddNode(volTapeWrapping, 1);
    
    if (NEBULA_gAlumniniumStrip_Thickness>0.0)
    {
        volPaddle->AddNode(AlStrip1Vol, 1, transfplus);
        volPaddle->AddNode(AlStrip2Vol, 1, transfmin);
    }

    return volPaddle;
}

// Main function definition:
void NEBULA_BuildGeometry()
{
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Specify global variables:
    NEBULA_gAlumniniumStrip_Width = Inputs->GetInputDouble("NEBULA_AluminiumFrame_Width","cm");
    NEBULA_gAlumniniumStrip_Thickness = Inputs->GetInputDouble("NEBULA_AluminiumFrame_Thickness","cm");
    NEBULA_gPaddleBaseLength = Inputs->GetInputDouble("NEBULA_Total_BarLength","cm")/2.0;
    NEBULA_gPaddleConeLength = Inputs->GetInputDouble("NEBULA_EndPoint_ConeLength","cm");
    NEBULA_gBC408ConeRadius = Inputs->GetInputDouble("NEBULA_EndPoint_ConeRadius","cm");
    NEBULA_gBC408Thickness = Inputs->GetInputDouble("NEBULA_BC408_BarThickness","cm")/2.0;
    NEBULA_gAlThickness = Inputs->GetInputDouble("NEBULA_ReflectionFoil_Thickness","cm");
    NEBULA_gTapeThickness = Inputs->GetInputDouble("NEBULA_WrappingTape_Thickness","cm");
    NEBULA_gAirGabThickness = Inputs->GetInputDouble("NEBULA_Bars_AirGab","cm");
    NEBULA_gPaddlesPerPlane = Inputs->GetInputInteger("NEBULA_Number_of_Bars_in_OnePlane");
    NEBULA_gNumberOfDoublePlanes = Inputs->GetInputInteger("NEBULA_Number_of_DoublePlaneModules");
    NEBULA_gModuleDistance = Inputs->GetInputDouble("NEBULA_Distance_BetweenModules","cm");
    NEBULA_gVisualizeOutput = Inputs->GetInputBoolian("NEBULA_Visualize_Geometry_DuringBuild");
    NEBULA_gFrameColor = Inputs->GetInputInteger("NEBULA_Frame_Color");
    NEBULA_gDetectorColor = Inputs->GetInputInteger("NEBULA_Detector_Color");

    NEBULA_gPaddleDistance_xy = NEBULA_gBC408Thickness + NEBULA_gAlThickness + NEBULA_gTapeThickness + NEBULA_gAlumniniumStrip_Thickness/2.0;
    if ((NEBULA_gAlumniniumStrip_Thickness/2.0)<NEBULA_gAirGabThickness) {NEBULA_gPaddleDistance_xy = NEBULA_gBC408Thickness + NEBULA_gAlThickness + NEBULA_gTapeThickness + NEBULA_gAirGabThickness;}
    
    NEBULA_gPaddleDistance_z = NEBULA_gBC408Thickness + NEBULA_gAlThickness + NEBULA_gTapeThickness + NEBULA_gAlumniniumStrip_Thickness;
    if (NEBULA_gAlumniniumStrip_Thickness<NEBULA_gAirGabThickness) {NEBULA_gPaddleDistance_z = NEBULA_gBC408Thickness + NEBULA_gAlThickness + NEBULA_gTapeThickness + NEBULA_gAirGabThickness;}
    
    TString geoFileName = Inputs->GetInputString("TheOutputPath") + Inputs->GetInputString("NEBULA_Geometry_FileName");
    
    // ------------------------------------------------------------------------
    
    // Begin construction of the geometry:
    TGeoRotation* rot90 = new TGeoRotation();
    rot90->RotateZ(90.);

    new FairGeoLoader("TGeo", "FairGeoLoader");

    auto volPaddle = NEBULA_BuildPaddleVolume(); // Will make sure gGeoManager is loaded

    auto volNEBULA = new TGeoVolumeAssembly("volNEBULA");
    
    // Define base startpoints:
    Double_t Initial_Z_Location = NEBULA_gPaddleDistance_z; // so the front of the first bar starts at z=0.
    Double_t Initial_Y_Location = 0.0; // all bars are same vertical stuf.
    Double_t Initial_X_Location = -1.0*NEBULA_gPaddleDistance_xy*((Int_t) NEBULA_gPaddlesPerPlane); // we start at negative x and work our way up.
    
    // Define current points:
    Double_t X_Location = Initial_X_Location;
    Double_t Y_Location = Initial_Y_Location;
    Double_t Z_Location = Initial_Z_Location;
    
    // Define Bar index:
    Int_t nIndex = 0;
    
    // Loop over the number of modules:
    for (Int_t kmodule = 0; kmodule<NEBULA_gNumberOfDoublePlanes; ++kmodule)
    {
        // Loop over the bars inside the module:
        for (Int_t kplane = 0; kplane<NEBULA_gPaddlesPerPlane; ++kplane)
        {
            // Update bar index:
            ++nIndex;
            
            // Update the location points:
            X_Location = Initial_X_Location + 2.0*NEBULA_gPaddleDistance_xy*((Int_t) kplane);
            Y_Location = 0.0;
            Z_Location = Initial_Z_Location + 4.0*NEBULA_gPaddleDistance_z*((Int_t) kmodule) + NEBULA_gModuleDistance*((Int_t) kmodule);
            
            // Add the bar:
            volNEBULA->AddNode(volPaddle, nIndex, new TGeoCombiTrans(X_Location,Y_Location,Z_Location,rot90));
            
            // Now, also add the bar behind:
            ++nIndex;
            Z_Location = Z_Location + 2.0*NEBULA_gPaddleDistance_z;
            volNEBULA->AddNode(volPaddle, nIndex, new TGeoCombiTrans(X_Location,Y_Location,Z_Location,rot90));
            
            // Done. close the loops.
        }
    }
     
    // Now continue with the macro:
    auto top = new TGeoVolumeAssembly("TOP");
    gGeoManager->SetTopVolume(top);
    
    // Add NEBULA total volume to the top:
    TGeoRotation* rottotal = new TGeoRotation("NEBULArottotal");
    rottotal->RotateX(Inputs->GetInputDouble("NEBULA_x_rotation_angle","degree"));
    rottotal->RotateY(Inputs->GetInputDouble("NEBULA_y_rotation_angle","degree"));
    rottotal->RotateZ(Inputs->GetInputDouble("NEBULA_z_rotation_angle","degree"));
    TGeoCombiTrans* transtotal = new TGeoCombiTrans("transtotal",Inputs->GetInputDouble("NEBULA_center_x_position","cm"),Inputs->GetInputDouble("NEBULA_center_y_position","cm"),
                                                    Inputs->GetInputDouble("NEBULA_front_z_position","cm"), rottotal);
    transtotal->RegisterYourself();

    // The new TGeoTranslation is necessary because of ROOT
    top->AddNode(volNEBULA, 1, transtotal);

    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();
    // gGeoManager->Test();

    TFile geoFile(geoFileName, "RECREATE");
    top->Write();
    geoFile.Close();

    cout << "Done. " << nIndex << " Paddles in " << NEBULA_gNumberOfDoublePlanes << " Double-Plane modules -> " << geoFileName << endl;

    if (NEBULA_gVisualizeOutput==kTRUE)
    {
        gStyle->SetCanvasPreferGL(kTRUE);
        volNEBULA->Draw("ogl");
        auto v = (TGLViewer*)gPad->GetViewer3D();
        v->CurrentCamera().RotateRad(0, 0.1);
        v->SetStyle(TGLRnrCtx::kOutline);
        v->RequestDraw();
    }
    
    Inputs->PrintAllErrors();
}
