// Declaration of Global variables:
Double_t gAlumniniumStrip_Width;
Double_t gAlumniniumStrip_Thickness;
Double_t gPaddleDistance_xy;
Double_t gPaddleDistance_z;
Double_t gPaddleBaseLength;
Double_t gPaddleConeLength;
Double_t gBC408ConeRadius;
Double_t gBC408Thickness;
Double_t gAlThickness;
Double_t gTapeThickness;
Double_t gAirGabThickness;
Double_t gModularDistance;
Int_t gNumberOfDoublePlanes_PerModule;
Int_t gPaddlesPerPlane;
Int_t gNumberOfDoublePlanes;
Int_t gFrameColor;
Int_t gDetectorColor;
Bool_t gVisualizeOutput;
Bool_t gDoModularSplit;

// Function to retrieve building materials:
TGeoMedium* BuildMaterial(TString const material, FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild)
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
TGeoShape* BuildPaddleShape(TString const &name, Double_t const length, Double_t const width, Double_t const coneRadius, Double_t const coneLength)
{
    new TGeoBBox(name + "Box", length, width, width);
    new TGeoCone(name + "Cone", coneLength + 0.001, 0., coneRadius, 0., width * TMath::Sqrt(2.));
    new TGeoBBox(name + "Conebox", width, width, coneLength);
    
    TGeoShape *shape = new TGeoCompositeShape(name,name + "Box + ((" + name + "Conebox*" + name + "Cone):trc1) + ((" + name + "Conebox*" + name + "Cone):trc2)");
    return shape;
}

