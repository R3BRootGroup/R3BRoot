// Declaration of Global variables:
Double_t NEBULA_VET_gAlumniniumStrip_Width;
Double_t NEBULA_VET_gAlumniniumStrip_Thickness;
Double_t NEBULA_VET_gPaddleDistance_xy;
Double_t NEBULA_VET_gPaddleDistance_z;
Double_t NEBULA_VET_gPaddleBaseLength;
Double_t NEBULA_VET_gPaddleConeLength;
Double_t NEBULA_VET_gBC408ConeRadius;
Double_t NEBULA_VET_gBC408Thickness_XY;
Double_t NEBULA_VET_gBC408Thickness_Z;
Double_t NEBULA_VET_gAlThickness;
Double_t NEBULA_VET_gTapeThickness;
Double_t NEBULA_VET_gAirGabThickness;
Int_t NEBULA_VET_gPaddlesPerPlane;
Int_t NEBULA_VET_gNumberOfDoublePlanes;
Int_t NEBULA_VET_gFrameColor;
Int_t NEBULA_VET_gDetectorColor;
Bool_t NEBULA_VET_gVisualizeOutput;

Int_t NEBULA_gNumberOfDoublePlanes;
Double_t NEBULA_gBC408Thickness;
Double_t NEBULA_gAlThickness;
Double_t NEBULA_gTapeThickness;
Double_t NEBULA_gAirGabThickness;
Double_t NEBULA_gAlumniniumStrip_Thickness;
Double_t NEBULA_gPaddleDistance_z;
Double_t NEBULA_gModuleDistance;

