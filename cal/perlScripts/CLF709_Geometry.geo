 //Generated 2/1/2013 11:57:23 from macro createGeometryCLF709718.pl by Hector Alvarez Pol (hector.alvarez.es) 

 //Alveolus
	 TGeoVolume *Alveolus_1 = gGeoManager->MakeTrd2("Alveolus_1", pCarbonFibreMedium,2.96,4.04375886320505,1.85,2.51444170011313,10); 
	 Alveolus_1->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_2 = gGeoManager->MakeTrd2("Alveolus_2", pCarbonFibreMedium,2.96,4.04375886320505,1.85,2.51444170011313,10); 
	 Alveolus_2->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_3 = gGeoManager->MakeTrd2("Alveolus_3", pCarbonFibreMedium,2.9575,4.23402135646078,1.65,2.31481836622547,10); 
	 Alveolus_3->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_4 = gGeoManager->MakeTrd2("Alveolus_4", pCarbonFibreMedium,2.9575,4.23402135646078,1.65,2.31481836622547,10); 
	 Alveolus_4->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_5 = gGeoManager->MakeTrd2("Alveolus_5", pCarbonFibreMedium,2.955,4.42511496483857,1.5,2.37557472245143,10); 
	 Alveolus_5->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_6 = gGeoManager->MakeTrd2("Alveolus_6", pCarbonFibreMedium,2.955,4.42511496483857,1.5,2.37557472245143,10); 
	 Alveolus_6->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_7 = gGeoManager->MakeTrd2("Alveolus_7", pCarbonFibreMedium,2.955,4.42511496483857,1.5,2.37557472245143,10); 
	 Alveolus_7->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_8 = gGeoManager->MakeTrd2("Alveolus_8", pCarbonFibreMedium,2.955,4.42511496483857,1.5,2.37557472245143,10); 
	 Alveolus_8->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_9 = gGeoManager->MakeTrd2("Alveolus_9", pCarbonFibreMedium,2.955,4.42511496483857,1.5,2.37557472245143,10); 
	 Alveolus_9->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_10 = gGeoManager->MakeTrd2("Alveolus_10", pCarbonFibreMedium,2.955,4.42511496483857,1.5,2.37557472245143,10); 
	 Alveolus_10->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_11 = gGeoManager->MakeTrd2("Alveolus_11", pCarbonFibreMedium,2.945,4.87263266167029,1.65,2.52726533056041,10); 
	 Alveolus_11->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_12 = gGeoManager->MakeTrd2("Alveolus_12", pCarbonFibreMedium,2.945,4.87263266167029,1.65,2.52726533056041,10); 
	 Alveolus_12->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_13 = gGeoManager->MakeTrd2("Alveolus_13", pCarbonFibreMedium,2.945,4.87263266167029,1.65,2.52726533056041,10); 
	 Alveolus_13->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_14 = gGeoManager->MakeTrd2("Alveolus_14", pCarbonFibreMedium,2.81,4.40409807285011,2.2,3.32171977966755,10); 
	 Alveolus_14->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_15 = gGeoManager->MakeTrd2("Alveolus_15", pCarbonFibreMedium,2.81,4.40409807285011,2.2,3.32171977966755,10); 
	 Alveolus_15->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_16 = gGeoManager->MakeTrd2("Alveolus_16", pCarbonFibreMedium,2.81,4.40409807285011,2.2,3.32171977966755,10); 
	 Alveolus_16->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_17 = gGeoManager->MakeTrd2("Alveolus_17", pCarbonFibreMedium,2.81,4.40409807285011,2.2,3.32171977966755,10); 
	 Alveolus_17->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_18 = gGeoManager->MakeTrd2("Alveolus_18", pCarbonFibreMedium,2.81,4.40409807285011,2.2,3.32171977966755,10); 
	 Alveolus_18->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_19 = gGeoManager->MakeTrd2("Alveolus_19", pCarbonFibreMedium,2.81,4.40409807285011,2.2,3.32171977966755,10); 
	 Alveolus_19->SetLineColor(kBlue); 



 //Crystals with wrapping
	 Double_t theta = 0.; 
	 Double_t phi = 0.; 

	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.528041124848496,2)+pow(0.323915328805217,2)),19.5)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.323915328805217,0.528041124848496)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_1A = gGeoManager->MakeTrap("CrystalWithWrapping_1A", pWrappingMedium,9.75,
		theta, phi,0.91, 1.465, 1.465, 0, 1.23391532880525, 1.99304112484853, 1.99304112484853,0); 
	 CrystalWithWrapping_1A->SetLineColor(kYellow); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.528041124848496,2)+pow(0.323915328805217,2)),19.44)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.323915328805217,0.528041124848496)/TMath::Pi(); 
	 TGeoVolume *Crystal_1A = gGeoManager->MakeTrap("Crystal_1A", pCsIMedium,9.72,
		theta,phi,0.88, 1.435, 1.435, 0, 1.20391532880525, 1.96304112484853, 1.96304112484853,0); 
	 Crystal_1A->SetLineColor(kRed); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.528041124848496,2)+pow(0.323440811890236,2)),19.5)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.323440811890236,0.528041124848496)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_1B = gGeoManager->MakeTrap("CrystalWithWrapping_1B", pWrappingMedium,9.75,
		theta, phi,0.91, 1.465, 1.465, 0, 1.23344081189024, 1.9930411248485, 1.9930411248485,0); 
	 CrystalWithWrapping_1B->SetLineColor(kGreen); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.528041124848496,2)+pow(0.323440811890236,2)),19.44)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.323440811890236,0.528041124848496)/TMath::Pi(); 
	 TGeoVolume *Crystal_1B = gGeoManager->MakeTrap("Crystal_1B", pCsIMedium,9.72,
		theta, phi,0.88, 1.435, 1.435, 0, 1.20344081189024, 1.9630411248485, 1.9630411248485,0); 
	 Crystal_1B->SetLineColor(kViolet); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.606347644318803,2)+pow(0.315788723957099,2)),19)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.315788723957099,0.606347644318803)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_2A = gGeoManager->MakeTrap("CrystalWithWrapping_2A", pWrappingMedium,9.5,
		theta, phi,0.81, 1.464, 1.464, 0, 1.12578872395707, 2.07034764431884, 2.07034764431884,0); 
	 CrystalWithWrapping_2A->SetLineColor(kYellow); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.606347644318803,2)+pow(0.315788723957099,2)),18.94)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.315788723957099,0.606347644318803)/TMath::Pi(); 
	 TGeoVolume *Crystal_2A = gGeoManager->MakeTrap("Crystal_2A", pCsIMedium,9.47,
		theta,phi,0.78, 1.434, 1.434, 0, 1.09578872395707, 2.04034764431884, 2.04034764431884,0); 
	 Crystal_2A->SetLineColor(kRed); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.606012927142453,2)+pow(0.315147457739262,2)),19)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.315147457739262,0.606012927142453)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_2B = gGeoManager->MakeTrap("CrystalWithWrapping_2B", pWrappingMedium,9.5,
		theta, phi,0.81, 1.464, 1.464, 0, 1.12514745773926, 2.07001292714245, 2.07001292714245,0); 
	 CrystalWithWrapping_2B->SetLineColor(kGreen); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.606012927142453,2)+pow(0.315147457739262,2)),18.94)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.315147457739262,0.606012927142453)/TMath::Pi(); 
	 TGeoVolume *Crystal_2B = gGeoManager->MakeTrap("Crystal_2B", pCsIMedium,9.47,
		theta, phi,0.78, 1.434, 1.434, 0, 1.09514745773926, 2.04001292714245, 2.04001292714245,0); 
	 Crystal_2B->SetLineColor(kViolet); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.661551734177358,2)+pow(0.394008625103204,2)),18)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.394008625103204,0.661551734177358)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_3A = gGeoManager->MakeTrap("CrystalWithWrapping_3A", pWrappingMedium,9,
		theta, phi,0.735, 1.4625, 1.4625, 0, 1.12900862510318, 2.12405173417739, 2.12405173417739,0); 
	 CrystalWithWrapping_3A->SetLineColor(kYellow); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.661551734177358,2)+pow(0.394008625103204,2)),17.94)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.394008625103204,0.661551734177358)/TMath::Pi(); 
	 TGeoVolume *Crystal_3A = gGeoManager->MakeTrap("Crystal_3A", pCsIMedium,8.97,
		theta,phi,0.705, 1.4325, 1.4325, 0, 1.09900862510318, 2.09405173417739, 2.09405173417739,0); 
	 Crystal_3A->SetLineColor(kRed); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.660918686333585,2)+pow(0.392948486176609,2)),18)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.392948486176609,0.660918686333585)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_3B = gGeoManager->MakeTrap("CrystalWithWrapping_3B", pWrappingMedium,9,
		theta, phi,0.735, 1.4625, 1.4625, 0, 1.12794848617661, 2.12341868633362, 2.12341868633362,0); 
	 CrystalWithWrapping_3B->SetLineColor(kGreen); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.660918686333585,2)+pow(0.392948486176609,2)),17.94)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.392948486176609,0.660918686333585)/TMath::Pi(); 
	 TGeoVolume *Crystal_3B = gGeoManager->MakeTrap("Crystal_3B", pCsIMedium,8.97,
		theta, phi,0.705, 1.4325, 1.4325, 0, 1.09794848617661, 2.09341868633362, 2.09341868633362,0); 
	 Crystal_3B->SetLineColor(kViolet); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.819243881209871,2)+pow(0.372837765488235,2)),17)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.372837765488235,0.819243881209871)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_4A = gGeoManager->MakeTrap("CrystalWithWrapping_4A", pWrappingMedium,8.5,
		theta, phi,0.81, 1.4575, 1.4575, 0, 1.18283776548821, 2.27674388120987, 2.27674388120987,0); 
	 CrystalWithWrapping_4A->SetLineColor(kYellow); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.819243881209871,2)+pow(0.372837765488235,2)),16.94)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.372837765488235,0.819243881209871)/TMath::Pi(); 
	 TGeoVolume *Crystal_4A = gGeoManager->MakeTrap("Crystal_4A", pCsIMedium,8.47,
		theta,phi,0.78, 1.4275, 1.4275, 0, 1.15283776548821, 2.24674388120987, 2.24674388120987,0); 
	 Crystal_4A->SetLineColor(kRed); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.81845690967905,2)+pow(0.371118014722352,2)),17)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.371118014722352,0.81845690967905)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_4B = gGeoManager->MakeTrap("CrystalWithWrapping_4B", pWrappingMedium,8.5,
		theta, phi,0.81, 1.4575, 1.4575, 0, 1.18111801472235, 2.27595690967905, 2.27595690967905,0); 
	 CrystalWithWrapping_4B->SetLineColor(kGreen); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.81845690967905,2)+pow(0.371118014722352,2)),16.94)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.371118014722352,0.81845690967905)/TMath::Pi(); 
	 TGeoVolume *Crystal_4B = gGeoManager->MakeTrap("Crystal_4B", pCsIMedium,8.47,
		theta, phi,0.78, 1.4275, 1.4275, 0, 1.15111801472235, 2.24595690967905, 2.24595690967905,0); 
	 Crystal_4B->SetLineColor(kViolet); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.637639229140044,2)+pow(0.448687911867021,2)),16)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.448687911867021,0.637639229140044)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_5A = gGeoManager->MakeTrap("CrystalWithWrapping_5A", pWrappingMedium,8,
		theta, phi,1.085, 1.39, 1.39, 0, 1.53368791186699, 2.02763922914007, 2.02763922914007,0); 
	 CrystalWithWrapping_5A->SetLineColor(kYellow); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.637639229140044,2)+pow(0.448687911867021,2)),15.94)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.448687911867021,0.637639229140044)/TMath::Pi(); 
	 TGeoVolume *Crystal_5A = gGeoManager->MakeTrap("Crystal_5A", pCsIMedium,7.97,
		theta,phi,1.055, 1.36, 1.36, 0, 1.50368791186699, 1.99763922914007, 1.99763922914007,0); 
	 Crystal_5A->SetLineColor(kRed); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.636638697858455,2)+pow(0.447269438474086,2)),16)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.447269438474086,0.636638697858455)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_5B = gGeoManager->MakeTrap("CrystalWithWrapping_5B", pWrappingMedium,8,
		theta, phi,1.085, 1.39, 1.39, 0, 1.53226943847409, 2.02663869785845, 2.02663869785845,0); 
	 CrystalWithWrapping_5B->SetLineColor(kGreen); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.636638697858455,2)+pow(0.447269438474085,2)),15.94)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.447269438474085,0.636638697858455)/TMath::Pi(); 
	 TGeoVolume *Crystal_5B = gGeoManager->MakeTrap("Crystal_5B", pCsIMedium,7.97,
		theta, phi,1.055, 1.36, 1.36, 0, 1.50226943847409, 1.99663869785845, 1.99663869785845,0); 
	 Crystal_5B->SetLineColor(kViolet); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.557934325497538,2)+pow(0.392601922883643,2)),14)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.392601922883643,0.557934325497538)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_6A = gGeoManager->MakeTrap("CrystalWithWrapping_6A", pWrappingMedium,7,
		0,0,2.17, 2.78, 2.78, 0, 2.95520384576724, 3.89586865099512, 3.89586865099512,0); 
	 CrystalWithWrapping_6A->SetLineColor(kYellow); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.557934325497538,2)+pow(0.392601922883643,2)),13.94)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.392601922883643,0.557934325497538)/TMath::Pi(); 
	 TGeoVolume *Crystal_6A = gGeoManager->MakeTrap("Crystal_6A", pCsIMedium,6.97,
		0,0,2.14, 2.75, 2.75, 0, 2.92520384576724, 3.86586865099512, 3.86586865099512,0); 

 //Some common geometrical operations 
	 TGeoRotation *rotUni = new TGeoRotation();          //unitary rotation  
	 TGeoTranslation* noTrans=new TGeoTranslation("noTrans",0.,0.,0.); 
	 TGeoRotation *rotSymmetric = new TGeoRotation(); //Symmetric crystal 
	 rotSymmetric->RotateZ(180); 
 //The right transformation describing the difference in the coordinate system in CAD and simulation 
	 TGeoRotation *rotDef = new TGeoRotation(); 
	 rotDef->RotateX(-90); 
	 TGeoTranslation* shiftDef=new TGeoTranslation("shiftDef",0.,10.,0.); 
	 TGeoCombiTrans* transDef = new TGeoCombiTrans(*shiftDef,*rotDef); 

	 TGeoRotation *rotDef1 = new TGeoRotation(); 
	 rotDef1->RotateZ(-11.25*1); 
	 TGeoCombiTrans* transDefRot1 = new TGeoCombiTrans(*noTrans,*rotDef1); 

	 TGeoRotation *rotDef2 = new TGeoRotation(); 
	 rotDef2->RotateZ(-11.25*2); 
	 TGeoCombiTrans* transDefRot2 = new TGeoCombiTrans(*noTrans,*rotDef2); 

	 TGeoRotation *rotDef3 = new TGeoRotation(); 
	 rotDef3->RotateZ(-11.25*3); 
	 TGeoCombiTrans* transDefRot3 = new TGeoCombiTrans(*noTrans,*rotDef3); 

	 TGeoRotation *rotDef4 = new TGeoRotation(); 
	 rotDef4->RotateZ(-11.25*4); 
	 TGeoCombiTrans* transDefRot4 = new TGeoCombiTrans(*noTrans,*rotDef4); 

	 TGeoRotation *rotDef5 = new TGeoRotation(); 
	 rotDef5->RotateZ(-11.25*5); 
	 TGeoCombiTrans* transDefRot5 = new TGeoCombiTrans(*noTrans,*rotDef5); 

	 TGeoRotation *rotDef6 = new TGeoRotation(); 
	 rotDef6->RotateZ(-11.25*6); 
	 TGeoCombiTrans* transDefRot6 = new TGeoCombiTrans(*noTrans,*rotDef6); 

	 TGeoRotation *rotDef7 = new TGeoRotation(); 
	 rotDef7->RotateZ(-11.25*7); 
	 TGeoCombiTrans* transDefRot7 = new TGeoCombiTrans(*noTrans,*rotDef7); 

	 TGeoRotation *rotDef8 = new TGeoRotation(); 
	 rotDef8->RotateZ(-11.25*8); 
	 TGeoCombiTrans* transDefRot8 = new TGeoCombiTrans(*noTrans,*rotDef8); 

	 TGeoRotation *rotDef9 = new TGeoRotation(); 
	 rotDef9->RotateZ(-11.25*9); 
	 TGeoCombiTrans* transDefRot9 = new TGeoCombiTrans(*noTrans,*rotDef9); 

	 TGeoRotation *rotDef10 = new TGeoRotation(); 
	 rotDef10->RotateZ(-11.25*10); 
	 TGeoCombiTrans* transDefRot10 = new TGeoCombiTrans(*noTrans,*rotDef10); 

	 TGeoRotation *rotDef11 = new TGeoRotation(); 
	 rotDef11->RotateZ(-11.25*11); 
	 TGeoCombiTrans* transDefRot11 = new TGeoCombiTrans(*noTrans,*rotDef11); 

	 TGeoRotation *rotDef12 = new TGeoRotation(); 
	 rotDef12->RotateZ(-11.25*12); 
	 TGeoCombiTrans* transDefRot12 = new TGeoCombiTrans(*noTrans,*rotDef12); 

	 TGeoRotation *rotDef13 = new TGeoRotation(); 
	 rotDef13->RotateZ(-11.25*13); 
	 TGeoCombiTrans* transDefRot13 = new TGeoCombiTrans(*noTrans,*rotDef13); 

	 TGeoRotation *rotDef14 = new TGeoRotation(); 
	 rotDef14->RotateZ(-11.25*14); 
	 TGeoCombiTrans* transDefRot14 = new TGeoCombiTrans(*noTrans,*rotDef14); 

	 TGeoRotation *rotDef15 = new TGeoRotation(); 
	 rotDef15->RotateZ(-11.25*15); 
	 TGeoCombiTrans* transDefRot15 = new TGeoCombiTrans(*noTrans,*rotDef15); 

	 TGeoRotation *rotDef16 = new TGeoRotation(); 
	 rotDef16->RotateZ(-11.25*16); 
	 TGeoCombiTrans* transDefRot16 = new TGeoCombiTrans(*noTrans,*rotDef16); 

	 TGeoRotation *rotDef17 = new TGeoRotation(); 
	 rotDef17->RotateZ(-11.25*17); 
	 TGeoCombiTrans* transDefRot17 = new TGeoCombiTrans(*noTrans,*rotDef17); 

	 TGeoRotation *rotDef18 = new TGeoRotation(); 
	 rotDef18->RotateZ(-11.25*18); 
	 TGeoCombiTrans* transDefRot18 = new TGeoCombiTrans(*noTrans,*rotDef18); 

	 TGeoRotation *rotDef19 = new TGeoRotation(); 
	 rotDef19->RotateZ(-11.25*19); 
	 TGeoCombiTrans* transDefRot19 = new TGeoCombiTrans(*noTrans,*rotDef19); 

	 TGeoRotation *rotDef20 = new TGeoRotation(); 
	 rotDef20->RotateZ(-11.25*20); 
	 TGeoCombiTrans* transDefRot20 = new TGeoCombiTrans(*noTrans,*rotDef20); 

	 TGeoRotation *rotDef21 = new TGeoRotation(); 
	 rotDef21->RotateZ(-11.25*21); 
	 TGeoCombiTrans* transDefRot21 = new TGeoCombiTrans(*noTrans,*rotDef21); 

	 TGeoRotation *rotDef22 = new TGeoRotation(); 
	 rotDef22->RotateZ(-11.25*22); 
	 TGeoCombiTrans* transDefRot22 = new TGeoCombiTrans(*noTrans,*rotDef22); 

	 TGeoRotation *rotDef23 = new TGeoRotation(); 
	 rotDef23->RotateZ(-11.25*23); 
	 TGeoCombiTrans* transDefRot23 = new TGeoCombiTrans(*noTrans,*rotDef23); 

	 TGeoRotation *rotDef24 = new TGeoRotation(); 
	 rotDef24->RotateZ(-11.25*24); 
	 TGeoCombiTrans* transDefRot24 = new TGeoCombiTrans(*noTrans,*rotDef24); 

	 TGeoRotation *rotDef25 = new TGeoRotation(); 
	 rotDef25->RotateZ(-11.25*25); 
	 TGeoCombiTrans* transDefRot25 = new TGeoCombiTrans(*noTrans,*rotDef25); 

	 TGeoRotation *rotDef26 = new TGeoRotation(); 
	 rotDef26->RotateZ(-11.25*26); 
	 TGeoCombiTrans* transDefRot26 = new TGeoCombiTrans(*noTrans,*rotDef26); 

	 TGeoRotation *rotDef27 = new TGeoRotation(); 
	 rotDef27->RotateZ(-11.25*27); 
	 TGeoCombiTrans* transDefRot27 = new TGeoCombiTrans(*noTrans,*rotDef27); 

	 TGeoRotation *rotDef28 = new TGeoRotation(); 
	 rotDef28->RotateZ(-11.25*28); 
	 TGeoCombiTrans* transDefRot28 = new TGeoCombiTrans(*noTrans,*rotDef28); 

	 TGeoRotation *rotDef29 = new TGeoRotation(); 
	 rotDef29->RotateZ(-11.25*29); 
	 TGeoCombiTrans* transDefRot29 = new TGeoCombiTrans(*noTrans,*rotDef29); 

	 TGeoRotation *rotDef30 = new TGeoRotation(); 
	 rotDef30->RotateZ(-11.25*30); 
	 TGeoCombiTrans* transDefRot30 = new TGeoCombiTrans(*noTrans,*rotDef30); 

	 TGeoRotation *rotDef31 = new TGeoRotation(); 
	 rotDef31->RotateZ(-11.25*31); 
	 TGeoCombiTrans* transDefRot31 = new TGeoCombiTrans(*noTrans,*rotDef31); 

	 CrystalWithWrapping_1A->AddNode(Crystal_1A,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_1A);  
	 CrystalWithWrapping_1B->AddNode(Crystal_1B,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_1B);  
	 CrystalWithWrapping_2A->AddNode(Crystal_2A,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_2A);  
	 CrystalWithWrapping_2B->AddNode(Crystal_2B,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_2B);  
	 CrystalWithWrapping_3A->AddNode(Crystal_3A,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_3A);  
	 CrystalWithWrapping_3B->AddNode(Crystal_3B,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_3B);  
	 CrystalWithWrapping_4A->AddNode(Crystal_4A,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_4A);  
	 CrystalWithWrapping_4B->AddNode(Crystal_4B,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_4B);  
	 CrystalWithWrapping_5A->AddNode(Crystal_5A,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_5A);  
	 CrystalWithWrapping_5B->AddNode(Crystal_5B,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_5B);  
	 CrystalWithWrapping_6A->AddNode(Crystal_6A,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_6A);  
 //Crystals inside each alveoli 
	 Alveolus_1->AddNode(CrystalWithWrapping_1A,1,new TGeoCombiTrans(1.72902056242425,1.07195766440261,-0.22,rotUni)); 
	 Alveolus_1->AddNode(CrystalWithWrapping_1A,2,new TGeoCombiTrans(-1.72902056242425,-1.07195766440261,-0.22,rotSymmetric)); 
	 Alveolus_1->AddNode(CrystalWithWrapping_1B,3,new TGeoCombiTrans(1.72902056242425,-1.07172040594512,-0.22,rotUni)); 
	 Alveolus_1->AddNode(CrystalWithWrapping_1B,4,new TGeoCombiTrans(-1.72902056242425,1.07172040594512,-0.22,rotSymmetric)); 
	 Alveolus_2->AddNode(CrystalWithWrapping_1A,1,new TGeoCombiTrans(1.72902056242425,1.07195766440261,-0.22,rotUni)); 
	 Alveolus_2->AddNode(CrystalWithWrapping_1A,2,new TGeoCombiTrans(-1.72902056242425,-1.07195766440261,-0.22,rotSymmetric)); 
	 Alveolus_2->AddNode(CrystalWithWrapping_1B,3,new TGeoCombiTrans(1.72902056242425,-1.07172040594512,-0.22,rotUni)); 
	 Alveolus_2->AddNode(CrystalWithWrapping_1B,4,new TGeoCombiTrans(-1.72902056242425,1.07172040594512,-0.22,rotSymmetric)); 
	 Alveolus_3->AddNode(CrystalWithWrapping_2A,1,new TGeoCombiTrans(1.7671738221594,0.967894361978549,-0.47,rotUni)); 
	 Alveolus_3->AddNode(CrystalWithWrapping_2A,2,new TGeoCombiTrans(-1.7671738221594,-0.967894361978549,-0.47,rotSymmetric)); 
	 Alveolus_3->AddNode(CrystalWithWrapping_2B,3,new TGeoCombiTrans(1.76700646357123,-0.967573728869631,-0.47,rotUni)); 
	 Alveolus_3->AddNode(CrystalWithWrapping_2B,4,new TGeoCombiTrans(-1.76700646357123,0.967573728869631,-0.47,rotSymmetric)); 
	 Alveolus_4->AddNode(CrystalWithWrapping_2A,1,new TGeoCombiTrans(1.7671738221594,0.967894361978549,-0.47,rotUni)); 
	 Alveolus_4->AddNode(CrystalWithWrapping_2A,2,new TGeoCombiTrans(-1.7671738221594,-0.967894361978549,-0.47,rotSymmetric)); 
	 Alveolus_4->AddNode(CrystalWithWrapping_2B,3,new TGeoCombiTrans(1.76700646357123,-0.967573728869631,-0.47,rotUni)); 
	 Alveolus_4->AddNode(CrystalWithWrapping_2B,4,new TGeoCombiTrans(-1.76700646357123,0.967573728869631,-0.47,rotSymmetric)); 
	 Alveolus_5->AddNode(CrystalWithWrapping_3A,1,new TGeoCombiTrans(1.79327586708868,0.932004312551602,-0.97,rotUni)); 
	 Alveolus_5->AddNode(CrystalWithWrapping_3A,2,new TGeoCombiTrans(-1.79327586708868,-0.932004312551602,-0.97,rotSymmetric)); 
	 Alveolus_5->AddNode(CrystalWithWrapping_3B,3,new TGeoCombiTrans(1.79295934316679,-0.931474243088304,-0.97,rotUni)); 
	 Alveolus_5->AddNode(CrystalWithWrapping_3B,4,new TGeoCombiTrans(-1.79295934316679,0.931474243088304,-0.97,rotSymmetric)); 
	 Alveolus_6->AddNode(CrystalWithWrapping_3A,1,new TGeoCombiTrans(1.79327586708868,0.932004312551602,-0.97,rotUni)); 
	 Alveolus_6->AddNode(CrystalWithWrapping_3A,2,new TGeoCombiTrans(-1.79327586708868,-0.932004312551602,-0.97,rotSymmetric)); 
	 Alveolus_6->AddNode(CrystalWithWrapping_3B,3,new TGeoCombiTrans(1.79295934316679,-0.931474243088304,-0.97,rotUni)); 
	 Alveolus_6->AddNode(CrystalWithWrapping_3B,4,new TGeoCombiTrans(-1.79295934316679,0.931474243088304,-0.97,rotSymmetric)); 
	 Alveolus_7->AddNode(CrystalWithWrapping_3A,1,new TGeoCombiTrans(1.79327586708868,0.932004312551602,-0.97,rotUni)); 
	 Alveolus_7->AddNode(CrystalWithWrapping_3A,2,new TGeoCombiTrans(-1.79327586708868,-0.932004312551602,-0.97,rotSymmetric)); 
	 Alveolus_7->AddNode(CrystalWithWrapping_3B,3,new TGeoCombiTrans(1.79295934316679,-0.931474243088304,-0.97,rotUni)); 
	 Alveolus_7->AddNode(CrystalWithWrapping_3B,4,new TGeoCombiTrans(-1.79295934316679,0.931474243088304,-0.97,rotSymmetric)); 
	 Alveolus_8->AddNode(CrystalWithWrapping_3A,1,new TGeoCombiTrans(1.79327586708868,0.932004312551602,-0.97,rotUni)); 
	 Alveolus_8->AddNode(CrystalWithWrapping_3A,2,new TGeoCombiTrans(-1.79327586708868,-0.932004312551602,-0.97,rotSymmetric)); 
	 Alveolus_8->AddNode(CrystalWithWrapping_3B,3,new TGeoCombiTrans(1.79295934316679,-0.931474243088304,-0.97,rotUni)); 
	 Alveolus_8->AddNode(CrystalWithWrapping_3B,4,new TGeoCombiTrans(-1.79295934316679,0.931474243088304,-0.97,rotSymmetric)); 
	 Alveolus_9->AddNode(CrystalWithWrapping_3A,1,new TGeoCombiTrans(1.79327586708868,0.932004312551602,-0.97,rotUni)); 
	 Alveolus_9->AddNode(CrystalWithWrapping_3A,2,new TGeoCombiTrans(-1.79327586708868,-0.932004312551602,-0.97,rotSymmetric)); 
	 Alveolus_9->AddNode(CrystalWithWrapping_3B,3,new TGeoCombiTrans(1.79295934316679,-0.931474243088304,-0.97,rotUni)); 
	 Alveolus_9->AddNode(CrystalWithWrapping_3B,4,new TGeoCombiTrans(-1.79295934316679,0.931474243088304,-0.97,rotSymmetric)); 
	 Alveolus_10->AddNode(CrystalWithWrapping_3A,1,new TGeoCombiTrans(1.79327586708868,0.932004312551602,-0.97,rotUni)); 
	 Alveolus_10->AddNode(CrystalWithWrapping_3A,2,new TGeoCombiTrans(-1.79327586708868,-0.932004312551602,-0.97,rotSymmetric)); 
	 Alveolus_10->AddNode(CrystalWithWrapping_3B,3,new TGeoCombiTrans(1.79295934316679,-0.931474243088304,-0.97,rotUni)); 
	 Alveolus_10->AddNode(CrystalWithWrapping_3B,4,new TGeoCombiTrans(-1.79295934316679,0.931474243088304,-0.97,rotSymmetric)); 
	 Alveolus_11->AddNode(CrystalWithWrapping_4A,1,new TGeoCombiTrans(1.86712194060494,0.996418882744118,-1.47,rotUni)); 
	 Alveolus_11->AddNode(CrystalWithWrapping_4A,2,new TGeoCombiTrans(-1.86712194060494,-0.996418882744118,-1.47,rotSymmetric)); 
	 Alveolus_11->AddNode(CrystalWithWrapping_4B,3,new TGeoCombiTrans(1.86672845483952,-0.995559007361176,-1.47,rotUni)); 
	 Alveolus_11->AddNode(CrystalWithWrapping_4B,4,new TGeoCombiTrans(-1.86672845483952,0.995559007361176,-1.47,rotSymmetric)); 
	 Alveolus_12->AddNode(CrystalWithWrapping_4A,1,new TGeoCombiTrans(1.86712194060494,0.996418882744118,-1.47,rotUni)); 
	 Alveolus_12->AddNode(CrystalWithWrapping_4A,2,new TGeoCombiTrans(-1.86712194060494,-0.996418882744118,-1.47,rotSymmetric)); 
	 Alveolus_12->AddNode(CrystalWithWrapping_4B,3,new TGeoCombiTrans(1.86672845483952,-0.995559007361176,-1.47,rotUni)); 
	 Alveolus_12->AddNode(CrystalWithWrapping_4B,4,new TGeoCombiTrans(-1.86672845483952,0.995559007361176,-1.47,rotSymmetric)); 
	 Alveolus_13->AddNode(CrystalWithWrapping_4A,1,new TGeoCombiTrans(1.86712194060494,0.996418882744118,-1.47,rotUni)); 
	 Alveolus_13->AddNode(CrystalWithWrapping_4A,2,new TGeoCombiTrans(-1.86712194060494,-0.996418882744118,-1.47,rotSymmetric)); 
	 Alveolus_13->AddNode(CrystalWithWrapping_4B,3,new TGeoCombiTrans(1.86672845483952,-0.995559007361176,-1.47,rotUni)); 
	 Alveolus_13->AddNode(CrystalWithWrapping_4B,4,new TGeoCombiTrans(-1.86672845483952,0.995559007361176,-1.47,rotSymmetric)); 
	 Alveolus_14->AddNode(CrystalWithWrapping_5A,1,new TGeoCombiTrans(1.70881961457002,1.30934395593351,-1.97,rotUni)); 
	 Alveolus_14->AddNode(CrystalWithWrapping_5A,2,new TGeoCombiTrans(-1.70881961457002,-1.30934395593351,-1.97,rotSymmetric)); 
	 Alveolus_14->AddNode(CrystalWithWrapping_5B,3,new TGeoCombiTrans(1.70831934892923,-1.30863471923704,-1.97,rotUni)); 
	 Alveolus_14->AddNode(CrystalWithWrapping_5B,4,new TGeoCombiTrans(-1.70831934892923,1.30863471923704,-1.97,rotSymmetric)); 
	 Alveolus_15->AddNode(CrystalWithWrapping_5A,1,new TGeoCombiTrans(1.70881961457002,1.30934395593351,-1.97,rotUni)); 
	 Alveolus_15->AddNode(CrystalWithWrapping_5A,2,new TGeoCombiTrans(-1.70881961457002,-1.30934395593351,-1.97,rotSymmetric)); 
	 Alveolus_15->AddNode(CrystalWithWrapping_5B,3,new TGeoCombiTrans(1.70831934892923,-1.30863471923704,-1.97,rotUni)); 
	 Alveolus_15->AddNode(CrystalWithWrapping_5B,4,new TGeoCombiTrans(-1.70831934892923,1.30863471923704,-1.97,rotSymmetric)); 
	 Alveolus_16->AddNode(CrystalWithWrapping_5A,1,new TGeoCombiTrans(1.70881961457002,1.30934395593351,-1.97,rotUni)); 
	 Alveolus_16->AddNode(CrystalWithWrapping_5A,2,new TGeoCombiTrans(-1.70881961457002,-1.30934395593351,-1.97,rotSymmetric)); 
	 Alveolus_16->AddNode(CrystalWithWrapping_5B,3,new TGeoCombiTrans(1.70831934892923,-1.30863471923704,-1.97,rotUni)); 
	 Alveolus_16->AddNode(CrystalWithWrapping_5B,4,new TGeoCombiTrans(-1.70831934892923,1.30863471923704,-1.97,rotSymmetric)); 
	 Alveolus_17->AddNode(CrystalWithWrapping_6A,1,new TGeoCombiTrans(0,0,-2.97,rotUni)); 
	 Alveolus_18->AddNode(CrystalWithWrapping_6A,1,new TGeoCombiTrans(0,0,-2.97,rotUni)); 
	 Alveolus_19->AddNode(CrystalWithWrapping_6A,1,new TGeoCombiTrans(0,0,-2.97,rotUni)); 

 //Positioning in space of alveoli 
	 Double_t phiEuler=0; Double_t thetaEuler = 0; Double_t psiEuler=0; 
	 Double_t rotAngle = 0; 		 phiEuler = 180.*TMath::ATan2(0.813559844749710,-0.042636864777188)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.813559844749710/TMath::Sin(TMath::Pi()*phiEuler/180.),0.5799159221583750)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(0.0000000000000000/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_1 = new TGeoRotation("rotAlv1",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_1_0 = new TGeoCombiTrans(-31.5461433249001,-0.0810299381113108,47.4332635320158,rotAlv_1); 
	 *transAlv_1_0 =  (*transAlv_1_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_1,0,transAlv_1_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_1 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_1 =  (*transAlv_1_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,1,transAlv_1_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_2 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_2 =  (*transAlv_1_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,2,transAlv_1_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_3 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_3 =  (*transAlv_1_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,3,transAlv_1_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_4 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_4 =  (*transAlv_1_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,4,transAlv_1_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_5 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_5 =  (*transAlv_1_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,5,transAlv_1_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_6 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_6 =  (*transAlv_1_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,6,transAlv_1_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_7 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_7 =  (*transAlv_1_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,7,transAlv_1_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_8 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_8 =  (*transAlv_1_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,8,transAlv_1_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_9 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_9 =  (*transAlv_1_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,9,transAlv_1_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_10 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_10 =  (*transAlv_1_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,10,transAlv_1_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_11 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_11 =  (*transAlv_1_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,11,transAlv_1_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_12 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_12 =  (*transAlv_1_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,12,transAlv_1_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_13 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_13 =  (*transAlv_1_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,13,transAlv_1_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_14 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_14 =  (*transAlv_1_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,14,transAlv_1_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_15 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_15 =  (*transAlv_1_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,15,transAlv_1_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_16 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_16 =  (*transAlv_1_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,16,transAlv_1_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_17 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_17 =  (*transAlv_1_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,17,transAlv_1_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_18 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_18 =  (*transAlv_1_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,18,transAlv_1_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_19 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_19 =  (*transAlv_1_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,19,transAlv_1_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_20 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_20 =  (*transAlv_1_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,20,transAlv_1_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_21 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_21 =  (*transAlv_1_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,21,transAlv_1_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_22 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_22 =  (*transAlv_1_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,22,transAlv_1_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_23 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_23 =  (*transAlv_1_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,23,transAlv_1_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_24 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_24 =  (*transAlv_1_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,24,transAlv_1_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_25 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_25 =  (*transAlv_1_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,25,transAlv_1_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_26 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_26 =  (*transAlv_1_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,26,transAlv_1_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_27 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_27 =  (*transAlv_1_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,27,transAlv_1_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_28 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_28 =  (*transAlv_1_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,28,transAlv_1_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_29 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_29 =  (*transAlv_1_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,29,transAlv_1_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_30 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_30 =  (*transAlv_1_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,30,transAlv_1_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_31 = new TGeoCombiTrans(-31.5461433249001*cos(rotAngle)+-0.0810299381113108*sin(rotAngle),31.5461433249001*sin(rotAngle)+-0.0810299381113108*cos(rotAngle),47.4332635320158,rotAlv_1); 
	 *transAlv_1_31 =  (*transAlv_1_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,31,transAlv_1_31); 

	 phiEuler = 180.*TMath::ATan2(0.773329159042415,-0.040528463880234)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.773329159042415/TMath::Sin(TMath::Pi()*phiEuler/180.),0.6327080332904430)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(0.0000000000000001/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_2 = new TGeoRotation("rotAlv2",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_2_0 = new TGeoCombiTrans(-31.4717165563416,-0.0771293964519507,41.2398571178426,rotAlv_2); 
	 *transAlv_2_0 =  (*transAlv_2_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_2,0,transAlv_2_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_1 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_1 =  (*transAlv_2_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,1,transAlv_2_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_2 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_2 =  (*transAlv_2_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,2,transAlv_2_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_3 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_3 =  (*transAlv_2_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,3,transAlv_2_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_4 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_4 =  (*transAlv_2_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,4,transAlv_2_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_5 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_5 =  (*transAlv_2_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,5,transAlv_2_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_6 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_6 =  (*transAlv_2_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,6,transAlv_2_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_7 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_7 =  (*transAlv_2_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,7,transAlv_2_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_8 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_8 =  (*transAlv_2_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,8,transAlv_2_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_9 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_9 =  (*transAlv_2_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,9,transAlv_2_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_10 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_10 =  (*transAlv_2_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,10,transAlv_2_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_11 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_11 =  (*transAlv_2_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,11,transAlv_2_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_12 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_12 =  (*transAlv_2_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,12,transAlv_2_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_13 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_13 =  (*transAlv_2_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,13,transAlv_2_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_14 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_14 =  (*transAlv_2_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,14,transAlv_2_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_15 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_15 =  (*transAlv_2_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,15,transAlv_2_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_16 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_16 =  (*transAlv_2_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,16,transAlv_2_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_17 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_17 =  (*transAlv_2_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,17,transAlv_2_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_18 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_18 =  (*transAlv_2_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,18,transAlv_2_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_19 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_19 =  (*transAlv_2_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,19,transAlv_2_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_20 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_20 =  (*transAlv_2_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,20,transAlv_2_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_21 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_21 =  (*transAlv_2_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,21,transAlv_2_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_22 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_22 =  (*transAlv_2_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,22,transAlv_2_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_23 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_23 =  (*transAlv_2_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,23,transAlv_2_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_24 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_24 =  (*transAlv_2_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,24,transAlv_2_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_25 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_25 =  (*transAlv_2_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,25,transAlv_2_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_26 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_26 =  (*transAlv_2_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,26,transAlv_2_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_27 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_27 =  (*transAlv_2_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,27,transAlv_2_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_28 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_28 =  (*transAlv_2_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,28,transAlv_2_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_29 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_29 =  (*transAlv_2_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,29,transAlv_2_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_30 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_30 =  (*transAlv_2_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,30,transAlv_2_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_31 = new TGeoCombiTrans(-31.4717165563416*cos(rotAngle)+-0.0771293964519507*sin(rotAngle),31.4717165563416*sin(rotAngle)+-0.0771293964519507*cos(rotAngle),41.2398571178426,rotAlv_2); 
	 *transAlv_2_31 =  (*transAlv_2_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,31,transAlv_2_31); 

	 phiEuler = 180.*TMath::ATan2(0.729675296319800,-0.038240661877815)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.729675296319800/TMath::Sin(TMath::Pi()*phiEuler/180.),0.6827236730330640)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(0.0000000000000000/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_3 = new TGeoRotation("rotAlv3",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_3_0 = new TGeoCombiTrans(-31.2450218510408,-0.065248830371914,35.664486688724,rotAlv_3); 
	 *transAlv_3_0 =  (*transAlv_3_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_3,0,transAlv_3_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_1 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_1 =  (*transAlv_3_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,1,transAlv_3_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_2 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_2 =  (*transAlv_3_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,2,transAlv_3_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_3 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_3 =  (*transAlv_3_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,3,transAlv_3_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_4 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_4 =  (*transAlv_3_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,4,transAlv_3_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_5 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_5 =  (*transAlv_3_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,5,transAlv_3_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_6 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_6 =  (*transAlv_3_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,6,transAlv_3_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_7 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_7 =  (*transAlv_3_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,7,transAlv_3_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_8 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_8 =  (*transAlv_3_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,8,transAlv_3_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_9 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_9 =  (*transAlv_3_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,9,transAlv_3_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_10 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_10 =  (*transAlv_3_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,10,transAlv_3_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_11 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_11 =  (*transAlv_3_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,11,transAlv_3_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_12 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_12 =  (*transAlv_3_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,12,transAlv_3_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_13 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_13 =  (*transAlv_3_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,13,transAlv_3_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_14 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_14 =  (*transAlv_3_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,14,transAlv_3_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_15 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_15 =  (*transAlv_3_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,15,transAlv_3_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_16 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_16 =  (*transAlv_3_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,16,transAlv_3_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_17 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_17 =  (*transAlv_3_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,17,transAlv_3_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_18 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_18 =  (*transAlv_3_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,18,transAlv_3_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_19 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_19 =  (*transAlv_3_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,19,transAlv_3_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_20 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_20 =  (*transAlv_3_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,20,transAlv_3_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_21 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_21 =  (*transAlv_3_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,21,transAlv_3_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_22 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_22 =  (*transAlv_3_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,22,transAlv_3_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_23 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_23 =  (*transAlv_3_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,23,transAlv_3_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_24 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_24 =  (*transAlv_3_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,24,transAlv_3_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_25 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_25 =  (*transAlv_3_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,25,transAlv_3_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_26 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_26 =  (*transAlv_3_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,26,transAlv_3_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_27 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_27 =  (*transAlv_3_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,27,transAlv_3_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_28 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_28 =  (*transAlv_3_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,28,transAlv_3_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_29 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_29 =  (*transAlv_3_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,29,transAlv_3_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_30 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_30 =  (*transAlv_3_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,30,transAlv_3_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_31 = new TGeoCombiTrans(-31.2450218510408*cos(rotAngle)+-0.065248830371914*sin(rotAngle),31.2450218510408*sin(rotAngle)+-0.065248830371914*cos(rotAngle),35.664486688724,rotAlv_3); 
	 *transAlv_3_31 =  (*transAlv_3_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,31,transAlv_3_31); 

	 phiEuler = 180.*TMath::ATan2(0.682788062692628,-0.03578340608669)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.682788062692628/TMath::Sin(TMath::Pi()*phiEuler/180.),0.7297396859793790)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000001/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_4 = new TGeoRotation("rotAlv4",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_4_0 = new TGeoCombiTrans(-31.1259816669844,-0.0590101986800681,30.8775893960182,rotAlv_4); 
	 *transAlv_4_0 =  (*transAlv_4_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_4,0,transAlv_4_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_1 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_1 =  (*transAlv_4_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,1,transAlv_4_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_2 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_2 =  (*transAlv_4_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,2,transAlv_4_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_3 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_3 =  (*transAlv_4_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,3,transAlv_4_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_4 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_4 =  (*transAlv_4_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,4,transAlv_4_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_5 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_5 =  (*transAlv_4_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,5,transAlv_4_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_6 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_6 =  (*transAlv_4_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,6,transAlv_4_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_7 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_7 =  (*transAlv_4_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,7,transAlv_4_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_8 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_8 =  (*transAlv_4_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,8,transAlv_4_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_9 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_9 =  (*transAlv_4_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,9,transAlv_4_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_10 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_10 =  (*transAlv_4_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,10,transAlv_4_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_11 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_11 =  (*transAlv_4_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,11,transAlv_4_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_12 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_12 =  (*transAlv_4_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,12,transAlv_4_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_13 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_13 =  (*transAlv_4_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,13,transAlv_4_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_14 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_14 =  (*transAlv_4_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,14,transAlv_4_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_15 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_15 =  (*transAlv_4_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,15,transAlv_4_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_16 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_16 =  (*transAlv_4_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,16,transAlv_4_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_17 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_17 =  (*transAlv_4_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,17,transAlv_4_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_18 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_18 =  (*transAlv_4_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,18,transAlv_4_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_19 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_19 =  (*transAlv_4_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,19,transAlv_4_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_20 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_20 =  (*transAlv_4_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,20,transAlv_4_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_21 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_21 =  (*transAlv_4_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,21,transAlv_4_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_22 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_22 =  (*transAlv_4_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,22,transAlv_4_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_23 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_23 =  (*transAlv_4_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,23,transAlv_4_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_24 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_24 =  (*transAlv_4_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,24,transAlv_4_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_25 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_25 =  (*transAlv_4_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,25,transAlv_4_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_26 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_26 =  (*transAlv_4_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,26,transAlv_4_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_27 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_27 =  (*transAlv_4_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,27,transAlv_4_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_28 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_28 =  (*transAlv_4_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,28,transAlv_4_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_29 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_29 =  (*transAlv_4_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,29,transAlv_4_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_30 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_30 =  (*transAlv_4_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,30,transAlv_4_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_31 = new TGeoCombiTrans(-31.1259816669844*cos(rotAngle)+-0.0590101986800681*sin(rotAngle),31.1259816669844*sin(rotAngle)+-0.0590101986800681*cos(rotAngle),30.8775893960182,rotAlv_4); 
	 *transAlv_4_31 =  (*transAlv_4_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,31,transAlv_4_31); 

	 phiEuler = 180.*TMath::ATan2(0.624723405220401,-0.03274036633374)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.624723405220401/TMath::Sin(TMath::Pi()*phiEuler/180.),0.7801594294643620)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000002/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_5 = new TGeoRotation("rotAlv5",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_5_0 = new TGeoCombiTrans(-30.9781427199437,-0.0512622877741241,26.5624237999229,rotAlv_5); 
	 *transAlv_5_0 =  (*transAlv_5_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_5,0,transAlv_5_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_1 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_1 =  (*transAlv_5_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,1,transAlv_5_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_2 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_2 =  (*transAlv_5_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,2,transAlv_5_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_3 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_3 =  (*transAlv_5_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,3,transAlv_5_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_4 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_4 =  (*transAlv_5_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,4,transAlv_5_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_5 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_5 =  (*transAlv_5_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,5,transAlv_5_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_6 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_6 =  (*transAlv_5_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,6,transAlv_5_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_7 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_7 =  (*transAlv_5_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,7,transAlv_5_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_8 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_8 =  (*transAlv_5_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,8,transAlv_5_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_9 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_9 =  (*transAlv_5_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,9,transAlv_5_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_10 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_10 =  (*transAlv_5_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,10,transAlv_5_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_11 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_11 =  (*transAlv_5_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,11,transAlv_5_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_12 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_12 =  (*transAlv_5_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,12,transAlv_5_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_13 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_13 =  (*transAlv_5_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,13,transAlv_5_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_14 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_14 =  (*transAlv_5_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,14,transAlv_5_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_15 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_15 =  (*transAlv_5_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,15,transAlv_5_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_16 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_16 =  (*transAlv_5_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,16,transAlv_5_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_17 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_17 =  (*transAlv_5_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,17,transAlv_5_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_18 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_18 =  (*transAlv_5_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,18,transAlv_5_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_19 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_19 =  (*transAlv_5_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,19,transAlv_5_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_20 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_20 =  (*transAlv_5_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,20,transAlv_5_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_21 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_21 =  (*transAlv_5_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,21,transAlv_5_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_22 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_22 =  (*transAlv_5_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,22,transAlv_5_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_23 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_23 =  (*transAlv_5_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,23,transAlv_5_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_24 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_24 =  (*transAlv_5_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,24,transAlv_5_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_25 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_25 =  (*transAlv_5_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,25,transAlv_5_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_26 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_26 =  (*transAlv_5_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,26,transAlv_5_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_27 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_27 =  (*transAlv_5_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,27,transAlv_5_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_28 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_28 =  (*transAlv_5_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,28,transAlv_5_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_29 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_29 =  (*transAlv_5_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,29,transAlv_5_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_30 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_30 =  (*transAlv_5_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,30,transAlv_5_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_31 = new TGeoCombiTrans(-30.9781427199437*cos(rotAngle)+-0.0512622877741241*sin(rotAngle),30.9781427199437*sin(rotAngle)+-0.0512622877741241*cos(rotAngle),26.5624237999229,rotAlv_5); 
	 *transAlv_5_31 =  (*transAlv_5_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,31,transAlv_5_31); 

	 phiEuler = 180.*TMath::ATan2(0.554248640872151,-0.029046940438753)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.554248640872151/TMath::Sin(TMath::Pi()*phiEuler/180.),0.8318441677036160)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000002/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_6 = new TGeoRotation("rotAlv6",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_6_0 = new TGeoCombiTrans(-30.8724305734213,-0.045722148931643,22.7699406331834,rotAlv_6); 
	 *transAlv_6_0 =  (*transAlv_6_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_6,0,transAlv_6_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_1 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_1 =  (*transAlv_6_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,1,transAlv_6_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_2 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_2 =  (*transAlv_6_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,2,transAlv_6_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_3 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_3 =  (*transAlv_6_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,3,transAlv_6_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_4 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_4 =  (*transAlv_6_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,4,transAlv_6_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_5 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_5 =  (*transAlv_6_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,5,transAlv_6_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_6 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_6 =  (*transAlv_6_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,6,transAlv_6_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_7 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_7 =  (*transAlv_6_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,7,transAlv_6_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_8 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_8 =  (*transAlv_6_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,8,transAlv_6_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_9 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_9 =  (*transAlv_6_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,9,transAlv_6_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_10 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_10 =  (*transAlv_6_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,10,transAlv_6_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_11 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_11 =  (*transAlv_6_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,11,transAlv_6_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_12 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_12 =  (*transAlv_6_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,12,transAlv_6_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_13 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_13 =  (*transAlv_6_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,13,transAlv_6_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_14 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_14 =  (*transAlv_6_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,14,transAlv_6_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_15 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_15 =  (*transAlv_6_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,15,transAlv_6_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_16 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_16 =  (*transAlv_6_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,16,transAlv_6_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_17 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_17 =  (*transAlv_6_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,17,transAlv_6_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_18 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_18 =  (*transAlv_6_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,18,transAlv_6_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_19 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_19 =  (*transAlv_6_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,19,transAlv_6_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_20 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_20 =  (*transAlv_6_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,20,transAlv_6_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_21 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_21 =  (*transAlv_6_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,21,transAlv_6_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_22 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_22 =  (*transAlv_6_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,22,transAlv_6_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_23 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_23 =  (*transAlv_6_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,23,transAlv_6_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_24 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_24 =  (*transAlv_6_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,24,transAlv_6_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_25 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_25 =  (*transAlv_6_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,25,transAlv_6_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_26 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_26 =  (*transAlv_6_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,26,transAlv_6_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_27 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_27 =  (*transAlv_6_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,27,transAlv_6_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_28 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_28 =  (*transAlv_6_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,28,transAlv_6_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_29 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_29 =  (*transAlv_6_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,29,transAlv_6_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_30 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_30 =  (*transAlv_6_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,30,transAlv_6_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_31 = new TGeoCombiTrans(-30.8724305734213*cos(rotAngle)+-0.045722148931643*sin(rotAngle),30.8724305734213*sin(rotAngle)+-0.045722148931643*cos(rotAngle),22.7699406331834,rotAlv_6); 
	 *transAlv_6_31 =  (*transAlv_6_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,31,transAlv_6_31); 

	 phiEuler = 180.*TMath::ATan2(0.479532962143682,-0.025131257638969)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.479532962143682/TMath::Sin(TMath::Pi()*phiEuler/180.),0.8771639288680240)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000003/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_7 = new TGeoRotation("rotAlv7",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_7_0 = new TGeoCombiTrans(-30.7603570553286,-0.0398486247319627,19.1978684461435,rotAlv_7); 
	 *transAlv_7_0 =  (*transAlv_7_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_7,0,transAlv_7_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_1 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_1 =  (*transAlv_7_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,1,transAlv_7_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_2 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_2 =  (*transAlv_7_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,2,transAlv_7_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_3 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_3 =  (*transAlv_7_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,3,transAlv_7_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_4 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_4 =  (*transAlv_7_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,4,transAlv_7_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_5 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_5 =  (*transAlv_7_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,5,transAlv_7_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_6 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_6 =  (*transAlv_7_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,6,transAlv_7_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_7 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_7 =  (*transAlv_7_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,7,transAlv_7_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_8 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_8 =  (*transAlv_7_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,8,transAlv_7_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_9 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_9 =  (*transAlv_7_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,9,transAlv_7_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_10 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_10 =  (*transAlv_7_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,10,transAlv_7_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_11 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_11 =  (*transAlv_7_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,11,transAlv_7_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_12 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_12 =  (*transAlv_7_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,12,transAlv_7_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_13 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_13 =  (*transAlv_7_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,13,transAlv_7_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_14 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_14 =  (*transAlv_7_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,14,transAlv_7_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_15 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_15 =  (*transAlv_7_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,15,transAlv_7_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_16 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_16 =  (*transAlv_7_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,16,transAlv_7_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_17 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_17 =  (*transAlv_7_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,17,transAlv_7_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_18 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_18 =  (*transAlv_7_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,18,transAlv_7_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_19 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_19 =  (*transAlv_7_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,19,transAlv_7_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_20 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_20 =  (*transAlv_7_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,20,transAlv_7_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_21 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_21 =  (*transAlv_7_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,21,transAlv_7_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_22 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_22 =  (*transAlv_7_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,22,transAlv_7_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_23 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_23 =  (*transAlv_7_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,23,transAlv_7_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_24 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_24 =  (*transAlv_7_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,24,transAlv_7_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_25 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_25 =  (*transAlv_7_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,25,transAlv_7_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_26 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_26 =  (*transAlv_7_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,26,transAlv_7_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_27 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_27 =  (*transAlv_7_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,27,transAlv_7_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_28 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_28 =  (*transAlv_7_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,28,transAlv_7_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_29 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_29 =  (*transAlv_7_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,29,transAlv_7_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_30 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_30 =  (*transAlv_7_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,30,transAlv_7_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_31 = new TGeoCombiTrans(-30.7603570553286*cos(rotAngle)+-0.0398486247319627*sin(rotAngle),30.7603570553286*sin(rotAngle)+-0.0398486247319627*cos(rotAngle),19.1978684461435,rotAlv_7); 
	 *transAlv_7_31 =  (*transAlv_7_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,31,transAlv_7_31); 

	 phiEuler = 180.*TMath::ATan2(0.401148066957157,-0.021023279352909)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.401148066957157/TMath::Sin(TMath::Pi()*phiEuler/180.),0.9157719421896400)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000003/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_8 = new TGeoRotation("rotAlv8",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_8_0 = new TGeoCombiTrans(-30.6038250889926,-0.0316451319894881,15.7807044574297,rotAlv_8); 
	 *transAlv_8_0 =  (*transAlv_8_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_8,0,transAlv_8_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_1 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_1 =  (*transAlv_8_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,1,transAlv_8_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_2 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_2 =  (*transAlv_8_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,2,transAlv_8_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_3 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_3 =  (*transAlv_8_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,3,transAlv_8_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_4 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_4 =  (*transAlv_8_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,4,transAlv_8_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_5 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_5 =  (*transAlv_8_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,5,transAlv_8_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_6 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_6 =  (*transAlv_8_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,6,transAlv_8_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_7 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_7 =  (*transAlv_8_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,7,transAlv_8_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_8 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_8 =  (*transAlv_8_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,8,transAlv_8_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_9 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_9 =  (*transAlv_8_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,9,transAlv_8_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_10 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_10 =  (*transAlv_8_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,10,transAlv_8_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_11 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_11 =  (*transAlv_8_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,11,transAlv_8_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_12 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_12 =  (*transAlv_8_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,12,transAlv_8_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_13 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_13 =  (*transAlv_8_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,13,transAlv_8_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_14 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_14 =  (*transAlv_8_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,14,transAlv_8_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_15 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_15 =  (*transAlv_8_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,15,transAlv_8_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_16 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_16 =  (*transAlv_8_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,16,transAlv_8_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_17 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_17 =  (*transAlv_8_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,17,transAlv_8_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_18 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_18 =  (*transAlv_8_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,18,transAlv_8_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_19 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_19 =  (*transAlv_8_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,19,transAlv_8_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_20 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_20 =  (*transAlv_8_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,20,transAlv_8_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_21 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_21 =  (*transAlv_8_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,21,transAlv_8_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_22 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_22 =  (*transAlv_8_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,22,transAlv_8_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_23 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_23 =  (*transAlv_8_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,23,transAlv_8_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_24 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_24 =  (*transAlv_8_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,24,transAlv_8_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_25 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_25 =  (*transAlv_8_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,25,transAlv_8_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_26 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_26 =  (*transAlv_8_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,26,transAlv_8_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_27 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_27 =  (*transAlv_8_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,27,transAlv_8_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_28 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_28 =  (*transAlv_8_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,28,transAlv_8_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_29 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_29 =  (*transAlv_8_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,29,transAlv_8_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_30 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_30 =  (*transAlv_8_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,30,transAlv_8_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_31 = new TGeoCombiTrans(-30.6038250889926*cos(rotAngle)+-0.0316451319894881*sin(rotAngle),30.6038250889926*sin(rotAngle)+-0.0316451319894881*cos(rotAngle),15.7807044574297,rotAlv_8); 
	 *transAlv_8_31 =  (*transAlv_8_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,31,transAlv_8_31); 

	 phiEuler = 180.*TMath::ATan2(0.319693728780067,-0.016754438376078)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.319693728780067/TMath::Sin(TMath::Pi()*phiEuler/180.),0.9473727928188560)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000003/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_9 = new TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_9_0 = new TGeoCombiTrans(-30.4830358471327,-0.0253148360623216,12.5196418598667,rotAlv_9); 
	 *transAlv_9_0 =  (*transAlv_9_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_9,0,transAlv_9_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_1 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_1 =  (*transAlv_9_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,1,transAlv_9_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_2 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_2 =  (*transAlv_9_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,2,transAlv_9_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_3 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_3 =  (*transAlv_9_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,3,transAlv_9_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_4 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_4 =  (*transAlv_9_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,4,transAlv_9_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_5 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_5 =  (*transAlv_9_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,5,transAlv_9_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_6 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_6 =  (*transAlv_9_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,6,transAlv_9_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_7 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_7 =  (*transAlv_9_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,7,transAlv_9_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_8 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_8 =  (*transAlv_9_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,8,transAlv_9_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_9 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_9 =  (*transAlv_9_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,9,transAlv_9_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_10 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_10 =  (*transAlv_9_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,10,transAlv_9_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_11 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_11 =  (*transAlv_9_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,11,transAlv_9_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_12 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_12 =  (*transAlv_9_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,12,transAlv_9_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_13 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_13 =  (*transAlv_9_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,13,transAlv_9_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_14 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_14 =  (*transAlv_9_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,14,transAlv_9_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_15 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_15 =  (*transAlv_9_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,15,transAlv_9_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_16 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_16 =  (*transAlv_9_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,16,transAlv_9_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_17 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_17 =  (*transAlv_9_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,17,transAlv_9_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_18 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_18 =  (*transAlv_9_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,18,transAlv_9_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_19 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_19 =  (*transAlv_9_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,19,transAlv_9_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_20 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_20 =  (*transAlv_9_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,20,transAlv_9_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_21 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_21 =  (*transAlv_9_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,21,transAlv_9_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_22 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_22 =  (*transAlv_9_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,22,transAlv_9_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_23 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_23 =  (*transAlv_9_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,23,transAlv_9_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_24 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_24 =  (*transAlv_9_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,24,transAlv_9_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_25 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_25 =  (*transAlv_9_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,25,transAlv_9_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_26 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_26 =  (*transAlv_9_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,26,transAlv_9_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_27 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_27 =  (*transAlv_9_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,27,transAlv_9_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_28 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_28 =  (*transAlv_9_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,28,transAlv_9_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_29 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_29 =  (*transAlv_9_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,29,transAlv_9_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_30 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_30 =  (*transAlv_9_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,30,transAlv_9_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_31 = new TGeoCombiTrans(-30.4830358471327*cos(rotAngle)+-0.0253148360623216*sin(rotAngle),30.4830358471327*sin(rotAngle)+-0.0253148360623216*cos(rotAngle),12.5196418598667,rotAlv_9); 
	 *transAlv_9_31 =  (*transAlv_9_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,31,transAlv_9_31); 

	 phiEuler = 180.*TMath::ATan2(0.235793207370996,-0.012357398368339)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.235793207370996/TMath::Sin(TMath::Pi()*phiEuler/180.),0.9717246822342550)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000004/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_10 = new TGeoRotation("rotAlv10",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_10_0 = new TGeoCombiTrans(-30.3947474231694,-0.0206878358259987,9.37352740964767,rotAlv_10); 
	 *transAlv_10_0 =  (*transAlv_10_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_10,0,transAlv_10_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_1 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_1 =  (*transAlv_10_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,1,transAlv_10_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_2 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_2 =  (*transAlv_10_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,2,transAlv_10_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_3 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_3 =  (*transAlv_10_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,3,transAlv_10_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_4 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_4 =  (*transAlv_10_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,4,transAlv_10_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_5 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_5 =  (*transAlv_10_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,5,transAlv_10_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_6 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_6 =  (*transAlv_10_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,6,transAlv_10_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_7 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_7 =  (*transAlv_10_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,7,transAlv_10_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_8 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_8 =  (*transAlv_10_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,8,transAlv_10_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_9 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_9 =  (*transAlv_10_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,9,transAlv_10_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_10 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_10 =  (*transAlv_10_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,10,transAlv_10_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_11 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_11 =  (*transAlv_10_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,11,transAlv_10_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_12 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_12 =  (*transAlv_10_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,12,transAlv_10_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_13 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_13 =  (*transAlv_10_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,13,transAlv_10_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_14 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_14 =  (*transAlv_10_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,14,transAlv_10_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_15 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_15 =  (*transAlv_10_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,15,transAlv_10_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_16 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_16 =  (*transAlv_10_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,16,transAlv_10_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_17 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_17 =  (*transAlv_10_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,17,transAlv_10_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_18 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_18 =  (*transAlv_10_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,18,transAlv_10_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_19 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_19 =  (*transAlv_10_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,19,transAlv_10_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_20 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_20 =  (*transAlv_10_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,20,transAlv_10_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_21 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_21 =  (*transAlv_10_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,21,transAlv_10_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_22 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_22 =  (*transAlv_10_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,22,transAlv_10_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_23 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_23 =  (*transAlv_10_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,23,transAlv_10_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_24 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_24 =  (*transAlv_10_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,24,transAlv_10_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_25 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_25 =  (*transAlv_10_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,25,transAlv_10_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_26 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_26 =  (*transAlv_10_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,26,transAlv_10_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_27 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_27 =  (*transAlv_10_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,27,transAlv_10_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_28 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_28 =  (*transAlv_10_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,28,transAlv_10_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_29 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_29 =  (*transAlv_10_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,29,transAlv_10_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_30 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_30 =  (*transAlv_10_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,30,transAlv_10_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_31 = new TGeoCombiTrans(-30.3947474231694*cos(rotAngle)+-0.0206878358259987*sin(rotAngle),30.3947474231694*sin(rotAngle)+-0.0206878358259987*cos(rotAngle),9.37352740964767,rotAlv_10); 
	 *transAlv_10_31 =  (*transAlv_10_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,31,transAlv_10_31); 

	 phiEuler = 180.*TMath::ATan2(0.150005183515807,-0.007861438549008)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.150005183515807/TMath::Sin(TMath::Pi()*phiEuler/180.),0.9886539549823940)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000004/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_11 = new TGeoRotation("rotAlv11",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_11_0 = new TGeoCombiTrans(-30.2885661649142,-0.0151231118793882,6.14503714445403,rotAlv_11); 
	 *transAlv_11_0 =  (*transAlv_11_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_11,0,transAlv_11_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_1 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_1 =  (*transAlv_11_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,1,transAlv_11_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_2 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_2 =  (*transAlv_11_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,2,transAlv_11_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_3 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_3 =  (*transAlv_11_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,3,transAlv_11_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_4 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_4 =  (*transAlv_11_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,4,transAlv_11_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_5 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_5 =  (*transAlv_11_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,5,transAlv_11_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_6 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_6 =  (*transAlv_11_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,6,transAlv_11_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_7 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_7 =  (*transAlv_11_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,7,transAlv_11_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_8 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_8 =  (*transAlv_11_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,8,transAlv_11_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_9 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_9 =  (*transAlv_11_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,9,transAlv_11_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_10 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_10 =  (*transAlv_11_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,10,transAlv_11_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_11 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_11 =  (*transAlv_11_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,11,transAlv_11_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_12 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_12 =  (*transAlv_11_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,12,transAlv_11_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_13 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_13 =  (*transAlv_11_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,13,transAlv_11_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_14 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_14 =  (*transAlv_11_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,14,transAlv_11_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_15 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_15 =  (*transAlv_11_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,15,transAlv_11_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_16 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_16 =  (*transAlv_11_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,16,transAlv_11_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_17 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_17 =  (*transAlv_11_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,17,transAlv_11_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_18 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_18 =  (*transAlv_11_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,18,transAlv_11_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_19 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_19 =  (*transAlv_11_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,19,transAlv_11_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_20 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_20 =  (*transAlv_11_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,20,transAlv_11_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_21 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_21 =  (*transAlv_11_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,21,transAlv_11_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_22 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_22 =  (*transAlv_11_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,22,transAlv_11_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_23 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_23 =  (*transAlv_11_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,23,transAlv_11_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_24 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_24 =  (*transAlv_11_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,24,transAlv_11_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_25 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_25 =  (*transAlv_11_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,25,transAlv_11_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_26 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_26 =  (*transAlv_11_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,26,transAlv_11_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_27 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_27 =  (*transAlv_11_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,27,transAlv_11_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_28 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_28 =  (*transAlv_11_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,28,transAlv_11_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_29 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_29 =  (*transAlv_11_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,29,transAlv_11_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_30 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_30 =  (*transAlv_11_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,30,transAlv_11_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_31 = new TGeoCombiTrans(-30.2885661649142*cos(rotAngle)+-0.0151231118793882*sin(rotAngle),30.2885661649142*sin(rotAngle)+-0.0151231118793882*cos(rotAngle),6.14503714445403,rotAlv_11); 
	 *transAlv_11_31 =  (*transAlv_11_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,31,transAlv_11_31); 

	 phiEuler = 180.*TMath::ATan2(0.062983075266112,-0.003300803107114)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.062983075266112/TMath::Sin(TMath::Pi()*phiEuler/180.),0.9980091366960880)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000004/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_12 = new TGeoRotation("rotAlv12",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_12_0 = new TGeoCombiTrans(-30.1132720145459,-0.0059363347372676,2.81041828305859,rotAlv_12); 
	 *transAlv_12_0 =  (*transAlv_12_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_12,0,transAlv_12_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_1 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_1 =  (*transAlv_12_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,1,transAlv_12_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_2 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_2 =  (*transAlv_12_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,2,transAlv_12_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_3 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_3 =  (*transAlv_12_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,3,transAlv_12_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_4 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_4 =  (*transAlv_12_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,4,transAlv_12_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_5 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_5 =  (*transAlv_12_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,5,transAlv_12_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_6 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_6 =  (*transAlv_12_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,6,transAlv_12_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_7 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_7 =  (*transAlv_12_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,7,transAlv_12_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_8 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_8 =  (*transAlv_12_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,8,transAlv_12_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_9 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_9 =  (*transAlv_12_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,9,transAlv_12_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_10 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_10 =  (*transAlv_12_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,10,transAlv_12_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_11 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_11 =  (*transAlv_12_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,11,transAlv_12_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_12 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_12 =  (*transAlv_12_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,12,transAlv_12_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_13 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_13 =  (*transAlv_12_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,13,transAlv_12_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_14 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_14 =  (*transAlv_12_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,14,transAlv_12_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_15 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_15 =  (*transAlv_12_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,15,transAlv_12_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_16 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_16 =  (*transAlv_12_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,16,transAlv_12_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_17 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_17 =  (*transAlv_12_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,17,transAlv_12_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_18 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_18 =  (*transAlv_12_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,18,transAlv_12_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_19 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_19 =  (*transAlv_12_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,19,transAlv_12_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_20 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_20 =  (*transAlv_12_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,20,transAlv_12_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_21 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_21 =  (*transAlv_12_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,21,transAlv_12_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_22 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_22 =  (*transAlv_12_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,22,transAlv_12_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_23 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_23 =  (*transAlv_12_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,23,transAlv_12_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_24 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_24 =  (*transAlv_12_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,24,transAlv_12_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_25 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_25 =  (*transAlv_12_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,25,transAlv_12_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_26 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_26 =  (*transAlv_12_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,26,transAlv_12_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_27 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_27 =  (*transAlv_12_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,27,transAlv_12_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_28 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_28 =  (*transAlv_12_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,28,transAlv_12_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_29 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_29 =  (*transAlv_12_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,29,transAlv_12_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_30 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_30 =  (*transAlv_12_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,30,transAlv_12_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_31 = new TGeoCombiTrans(-30.1132720145459*cos(rotAngle)+-0.0059363347372676*sin(rotAngle),30.1132720145459*sin(rotAngle)+-0.0059363347372676*cos(rotAngle),2.81041828305859,rotAlv_12); 
	 *transAlv_12_31 =  (*transAlv_12_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,31,transAlv_12_31); 

	 phiEuler = 180.*TMath::ATan2(+0.024522816449008,-0.001285186351857)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(-0.024522816449008/TMath::Sin(TMath::Pi()*phiEuler/180.),0.9996984444168400)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000005/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_13 = new TGeoRotation("rotAlv13",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_13_0 = new TGeoCombiTrans(-30.0005949649723,-3.11807929542e-05,-0.489199553504785,rotAlv_13); 
	 *transAlv_13_0 =  (*transAlv_13_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_13,0,transAlv_13_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_1 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_1 =  (*transAlv_13_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,1,transAlv_13_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_2 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_2 =  (*transAlv_13_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,2,transAlv_13_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_3 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_3 =  (*transAlv_13_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,3,transAlv_13_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_4 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_4 =  (*transAlv_13_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,4,transAlv_13_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_5 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_5 =  (*transAlv_13_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,5,transAlv_13_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_6 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_6 =  (*transAlv_13_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,6,transAlv_13_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_7 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_7 =  (*transAlv_13_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,7,transAlv_13_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_8 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_8 =  (*transAlv_13_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,8,transAlv_13_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_9 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_9 =  (*transAlv_13_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,9,transAlv_13_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_10 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_10 =  (*transAlv_13_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,10,transAlv_13_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_11 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_11 =  (*transAlv_13_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,11,transAlv_13_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_12 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_12 =  (*transAlv_13_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,12,transAlv_13_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_13 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_13 =  (*transAlv_13_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,13,transAlv_13_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_14 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_14 =  (*transAlv_13_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,14,transAlv_13_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_15 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_15 =  (*transAlv_13_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,15,transAlv_13_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_16 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_16 =  (*transAlv_13_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,16,transAlv_13_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_17 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_17 =  (*transAlv_13_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,17,transAlv_13_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_18 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_18 =  (*transAlv_13_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,18,transAlv_13_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_19 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_19 =  (*transAlv_13_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,19,transAlv_13_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_20 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_20 =  (*transAlv_13_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,20,transAlv_13_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_21 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_21 =  (*transAlv_13_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,21,transAlv_13_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_22 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_22 =  (*transAlv_13_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,22,transAlv_13_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_23 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_23 =  (*transAlv_13_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,23,transAlv_13_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_24 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_24 =  (*transAlv_13_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,24,transAlv_13_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_25 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_25 =  (*transAlv_13_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,25,transAlv_13_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_26 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_26 =  (*transAlv_13_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,26,transAlv_13_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_27 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_27 =  (*transAlv_13_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,27,transAlv_13_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_28 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_28 =  (*transAlv_13_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,28,transAlv_13_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_29 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_29 =  (*transAlv_13_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,29,transAlv_13_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_30 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_30 =  (*transAlv_13_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,30,transAlv_13_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_31 = new TGeoCombiTrans(-30.0005949649723*cos(rotAngle)+-3.11807929542e-05*sin(rotAngle),30.0005949649723*sin(rotAngle)+-3.11807929542e-05*cos(rotAngle),-0.489199553504785,rotAlv_13); 
	 *transAlv_13_31 =  (*transAlv_13_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,31,transAlv_13_31); 

	 phiEuler = 180.*TMath::ATan2(+0.123930549486792,-0.006494924883929)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(-0.123930549486792/TMath::Sin(TMath::Pi()*phiEuler/180.),0.9922696381803960)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000006/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_14 = new TGeoRotation("rotAlv14",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_14_0 = new TGeoCombiTrans(-29.7421983467086,0.0135108121444779,-4.32544824301447,rotAlv_14); 
	 *transAlv_14_0 =  (*transAlv_14_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_14,0,transAlv_14_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_1 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_1 =  (*transAlv_14_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,1,transAlv_14_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_2 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_2 =  (*transAlv_14_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,2,transAlv_14_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_3 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_3 =  (*transAlv_14_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,3,transAlv_14_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_4 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_4 =  (*transAlv_14_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,4,transAlv_14_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_5 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_5 =  (*transAlv_14_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,5,transAlv_14_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_6 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_6 =  (*transAlv_14_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,6,transAlv_14_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_7 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_7 =  (*transAlv_14_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,7,transAlv_14_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_8 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_8 =  (*transAlv_14_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,8,transAlv_14_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_9 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_9 =  (*transAlv_14_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,9,transAlv_14_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_10 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_10 =  (*transAlv_14_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,10,transAlv_14_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_11 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_11 =  (*transAlv_14_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,11,transAlv_14_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_12 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_12 =  (*transAlv_14_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,12,transAlv_14_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_13 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_13 =  (*transAlv_14_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,13,transAlv_14_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_14 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_14 =  (*transAlv_14_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,14,transAlv_14_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_15 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_15 =  (*transAlv_14_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,15,transAlv_14_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_16 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_16 =  (*transAlv_14_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,16,transAlv_14_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_17 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_17 =  (*transAlv_14_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,17,transAlv_14_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_18 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_18 =  (*transAlv_14_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,18,transAlv_14_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_19 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_19 =  (*transAlv_14_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,19,transAlv_14_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_20 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_20 =  (*transAlv_14_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,20,transAlv_14_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_21 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_21 =  (*transAlv_14_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,21,transAlv_14_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_22 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_22 =  (*transAlv_14_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,22,transAlv_14_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_23 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_23 =  (*transAlv_14_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,23,transAlv_14_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_24 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_24 =  (*transAlv_14_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,24,transAlv_14_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_25 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_25 =  (*transAlv_14_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,25,transAlv_14_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_26 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_26 =  (*transAlv_14_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,26,transAlv_14_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_27 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_27 =  (*transAlv_14_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,27,transAlv_14_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_28 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_28 =  (*transAlv_14_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,28,transAlv_14_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_29 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_29 =  (*transAlv_14_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,29,transAlv_14_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_30 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_30 =  (*transAlv_14_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,30,transAlv_14_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_31 = new TGeoCombiTrans(-29.7421983467086*cos(rotAngle)+0.0135108121444779*sin(rotAngle),29.7421983467086*sin(rotAngle)+0.0135108121444779*cos(rotAngle),-4.32544824301447,rotAlv_14); 
	 *transAlv_14_31 =  (*transAlv_14_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,31,transAlv_14_31); 

	 phiEuler = 180.*TMath::ATan2(+0.233957073089363,-0.012261170648173)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(-0.233957073089363/TMath::Sin(TMath::Pi()*phiEuler/180.),0.9721696105339820)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000006/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_15 = new TGeoRotation("rotAlv15",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_15_0 = new TGeoCombiTrans(-29.5263520513166,0.0248228371524423,-8.75161937874914,rotAlv_15); 
	 *transAlv_15_0 =  (*transAlv_15_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_15,0,transAlv_15_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_1 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_1 =  (*transAlv_15_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,1,transAlv_15_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_2 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_2 =  (*transAlv_15_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,2,transAlv_15_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_3 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_3 =  (*transAlv_15_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,3,transAlv_15_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_4 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_4 =  (*transAlv_15_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,4,transAlv_15_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_5 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_5 =  (*transAlv_15_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,5,transAlv_15_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_6 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_6 =  (*transAlv_15_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,6,transAlv_15_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_7 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_7 =  (*transAlv_15_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,7,transAlv_15_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_8 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_8 =  (*transAlv_15_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,8,transAlv_15_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_9 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_9 =  (*transAlv_15_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,9,transAlv_15_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_10 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_10 =  (*transAlv_15_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,10,transAlv_15_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_11 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_11 =  (*transAlv_15_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,11,transAlv_15_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_12 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_12 =  (*transAlv_15_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,12,transAlv_15_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_13 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_13 =  (*transAlv_15_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,13,transAlv_15_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_14 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_14 =  (*transAlv_15_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,14,transAlv_15_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_15 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_15 =  (*transAlv_15_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,15,transAlv_15_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_16 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_16 =  (*transAlv_15_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,16,transAlv_15_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_17 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_17 =  (*transAlv_15_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,17,transAlv_15_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_18 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_18 =  (*transAlv_15_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,18,transAlv_15_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_19 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_19 =  (*transAlv_15_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,19,transAlv_15_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_20 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_20 =  (*transAlv_15_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,20,transAlv_15_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_21 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_21 =  (*transAlv_15_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,21,transAlv_15_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_22 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_22 =  (*transAlv_15_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,22,transAlv_15_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_23 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_23 =  (*transAlv_15_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,23,transAlv_15_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_24 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_24 =  (*transAlv_15_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,24,transAlv_15_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_25 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_25 =  (*transAlv_15_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,25,transAlv_15_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_26 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_26 =  (*transAlv_15_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,26,transAlv_15_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_27 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_27 =  (*transAlv_15_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,27,transAlv_15_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_28 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_28 =  (*transAlv_15_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,28,transAlv_15_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_29 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_29 =  (*transAlv_15_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,29,transAlv_15_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_30 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_30 =  (*transAlv_15_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,30,transAlv_15_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_31 = new TGeoCombiTrans(-29.5263520513166*cos(rotAngle)+0.0248228371524423*sin(rotAngle),29.5263520513166*sin(rotAngle)+0.0248228371524423*cos(rotAngle),-8.75161937874914,rotAlv_15); 
	 *transAlv_15_31 =  (*transAlv_15_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,31,transAlv_15_31); 

	 phiEuler = 180.*TMath::ATan2(+0.341049050524467,-0.017873623364576)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(-0.341049050524467/TMath::Sin(TMath::Pi()*phiEuler/180.),0.9398755655533240)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000007/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_16 = new TGeoRotation("rotAlv16",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_16_0 = new TGeoCombiTrans(-29.2907497009593,0.0371702331284625,-13.2685421037292,rotAlv_16); 
	 *transAlv_16_0 =  (*transAlv_16_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_16,0,transAlv_16_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_1 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_1 =  (*transAlv_16_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,1,transAlv_16_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_2 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_2 =  (*transAlv_16_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,2,transAlv_16_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_3 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_3 =  (*transAlv_16_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,3,transAlv_16_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_4 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_4 =  (*transAlv_16_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,4,transAlv_16_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_5 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_5 =  (*transAlv_16_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,5,transAlv_16_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_6 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_6 =  (*transAlv_16_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,6,transAlv_16_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_7 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_7 =  (*transAlv_16_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,7,transAlv_16_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_8 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_8 =  (*transAlv_16_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,8,transAlv_16_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_9 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_9 =  (*transAlv_16_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,9,transAlv_16_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_10 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_10 =  (*transAlv_16_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,10,transAlv_16_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_11 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_11 =  (*transAlv_16_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,11,transAlv_16_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_12 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_12 =  (*transAlv_16_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,12,transAlv_16_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_13 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_13 =  (*transAlv_16_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,13,transAlv_16_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_14 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_14 =  (*transAlv_16_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,14,transAlv_16_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_15 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_15 =  (*transAlv_16_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,15,transAlv_16_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_16 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_16 =  (*transAlv_16_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,16,transAlv_16_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_17 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_17 =  (*transAlv_16_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,17,transAlv_16_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_18 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_18 =  (*transAlv_16_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,18,transAlv_16_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_19 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_19 =  (*transAlv_16_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,19,transAlv_16_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_20 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_20 =  (*transAlv_16_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,20,transAlv_16_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_21 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_21 =  (*transAlv_16_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,21,transAlv_16_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_22 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_22 =  (*transAlv_16_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,22,transAlv_16_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_23 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_23 =  (*transAlv_16_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,23,transAlv_16_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_24 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_24 =  (*transAlv_16_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,24,transAlv_16_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_25 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_25 =  (*transAlv_16_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,25,transAlv_16_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_26 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_26 =  (*transAlv_16_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,26,transAlv_16_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_27 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_27 =  (*transAlv_16_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,27,transAlv_16_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_28 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_28 =  (*transAlv_16_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,28,transAlv_16_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_29 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_29 =  (*transAlv_16_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,29,transAlv_16_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_30 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_30 =  (*transAlv_16_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,30,transAlv_16_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_31 = new TGeoCombiTrans(-29.2907497009593*cos(rotAngle)+0.0371702331284625*sin(rotAngle),29.2907497009593*sin(rotAngle)+0.0371702331284625*cos(rotAngle),-13.2685421037292,rotAlv_16); 
	 *transAlv_16_31 =  (*transAlv_16_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,31,transAlv_16_31); 

	 phiEuler = 180.*TMath::ATan2(+0.443863216774299,-0.023261885496567)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(-0.443863216774299/TMath::Sin(TMath::Pi()*phiEuler/180.),0.8957925705641430)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000007/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_17 = new TGeoRotation("rotAlv17",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_17_0 = new TGeoCombiTrans(-31.0175566890166,-0.053327886366161,-18.7877967350678,rotAlv_17); 
	 *transAlv_17_0 =  (*transAlv_17_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_17,0,transAlv_17_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_1 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_1 =  (*transAlv_17_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,1,transAlv_17_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_2 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_2 =  (*transAlv_17_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,2,transAlv_17_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_3 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_3 =  (*transAlv_17_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,3,transAlv_17_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_4 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_4 =  (*transAlv_17_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,4,transAlv_17_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_5 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_5 =  (*transAlv_17_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,5,transAlv_17_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_6 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_6 =  (*transAlv_17_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,6,transAlv_17_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_7 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_7 =  (*transAlv_17_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,7,transAlv_17_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_8 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_8 =  (*transAlv_17_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,8,transAlv_17_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_9 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_9 =  (*transAlv_17_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,9,transAlv_17_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_10 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_10 =  (*transAlv_17_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,10,transAlv_17_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_11 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_11 =  (*transAlv_17_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,11,transAlv_17_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_12 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_12 =  (*transAlv_17_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,12,transAlv_17_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_13 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_13 =  (*transAlv_17_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,13,transAlv_17_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_14 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_14 =  (*transAlv_17_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,14,transAlv_17_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_15 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_15 =  (*transAlv_17_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,15,transAlv_17_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_16 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_16 =  (*transAlv_17_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,16,transAlv_17_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_17 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_17 =  (*transAlv_17_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,17,transAlv_17_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_18 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_18 =  (*transAlv_17_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,18,transAlv_17_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_19 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_19 =  (*transAlv_17_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,19,transAlv_17_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_20 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_20 =  (*transAlv_17_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,20,transAlv_17_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_21 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_21 =  (*transAlv_17_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,21,transAlv_17_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_22 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_22 =  (*transAlv_17_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,22,transAlv_17_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_23 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_23 =  (*transAlv_17_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,23,transAlv_17_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_24 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_24 =  (*transAlv_17_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,24,transAlv_17_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_25 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_25 =  (*transAlv_17_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,25,transAlv_17_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_26 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_26 =  (*transAlv_17_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,26,transAlv_17_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_27 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_27 =  (*transAlv_17_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,27,transAlv_17_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_28 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_28 =  (*transAlv_17_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,28,transAlv_17_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_29 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_29 =  (*transAlv_17_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,29,transAlv_17_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_30 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_30 =  (*transAlv_17_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,30,transAlv_17_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_17->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_17_31 = new TGeoCombiTrans(-31.0175566890166*cos(rotAngle)+-0.053327886366161*sin(rotAngle),31.0175566890166*sin(rotAngle)+-0.053327886366161*cos(rotAngle),-18.7877967350678,rotAlv_17); 
	 *transAlv_17_31 =  (*transAlv_17_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_17,31,transAlv_17_31); 

	 phiEuler = 180.*TMath::ATan2(+0.541109963819981,-0.028358371551731)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(-0.541109963819981/TMath::Sin(TMath::Pi()*phiEuler/180.),0.8404735628309040)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000007/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_18 = new TGeoRotation("rotAlv18",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_18_0 = new TGeoCombiTrans(-31.2314995325171,-0.0645401556873227,-23.9619527921068,rotAlv_18); 
	 *transAlv_18_0 =  (*transAlv_18_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_18,0,transAlv_18_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_1 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_1 =  (*transAlv_18_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,1,transAlv_18_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_2 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_2 =  (*transAlv_18_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,2,transAlv_18_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_3 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_3 =  (*transAlv_18_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,3,transAlv_18_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_4 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_4 =  (*transAlv_18_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,4,transAlv_18_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_5 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_5 =  (*transAlv_18_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,5,transAlv_18_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_6 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_6 =  (*transAlv_18_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,6,transAlv_18_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_7 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_7 =  (*transAlv_18_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,7,transAlv_18_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_8 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_8 =  (*transAlv_18_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,8,transAlv_18_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_9 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_9 =  (*transAlv_18_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,9,transAlv_18_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_10 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_10 =  (*transAlv_18_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,10,transAlv_18_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_11 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_11 =  (*transAlv_18_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,11,transAlv_18_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_12 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_12 =  (*transAlv_18_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,12,transAlv_18_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_13 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_13 =  (*transAlv_18_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,13,transAlv_18_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_14 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_14 =  (*transAlv_18_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,14,transAlv_18_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_15 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_15 =  (*transAlv_18_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,15,transAlv_18_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_16 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_16 =  (*transAlv_18_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,16,transAlv_18_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_17 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_17 =  (*transAlv_18_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,17,transAlv_18_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_18 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_18 =  (*transAlv_18_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,18,transAlv_18_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_19 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_19 =  (*transAlv_18_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,19,transAlv_18_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_20 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_20 =  (*transAlv_18_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,20,transAlv_18_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_21 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_21 =  (*transAlv_18_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,21,transAlv_18_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_22 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_22 =  (*transAlv_18_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,22,transAlv_18_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_23 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_23 =  (*transAlv_18_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,23,transAlv_18_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_24 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_24 =  (*transAlv_18_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,24,transAlv_18_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_25 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_25 =  (*transAlv_18_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,25,transAlv_18_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_26 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_26 =  (*transAlv_18_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,26,transAlv_18_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_27 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_27 =  (*transAlv_18_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,27,transAlv_18_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_28 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_28 =  (*transAlv_18_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,28,transAlv_18_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_29 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_29 =  (*transAlv_18_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,29,transAlv_18_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_30 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_30 =  (*transAlv_18_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,30,transAlv_18_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_18->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_18_31 = new TGeoCombiTrans(-31.2314995325171*cos(rotAngle)+-0.0645401556873227*sin(rotAngle),31.2314995325171*sin(rotAngle)+-0.0645401556873227*cos(rotAngle),-23.9619527921068,rotAlv_18); 
	 *transAlv_18_31 =  (*transAlv_18_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_18,31,transAlv_18_31); 

	 phiEuler = 180.*TMath::ATan2(+0.631569516319508,-0.033099155813169)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(-0.631569516319508/TMath::Sin(TMath::Pi()*phiEuler/180.),0.7746124140112900)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-0.0000000000000008/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_19 = new TGeoRotation("rotAlv19",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_19_0 = new TGeoCombiTrans(-31.430510548016,-0.074969881062487,-29.5383880011007,rotAlv_19); 
	 *transAlv_19_0 =  (*transAlv_19_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_19,0,transAlv_19_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_1 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_1 =  (*transAlv_19_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,1,transAlv_19_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_2 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_2 =  (*transAlv_19_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,2,transAlv_19_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_3 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_3 =  (*transAlv_19_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,3,transAlv_19_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_4 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_4 =  (*transAlv_19_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,4,transAlv_19_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_5 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_5 =  (*transAlv_19_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,5,transAlv_19_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_6 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_6 =  (*transAlv_19_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,6,transAlv_19_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_7 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_7 =  (*transAlv_19_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,7,transAlv_19_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_8 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_8 =  (*transAlv_19_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,8,transAlv_19_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_9 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_9 =  (*transAlv_19_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,9,transAlv_19_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_10 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_10 =  (*transAlv_19_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,10,transAlv_19_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_11 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_11 =  (*transAlv_19_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,11,transAlv_19_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_12 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_12 =  (*transAlv_19_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,12,transAlv_19_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_13 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_13 =  (*transAlv_19_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,13,transAlv_19_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_14 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_14 =  (*transAlv_19_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,14,transAlv_19_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_15 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_15 =  (*transAlv_19_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,15,transAlv_19_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_16 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_16 =  (*transAlv_19_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,16,transAlv_19_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_17 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_17 =  (*transAlv_19_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,17,transAlv_19_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_18 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_18 =  (*transAlv_19_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,18,transAlv_19_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_19 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_19 =  (*transAlv_19_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,19,transAlv_19_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_20 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_20 =  (*transAlv_19_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,20,transAlv_19_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_21 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_21 =  (*transAlv_19_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,21,transAlv_19_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_22 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_22 =  (*transAlv_19_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,22,transAlv_19_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_23 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_23 =  (*transAlv_19_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,23,transAlv_19_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_24 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_24 =  (*transAlv_19_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,24,transAlv_19_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_25 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_25 =  (*transAlv_19_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,25,transAlv_19_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_26 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_26 =  (*transAlv_19_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,26,transAlv_19_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_27 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_27 =  (*transAlv_19_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,27,transAlv_19_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_28 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_28 =  (*transAlv_19_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,28,transAlv_19_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_29 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_29 =  (*transAlv_19_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,29,transAlv_19_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_30 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_30 =  (*transAlv_19_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,30,transAlv_19_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_19->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_19_31 = new TGeoCombiTrans(-31.430510548016*cos(rotAngle)+-0.074969881062487*sin(rotAngle),31.430510548016*sin(rotAngle)+-0.074969881062487*cos(rotAngle),-29.5383880011007,rotAlv_19); 
	 *transAlv_19_31 =  (*transAlv_19_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_19,31,transAlv_19_31); 