// Function to build a single NeuLAND bar. It consists of 3 paddle shapes: Inner: BC408 scintillator. 
// middle: Al reflection foil and outer: wrapping tape. It also contaisn Aluminium frame holders.
TGeoVolume* BuildPaddleVolume()
{
    // Load Interfaces to build materials
    FairGeoLoader    *geoLoad = FairGeoLoader::Instance();
    FairGeoInterface *geoFace = geoLoad->getGeoInterface();
    geoFace->setMediaFile(TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/media_r3b.geo");
    geoFace->readMedia();
    FairGeoBuilder  *geoBuild = geoLoad->getGeoBuilder();
    FairGeoMedia    *geoMedia = geoFace->getMedia();
    const TGeoMedium *medBC408 = BuildMaterial("BC408", geoMedia, geoBuild);
    const TGeoMedium *medCH2   = BuildMaterial("polyethylene", geoMedia, geoBuild); // ATTENTION polyethylene
    const TGeoMedium *medAl    = BuildMaterial("aluminium", geoMedia, geoBuild);

    // Prepare Transformations for cones
    TGeoRotation *r1 = new TGeoRotation();
    r1->RotateY(90);
    TGeoCombiTrans *trc1 = new TGeoCombiTrans(TGeoTranslation(-(gPaddleBaseLength + gPaddleConeLength), 0., 0.), *r1);
    trc1->SetName("trc1");
    trc1->RegisterYourself();

    TGeoRotation *r2 = new TGeoRotation();
    r2->RotateY(-90);
    TGeoCombiTrans *trc2 = new TGeoCombiTrans(TGeoTranslation(+(gPaddleBaseLength + gPaddleConeLength), 0., 0.), *r2);
    trc2->SetName("trc2");
    trc2->RegisterYourself();

    // Build shapes
    const TGeoShape *shapeBC408 = BuildPaddleShape("shapeBC408",
                                  gPaddleBaseLength,
                                  gBC408Thickness,
                                  gBC408ConeRadius,
                                  gPaddleConeLength);

    BuildPaddleShape("shapeAlWrappingSolid",
                     gPaddleBaseLength,
                     gBC408Thickness + gAlThickness,
                     gBC408ConeRadius +  gAlThickness,
                     gPaddleConeLength);
    const TGeoShape *shapeAlWrapping = new TGeoCompositeShape("shapeAlWrapping", "shapeAlWrappingSolid - shapeBC408");

    BuildPaddleShape("shapeTapeWrappingSolid",
                     gPaddleBaseLength,
                     gBC408Thickness + gAlThickness + gTapeThickness,
                     gBC408ConeRadius +  gAlThickness + gTapeThickness,
                     gPaddleConeLength);
    const TGeoShape *shapeTapeWrapping = new TGeoCompositeShape("shapeTapeWrapping", "shapeTapeWrappingSolid - shapeAlWrappingSolid");
    
    // Build the components of the Aluminium frame:
    TGeoRotation* rotplus = new TGeoRotation("rotplus");
    rotplus->RotateX(0.0);
    rotplus->RotateY(0.0);
    rotplus->RotateZ(0.0);
    TGeoCombiTrans* transfplus = new TGeoCombiTrans("transfplus",gPaddleBaseLength/3.0,0.0,0.0,rotplus);
    transfplus->RegisterYourself();
    
    TGeoRotation* rotmin = new TGeoRotation("rotmin");
    rotmin->RotateX(0.0);
    rotmin->RotateY(0.0);
    rotmin->RotateZ(0.0);
    TGeoCombiTrans* transfmin = new TGeoCombiTrans("transfmin",-1.0*gPaddleBaseLength/3.0,0.0,0.0,rotmin);
    transfmin->RegisterYourself();
    
    new TGeoBBox("Albox1",gAlumniniumStrip_Width/2.0,gPaddleDistance_xy,gPaddleDistance_z);
    new TGeoBBox("Albox2",gAlumniniumStrip_Width/1.0,gBC408Thickness+gAlThickness+gTapeThickness,gBC408Thickness+gAlThickness+gTapeThickness);
      
    TGeoCompositeShape* AlStrip1 = new TGeoCompositeShape("AlStrip1","Albox1-Albox2:");
    TGeoVolume* AlStrip1Vol = new TGeoVolume("AlStrip1Vol",AlStrip1,medAl);
    AlStrip1Vol->SetLineColor(gFrameColor);
    
    TGeoCompositeShape* AlStrip2 = new TGeoCompositeShape("AlStrip2","Albox1-Albox2");
    TGeoVolume* AlStrip2Vol = new TGeoVolume("AlStrip2Vol",AlStrip2,medAl);
    AlStrip2Vol->SetLineColor(gFrameColor);

    // Build total Volumes. Note that the volume names are important and reappear in R3BNeuland()
    TGeoVolume *volBC408 = new TGeoVolume("volBC408", shapeBC408, medBC408);
    volBC408->SetLineColor(gDetectorColor); 
    TGeoVolume *volAlWrapping = new TGeoVolume("volAlWrapping", shapeAlWrapping, medAl);
    volAlWrapping->SetLineColor(gDetectorColor);
    TGeoVolume *volTapeWrapping = new TGeoVolume("volTapeWrapping", shapeTapeWrapping, medCH2);
    volTapeWrapping->SetLineColor(gDetectorColor);

    // Make the elementary assembly
    TGeoVolume *volPaddle = new TGeoVolumeAssembly("volPaddle");
    volPaddle->AddNode(volBC408, 1);
    volPaddle->AddNode(volAlWrapping, 1);
    volPaddle->AddNode(volTapeWrapping, 1);
    
    if (gAlumniniumStrip_Thickness>0.0)
    {
        volPaddle->AddNode(AlStrip1Vol, 1, transfplus);
        volPaddle->AddNode(AlStrip2Vol, 1, transfmin);
    }

    return volPaddle;
}

// Main function definition:
void NeuLAND_BuildGeometry()
{
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Specify global variables:
    gAlumniniumStrip_Width = Inputs->GetInputDouble("NeuLAND_AluminiumFrame_Width","cm");
    gAlumniniumStrip_Thickness = Inputs->GetInputDouble("NeuLAND_AluminiumFrame_Thickness","cm");
    gPaddleBaseLength = Inputs->GetInputDouble("NeuLAND_Total_BarLength","cm")/2.0;
    gPaddleConeLength = Inputs->GetInputDouble("NeuLAND_EndPoint_ConeLength","cm");
    gBC408ConeRadius = Inputs->GetInputDouble("NeuLAND_EndPoint_ConeRadius","cm");
    gBC408Thickness = Inputs->GetInputDouble("NeuLAND_BC408_BarThickness","cm")/2.0;
    gAlThickness = Inputs->GetInputDouble("NeuLAND_ReflectionFoil_Thickness","cm");
    gTapeThickness = Inputs->GetInputDouble("NeuLAND_WrappingTape_Thickness","cm");
    gAirGabThickness = Inputs->GetInputDouble("NeuLAND_Bars_AirGab","cm");
    gPaddlesPerPlane = Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane");
    gNumberOfDoublePlanes = Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes");
    gVisualizeOutput = Inputs->GetInputBoolian("NeuLAND_Visualize_Geometry_DuringBuild");
    gFrameColor = Inputs->GetInputInteger("NeuLAND_Frame_Color");
    gDetectorColor = Inputs->GetInputInteger("NeuLAND_Detector_Color");
    gNumberOfDoublePlanes_PerModule = Inputs->GetInputInteger("NeuLAND_DoublePlanes_PerModule");
    gDoModularSplit = Inputs->GetInputBoolian("NeuLAND_UseModularSplit");
    gModularDistance = Inputs->GetInputDouble("NeuLAND_Distance_BetweenModules","cm");

    gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness/2.0;
    if ((gAlumniniumStrip_Thickness/2.0)<gAirGabThickness) {gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAirGabThickness;}
    
    gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness;
    if (gAlumniniumStrip_Thickness<gAirGabThickness) {gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAirGabThickness;}
    
    TString geoFileName = Inputs->GetInputString("TheOutputPath") + Inputs->GetInputString("NeuLAND_Geometry_FileName");
    
    // ------------------------------------------------------------------------
    
    // Begin construction of the geometry:
    TGeoRotation* rot90 = new TGeoRotation();
    rot90->RotateZ(90.);

    new FairGeoLoader("TGeo", "FairGeoLoader");

    auto volPaddle = BuildPaddleVolume(); // Will make sure gGeoManager is loaded

    auto volNeuland = new TGeoVolumeAssembly("volNeuland");
    
    const Int_t nPlanes = gNumberOfDoublePlanes * 2;
    Int_t ModularSkip = 0;
    Int_t nindex = 0, nPlane = 0;
    for (Double_t b = -nPlanes * gPaddleDistance_z + gPaddleDistance_z; b < nPlanes * gPaddleDistance_z;
         b += gPaddleDistance_z * 2)
    {
        nPlane++;
        for (Double_t a = -gPaddlesPerPlane * gPaddleDistance_xy + gPaddleDistance_xy; a < gPaddleDistance_xy * gPaddlesPerPlane;
             a += gPaddleDistance_xy * 2)
        {
            if ((nindex!=0)&&(nindex%(gNumberOfDoublePlanes_PerModule*gPaddlesPerPlane*2)==0)&&(gDoModularSplit==kTRUE)) {ModularSkip = ModularSkip + 1;}
            
            nindex++;
            
            if (nPlane % 2 == 1)
            {
                volNeuland->AddNode(volPaddle, nindex, new TGeoTranslation(0.0, a, b + ((Int_t) ModularSkip)*gModularDistance));
            }
            else
            {
                volNeuland->AddNode(volPaddle, nindex, new TGeoCombiTrans(a, 0.0, b + ((Int_t) ModularSkip)*gModularDistance, rot90));
            }
        }
    }

    auto top = new TGeoVolumeAssembly("TOP");
    gGeoManager->SetTopVolume(top);
    
    // Add NeuLANd total volume to the top:
    TGeoRotation* rottotal = new TGeoRotation("rottotal");
    rottotal->RotateX(Inputs->GetInputDouble("NeuLAND_x_rotation_angle","degree"));
    rottotal->RotateY(Inputs->GetInputDouble("NeuLAND_y_rotation_angle","degree"));
    rottotal->RotateZ(Inputs->GetInputDouble("NeuLAND_z_rotation_angle","degree"));
    TGeoCombiTrans* transtotal = new TGeoCombiTrans("transtotal",Inputs->GetInputDouble("NeuLAND_center_x_position","cm"),Inputs->GetInputDouble("NeuLAND_center_y_position","cm"),
                                                    Inputs->GetInputDouble("NeuLAND_front_z_position","cm") + gPaddleDistance_z*((Int_t) gNumberOfDoublePlanes)*2.0, rottotal);
    transtotal->RegisterYourself();

    // The new TGeoTranslation is necessary because of ROOT
    top->AddNode(volNeuland, 1, transtotal);

    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();
    // gGeoManager->Test();

    TFile geoFile(geoFileName, "RECREATE");
    top->Write();
    geoFile.Close();

    cout << "Done. " << nindex << " Paddles in " << nPlane << " Planes = " << gNumberOfDoublePlanes << " dp -> " << geoFileName << endl;

    if (gVisualizeOutput==kTRUE)
    {
        gStyle->SetCanvasPreferGL(kTRUE);
        volNeuland->Draw("ogl");
        auto v = (TGLViewer*)gPad->GetViewer3D();
        v->CurrentCamera().RotateRad(0, 0.1);
        v->SetStyle(TGLRnrCtx::kOutline);
        v->RequestDraw();
    }
    
    Inputs->PrintAllErrors();
}
