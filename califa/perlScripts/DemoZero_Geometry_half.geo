 //Generated 7/9/2011 17:54:9 from macro createGeometryCLF705.pl by Hector Alvarez Pol (hector.alvarez@usc.es) 

 //Alveolus
	 TGeoVolume *Alveolus_3 = gGeoManager->MakeTrd2("Alveolus_3", pCarbonFibreMedium,2.35,3.3460011312493,1.25,1.77436745203728,10); 
	 Alveolus_3->SetLineColor(kBlue); 

	 TGeoVolume *Alveolus_4 = gGeoManager->MakeTrd2("Alveolus_4", pCarbonFibreMedium,2.35,3.3460011312493,1.25,1.77436745203728,10); 
	 Alveolus_4->SetLineColor(kBlue); 


 //Crystals with wrapping
	 Double_t theta = 0.; 
	 Double_t phi = 0.; 

	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.423300480780951,2)+pow(0.222856167115845,2)),17)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.222856167115845,0.423300480780951)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_2A = gGeoManager->MakeTrap("CrystalWithWrapping_2A", pWrappingMedium,8.5,
		theta, phi,0.61, 1.16, 1.16, 0, 0.832856167115845, 1.58330048078095, 1.58330048078095,0); 
	 CrystalWithWrapping_2A->SetLineColor(kYellow); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.423300480780951,2)+pow(0.222856167115845,2)),16.94)/TMath::Pi(); 
	 phi = 180.*TMath::ATan2(0.222856167115845,0.423300480780951)/TMath::Pi(); 
	 TGeoVolume *Crystal_2A = gGeoManager->MakeTrap("Crystal_2A", pCsIMedium,8.47,
		theta,phi,0.58, 1.13, 1.13, 0, 0.802856167115845, 1.55330048078095, 1.55330048078095,0); 
	 Crystal_2A->SetLineColor(kRed); 


	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.423155066721916,2)+pow(0.222580333338088,2)),17)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.222580333338088,0.423155066721916)/TMath::Pi(); 
	 TGeoVolume *CrystalWithWrapping_2B = gGeoManager->MakeTrap("CrystalWithWrapping_2B", pWrappingMedium,8.5,
		theta, phi,0.61, 1.16, 1.16, 0, 0.832580333338088, 1.58315506672192, 1.58315506672192,0); 
	 CrystalWithWrapping_2B->SetLineColor(kGreen); 
	 theta = 180.*TMath::ATan2(TMath::Sqrt(pow(0.423155066721916,2)+pow(0.222580333338088,2)),16.94)/TMath::Pi(); 
	 phi = -180.*TMath::ATan2(0.222580333338088,0.423155066721916)/TMath::Pi(); 
	 TGeoVolume *Crystal_2B = gGeoManager->MakeTrap("Crystal_2B", pCsIMedium,8.47,
		theta, phi,0.58, 1.13, 1.13, 0, 0.802580333338088, 1.55315506672192, 1.55315506672192,0); 
	 Crystal_2B->SetLineColor(kViolet); 

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
	 
	 CrystalWithWrapping_2A->AddNode(Crystal_2A,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_2A);  
	 CrystalWithWrapping_2B->AddNode(Crystal_2B,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AddSensitiveVolume(Crystal_2B);  

 //Crystals inside each alveoli 
	 Alveolus_3->AddNode(CrystalWithWrapping_2A,1,new TGeoCombiTrans(1.37165024039048,0.721428083557922,-1.47,rotUni)); 
	 Alveolus_3->AddNode(CrystalWithWrapping_2A,2,new TGeoCombiTrans(-1.37165024039048,-0.721428083557922,-1.47,rotSymmetric)); 
	 Alveolus_3->AddNode(CrystalWithWrapping_2B,3,new TGeoCombiTrans(1.37157753336096,-0.721290166669044,-1.47,rotUni)); 
	 Alveolus_3->AddNode(CrystalWithWrapping_2B,4,new TGeoCombiTrans(-1.37157753336096,0.721290166669044,-1.47,rotSymmetric)); 
	 Alveolus_4->AddNode(CrystalWithWrapping_2A,1,new TGeoCombiTrans(1.37165024039048,0.721428083557922,-1.47,rotUni)); 
	 Alveolus_4->AddNode(CrystalWithWrapping_2A,2,new TGeoCombiTrans(-1.37165024039048,-0.721428083557922,-1.47,rotSymmetric)); 
	 Alveolus_4->AddNode(CrystalWithWrapping_2B,3,new TGeoCombiTrans(1.37157753336096,-0.721290166669044,-1.47,rotUni)); 
	 Alveolus_4->AddNode(CrystalWithWrapping_2B,4,new TGeoCombiTrans(-1.37157753336096,0.721290166669044,-1.47,rotSymmetric)); 


 //Positioning in space of alveoli 
	 Double_t phiEuler=0; Double_t thetaEuler = 0; Double_t psiEuler=0; 

	 Double_t rotAngle = 0; 		 

	 phiEuler = 180.*TMath::ATan2(0.02647585574965603800,0.758169311129086)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.02647585574965603800/TMath::Sin(TMath::Pi()*phiEuler/180.),0.6515200109930395800)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-3.631899521186935100E-018/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_3_0 = new TGeoRotation("rotAlv3_0",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_3_0 = new TGeoCombiTrans(-0.0337326146002194,30.9659756955869,38.0362616116977,rotAlv_3_0); 
	 *transAlv_3_0 =  (*transAlv_3_0) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,0,transAlv_3_0); 
	 phiEuler = 180.*TMath::ATan2(-0.09245391656518520100,0.752976725129925)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(-0.09245391656518520100/TMath::Sin(TMath::Pi()*phiEuler/180.),0.6515200109930395800)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-3.631899521186934400E-018/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_3_1 = new TGeoRotation("rotAlv3_1",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_3_1 = new TGeoCombiTrans(4.81082853224951,30.590010093203,38.0362616116977,rotAlv_3_1); 
	 *transAlv_3_1 =  (*transAlv_3_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,1,transAlv_3_1); 

	 phiEuler = 180.*TMath::ATan2(0.02520322091828861000,0.721725817761388)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.02520322091828861000/TMath::Sin(TMath::Pi()*phiEuler/180.),0.6917200601630694200)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-1.303617529071733500E-018/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_4_0 = new TGeoRotation("rotAlv4_0",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_4_0 = new TGeoCombiTrans(-0.0321418210610101,30.9204213288772,34.2628729912306,rotAlv_4_0); 
	 *transAlv_4_0 =  (*transAlv_4_0) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,0,transAlv_4_0); 
	 phiEuler = 180.*TMath::ATan2(-0.08800986476078889700,0.716782827691057)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(-0.08800986476078889700/TMath::Sin(TMath::Pi()*phiEuler/180.),0.6917200601630696400)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-1.303617529071733100E-018/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); 

	 TGeoRotation *rotAlv_4_1 = new TGeoRotation("rotAlv4_1",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_4_1 = new TGeoCombiTrans(4.80527346749401,30.5447677214044,34.2628729912306,rotAlv_4_1); 
	 *transAlv_4_1 =  (*transAlv_4_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,1,transAlv_4_1); 

	 phiEuler = 180.*TMath::ATan2(0.02377314188053887200,0.680773712221798)/TMath::Pi(); 
	 thetaEuler = 180.*TMath::ATan2(0.02377314188053887200/TMath::Sin(TMath::Pi()*phiEuler/180.),0.7321079090358749700)/TMath::Pi(); 
	 psiEuler = 180.*TMath::ASin(-4.961077993545572900E-018/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi();
