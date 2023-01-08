/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// FINAL CALIFA CARREL + iPHOS
// Usage after loading:
// create_califa_barrel_iphos_geo(A, B, C)
// A: geoTag. Name tagging the output root file.
// B: known alignment for some experiments
//    [nominal, s522, s509, s455, s515, s494, s444, s467]
// C: File with Installed Crystal
//    [./califa_AllCrystalsInstalled.txt,
//     ./califa_InstalledCrystals_March2021.txt,
//     ./califa_InstalledCrystals_Nov2019.txt]
// Requires file CLF-ALL-oneCrystal.txt for crystals coordinates
// Requires file CLF-ALL-onePart.txt for alveoli coordinates

#include "TGeoManager.h"
#include "TMath.h"
#include <iomanip>
#include <iostream>

const int nbcrystals = 2432;

Bool_t isCrystalInstalled(Int_t alvType, Int_t alveolusCopy, Int_t instCry[]);

void create_califa_geo(const char* expNumber = "nominal", TString geoTag = "full");

void create_califa_geo(const int index)
{
    if (index == 1)
        create_califa_geo("s467");
    else if (index == 2)
        create_califa_geo("s455");
}

void create_califa_geo(const char* expNumber, TString geoTag)
{
    TGeoRotation* fRefRot = NULL;
    TGeoManager* gGeoMan = NULL;

    // -------   Load media from media file   -------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media_r3b.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;

    TString installedCrystalsFile = "califa_AllCrystalsInstalled.txt";
    if ((strncmp(expNumber, "s522", 4) == 0) || (strncmp(expNumber, "s515", 4) == 0) ||
        (strncmp(expNumber, "s509", 4) == 0) || (strncmp(expNumber, "s494", 4) == 0) ||
        (strncmp(expNumber, "s455", 4) == 0) || (strncmp(expNumber, "s444", 4) == 0))
    {
        installedCrystalsFile = "califa_InstalledCrystals_March2021.txt";
        geoTag = "v2021.3";
    }
    else if (strncmp(expNumber, "s467", 4) == 0)
    {
        installedCrystalsFile = "califa_InstalledCrystals_Nov2019.txt";
        geoTag = "v2019.11";
    }
    else if (strncmp(expNumber, "nominal", 7) == 0)
    {
        // Nominal configuration, nothing to say.
        std::cout << std::endl;
        std::cout << "\033[35m"
                  << "Loading the ideal nominal configuration."
                  << " \033[0m" << std::endl;
    }
    else
    {
        std::cout << std::endl
                  << "\033[35m WARNING: Using a non-standard experiment number (" << expNumber
                  << ") therefore the CALIFA geometry will be generated with all crystals using the file "
                     "<califa_AllCrystalsInstalled.txt> \033[0m"
                  << std::endl;
        std::cout << "\033[33m"
                  << "  -> Is this really what you want??"
                  << " \033[0m" << std::endl;
    }

    /*
        if ((strncmp(expNumber, "s522", 4) == 0 &&
             strncmp(installedCrystalsFile, "califa_InstalledCrystals_March2021.txt", 38) != 0) ||
            (strncmp(expNumber, "s509", 4) == 0 &&
             strncmp(installedCrystalsFile, "califa_InstalledCrystals_March2021.txt", 38) != 0) ||
            (strncmp(expNumber, "s455", 4) == 0 &&
             strncmp(installedCrystalsFile, "califa_InstalledCrystals_March2021.txt", 38) != 0) ||
            (strncmp(expNumber, "s515", 4) == 0 &&
             strncmp(installedCrystalsFile, "califa_InstalledCrystals_March2021.txt", 38) != 0) ||
            (strncmp(expNumber, "s494", 4) == 0 &&
             strncmp(installedCrystalsFile, "califa_InstalledCrystals_March2021.txt", 38) != 0) ||
            (strncmp(expNumber, "s444", 4) == 0 &&
             strncmp(installedCrystalsFile, "califa_InstalledCrystals_March2021.txt", 38) != 0) ||
            (strncmp(expNumber, "s467", 4) == 0 &&
             strncmp(installedCrystalsFile, "califa_InstalledCrystals_Nov2019.txt", 38) != 0))
        {
            std::cout << std::endl
                      << "WARNING: Using a non-standard combination of experiment number (" << expNumber
                      << ") and installed crystals file (" << installedCrystalsFile << ")" << std::endl;
            std::cout << "\033[33m"
                      << "  -> Is this really what you want??"
                      << " \033[0m" << std::endl;
        }*/

    TString fFilePath = geoPath + "/macros/r3b/geo/" + installedCrystalsFile;
    fFilePath.ReplaceAll("./", "/");
    fFilePath.ReplaceAll("//", "/");

    std::cout << std::endl
              << " geoTag: "
              << "\033[33m" << geoTag << " \033[0m" << std::endl;
    std::cout << " alignment (nominal or experiment code): "
              << "\033[33m" << expNumber << " \033[0m" << std::endl;
    std::cout << " installed Crystals File: "
              << "\033[33m" << fFilePath << " \033[0m" << std::endl
              << std::endl;

    if (strncmp(expNumber, "s522", 4) == 0 || strncmp(expNumber, "s509", 4) == 0 ||
        strncmp(expNumber, "s494", 4) == 0 || strncmp(expNumber, "s444", 4) == 0 || strncmp(expNumber, "s467", 4) == 0)
    {
        std::cout << "\033[31m No alignment data for this experiment yet \033[0m " << std::endl << std::endl;
    }

    ifstream wc1, in1, in2;
    wc1.open(fFilePath.Data());
    in1.open((geoPath + "/macros/r3b/geo/CLF-ALL-onePart.txt").Data());
    in2.open((geoPath + "/macros/r3b/geo/CLF-ALL-oneCrystal.txt").Data());

    if (wc1.fail())
    {
        std::cout << "\033[31m FATAL ERROR: Unable to open file or read problem in \033[0m "
                  << "\033[31m" << installedCrystalsFile << " \033[0m" << std::endl
                  << std::endl;
        return;
    }

    Int_t installedCrystals[nbcrystals];
    Int_t read = 0;
    Int_t crycounter = 0;
    for (Int_t i = 0; i < nbcrystals; i++)
        installedCrystals[i] = 0;
    while (1)
    { // reading the file with all alveoli vertices
        wc1 >> read;
        if (!wc1.good())
        {
            break;
        }
        if (crycounter > (nbcrystals - 1))
        {
            std::cout << "\033[31m FATAL ERROR: more lines than crystals in the file \033[0m "
                      << "\033[31m" << installedCrystalsFile << " \033[0m" << std::endl
                      << std::endl;
            return;
        }
        installedCrystals[crycounter] = read;
        crycounter++;
    }

    // -------   Geometry file name (output)   ------------------------
    TString geoFileName = geoPath + "/geometry/califa_";
    geoFileName = geoFileName + geoTag + ".geo.root";

    // -----------------   Get and create the required media    -------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("Air");
    if (!mAir)
        Fatal("Main", "FairMedium Air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pAirMedium = gGeoMan->GetMedium("Air");
    if (!pAirMedium)
        Fatal("Main", "Medium Air not found");

    FairGeoMedium* mCsI = geoMedia->getMedium("CsI");
    if (!mCsI)
        Fatal("Main", "FairMedium CsI not found");
    geoBuild->createMedium(mCsI);
    TGeoMedium* pCsIMedium = gGeoMan->GetMedium("CsI");
    if (!pCsIMedium)
        Fatal("Main", "Medium CsI not found");

    FairGeoMedium* mCar = geoMedia->getMedium("CarbonFibre");
    if (!mCar)
        Fatal("Main", "FairMedium CarbonFibre not found");
    geoBuild->createMedium(mCar);
    TGeoMedium* pCarbonFibreMedium = gGeoMan->GetMedium("CarbonFibre");
    if (!pCarbonFibreMedium)
        Fatal("Main", "Medium CarbonFibre not found");

    FairGeoMedium* mTfl = geoMedia->getMedium("Tefflon");
    if (!mTfl)
        Fatal("Main", "FairMedium Tefflon not found");
    geoBuild->createMedium(mTfl);
    TGeoMedium* pWrappingMedium = gGeoMan->GetMedium("Tefflon");
    if (!pWrappingMedium)
        Fatal("Main", "Medium Tefflon not found");
    // ----------------------------------------------------------------

    // --------------   Create geometry and top volume  ---------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName("CALIFAgeom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    gGeoMan->SetTopVolume(top);
    // ----------------------------------------------------------------

    // Defintion of the Mother Volume
    auto pCBWorldOut = new TGeoTube("Califa_boxOut",
                                    0.,               // Rmin
                                    59.,              // Rmax
                                    (55. + 70) / 2.); // half length
    auto trans0 = new TGeoCombiTrans("trans0", 0., 0., 7.5, fRefRot);
    trans0->RegisterYourself();

    TGeoShape* pCBWorldIn1 = new TGeoTube("Califa_Centerpart1", // hole to accommodate the tracker
                                          0.,                   // Rmin
                                          27.,                  // Rmax
                                          130 / 2.);            // half length
    auto trans1 = new TGeoCombiTrans("trans1", 0., 0., 7.5, fRefRot);
    trans1->RegisterYourself();

    auto Part2 = new TGeoSphere("Part2", 38., 73.5, 7., 90., 0., 360.);
    auto trans2 = new TGeoCombiTrans("trans2", 0, 0, 0., fRefRot);
    trans2->RegisterYourself();

    auto pCBWorld = new TGeoCompositeShape("Califa_box", "Califa_boxOut:trans0-Califa_Centerpart1:trans1+Part2:trans2");

    auto pWorld = new TGeoVolume("CalifaWorld", pCBWorld, pAirMedium);
    auto t0 = new TGeoCombiTrans(0., 0., 0., fRefRot);
    top->AddNode(pWorld, 0, t0);

    // FINAL CALIFA CARREL + iPHOS VERSION (SINCE NOV 2019)
    const Int_t N_ALV_TYPES = 23; // alveolar structures
    const Int_t N_CRY_TYPES = 85; // crystal elements

    Int_t counter = 0;
    Float_t x, y, z;

    Double_t wrapping_thickness = 0.0065; // in cm.
    // target reference in mm. OFFSET INFO FROM UVIGO
    TVector3 target_ref(4.1, 2304.0809, 325.0);

    // 23 geometries, 8 vertices, outer and inner: (23*8*2)
    TVector3 points[N_ALV_TYPES * 8 * 2];
    TVector3 points_local[N_ALV_TYPES * 8 * 2];

    // 18 alv with 4 cry, 4 with 3 cry, 1 with 1 cry, 8 vertices each: (15*4+4*3+3*4+1)*8=85*8
    TVector3 points_cry[N_CRY_TYPES * 8];
    TVector3 points_cry_local[N_CRY_TYPES * 8];

    while (1)
    { // reading the file with all alveoli vertices
        in1 >> x >> y >> z;
        if (!in1.good())
            break;
        points[counter].SetXYZ(
            (x - target_ref.X()) / 10, (y - target_ref.Y()) / 10, (z - target_ref.Z()) / 10); // in cm;
        if (points[counter].X() > 100 || points[counter].Y() > 100 || points[counter].Z() > 100)
            cout << "warn: points exceed top volume!!" << endl;
        counter++;
        // printf("x=%8f, y=%8f, z=%8f\n",x,y,z);
    }
    if (counter != N_ALV_TYPES * 8 * 2)
        cout << "PROBLEM! Counter=" << counter << endl;
    counter = 0;

    while (1)
    { // reading the file with all crystal vertices
        in2 >> x >> y >> z;
        if (!in2.good())
            break;
        points_cry[counter].SetXYZ(
            (x - target_ref.X()) / 10, (y - target_ref.Y()) / 10, (z - target_ref.Z()) / 10); // in cm;
        if (points_cry[counter].X() > 100 || points_cry[counter].Y() > 100 || points_cry[counter].Z() > 100)
            cout << "warn: points exceed top volume!!" << endl;
        counter++;
        // printf("x=%8f, y=%8f, z=%8f\n",x,y,z);
    }
    if (counter != N_CRY_TYPES * 8)
        cout << "PROBLEM! Counter=" << counter << endl;

    // centers of faces
    TVector3 center[N_ALV_TYPES * 2 * 2]; // 23 geometries, 2 face centers, outer and inner (23*2*2)
    TVector3 x_uni[N_ALV_TYPES * 2 * 2];  // unit vectors for each face
    TVector3 y_uni[N_ALV_TYPES * 2 * 2];
    TVector3 z_uni[N_ALV_TYPES * 2 * 2];
    TVector3 center_cry[N_CRY_TYPES * 2]; // 85 types of crystals, 2 face centers (85*2)
    TVector3 x_uni_cry[N_CRY_TYPES * 2];  // unit vectors for each face for crystals
    TVector3 y_uni_cry[N_CRY_TYPES * 2];
    TVector3 z_uni_cry[N_CRY_TYPES * 2];
    TRotation rot[N_ALV_TYPES * 2 * 2]; // calculated in each face, but only 23 are really different if all is ok
    TRotation rot_cry[N_CRY_TYPES * 2]; // only a few are really different if all is ok
    // volume centers
    TVector3 alv_cm[N_ALV_TYPES * 2];     // 23 geometries, outer and inner
    TVector3 alv_cm_rot[N_ALV_TYPES * 2]; // 23 geometries, outer and inner after final rotation
    TVector3 cry_cm[N_CRY_TYPES];         // 85 types of crystals

    // The center of the faces are first calculated. Then, the unit vectors defining the axis in each faces
    // Third, the rotation moving from the lab system to the unit vectors previously found. To define the
    // volume in Arb8 style, we need the 8 corners in the local frustrum coordinates. Then, we should express
    // the vertices in the coordinate system of the volume center of mass (cm)
    for (Int_t i = 0; i < N_ALV_TYPES * 2 * 2; i++)
    { // for 23 geometries, 2 face centers, outer and inner (23*2*2)
        center[i] = points[i * 4] + points[i * 4 + 1] + points[i * 4 + 2] + points[i * 4 + 3]; // face centers
        center[i] *= 0.25;                                                                     // face centers
        // center[i].Print();
        z_uni[i] = (points[i * 4 + 1] - points[i * 4]).Cross(points[i * 4 + 2] - points[i * 4 + 1]);
        z_uni[i] = z_uni[i].Unit(); // normal to face center
        x_uni[i] = points[i * 4 + 1] - points[i * 4];
        if (i > 75)
        { // for the irregular alveoli at the end of the iPhos, the X axis is taken from other vertices
            x_uni[i] = points[i * 4 + 2] - points[i * 4 + 3];
        }
        x_uni[i] = x_uni[i].Unit();          // unit along X
        y_uni[i] = z_uni[i].Cross(x_uni[i]); // unit along Y

        // x_uni[i].Print();  y_uni[i].Print();   z_uni[i].Print();

        // calculate rotation matrix for the 23 geometries (should be repeated 4 times, just checking)
        rot[i].SetZAxis(z_uni[i], x_uni[i]);

        // calculate vertices coordinates in local alveoli coordinates (not valid for the definition of the Arb8, see
        // below) points_local[i*4]=rot[i].Inverse()*(points[i*4]-center[i]);
        // points_local[i*4+1]=rot[i].Inverse()*(points[i*4+1]-center[i]);
        // points_local[i*4+2]=rot[i].Inverse()*(points[i*4+2]-center[i]);
        // points_local[i*4+3]=rot[i].Inverse()*(points[i*4+3]-center[i]);
        // cout<< "Points in face coordinates: "<< endl;
        // points_local[i*4].Print();points_local[i*4+1].Print();points_local[i*4+2].Print();points_local[i*4+3].Print();
    }
    for (Int_t i = 0; i < N_ALV_TYPES * 2; i++)
    { // for 23 geometries, outer and inner (23*2)
        alv_cm[i] = center[2 * i] + center[2 * i + 1];
        alv_cm[i] *= 0.5; // volume center for all alv (outer and inner)
        // alv_cm[i].Print();
        for (Int_t j = 0; j < 8; j++)
        { // for the 8 vertices of each alveolus
            points_local[i * 8 + j] = rot[2 * i].Inverse() * (points[i * 8 + j] - alv_cm[i]);
            // cout<< "Points in cm coordinates: "<< endl;
            // points_local[i*8+j].Print();
        }
    }

    // same procedure for crystals
    for (Int_t i = 0; i < N_CRY_TYPES * 2; i++)
    { // 85 types of crystals, 2 face centers (85*2)
        center_cry[i] =
            points_cry[i * 4] + points_cry[i * 4 + 1] + points_cry[i * 4 + 2] + points_cry[i * 4 + 3]; // face centers
        center_cry[i] *= 0.25;                                                                         // face centers
        // center_cry[i].Print();
        z_uni_cry[i] = (points_cry[i * 4 + 1] - points_cry[i * 4]).Cross(points_cry[i * 4 + 2] - points_cry[i * 4 + 1]);
        z_uni_cry[i] = z_uni_cry[i].Unit(); // normal to face center
        x_uni_cry[i] = points_cry[i * 4 + 1] - points_cry[i * 4];
        if (i > 145)
        { // for the irregular crystals at the end of the iPhos, the X axis is taken from other vertices
            x_uni[i] = points_cry[i * 4 + 2] - points_cry[i * 4 + 3];
        }
        x_uni_cry[i] = x_uni_cry[i].Unit();              // unit along X
        y_uni_cry[i] = z_uni_cry[i].Cross(x_uni_cry[i]); // unit along Y
        // x_uni_cry[i].Print();  y_uni_cry[i].Print();    z_uni_cry[i].Print();

        // calculate rotation matrix for the 85 types of crystals (should be repeated 4 times, just checking)
        rot_cry[i].SetZAxis(z_uni_cry[i], x_uni_cry[i]);

        // calculate vertices coordinates in local crystal coordinates (not valid for the definition of the Arb8, see
        // below) points_cry_local[i*4]=rot_cry[i].Inverse()*(points_cry[i*4]-center_cry[i]);
        // points_cry_local[i*4+1]=rot_cry[i].Inverse()*(points_cry[i*4+1]-center_cry[i]);
        // points_cry_local[i*4+2]=rot_cry[i].Inverse()*(points_cry[i*4+2]-center_cry[i]);
        // points_cry_local[i*4+3]=rot_cry[i].Inverse()*(points_cry[i*4+3]-center_cry[i]);
        // cout<< "Points in face coordinates: "<< endl;
        // points_cry_local[i*4].Print();points_cry_local[i*4+1].Print();
        // points_cry_local[i*4+2].Print();points_cry_local[i*4+3].Print();
    }
    for (Int_t i = 0; i < N_CRY_TYPES; i++)
    { // 85 types of crystals
        cry_cm[i] = center_cry[2 * i] + center_cry[2 * i + 1];
        cry_cm[i] *= 0.5; // volume center for each cry
        // cry_cm[i].Print();
        for (Int_t j = 0; j < 8; j++)
        { // for the 8 vertices of each crystal
            points_cry_local[i * 8 + j] = rot_cry[2 * i].Inverse() * (points_cry[i * 8 + j] - cry_cm[i]);
            // reducing 1mm the first crystal vertices to avoid collision with inner volume,
            // while the first crystal definition is not fixed
            if (i == 0)
            {
                if (points_cry_local[i * 8 + j].X() > 0)
                {
                    points_cry_local[i * 8 + j].SetX(points_cry_local[i * 8 + j].X() - 0.1);
                }
                else
                {
                    points_cry_local[i * 8 + j].SetX(points_cry_local[i * 8 + j].X() + 0.1);
                }
                if (points_cry_local[i * 8 + j].Y() > 0)
                {
                    points_cry_local[i * 8 + j].SetY(points_cry_local[i * 8 + j].Y() - 0.1);
                }
                else
                {
                    points_cry_local[i * 8 + j].SetY(points_cry_local[i * 8 + j].Y() + 0.1);
                }
            }
            if (i > 72)
            { // for the irregular crystals at the end of the iPhos, rotation is taken from the corresponding alveolus
                points_cry_local[i * 8 + j] =
                    rot[4 * (Int_t)((i - 73) / 3) + 78].Inverse() * (points_cry[i * 8 + j] - cry_cm[i]);
            }
            // cout<< "Points in cm coordinates.  crystal " << i << ",  vertex "<< j<< endl;
            // points_cry_local[i*8+j].Print();
        }
    }

    // location of the crystals in the alveoli
    TVector3 cry_position[N_CRY_TYPES];       // 85 types of crystals
    TVector3 cry_position_local[N_CRY_TYPES]; // 85 types of crystals

    cry_position[0] = cry_cm[0] - alv_cm[1]; // first alveolus with a single crystal wrt inner alv
    cry_position_local[0] = rot[1].Inverse() * (cry_cm[0] - alv_cm[0]);
    // Relative Crystal rotation in each alveoli. Obtained from the crystal unit vector in
    for (Int_t i = 1; i < N_CRY_TYPES; i++)
    {
        if (i < 73)
        { // four crystals per alv
            // cout << i <<  " " << 2*(Int_t)((i-1)/4)+3 <<  " "<< 4*(Int_t)((i-1)/4)+6 << endl;
            cry_position[i] = cry_cm[i] - alv_cm[2 * (Int_t)((i - 1) / 4) + 3];
            // cry_cm[i].Print();alv_cm[(Int_t)((i-1)/4)+1].Print();
            cry_position_local[i] =
                rot[4 * (Int_t)((i - 1) / 4) + 6].Inverse() * (cry_cm[i] - alv_cm[2 * (Int_t)((i - 1) / 4) + 3]);
            // TODO, KNOWN ISSUE TO SOLVE! Moving up 15mm all crystals in BARREL and iPhos to avoid collisions!!!
            // All collisions were below 200 microns, but it should be checked in the original CAD files
            cry_position_local[i].SetZ(cry_position_local[i].Z() + 1.5);
        }
        else
        { // last four alveoli with three crystals per alv
            // cout << i <<  " " << 2*(Int_t)((i-73)/3)+39 <<  " " << 4*(Int_t)((i-73)/3)+78 << endl;
            cry_position[i] = alv_cm[2 * (Int_t)((i - 73) / 3) + 39] - cry_cm[i];
            cry_position_local[i] =
                rot[4 * (Int_t)((i - 73) / 3) + 78].Inverse() * (cry_cm[i] - alv_cm[2 * (Int_t)((i - 73) / 3) + 39]);
            // TODO, KNOWN ISSUE TO SOLVE! Moving up 15mm all crystals in BARREL and iPhos to avoid collisions!!!
            // There is no collision for the alveoli with 3 crystals... anyway, moving them for coherence
            cry_position_local[i].SetZ(cry_position_local[i].Z() + 1.5);
            // cry_position[i].Print();
            // cry_position_local[i].Print();
        }
    }

    // Redefinition of vertices for the construction of the Alveoli, using TGeoArb8
    Double_t* vertices_Alv[N_ALV_TYPES]; // 23 geometries
    Double_t* vertices_inner_Alv[N_ALV_TYPES];
    for (Int_t i = 0; i < N_ALV_TYPES; i++)
    {
        vertices_Alv[i] = new Double_t[16];
        vertices_inner_Alv[i] = new Double_t[16];
    }
    for (Int_t i = 0; i < N_ALV_TYPES; i++)
    {
        for (Int_t j = 0; j < 8; j++)
        {
            if ((4 - j) > 0)
            { // reversing order for being clockwise filling TGeoArb8
                vertices_Alv[i][2 * j] = points_local[16 * i + 3 - j].X();
                vertices_Alv[i][2 * j + 1] = points_local[16 * i + 3 - j].Y();
                vertices_inner_Alv[i][2 * j] = points_local[16 * i + 3 - j + 8].X();
                vertices_inner_Alv[i][2 * j + 1] = points_local[16 * i + 3 - j + 8].Y();
            }
            else
            {
                vertices_Alv[i][2 * j] = points_local[16 * i + 11 - j].X();
                vertices_Alv[i][2 * j + 1] = points_local[16 * i + 11 - j].Y();
                vertices_inner_Alv[i][2 * j] = points_local[16 * i + 11 - j + 8].X();
                vertices_inner_Alv[i][2 * j + 1] = points_local[16 * i + 11 - j + 8].Y();
            }
        }
    }

    // Redefinition of vertices for the construction of the Crystals, using TGeoArb8
    Double_t* vertices_Cry[N_CRY_TYPES];      // 680/8=85 (85 crystal types)
    Double_t* vertices_Cry_Wrap[N_CRY_TYPES]; // 680/8=85 (85 crystal types)
    for (Int_t i = 0; i < N_CRY_TYPES; i++)
    {
        vertices_Cry[i] = new Double_t[16];
        vertices_Cry_Wrap[i] = new Double_t[16];
    }
    for (Int_t i = 0; i < N_CRY_TYPES; i++)
    {
        for (Int_t j = 0; j < 8; j++)
        {
            if ((4 - j) > 0)
            { // reversing order for being clockwise filling TGeoArb8
                vertices_Cry_Wrap[i][2 * j] = points_cry_local[8 * i + 3 - j].X();
                vertices_Cry_Wrap[i][2 * j + 1] = points_cry_local[8 * i + 3 - j].Y();
            }
            else
            {
                vertices_Cry_Wrap[i][2 * j] = points_cry_local[8 * i + 11 - j].X();
                vertices_Cry_Wrap[i][2 * j + 1] = points_cry_local[8 * i + 11 - j].Y();
            }
        }
    }
    for (Int_t i = 0; i < N_CRY_TYPES; i++)
    {
        for (Int_t j = 0; j < 16; j++)
        {
            if (vertices_Cry_Wrap[i][j] > 0)
                vertices_Cry[i][j] = vertices_Cry_Wrap[i][j] - wrapping_thickness;
            else
                vertices_Cry[i][j] = vertices_Cry_Wrap[i][j] + wrapping_thickness;
        }
    }

    TGeoVolume** Alv_vol;
    Alv_vol = new TGeoVolume*[N_ALV_TYPES];
    TGeoVolume** Alv_inner_vol;
    Alv_inner_vol = new TGeoVolume*[N_ALV_TYPES];
    TGeoVolume** Cry_vol_wrap;
    Cry_vol_wrap = new TGeoVolume*[N_CRY_TYPES];
    TGeoVolume** Cry_vol;
    Cry_vol = new TGeoVolume*[N_CRY_TYPES];

    TString AlvGlobalName = "Alveolus_";
    TString AlvGlobalNameInner = "InnerAlv_";
    // TString name_Alv[N_ALV_TYPES] = { "BR_16a", "BR_15a", "BR_14a", "BR_13a", "BR_12a", "BR_11a", "BR_10a", "BR_09a",
    //                                  "BR_08a", "BR_07a", "BR_06a", "BR_05a", "BR_04a", "BR_03a", "BR_02a", "BR_01a",
    //                                  "iP_04a", "iP_03a", "iP_02a", "iP_01d", "iP_01c", "iP_01b", "iP_01a" };
    // Substitute names in previous array (CAD names) to simplify the R3BRoot code
    TString name_Alv[N_ALV_TYPES] = { "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12",
                                      "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23" };
    Double_t halfLengthAlv[N_ALV_TYPES] = { 8.5,  9.5,  9.5,  9.5,  9.5,  10.5, 10.5, 11.0, 11.0, 11.0, 11.5, 11.5,
                                            11.5, 13.5, 13.5, 13.5, 13.6, 13.6, 13.6, 13.6, 13.6, 13.6, 13.6 }; // cm
    Double_t halfLengthAlv_inner[N_ALV_TYPES] = { 8.485,  9.485,  9.485,  9.485,  9.485,  10.485, 10.485, 10.985,
                                                  10.985, 10.985, 11.485, 11.485, 11.485, 13.485, 13.485, 13.485,
                                                  13.585, 13.585, 13.585, 13.585, 13.585, 13.585, 13.585 }; // cm
    TString WrapCryGlobalName = "WrapCry_";
    TString CryGlobalName = "Crystal_";
    TString name_Cry[4] = { "_1", "_2", "_3", "_4" };
    // TODO, KNOWN ISSUE TO SOLVE!
    // Crystal BR_16_1 has the same length and size as the inner alv (-1mm) (no crystal was defined in CAD)
    Double_t halfLengthCry[N_ALV_TYPES] = { 8.385,   7.0,     7.0,     7.0,    7.0,     8.0,     8.0,   8.5,
                                            8.5,     8.5,     9.0,     9.0,    9.0,     11.0,    11.0,  11.0,
                                            10.9754, 10.9799, 10.9841, 10.985, 10.9818, 10.9819, 10.983 }; // cm

    // length checks
    /*
    cout << "Check alveoli length (below 1 microns). No error message indicates that all is ok..." << endl;
    for(Int_t i=0;i<N_ALV_TYPES;i++) cout << halfLengthAlv[i] << "\t";
    cout << endl;
    for(Int_t i=0;i<N_ALV_TYPES;i++) cout << halfLengthAlv_inner[i] << "\t";
    cout << endl;
    for(Int_t i=0;i<N_ALV_TYPES;i++){
      for(Int_t j=0;j<8;j++){
        if(abs(abs(points_local[i*16+j].Z())-halfLengthAlv[i]) > 0.0001) {
      cout << "error! Check alv problem at i=" << i << ", j=" << j<< endl;
      cout << abs(points_local[i*8+j].Z()) << " " << halfLengthAlv[i] << endl;
        }
        if(abs(abs(points_local[i*16+j+8].Z())-halfLengthAlv_inner[i]) > 0.0001) {
      cout << "error! Check inner alv problem at i=" << i << ", j=" << j<< endl;
      cout << abs(points_local[i*8+j+8].Z()) << " " << halfLengthAlv_inner[i] << endl;
        }
      }
    }
    cout << "Check crystal length (deviation below 1 micron). No error message indicates that all is ok..." << endl;
    for(Int_t i=0;i<N_ALV_TYPES;i++) cout << halfLengthCry[i] << "\t";
    cout << endl;
    for(Int_t i=0;i<N_ALV_TYPES;i++){
      for(Int_t k=0;k<4;k++){ //four crystal per alveoli
        for(Int_t j=0;j<8;j++){
      //cout << "  i=" << i << ",  k=" << k <<  ",  j=" << j <<  ",  a=" << i*8+j
      //     <<",  b=" << (i-1)*32+k*8+j+8 << ",  c=" << (i-19)*24+k*8+j+584 << endl;
      if(i== 0 && k==0 && abs(abs(points_cry_local[i*8+j].Z())-halfLengthCry[i]) > 0.0001) {
        cout << "error! Check crystal problem at i=0" << ", j=" << j << endl;
        cout << abs(points_cry_local[i*8+j].Z()) << " " << halfLengthCry[i] << endl;
      }
      else if(i> 0 && i< 19 && abs(abs(points_cry_local[(i-1)*32+k*8+j+8].Z())-halfLengthCry[i]) > 0.0001) {
        cout << "error! Check crystal problem at i=" << i << ", j=" << j << ", k=" << k << endl;
        cout << abs(points_cry_local[(i-1)*32+k*8+j+8].Z()) << " " << halfLengthCry[i] << endl;
      }
      else if(i>18 && k<3 && abs(abs(points_cry_local[(i-19)*24+k*8+j+584].Z())-halfLengthCry[i]) > 0.0001) {
        cout << "error! Check crystal problem at i=" << i << ", j=" << j << ", k=" << k << endl;
        cout << abs(points_cry_local[(i-19)*24+k*8+j+584].Z()) << " " << halfLengthCry[i] << endl;
      }
        }
      }
    }
    */

    TGeoRotation* rotUni = new TGeoRotation();
    TGeoRotation** rotAlv = new TGeoRotation*[N_ALV_TYPES];
    for (Int_t i = 0; i < N_ALV_TYPES; i++)
        rotAlv[i] = new TGeoRotation();
    TGeoRotation** rotCry = new TGeoRotation*[N_CRY_TYPES];
    for (Int_t i = 0; i < N_CRY_TYPES; i++)
        rotCry[i] = new TGeoRotation();
    Double_t rotEle[9];

    // rotation
    TGeoRotation** rotOnZ = new TGeoRotation*[32];
    for (Int_t i = 0; i < 32; i++)
        rotOnZ[i] = new TGeoRotation();
    for (Int_t i = 0; i < 32; i++)
        rotOnZ[i]->RotateZ(-11.25 * i);
    TRotation** rotationOnZ = new TRotation*[32];
    for (Int_t i = 0; i < 32; i++)
        rotationOnZ[i] = new TRotation();
    for (Int_t i = 0; i < 32; i++)
        rotationOnZ[i]->RotateZ(i * (-11.25 * TMath::Pi()) / 180);

    TGeoRotation** rotAlvFinal = new TGeoRotation*[32 * N_ALV_TYPES];
    for (Int_t i = 0; i < N_ALV_TYPES; i++)
    {
        for (Int_t j = 0; j < 32; j++)
        {
            rotAlvFinal[i * 32 + j] = new TGeoRotation((*rotOnZ[j]) * (*rotAlv[i]));
        }
    }

    for (Int_t i = 0; i < N_ALV_TYPES; i++)
    {
        Alv_vol[i] =
            gGeoManager->MakeArb8(AlvGlobalName + name_Alv[i], pCarbonFibreMedium, halfLengthAlv[i], vertices_Alv[i]);
        Alv_vol[i]->SetLineColor(kBlue);
        Alv_vol[i]->SetVisLeaves(kTRUE);
        Alv_vol[i]->SetVisibility(kTRUE);
        Alv_vol[i]->SetVisContainers(kTRUE);

        Alv_inner_vol[i] = gGeoManager->MakeArb8(
            AlvGlobalNameInner + name_Alv[i], pAirMedium, halfLengthAlv_inner[i], vertices_inner_Alv[i]);
        Alv_inner_vol[i]->SetLineColor(kRed);
        Alv_inner_vol[i]->SetVisLeaves(kTRUE);
        Alv_inner_vol[i]->SetVisibility(kTRUE);
        Alv_inner_vol[i]->SetVisContainers(kTRUE);

        if (i == 0)
        { // first alveolus with a single crystal
            Cry_vol[i] = gGeoManager->MakeArb8(CryGlobalName + name_Alv[i] + name_Cry[i],
                                               pCsIMedium,
                                               halfLengthCry[i] - wrapping_thickness,
                                               vertices_Cry[i]);
            Cry_vol[i]->SetLineColor(kMagenta);
            Cry_vol[i]->SetVisLeaves(kTRUE);
            Cry_vol[i]->SetVisibility(kTRUE);
            Cry_vol[i]->SetVisContainers(kTRUE);

            Cry_vol_wrap[i] = gGeoManager->MakeArb8(
                WrapCryGlobalName + name_Alv[i] + name_Cry[i], pWrappingMedium, halfLengthCry[i], vertices_Cry_Wrap[i]);
            Cry_vol_wrap[i]->SetLineColor(kGreen);
            Cry_vol_wrap[i]->SetVisLeaves(kTRUE);
            Cry_vol_wrap[i]->SetVisibility(kTRUE);
            Cry_vol_wrap[i]->SetVisContainers(kTRUE);

            Cry_vol_wrap[i]->AddNode(Cry_vol[i], 0, new TGeoCombiTrans(0, 0, 0, rotUni));
            Alv_inner_vol[i]->AddNode(
                Cry_vol_wrap[i],
                0,
                new TGeoCombiTrans(
                    cry_position_local[i].X(), cry_position_local[i].Y(), cry_position_local[i].Z(), rotUni));
        }
        else if (i < 19)
        { // four crystals per alv
            for (Int_t j = 0; j < 4; j++)
            {
                Cry_vol[4 * i - 3 + j] = gGeoManager->MakeArb8(CryGlobalName + name_Alv[i] + name_Cry[j],
                                                               pCsIMedium,
                                                               halfLengthCry[i] - wrapping_thickness,
                                                               vertices_Cry[4 * i - 3 + j]);
                Cry_vol[4 * i - 3 + j]->SetLineColor(kMagenta);
                Cry_vol[4 * i - 3 + j]->SetVisLeaves(kTRUE);
                Cry_vol[4 * i - 3 + j]->SetVisibility(kTRUE);
                Cry_vol[4 * i - 3 + j]->SetVisContainers(kTRUE);

                Cry_vol_wrap[4 * i - 3 + j] = gGeoManager->MakeArb8(WrapCryGlobalName + name_Alv[i] + name_Cry[j],
                                                                    pWrappingMedium,
                                                                    halfLengthCry[i],
                                                                    vertices_Cry_Wrap[4 * i - 3 + j]);
                Cry_vol_wrap[4 * i - 3 + j]->SetLineColor(kGreen);
                Cry_vol_wrap[4 * i - 3 + j]->SetVisLeaves(kTRUE);
                Cry_vol_wrap[4 * i - 3 + j]->SetVisibility(kTRUE);
                Cry_vol_wrap[4 * i - 3 + j]->SetVisContainers(kTRUE);

                Cry_vol_wrap[4 * i - 3 + j]->AddNode(Cry_vol[4 * i - 3 + j], 0, new TGeoCombiTrans(0, 0, 0, rotUni));
                Alv_inner_vol[i]->AddNode(Cry_vol_wrap[4 * i - 3 + j],
                                          0,
                                          new TGeoCombiTrans(cry_position_local[4 * i - 3 + j].X(),
                                                             cry_position_local[4 * i - 3 + j].Y(),
                                                             cry_position_local[4 * i - 3 + j].Z(),
                                                             rotUni));
            }
        }
        else
        { // last four alveoli with three crystals per alv
            for (Int_t j = 0; j < 3; j++)
            {
                Cry_vol[73 + 3 * (i - 19) + j] = gGeoManager->MakeArb8(CryGlobalName + name_Alv[i] + name_Cry[j],
                                                                       pCsIMedium,
                                                                       halfLengthCry[i] - wrapping_thickness,
                                                                       vertices_Cry[73 + 3 * (i - 19) + j]);
                Cry_vol[73 + 3 * (i - 19) + j]->SetLineColor(kMagenta);
                Cry_vol[73 + 3 * (i - 19) + j]->SetVisLeaves(kTRUE);
                Cry_vol[73 + 3 * (i - 19) + j]->SetVisibility(kTRUE);
                Cry_vol[73 + 3 * (i - 19) + j]->SetVisContainers(kTRUE);

                Cry_vol_wrap[73 + 3 * (i - 19) + j] =
                    gGeoManager->MakeArb8(WrapCryGlobalName + name_Alv[i] + name_Cry[j],
                                          pWrappingMedium,
                                          halfLengthCry[i],
                                          vertices_Cry_Wrap[73 + 3 * (i - 19) + j]);
                Cry_vol_wrap[73 + 3 * (i - 19) + j]->SetLineColor(kGreen);
                Cry_vol_wrap[73 + 3 * (i - 19) + j]->SetVisLeaves(kTRUE);
                Cry_vol_wrap[73 + 3 * (i - 19) + j]->SetVisibility(kTRUE);
                Cry_vol_wrap[73 + 3 * (i - 19) + j]->SetVisContainers(kTRUE);

                Cry_vol_wrap[73 + 3 * (i - 19) + j]->AddNode(
                    Cry_vol[73 + 3 * (i - 19) + j], 0, new TGeoCombiTrans(0, 0, 0, rotUni));
                Alv_inner_vol[i]->AddNode(Cry_vol_wrap[73 + 3 * (i - 19) + j],
                                          0,
                                          new TGeoCombiTrans(cry_position_local[73 + 3 * (i - 19) + j].X(),
                                                             cry_position_local[73 + 3 * (i - 19) + j].Y(),
                                                             cry_position_local[73 + 3 * (i - 19) + j].Z(),
                                                             rotUni));
            }
        }
        // Inner volume center is displaced 150 microns along Z
        Alv_vol[i]->AddNode(Alv_inner_vol[i], 0, new TGeoCombiTrans(0, 0, 0.015, rotUni));

        rotEle[0] = rot[4 * i].XX();
        rotEle[1] = rot[4 * i].XY();
        rotEle[2] = rot[4 * i].XZ();
        rotEle[3] = rot[4 * i].YX();
        rotEle[4] = rot[4 * i].YY();
        rotEle[5] = rot[4 * i].YZ();
        rotEle[6] = rot[4 * i].ZX();
        rotEle[7] = rot[4 * i].ZY();
        rotEle[8] = rot[4 * i].ZZ();
        rotAlv[i]->SetMatrix(rotEle);

        for (Int_t j = 0; j < 32; j++)
        { // rotation around Z
            // rotAlvFinal[i * 32 + j] = new TGeoRotation((*rotOnZ[j]) * (*rotAlv[i]));
            // alv_cm_rot[2 * i] = (*rotationOnZ[j]) * alv_cm[2 * i];

            Double_t disp_halfBarrel = 0;
            if (strncmp(expNumber, "s455", 4) == 0 || strncmp(expNumber, "s515", 4) == 0)
            {
                if (j < 16)
                    disp_halfBarrel = 1.25;
                else
                    disp_halfBarrel = -1.25;
            }

            if (i > 18 && j > 7)
                continue;

            if (i > 18)
            {
                if (strncmp(expNumber, "s455", 4) == 0 || strncmp(expNumber, "s515", 4) == 0)
                {
                    if (j < 4)
                        disp_halfBarrel = 1.25;
                    else
                        disp_halfBarrel = -1.25;
                }

                rotAlvFinal[i * 32 + 4 * j] = new TGeoRotation((*rotOnZ[4 * j]) * (*rotAlv[i]));
                alv_cm_rot[2 * i] = (*rotationOnZ[4 * j]) * alv_cm[2 * i];
                if (isCrystalInstalled(i + 1, j, installedCrystals))
                {
                    pWorld->AddNode(Alv_vol[i],
                                    j,
                                    new TGeoCombiTrans(alv_cm_rot[2 * i].X() + disp_halfBarrel,
                                                       alv_cm_rot[2 * i].Y(),
                                                       alv_cm_rot[2 * i].Z(),
                                                       rotAlvFinal[i * 32 + 4 * j]));
                }
            }
            else
            {
                rotAlvFinal[i * 32 + j] = new TGeoRotation((*rotOnZ[j]) * (*rotAlv[i]));
                alv_cm_rot[2 * i] = (*rotationOnZ[j]) * alv_cm[2 * i];
                if (isCrystalInstalled(i + 1, j, installedCrystals))
                {
                    pWorld->AddNode(Alv_vol[i],
                                    j,
                                    new TGeoCombiTrans(alv_cm_rot[2 * i].X() + disp_halfBarrel,
                                                       alv_cm_rot[2 * i].Y(),
                                                       alv_cm_rot[2 * i].Z(),
                                                       rotAlvFinal[i * 32 + j]));
                }
            }
        }
    }

    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();

    std::cout << "\033[34m Creating geometry:\033[0m "
              << "\033[33m" << geoFileName << " \033[0m" << std::endl;
    std::cout << "Macro finished successfully." << std::endl;
}

Bool_t isCrystalInstalled(Int_t alvType, Int_t alveolusCopy, Int_t instCry[])
{
    // reproduces partially the algorithm of R3BCalifaGeometry::GetCrystalId(const char* volumePath)
    Bool_t found = kFALSE;
    Int_t crystalId = 0;
    Int_t cryType = 1; // first crystal of the alveoli... if not present, alveoli is removed.

    if (alvType == 1)
        crystalId = 1 + alveolusCopy; // first alveoli ring, one crystal per alveolus
    else if (alvType < 20)
        crystalId = 33 + (alvType - 2) * 128 + alveolusCopy * 4 + (cryType - 1); // four crystal per alveolus
    else
        crystalId = 2337 + (alvType - 20) * 24 + alveolusCopy * 3 + (cryType - 1); // three crystal per alveolus

    if (crystalId < 1 || crystalId > nbcrystals)
    { // crystalId runs from 1 to nbcrystals
        cout << "R3BCalifaGeometry: Wrong crystal number ";
        cout << "---- crystalId: " << crystalId << endl;
        return 0;
    }
    for (Int_t i = 0; i < nbcrystals; i++)
    {
        if (crystalId == instCry[i])
            found = kTRUE;
    }
    return found;
}
