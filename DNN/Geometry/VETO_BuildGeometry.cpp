// Declaration of Global variables:
Double_t VETO_gAlumniniumStrip_Width;
Double_t VETO_gAlumniniumStrip_Thickness;
Double_t VETO_gPaddleDistance_xy;
Double_t VETO_gPaddleDistance_z;
Double_t VETO_gPaddleBaseLength;
Double_t VETO_gPaddleConeLength;
Double_t VETO_gBC408ConeRadius;
Double_t VETO_gBC408Thickness_XY;
Double_t VETO_gBC408Thickness_Z;
Double_t VETO_gAlThickness;
Double_t VETO_gTapeThickness;
Double_t VETO_gAirGabThickness;
Int_t VETO_gPaddlesPerPlane;
Int_t VETO_gNumberOfDoublePlanes;
Int_t VETO_gFrameColor;
Int_t VETO_gDetectorColor;
Bool_t VETO_gVisualizeOutput;

// Function to retrieve building materials:
TGeoMedium* VETO_BuildMaterial(TString const material, FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild)
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
TGeoShape* VETO_BuildPaddleShape(TString const &name, Double_t const length, Double_t const width_XY, Double_t const width_Z, Double_t const coneRadius, Double_t const coneLength)
{
    Double_t MaxWidth = width_XY;
    if (width_Z>MaxWidth) {MaxWidth = width_Z;}
    
    new TGeoBBox(name + "Box", length, width_XY, width_Z);
    new TGeoCone(name + "Cone", coneLength + 0.001, 0., coneRadius, 0., MaxWidth * 1.005);
    new TGeoBBox(name + "Conebox", width_Z, width_XY, coneLength);
    
    TGeoShape *shape = new TGeoCompositeShape(name,name + "Box + ((" + name + "Conebox*" + name + "Cone):trc1) + ((" + name + "Conebox*" + name + "Cone):trc2)");
    return shape;
}