// Function to retrieve building materials:
TGeoMedium* NEBULA_VET_BuildMaterial(TString const material, FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild)
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
TGeoShape* NEBULA_VET_BuildPaddleShape(TString const &name, Double_t const length, Double_t const width_XY, Double_t const width_Z, Double_t const coneRadius, Double_t const coneLength)
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
TGeoVolume* NEBULA_VET_BuildPaddleVolume()
{
    // Load Interfaces to build materials
    FairGeoLoader    *geoLoad = FairGeoLoader::Instance();
    FairGeoInterface *geoFace = geoLoad->getGeoInterface();
    geoFace->setMediaFile(TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/media_r3b.geo");
    geoFace->readMedia();
    FairGeoBuilder  *geoBuild = geoLoad->getGeoBuilder();
    FairGeoMedia    *geoMedia = geoFace->getMedia();
    const TGeoMedium *medBC408 = NEBULA_VET_BuildMaterial("BC408", geoMedia, geoBuild);
    const TGeoMedium *medCH2   = NEBULA_VET_BuildMaterial("polyethylene", geoMedia, geoBuild); // ATTENTION polyethylene
    const TGeoMedium *medAl    = NEBULA_VET_BuildMaterial("aluminium", geoMedia, geoBuild);

    // Prepare Transformations for cones
    TGeoRotation *r1 = new TGeoRotation();
    r1->RotateY(90);
    TGeoCombiTrans *trc1 = new TGeoCombiTrans(TGeoTranslation(-(NEBULA_VET_gPaddleBaseLength + NEBULA_VET_gPaddleConeLength), 0., 0.), *r1);
    trc1->SetName("trc1");
    trc1->RegisterYourself();

    TGeoRotation *r2 = new TGeoRotation();
    r2->RotateY(-90);
    TGeoCombiTrans *trc2 = new TGeoCombiTrans(TGeoTranslation(+(NEBULA_VET_gPaddleBaseLength + NEBULA_VET_gPaddleConeLength), 0., 0.), *r2);
    trc2->SetName("trc2");
    trc2->RegisterYourself();

    // Build shapes
    const TGeoShape *shapeBC408 = NEBULA_VET_BuildPaddleShape("NEBULAVETOshapeBC408",
                                  NEBULA_VET_gPaddleBaseLength,
                                  NEBULA_VET_gBC408Thickness_XY,
                                  NEBULA_VET_gBC408Thickness_Z,
                                  NEBULA_VET_gBC408ConeRadius,
                                  NEBULA_VET_gPaddleConeLength);

    NEBULA_VET_BuildPaddleShape("NEBULAVETOshapeAlWrappingSolid",
                     NEBULA_VET_gPaddleBaseLength,
                     NEBULA_VET_gBC408Thickness_XY + NEBULA_VET_gAlThickness,
                     NEBULA_VET_gBC408Thickness_Z + NEBULA_VET_gAlThickness,
                     NEBULA_VET_gBC408ConeRadius +  NEBULA_VET_gAlThickness,
                     NEBULA_VET_gPaddleConeLength);
    const TGeoShape *shapeAlWrapping = new TGeoCompositeShape("NEBULAVETOshapeAlWrapping", "NEBULAVETOshapeAlWrappingSolid - NEBULAVETOshapeBC408");

    NEBULA_VET_BuildPaddleShape("NEBULAVETOshapeTapeWrappingSolid",
                     NEBULA_VET_gPaddleBaseLength,
                     NEBULA_VET_gBC408Thickness_XY + NEBULA_VET_gAlThickness + NEBULA_VET_gTapeThickness,
                     NEBULA_VET_gBC408Thickness_Z + NEBULA_VET_gAlThickness + NEBULA_VET_gTapeThickness,
                     NEBULA_VET_gBC408ConeRadius +  NEBULA_VET_gAlThickness + NEBULA_VET_gTapeThickness,
                     NEBULA_VET_gPaddleConeLength);
    const TGeoShape *shapeTapeWrapping = new TGeoCompositeShape("NEBULAVETOshapeTapeWrapping", "NEBULAVETOshapeTapeWrappingSolid - NEBULAVETOshapeAlWrappingSolid");
    
    // Build the components of the Aluminium frame:
    TGeoRotation* rotplus = new TGeoRotation("rotplus");
    rotplus->RotateX(0.0);
    rotplus->RotateY(0.0);
    rotplus->RotateZ(0.0);
    TGeoCombiTrans* transfplus = new TGeoCombiTrans("transfplus",NEBULA_VET_gPaddleBaseLength/3.0,0.0,0.0,rotplus);
    transfplus->RegisterYourself();
    
    TGeoRotation* rotmin = new TGeoRotation("rotmin");
    rotmin->RotateX(0.0);
    rotmin->RotateY(0.0);
    rotmin->RotateZ(0.0);
    TGeoCombiTrans* transfmin = new TGeoCombiTrans("transfmin",-1.0*NEBULA_VET_gPaddleBaseLength/3.0,0.0,0.0,rotmin);
    transfmin->RegisterYourself();
    
    new TGeoBBox("NEBULAVETOAlbox1",NEBULA_VET_gAlumniniumStrip_Width/2.0,NEBULA_VET_gPaddleDistance_xy,NEBULA_VET_gPaddleDistance_z);
    new TGeoBBox("NEBULAVETOAlbox2",NEBULA_VET_gAlumniniumStrip_Width/1.0,NEBULA_VET_gBC408Thickness_XY+NEBULA_VET_gAlThickness+NEBULA_VET_gTapeThickness,NEBULA_VET_gBC408Thickness_Z+NEBULA_VET_gAlThickness+NEBULA_VET_gTapeThickness);
      
    TGeoCompositeShape* AlStrip1 = new TGeoCompositeShape("NEBULAVETOAlStrip1","NEBULAVETOAlbox1-NEBULAVETOAlbox2:");
    TGeoVolume* AlStrip1Vol = new TGeoVolume("NEBULAVETOAlStrip1Vol",AlStrip1,medAl);
    AlStrip1Vol->SetLineColor(NEBULA_VET_gFrameColor);
    
    TGeoCompositeShape* AlStrip2 = new TGeoCompositeShape("NEBULAVETOAlStrip2","NEBULAVETOAlbox1-NEBULAVETOAlbox2");
    TGeoVolume* AlStrip2Vol = new TGeoVolume("NEBULAVETOAlStrip2Vol",AlStrip2,medAl);
    AlStrip2Vol->SetLineColor(NEBULA_VET_gFrameColor);

    // Build total Volumes. Note that the volume names are important and reappear in R3BNeuland()
    TGeoVolume *volBC408 = new TGeoVolume("NEBULAVETOvolBC408", shapeBC408, medBC408);
    volBC408->SetLineColor(NEBULA_VET_gDetectorColor); 
    TGeoVolume *volAlWrapping = new TGeoVolume("NEBULAVETOvolAlWrapping", shapeAlWrapping, medAl);
    volAlWrapping->SetLineColor(NEBULA_VET_gDetectorColor);
    TGeoVolume *volTapeWrapping = new TGeoVolume("NEBULAVETOvolTapeWrapping", shapeTapeWrapping, medCH2);
    volTapeWrapping->SetLineColor(NEBULA_VET_gDetectorColor);

    // Make the elementary assembly
    TGeoVolume *volPaddle = new TGeoVolumeAssembly("NEBULAVETOvolPaddle");
    volPaddle->AddNode(volBC408, 1);
    volPaddle->AddNode(volAlWrapping, 1);
    volPaddle->AddNode(volTapeWrapping, 1);
    
    if (NEBULA_VET_gAlumniniumStrip_Thickness>0.0)
    {
        volPaddle->AddNode(AlStrip1Vol, 1, transfplus);
        volPaddle->AddNode(AlStrip2Vol, 1, transfmin);
    }

    return volPaddle;
}

// Main function definition:
void NEBULA_VETO_BuildGeometry()
{
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Specify global variables:
    NEBULA_VET_gAlumniniumStrip_Width = Inputs->GetInputDouble("NEBULA_VET_AluminiumFrame_Width","cm");
    NEBULA_VET_gAlumniniumStrip_Thickness = Inputs->GetInputDouble("NEBULA_VET_AluminiumFrame_Thickness","cm");
    NEBULA_VET_gPaddleBaseLength = Inputs->GetInputDouble("NEBULA_VET_Total_BarLength","cm")/2.0;
    NEBULA_VET_gPaddleConeLength = Inputs->GetInputDouble("NEBULA_VET_EndPoint_ConeLength","cm");
    NEBULA_VET_gBC408ConeRadius = Inputs->GetInputDouble("NEBULA_VET_EndPoint_ConeRadius","cm");
    NEBULA_VET_gBC408Thickness_XY = Inputs->GetInputDouble("NEBULA_VET_BC408_BarThickness_XY","cm")/2.0;
    NEBULA_VET_gBC408Thickness_Z = Inputs->GetInputDouble("NEBULA_VET_BC408_BarThickness_Z","cm")/2.0;
    NEBULA_VET_gAlThickness = Inputs->GetInputDouble("NEBULA_VET_ReflectionFoil_Thickness","cm");
    NEBULA_VET_gTapeThickness = Inputs->GetInputDouble("NEBULA_VET_WrappingTape_Thickness","cm");
    NEBULA_VET_gAirGabThickness = Inputs->GetInputDouble("NEBULA_VET_Bars_AirGab","cm");
    NEBULA_VET_gPaddlesPerPlane = Inputs->GetInputInteger("NEBULA_VET_Number_of_Bars_in_plane");
    NEBULA_VET_gVisualizeOutput = Inputs->GetInputBoolian("NEBULA_VET_Visualize_Geometry_DuringBuild");
    NEBULA_VET_gFrameColor = Inputs->GetInputInteger("NEBULA_VET_Frame_Color");
    NEBULA_VET_gDetectorColor = Inputs->GetInputInteger("NEBULA_VET_Detector_Color");

    NEBULA_VET_gPaddleDistance_xy = NEBULA_VET_gBC408Thickness_XY + NEBULA_VET_gAlThickness + NEBULA_VET_gTapeThickness + NEBULA_VET_gAlumniniumStrip_Thickness/2.0;
    if ((NEBULA_VET_gAlumniniumStrip_Thickness/2.0)<NEBULA_VET_gAirGabThickness) {NEBULA_VET_gPaddleDistance_xy = NEBULA_VET_gBC408Thickness_XY + NEBULA_VET_gAlThickness + NEBULA_VET_gTapeThickness + NEBULA_VET_gAirGabThickness;}
    
    NEBULA_VET_gPaddleDistance_z = NEBULA_VET_gBC408Thickness_Z + NEBULA_VET_gAlThickness + NEBULA_VET_gTapeThickness + NEBULA_VET_gAlumniniumStrip_Thickness;
    if (NEBULA_VET_gAlumniniumStrip_Thickness<NEBULA_VET_gAirGabThickness) {NEBULA_VET_gPaddleDistance_z = NEBULA_VET_gBC408Thickness_Z + NEBULA_VET_gAlThickness + NEBULA_VET_gTapeThickness + NEBULA_VET_gAirGabThickness;}
    
    TString geoFileName = Inputs->GetInputString("TheOutputPath") + Inputs->GetInputString("NEBULA_VET_Geometry_FileName");
    
    // Obtain NEBULA parameters:
    NEBULA_gNumberOfDoublePlanes = Inputs->GetInputInteger("NEBULA_Number_of_DoublePlaneModules");
    NEBULA_gModuleDistance = Inputs->GetInputDouble("NEBULA_Distance_BetweenModules","cm");
    NEBULA_gBC408Thickness = Inputs->GetInputDouble("NEBULA_BC408_BarThickness","cm")/2.0;
    NEBULA_gAlThickness = Inputs->GetInputDouble("NEBULA_ReflectionFoil_Thickness","cm");
    NEBULA_gTapeThickness = Inputs->GetInputDouble("NEBULA_WrappingTape_Thickness","cm");
    NEBULA_gAirGabThickness = Inputs->GetInputDouble("NEBULA_Bars_AirGab","cm");
    NEBULA_gAlumniniumStrip_Thickness = Inputs->GetInputDouble("NEBULA_AluminiumFrame_Thickness","cm");
    
    NEBULA_gPaddleDistance_z = NEBULA_gBC408Thickness + NEBULA_gAlThickness + NEBULA_gTapeThickness + NEBULA_gAlumniniumStrip_Thickness;
    if (NEBULA_gAlumniniumStrip_Thickness<NEBULA_gAirGabThickness) {NEBULA_gPaddleDistance_z = NEBULA_gBC408Thickness + NEBULA_gAlThickness + NEBULA_gTapeThickness + NEBULA_gAirGabThickness;}
    
    // ------------------------------------------------------------------------
    
    // Begin construction of the geometry:
    TGeoRotation* rot90 = new TGeoRotation();
    rot90->RotateZ(90.);

    new FairGeoLoader("TGeo", "FairGeoLoader");

    auto volPaddle = NEBULA_VET_BuildPaddleVolume(); // Will make sure gGeoManager is loaded

    auto volNEBULAVETO = new TGeoVolumeAssembly("volNEBULAVETO");
    
    Int_t nindex = 0;
    Double_t Z_Location = 0.0;
    
    for (Int_t kmodule = 0; kmodule<NEBULA_gNumberOfDoublePlanes; ++kmodule)
    {
        for (Double_t a = -NEBULA_VET_gPaddlesPerPlane * NEBULA_VET_gPaddleDistance_xy + NEBULA_VET_gPaddleDistance_xy; a < NEBULA_VET_gPaddleDistance_xy * NEBULA_VET_gPaddlesPerPlane; a += NEBULA_VET_gPaddleDistance_xy * 2)
        {
            Z_Location = 4.0*NEBULA_gPaddleDistance_z*((Int_t) kmodule) + NEBULA_gModuleDistance*((Int_t) kmodule);
            nindex++;
            volNEBULAVETO->AddNode(volPaddle, nindex, new TGeoCombiTrans(a, 0.0, Z_Location, rot90));
        }
    }

    auto top = new TGeoVolumeAssembly("TOP");
    gGeoManager->SetTopVolume(top);
    
    // Add NeuLAND total volume to the top:
    TGeoRotation* rottotal = new TGeoRotation("rottotal");
    rottotal->RotateX(Inputs->GetInputDouble("NEBULA_VET_x_rotation_angle","degree"));
    rottotal->RotateY(Inputs->GetInputDouble("NEBULA_VET_y_rotation_angle","degree"));
    rottotal->RotateZ(Inputs->GetInputDouble("NEBULA_VET_z_rotation_angle","degree"));
    TGeoCombiTrans* transtotal = new TGeoCombiTrans("transtotal",Inputs->GetInputDouble("NEBULA_VET_center_x_position","cm"),Inputs->GetInputDouble("NEBULA_VET_center_y_position","cm"),
                                                    Inputs->GetInputDouble("NEBULA_VET_front_z_position","cm") + NEBULA_VET_gPaddleDistance_z, rottotal);
    transtotal->RegisterYourself();

    // The new TGeoTranslation is necessary because of ROOT
    top->AddNode(volNEBULAVETO, 1, transtotal);

    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();
    // gGeoManager->Test();

    TFile geoFile(geoFileName, "RECREATE");
    top->Write();
    geoFile.Close();

    cout << "Done. " << nindex << " Paddles in Planes = 1 sp -> " << geoFileName << endl;

    if (NEBULA_VET_gVisualizeOutput==kTRUE)
    {
        gStyle->SetCanvasPreferGL(kTRUE);
        volNEBULAVETO->Draw("ogl");
        auto v = (TGLViewer*)gPad->GetViewer3D();
        v->CurrentCamera().RotateRad(0, 0.1);
        v->SetStyle(TGLRnrCtx::kOutline);
        v->RequestDraw();
    }
    
    Inputs->PrintAllErrors();
}