// Function to build a single NeuLAND bar. It consists of 3 paddle shapes: Inner: BC408 scintillator. 
// middle: Al reflection foil and outer: wrapping tape. It also contaisn Aluminium frame holders.
TGeoVolume* VETO_BuildPaddleVolume()
{
    // Load Interfaces to build materials
    FairGeoLoader    *geoLoad = FairGeoLoader::Instance();
    FairGeoInterface *geoFace = geoLoad->getGeoInterface();
    geoFace->setMediaFile(TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/media_r3b.geo");
    geoFace->readMedia();
    FairGeoBuilder  *geoBuild = geoLoad->getGeoBuilder();
    FairGeoMedia    *geoMedia = geoFace->getMedia();
    const TGeoMedium *medBC408 = VETO_BuildMaterial("BC408", geoMedia, geoBuild);
    const TGeoMedium *medCH2   = VETO_BuildMaterial("polyethylene", geoMedia, geoBuild); // ATTENTION polyethylene
    const TGeoMedium *medAl    = VETO_BuildMaterial("aluminium", geoMedia, geoBuild);

    // Prepare Transformations for cones
    TGeoRotation *r1 = new TGeoRotation();
    r1->RotateY(90);
    TGeoCombiTrans *trc1 = new TGeoCombiTrans(TGeoTranslation(-(VETO_gPaddleBaseLength + VETO_gPaddleConeLength), 0., 0.), *r1);
    trc1->SetName("trc1");
    trc1->RegisterYourself();

    TGeoRotation *r2 = new TGeoRotation();
    r2->RotateY(-90);
    TGeoCombiTrans *trc2 = new TGeoCombiTrans(TGeoTranslation(+(VETO_gPaddleBaseLength + VETO_gPaddleConeLength), 0., 0.), *r2);
    trc2->SetName("trc2");
    trc2->RegisterYourself();

    // Build shapes
    const TGeoShape *shapeBC408 = VETO_BuildPaddleShape("VETOshapeBC408",
                                  VETO_gPaddleBaseLength,
                                  VETO_gBC408Thickness_XY,
                                  VETO_gBC408Thickness_Z,
                                  VETO_gBC408ConeRadius,
                                  VETO_gPaddleConeLength);

    VETO_BuildPaddleShape("VETOshapeAlWrappingSolid",
                     VETO_gPaddleBaseLength,
                     VETO_gBC408Thickness_XY + VETO_gAlThickness,
                     VETO_gBC408Thickness_Z + VETO_gAlThickness,
                     VETO_gBC408ConeRadius +  VETO_gAlThickness,
                     VETO_gPaddleConeLength);
    const TGeoShape *shapeAlWrapping = new TGeoCompositeShape("VETOshapeAlWrapping", "VETOshapeAlWrappingSolid - VETOshapeBC408");

    VETO_BuildPaddleShape("VETOshapeTapeWrappingSolid",
                     VETO_gPaddleBaseLength,
                     VETO_gBC408Thickness_XY + VETO_gAlThickness + VETO_gTapeThickness,
                     VETO_gBC408Thickness_Z + VETO_gAlThickness + VETO_gTapeThickness,
                     VETO_gBC408ConeRadius +  VETO_gAlThickness + VETO_gTapeThickness,
                     VETO_gPaddleConeLength);
    const TGeoShape *shapeTapeWrapping = new TGeoCompositeShape("VETOshapeTapeWrapping", "VETOshapeTapeWrappingSolid - VETOshapeAlWrappingSolid");
    
    // Build the components of the Aluminium frame:
    TGeoRotation* rotplus = new TGeoRotation("rotplus");
    rotplus->RotateX(0.0);
    rotplus->RotateY(0.0);
    rotplus->RotateZ(0.0);
    TGeoCombiTrans* transfplus = new TGeoCombiTrans("transfplus",VETO_gPaddleBaseLength/3.0,0.0,0.0,rotplus);
    transfplus->RegisterYourself();
    
    TGeoRotation* rotmin = new TGeoRotation("rotmin");
    rotmin->RotateX(0.0);
    rotmin->RotateY(0.0);
    rotmin->RotateZ(0.0);
    TGeoCombiTrans* transfmin = new TGeoCombiTrans("transfmin",-1.0*VETO_gPaddleBaseLength/3.0,0.0,0.0,rotmin);
    transfmin->RegisterYourself();
    
    new TGeoBBox("VETOAlbox1",VETO_gAlumniniumStrip_Width/2.0,VETO_gPaddleDistance_xy,VETO_gPaddleDistance_z);
    new TGeoBBox("VETOAlbox2",VETO_gAlumniniumStrip_Width/1.0,VETO_gBC408Thickness_XY+VETO_gAlThickness+VETO_gTapeThickness,VETO_gBC408Thickness_Z+VETO_gAlThickness+VETO_gTapeThickness);
      
    TGeoCompositeShape* AlStrip1 = new TGeoCompositeShape("VETOAlStrip1","VETOAlbox1-VETOAlbox2:");
    TGeoVolume* AlStrip1Vol = new TGeoVolume("VETOAlStrip1Vol",AlStrip1,medAl);
    AlStrip1Vol->SetLineColor(VETO_gFrameColor);
    
    TGeoCompositeShape* AlStrip2 = new TGeoCompositeShape("VETOAlStrip2","VETOAlbox1-VETOAlbox2");
    TGeoVolume* AlStrip2Vol = new TGeoVolume("VETOAlStrip2Vol",AlStrip2,medAl);
    AlStrip2Vol->SetLineColor(VETO_gFrameColor);

    // Build total Volumes. Note that the volume names are important and reappear in R3BNeuland()
    TGeoVolume *volBC408 = new TGeoVolume("VETOvolBC408", shapeBC408, medBC408);
    volBC408->SetLineColor(VETO_gDetectorColor); 
    TGeoVolume *volAlWrapping = new TGeoVolume("VETOvolAlWrapping", shapeAlWrapping, medAl);
    volAlWrapping->SetLineColor(VETO_gDetectorColor);
    TGeoVolume *volTapeWrapping = new TGeoVolume("VETOvolTapeWrapping", shapeTapeWrapping, medCH2);
    volTapeWrapping->SetLineColor(VETO_gDetectorColor);

    // Make the elementary assembly
    TGeoVolume *volPaddle = new TGeoVolumeAssembly("VETOvolPaddle");
    volPaddle->AddNode(volBC408, 1);
    volPaddle->AddNode(volAlWrapping, 1);
    volPaddle->AddNode(volTapeWrapping, 1);
    
    if (VETO_gAlumniniumStrip_Thickness>0.0)
    {
        volPaddle->AddNode(AlStrip1Vol, 1, transfplus);
        volPaddle->AddNode(AlStrip2Vol, 1, transfmin);
    }

    return volPaddle;
}

// Main function definition:
void VETO_BuildGeometry()
{
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Specify global variables:
    VETO_gAlumniniumStrip_Width = Inputs->GetInputDouble("VETO_AluminiumFrame_Width","cm");
    VETO_gAlumniniumStrip_Thickness = Inputs->GetInputDouble("VETO_AluminiumFrame_Thickness","cm");
    VETO_gPaddleBaseLength = Inputs->GetInputDouble("VETO_Total_BarLength","cm")/2.0;
    VETO_gPaddleConeLength = Inputs->GetInputDouble("VETO_EndPoint_ConeLength","cm");
    VETO_gBC408ConeRadius = Inputs->GetInputDouble("VETO_EndPoint_ConeRadius","cm");
    VETO_gBC408Thickness_XY = Inputs->GetInputDouble("VETO_BC408_BarThickness_XY","cm")/2.0;
    VETO_gBC408Thickness_Z = Inputs->GetInputDouble("VETO_BC408_BarThickness_Z","cm")/2.0;
    VETO_gAlThickness = Inputs->GetInputDouble("VETO_ReflectionFoil_Thickness","cm");
    VETO_gTapeThickness = Inputs->GetInputDouble("VETO_WrappingTape_Thickness","cm");
    VETO_gAirGabThickness = Inputs->GetInputDouble("VETO_Bars_AirGab","cm");
    VETO_gPaddlesPerPlane = Inputs->GetInputInteger("VETO_Number_of_Bars_in_plane");
    VETO_gVisualizeOutput = Inputs->GetInputBoolian("VETO_Visualize_Geometry_DuringBuild");
    VETO_gFrameColor = Inputs->GetInputInteger("VETO_Frame_Color");
    VETO_gDetectorColor = Inputs->GetInputInteger("VETO_Detector_Color");

    VETO_gPaddleDistance_xy = VETO_gBC408Thickness_XY + VETO_gAlThickness + VETO_gTapeThickness + VETO_gAlumniniumStrip_Thickness/2.0;
    if ((VETO_gAlumniniumStrip_Thickness/2.0)<VETO_gAirGabThickness) {VETO_gPaddleDistance_xy = VETO_gBC408Thickness_XY + VETO_gAlThickness + VETO_gTapeThickness + VETO_gAirGabThickness;}
    
    VETO_gPaddleDistance_z = VETO_gBC408Thickness_Z + VETO_gAlThickness + VETO_gTapeThickness + VETO_gAlumniniumStrip_Thickness;
    if (VETO_gAlumniniumStrip_Thickness<VETO_gAirGabThickness) {VETO_gPaddleDistance_z = VETO_gBC408Thickness_Z + VETO_gAlThickness + VETO_gTapeThickness + VETO_gAirGabThickness;}
    
    TString geoFileName = Inputs->GetInputString("TheOutputPath") + Inputs->GetInputString("VETO_Geometry_FileName");
    
    // ------------------------------------------------------------------------
    
    // Begin construction of the geometry:
    TGeoRotation* rot90 = new TGeoRotation();
    rot90->RotateZ(90.);

    new FairGeoLoader("TGeo", "FairGeoLoader");

    auto volPaddle = VETO_BuildPaddleVolume(); // Will make sure gGeoManager is loaded

    auto volVETO = new TGeoVolumeAssembly("volVETO");
    
    Int_t nindex = 0;
    
    for (Double_t a = -VETO_gPaddlesPerPlane * VETO_gPaddleDistance_xy + VETO_gPaddleDistance_xy; a < VETO_gPaddleDistance_xy * VETO_gPaddlesPerPlane; a += VETO_gPaddleDistance_xy * 2)
    {
        nindex++;
        volVETO->AddNode(volPaddle, nindex, new TGeoCombiTrans(a, 0.0, 0.0, rot90));
    }

    auto top = new TGeoVolumeAssembly("TOP");
    gGeoManager->SetTopVolume(top);
    
    // Add NeuLANd total volume to the top:
    TGeoRotation* rottotal = new TGeoRotation("rottotal");
    rottotal->RotateX(Inputs->GetInputDouble("VETO_x_rotation_angle","degree"));
    rottotal->RotateY(Inputs->GetInputDouble("VETO_y_rotation_angle","degree"));
    rottotal->RotateZ(Inputs->GetInputDouble("VETO_z_rotation_angle","degree"));
    TGeoCombiTrans* transtotal = new TGeoCombiTrans("transtotal",Inputs->GetInputDouble("VETO_center_x_position","cm"),Inputs->GetInputDouble("VETO_center_y_position","cm"),
                                                    Inputs->GetInputDouble("VETO_front_z_position","cm") + VETO_gPaddleDistance_z, rottotal);
    transtotal->RegisterYourself();

    // The new TGeoTranslation is necessary because of ROOT
    top->AddNode(volVETO, 1, transtotal);

    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();
    // gGeoManager->Test();

    TFile geoFile(geoFileName, "RECREATE");
    top->Write();
    geoFile.Close();

    cout << "Done. " << nindex << " Paddles in Planes = 1 sp -> " << geoFileName << endl;

    if (VETO_gVisualizeOutput==kTRUE)
    {
        gStyle->SetCanvasPreferGL(kTRUE);
        volVETO->Draw("ogl");
        auto v = (TGLViewer*)gPad->GetViewer3D();
        v->CurrentCamera().RotateRad(0, 0.1);
        v->SetStyle(TGLRnrCtx::kOutline);
        v->RequestDraw();
    }
    
    Inputs->PrintAllErrors();
}
