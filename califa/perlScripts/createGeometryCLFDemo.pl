#!/usr/bin/perl
#
# AUTHOR: Hector Alvarez Pol, 01/10/11  (hector.alvarez@usc.es)
# Last modification: 01/12/11
# 
#
# Program to create a TGeo file with the BARREL CALIFA 8.11 
# Uses the information extracted from the STEP file for the CALIFA 8.11 alveoli
# See the README in this directory to understand the input files structure
###################################################################################
#
# How to: 
#	1) select the following parameters:
#	$alveolarThickness (in cm): the thickness of the alveolar structure
#	$wrappingThickness (in cm): the thickness of the wrapping material
#	@crystalLength (in cm): length of each crystal type (there are 12 crystal models)
# 2) run the perl script
#	perl createTGeo_CLF811.pl
#	3) the output file has to be compiled in R3BRoot
#
###################################################################################
# SOME CONSTRUCTION PARAMETERS!
# Thickness of the alveolar carbon fiber in cm 
$alveolarThickness=0.03;
$alveolarThicknessEpsilon=0.029; #to avoid extrusion of CrystalWithWrapping out of the AlveolusInner volume (added 19/12/2013)
$wrappingThickness=0.03;
#$wrappingThickness=0.5;
# Length of the different crystals models, in cm (crystal1_A, crystal1_B, crystal2_A, ...)
#
#@alveolusNames=("75_01","74_01","73_01","72_01","71_01","70_01","69_01","68_01","67_01","66_01","65_01","64_01","63_01","62_01","61_01","60_01");
#@crystalNames=("75_1","71_1_A","71_1_B","71_1_C","71_1_D","69_1_A","69_1_B","69_1_C","69_1_D","66_1_A","66_1_B","66_1_C","66_1_D","63_1_A","63_1_B","63_1_C","63_1_D","60_1_A","60_1_B","60_1_C","60_1_D");
#MODIFIED NAMES TO SIMPLIFY THE CODE IN R3BCalo.cxx!!!
@alveolusNames=("1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16");
@crystalNames=("1_1","2_1","2_2","2_3","2_4","3_1","3_2","3_3","3_4","4_1","4_2","4_3","4_4","5_1","5_2","5_3","5_4","6_1","6_2","6_3","6_4");
@crystalLength=(12.0,14.0,14.0,14.0,14.0,16.0,16.0,16.0,16.0,17.0,17.0,17.0,17.0,18.0,18.0,18.0,18.0,22.0,22.0,22.0,22.0);
@alveoliLength=(20.0,20.0,20.0,20.0,20.0,20.0,20.0,20.0,20.0,20.0,20.0,20.0,20.0,24.0,24.0,24.0);
###################################################################################
# Input files
$INPUTFILE1 = 'CLF811_OnlyAlveoli-points-ordered-filtered.dat';
$INPUTFILE2 = 'CLF811_BARREL_Vertex_Alveoli_all.dat';
$INPUTFILE3 = 'CLF811_BARREL_Vertex_Crystal_all.dat';
$OUTPUTFILE = 'CLF811_Geometry_Demo.geo';
open(INPUTFILE1);   #Open the alveoli positions and main axis directions file
open(INPUTFILE2);   #Open the vertex of the alveoli file
open(INPUTFILE3);   #Open the vertex of all crystals file
open(OUTPUTFILE, ">$OUTPUTFILE");
#Analyze all lines in input files, get contents and organize in arrays
foreach $line (<INPUTFILE1>) {
	if(!($line =~ "#")) {
		#$counter++;
		($xG, $yG, $zG, $zxG, $zyG ,$zzG, $xxG, $xyG ,$xzG) =  split /\s+/, $line;
		chop($xG);chop($yG);chop($zxG);chop($zyG);chop($xxG);chop($xyG);
		#NOTE: Moving all dimensions to cm!!!
		push(@x,$xG/10.);push(@y,$yG/10.);push(@z,$zG/10.);
		push(@zx,$zxG);push(@zy,$zyG);push(@zz,$zzG);
		push(@xx,$xxG);push(@xy,$xyG);push(@xz,$xzG);
	}
}
foreach $line (<INPUTFILE2>) {
	if(!($line =~ "#")) {
		#$counter++;
		($xVAG, $yVAG, $zVAG) =  split /\s+/, $line;
		chop($xVAG);chop($yVAG);
		#NOTE: Moving all dimensions to cm!!!
		push(@Alv_x_CAD,$xVAG/10.);push(@Alv_y_CAD,$yVAG/10.);push(@Alv_z_CAD,$zVAG/10.);
	}
}
foreach $line (<INPUTFILE3>) {
	if(!($line =~ "#")) {
		#$counter++;
		($xVG, $yVG, $zVG) =  split /\s+/, $line;
		chop($xVG);chop($yVG);
		#NOTE: Moving all dimensions to cm!!!
		push(@Cry_x_CAD,$xVG/10.);push(@Cry_y_CAD,$yVG/10.);push(@Cry_z_CAD,$zVG/10.);
	}
}
#present date
($Second, $Minute, $Hour, $Day, $Month, $Year, $WeekDay, $DayOfYear, $IsDST) = localtime(time);
$Year += 1900;
$Month++;
$date = "$Month/$Day/$Year $Hour:$Minute:$Second";
######################################################
# Working with the data to create the TGeo!
######################################################
# First the general macro structure
print OUTPUTFILE " //Generated ",$date , " from macro createGeometryCLF811.pl by Hector Alvarez Pol (hector.alvarez@usc.es) \n";print OUTPUTFILE "\n";
print OUTPUTFILE "\n";
print OUTPUTFILE " //Alveolus\n";
# Entering in the construction of the alveoli
#Checking the output
#for ($i = 0; $i<=$#Alv_x_CAD; $i++) {
#	print $Alv_x_CAD[$i]*10,",  ", $Alv_y_CAD[$i]*10,",  ", $Alv_z_CAD[$i]*10,"\n";	
#}
#print "\n\n\n";
#Add here some modification to the alveoli length, if requested (see the example below for the crystals).
for ($i = 0; $i<=$#Alv_x_CAD/8; $i++) {
	for ($j = 0; $j<8; $j++) {
		if($j<4) {
			push(@Alv_x,$Alv_x_CAD[$i*8+$j]);
			push(@Alv_y,$Alv_y_CAD[$i*8+$j]);
			push(@Alv_z,$Alv_z_CAD[$i*8+$j]);
		}
		else {			
			push(@Alv_x, $Alv_x_CAD[$i*8+$j-4] + $alveoliLength[$i]*($Alv_x_CAD[$i*8+$j]-$Alv_x_CAD[$i*8+$j-4])/20.0);
			push(@Alv_y,$alveoliLength[$i]);
			push(@Alv_z, $Alv_z_CAD[$i*8+$j-4] + $alveoliLength[$i]*($Alv_z_CAD[$i*8+$j]-$Alv_z_CAD[$i*8+$j-4])/20.0);
		}
	}
}
print OUTPUTFILE "\t Double_t* vertices_Alv=new Double_t[16]; \n";
print OUTPUTFILE "\t Double_t* vertices_inner_Alv=new Double_t[16]; \n";
#
for ($i = 0; $i<=$#Alv_x/8; $i++) {
	for($j=0;$j<8;$j++) {
		#NOTE: The file CLF811_BARREL_Vertex_Alveoli_all is written following the CAD convention (the simmetry axis run along Y)
		#The output should follow the ROOT/Geant convention (the simmetry axis run along Z)
		#So, the y coordinates in the ROOT vertices definition are filled by the z coordinates from the CAD file
		#NOTE2: the transformation is X->X´, Y->Z´, Z->-Y´, so it is mandatory a change in the sign of $Cry_z
		print OUTPUTFILE "\t vertices_Alv[",$j*2,"] = ", $Alv_x[$i*8+$j],"; vertices_Alv[",$j*2+1,"] = ", -$Alv_z[$i*8+$j], "; \n";
		if($j==0 || $j==3 || $j==4 || $j==7) {		
			print OUTPUTFILE "\t vertices_inner_Alv[",$j*2,"] = ", $Alv_x[$i*8+$j]+$alveolarThickness-$alveolarThicknessEpsilon,";";
		}
		else{
			print OUTPUTFILE "\t vertices_inner_Alv[",$j*2,"] = ", $Alv_x[$i*8+$j]-$alveolarThickness+$alveolarThicknessEpsilon,";";
		} 
		if($j==0 || $j==1 || $j==4 || $j==5) {
			print OUTPUTFILE "\t vertices_inner_Alv[",$j*2+1,"] = ", -$Alv_z[$i*8+$j]-$alveolarThickness+$alveolarThicknessEpsilon,"; \n";
		}
		else{
			print OUTPUTFILE "\t vertices_inner_Alv[",$j*2+1,"] = ", -$Alv_z[$i*8+$j]+$alveolarThickness-$alveolarThicknessEpsilon,"; \n";
		}
	}
	print OUTPUTFILE "\t TGeoVolume *Alveolus_",$alveolusNames[$i],"=gGeoManager->MakeArb8(\"Alveolus_",$alveolusNames[$i],"\", pCarbonFibreMedium,", $alveoliLength[$i]/2,", vertices_Alv); \n";
	print OUTPUTFILE "\t Alveolus_",$alveolusNames[$i],"->SetLineColor(kBlue); \n\n";
	print OUTPUTFILE "\t TGeoVolume *AlveolusInner_",$alveolusNames[$i],"=gGeoManager->MakeArb8(\"AlveolusInner_",$alveolusNames[$i],"\", pAirMedium,",( $alveoliLength[$i]-2*$alveolarThickness)/2,", vertices_inner_Alv); \n";
	print OUTPUTFILE "\t AlveolusInner_",$alveolusNames[$i],"->SetLineColor(kYellow); \n\n";
}
print OUTPUTFILE "\n\n //Crystals with and without wrapping\n";
# Entering in the construction of the crystals
# Requires a little bit of computing. The CAD model has 20cm long crystals. 
# Moreover, the crystals are not fully contained in the alveols! So, some geometry is required
# to calculate the crystals vertex from the CAD data.
#Checking the output
#for ($i = 0; $i<=$#Cry_x_CAD; $i++) {
#	print $Cry_x_CAD[$i]*10,",  ", $Cry_y_CAD[$i]*10,",  ", $Cry_z_CAD[$i]*10,"\n";	
#}
#print "\n\n\n";
for ($i = 0; $i<=$#Cry_x_CAD/8; $i++) {
	for ($j = 0; $j<8; $j++) {
		if($j<4) {
			push(@Cry_x,$Cry_x_CAD[$i*8+$j]);
			push(@Cry_y,$Cry_y_CAD[$i*8+$j]);
			push(@Cry_z,$Cry_z_CAD[$i*8+$j]);
		}
		else {			
			push(@Cry_x, $Cry_x_CAD[$i*8+$j-4] + $crystalLength[$i]*($Cry_x_CAD[$i*8+$j]-$Cry_x_CAD[$i*8+$j-4])/20.0);
			push(@Cry_y,$crystalLength[$i]);
			push(@Cry_z, $Cry_z_CAD[$i*8+$j-4] + $crystalLength[$i]*($Cry_z_CAD[$i*8+$j]-$Cry_z_CAD[$i*8+$j-4])/20.0);
		}
	}
}
for ($i = 0; $i<=$#Cry_x/8-2; $i++) {
	#A TRICK TO AVOID THE ROTATION IN VERSION 8.11 WHERE THE CRYSTALS ARE NOT DEFINED NEXT TO ORIGIN
	#THE LAST TWO CRYSTALS DO NOT REQUIRE THIS TRICK (WHY? WHO KNOWS...)
	if($i>0 && ($i-1)%4>1) {
		#print $i,"\n";    
		$origValue=$Cry_x[$i*8+1]-$Cry_x[$i*8];
		$origValue2=$Cry_x[$i*8+5]-$Cry_x[$i*8+4];
		for($j=0;$j<4;$j++) {
			#print $j,"\t",$Cry_x[$i*8+$j],"\n";    
			$Cry_x[$i*8+$j]	= $Cry_x[$i*8+$j] - $origValue;			
			#print $j,"\t",$Cry_x[$i*8+$j],"\n";    
		}
		for($j=4;$j<8;$j++) {
			$Cry_x[$i*8+$j]	= $Cry_x[$i*8+$j] - $origValue2;
		}
	}
}
#Checking the output
#for ($i = 0; $i<=$#Cry_x; $i++) {
#	print $Cry_x[$i]*10,",  ", $Cry_y[$i]*10,",  ", $Cry_z[$i]*10,"\n";
#}
print OUTPUTFILE "\t Double_t* vertices=new Double_t[16]; \n";
print OUTPUTFILE "\t Double_t* vertices2=new Double_t[16]; \n";
#print OUTPUTFILE "\t TGeoArb8 *tmpArb; \n";
#
#print OUTPUTFILE "\t Double_t newXCoor; Double_t newYCoor; \n";
for ($i = 0; $i<=$#Cry_x/8; $i++) {
	for($j=0;$j<8;$j++) {
		#NOTE: The file CLF705_Vertex_Crystal_all is written following the CAD convention (the simmetry axis run along Y)
		#The output should follow the ROOT/Geant convention (the simmetry axis run along Z)
		#So, the y coordinates in the ROOT vertices definition are filled by the z coordinates from the CAD file
		#NOTE2: the transformation is X->X´, Y->Z´, Z->-Y´, so it is mandatory a change in the sign of $Cry_z
		print OUTPUTFILE "\t vertices[",$j*2,"] = ", $Cry_x[$i*8+$j],"; vertices[",$j*2+1,"] = ", -$Cry_z[$i*8+$j], "; \t\t\t";
		if($j==0 || $j==3 || $j==4 || $j==7) { $newXCoor= $Cry_x[$i*8+$j]+$wrappingThickness;} 
		else { $newXCoor=$Cry_x[$i*8+$j]-$wrappingThickness;}
		if($j==0 || $j==1 || $j==4 || $j==5) { $newYCoor=-$Cry_z[$i*8+$j]-$wrappingThickness;}
		else { $newYCoor=-$Cry_z[$i*8+$j]+$wrappingThickness;}
		print OUTPUTFILE "\t vertices2[",$j*2,"] = ",$newXCoor,"; vertices2[",$j*2+1,"] = ",$newYCoor,"; \n";
	}
	print OUTPUTFILE "\t TGeoVolume *CrystalWithWrapping_",$crystalNames[$i]," = gGeoManager->MakeArb8(\"CrystalWithWrapping_",$crystalNames[$i],"\", pWrappingMedium,",$crystalLength[$i]/2,", vertices); \n";
	print OUTPUTFILE "\t CrystalWithWrapping_",$crystalNames[$i],"->SetLineColor(kGreen); \n";

        # Some hack to put the local origin to crystal center
#        print OUTPUTFILE "\t tmpArb = new TGeoArb8(", $crystalLength[$i]/2-$wrappingThickness, ", vertices2);\n";
        print OUTPUTFILE "\t Double_t originOffset_", $crystalNames[$i], "[3]; memset(originOffset_", $crystalNames[$i], ", 0, 3*sizeof(Double_t));\n";
        print OUTPUTFILE "\t for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_", $crystalNames[$i], "[0] += vertices2[2*tmpi]/8.0; originOffset_", $crystalNames[$i], "[1] += vertices2[2*tmpi+1]/8.0;}\n";
#        print OUTPUTFILE "\t memcpy(originOffset_", $crystalNames[$i], ", tmpArb->GetOrigin(), 3*sizeof(Double_t)); delete tmpArb;\n";
        print OUTPUTFILE "\t for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_", $crystalNames[$i], "[0]; vertices2[2*tmpi+1] -= originOffset_", $crystalNames[$i], "[1]; }\n";

	# Now the crystal bulk... 
	print OUTPUTFILE "\t TGeoVolume *Crystal_",$crystalNames[$i]," = gGeoManager->MakeArb8(\"Crystal_",$crystalNames[$i],"\", pCsIMedium,",$crystalLength[$i]/2-$wrappingThickness,", vertices2); \n";
	print OUTPUTFILE "\t Crystal_",$crystalNames[$i],"->SetLineColor(kRed); \n";
	print OUTPUTFILE "\n";
}
#Locating the crystals inside their corresponding alveoli
print OUTPUTFILE " //Some common geometrical operations \n";
print OUTPUTFILE "\t TGeoRotation *rotUni = new TGeoRotation();          //unitary rotation  \n";
print OUTPUTFILE "\t TGeoTranslation* noTrans=new TGeoTranslation(\"noTrans\",0.,0.,0.); \n";
print OUTPUTFILE "\t TGeoRotation *rotSymmetric = new TGeoRotation(); //Symmetric crystal \n";
print OUTPUTFILE "\t rotSymmetric->RotateZ(180); \n";
# See comments below in the alveoli construction
print OUTPUTFILE " //The right transformation describing the difference in the coordinate system in CAD and simulation \n";
print OUTPUTFILE "\t TGeoRotation *rotDef = new TGeoRotation(); \n";
print OUTPUTFILE "\t rotDef->RotateX(-90); \n";
print OUTPUTFILE "\t TGeoTranslation* shiftDef=new TGeoTranslation(\"shiftDef\",0.,10.,0.); \n";
print OUTPUTFILE "\t TGeoCombiTrans* transDef = new TGeoCombiTrans(*shiftDef,*rotDef); \n\n";
for($rot=1;$rot<10;$rot++) {
	print OUTPUTFILE "\t TGeoRotation *rotDef",$rot," = new TGeoRotation(); \n";
	print OUTPUTFILE "\t rotDef",$rot,"->RotateZ(-36.*",$rot,"); \n";
	print OUTPUTFILE "\t TGeoCombiTrans* transDefRot",$rot," = new TGeoCombiTrans(*noTrans,*rotDef",$rot,"); \n\n";
}
#Adding the crystal volumes inside each crystalWithWrapping volume
for ($i = 0; $i<=$#Cry_x/8; $i++) {
	print OUTPUTFILE "\t CrystalWithWrapping_",$crystalNames[$i],"->AddNode(Crystal_",$crystalNames[$i],",",1,",new TGeoCombiTrans(originOffset_", $crystalNames[$i], "[0], originOffset_", $crystalNames[$i], "[1],0.0,rotUni)); \n";
#	print OUTPUTFILE "\t AddSensitiveVolume(Crystal_",$crystalNames[$i],");  \n";
}
#Next array (@alveoliType) stores the crystal types in each alveoli
print OUTPUTFILE " //Crystals inside each alveoli \n";
#
@alveoliType=(-1,0,0,0,0,1,1,2,2,2,3,3,3,4,4,4);
#
print OUTPUTFILE "\t AlveolusInner_",$alveolusNames[0],"->AddNode(CrystalWithWrapping_",$crystalNames[0],",",0,",new TGeoCombiTrans(0,0,",-(20-$crystalLength[0])/2+$alveolarThickness,",rotUni)); \n";
print OUTPUTFILE "\t Alveolus_",$alveolusNames[0],"->AddNode(AlveolusInner_",$alveolusNames[0],",",1,",new TGeoCombiTrans(0,0,0,rotUni)); \n";
for ($h = 1; $h<=$#Alv_x/8; $h++) {
	# NOTE: Simulation requires the longest axis of the crystals and alveoli to be along Z! CAD defines it along Y. So...
	# ... the transformation is X->X´, Y->Z´, Z->-Y´, so it is mandatory a change in the sign of $Cry_z with respect to their expected values...
	for ($i = 0; $i<=3; $i++) {
		#print $Alv_x[$h*8+1],"\t",$Cry_x[8+($alveoliType[$h])*8*4],"\t",$Alv_z[$h*8+2],"\t",$Cry_z[8+($alveoliType[$h])*8*4+2],"\n";
		print OUTPUTFILE "\t AlveolusInner_",$alveolusNames[$h],"->AddNode(CrystalWithWrapping_",$crystalNames[$alveoliType[$h]*4+$i+1],",",$i,",new TGeoCombiTrans(",($Alv_x[$h*8+1]-2*($Cry_x[8+($alveoliType[$h])*8*4]))/2,",",-($Alv_z[$h*8+2]-2*($Cry_z[8+($alveoliType[$h])*8*4+2]))/2,",",-($alveoliLength[$h]-$crystalLength[$alveoliType[$h]*4+$i+1])/2+$alveolarThickness,",rotUni)); \n";
	}
	print OUTPUTFILE "\t Alveolus_",$alveolusNames[$h],"->AddNode(AlveolusInner_",$alveolusNames[$h],",",1,",new TGeoCombiTrans(0,0,0,rotUni)); \n";
}
#Position in space for all the alveoli
print OUTPUTFILE "\n //Positioning in space of alveoli \n";
print OUTPUTFILE "\t Double_t phiEuler=0; Double_t thetaEuler=0; Double_t psiEuler=0; \n";
print OUTPUTFILE "\t Double_t rotAngle=0;\n";
#
#print OUTPUTFILE "\t TGeoCombiTrans matDemo(0, -2.83, 11.91, new TGeoRotation(\"rotDemo\", 0, 4.12, 0));\n";
#print OUTPUTFILE "\t TGeoCombiTrans matDemo(0, 0, 0, new TGeoRotation(\"rotDemo\", 0, 0, 0));\n";
print OUTPUTFILE "\t TGeoCombiTrans matDemo(0, 1.92093, -1.75703, new TGeoRotation(\"rotDemo\", 0, 20.3304, 0));\n";
for ($h = 8; $h<=$#Alv_x/8; $h++) {
	############################################################################################################
	# CORRECT!!!!!
	# About the euler angles from the CAD transformation:
	# The CAD files contain ZX, ZY, ZZ, XX, XY and XZ components of the new Z and X vectors in the old 
	# coordinate system, for each axis. A typical rotation matrix for rotation is: MASTER = T * LOCAL    
	#	Using the SetAngles(phi, theta, psi) convention in ROOT 
	#	(NOTE: IT's NOT THE STANDARD CONVENTION, THE SECOND ANGLE IS AROUND X´ INSTEAD Y´!!)
	#		fRotationMatrix[0] =  cospsi*cosphi - costhe*sinphi*sinpsi;
	#		fRotationMatrix[1] = -sinpsi*cosphi - costhe*sinphi*cospsi;
	#		fRotationMatrix[2] =  sinthe*sinphi;
	#		fRotationMatrix[3] =  cospsi*sinphi + costhe*cosphi*sinpsi;
	#		fRotationMatrix[4] = -sinpsi*sinphi + costhe*cosphi*cospsi;
	#		fRotationMatrix[5] = -sinthe*cosphi;
	#		fRotationMatrix[6] =  sinpsi*sinthe;
	#		fRotationMatrix[7] =  cospsi*sinthe;
	#		fRotationMatrix[8] =  costhe;
	#
	# MASTER				  LOCAL			  	 LOCAL					 MASTER					  MASTER
	#	X		[0]	[1]	[2]		X´				     X´		[0]	[3]	[6]		X		  XX	XY	XZ		X
	#	Y	=	[3]	[4]	[5]		Y´    ->		   Y´	=	[1]	[4]	[7]		Y  =	YX	YY	YZ		Y
	#	Z		[6]	[7]	[8]		Z´				     Z´		[2]	[5]	[8]		Z		  ZX	ZY	ZZ		Z
	#
	#	We know ZX, ZY, ZZ, XX, XY, XZ
	#
	#	so,	from fRotationMatrix[2],[5]:					phiEuler=ATan2(ZX,-ZY)						
	#		from fRotationMatrix[8] and phiEuler:			thetaEuler=ATan2(ZX/sin(phiEuler), ZZ)		
	#		from fRotationMatrix[6]:						psiEuler=ASin(XZ/sin(thetaEuler))
	#
	#	It may require a change in the sign of the elements of phiEuler 
	#############################################################################################################
	#if($zy[$h]>0.) {
	#	print OUTPUTFILE "\t phiEuler = 180.*TMath::ATan2(",$zx[$h],",",-$zy[$h],")/TMath::Pi(); \n";
	#	print OUTPUTFILE "\t thetaEuler = 180.*TMath::ATan2(",$zx[$h],"/TMath::Sin(TMath::Pi()*phiEuler/180.),",$zz[$h],")/TMath::Pi(); \n";
	#	print OUTPUTFILE "\t psiEuler = 180.*TMath::ASin(",$xz[$h],"/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); \n\n";
	#}	
	#else {
	#	print OUTPUTFILE "\t phiEuler = 180.*TMath::ATan2(",-$zx[$h],",",$zy[$h],")/TMath::Pi(); \n";
	#	print OUTPUTFILE "\t thetaEuler = 180.*TMath::ATan2(",$zx[$h],"/TMath::Sin(TMath::Pi()*phiEuler/180.),",$zz[$h],")/TMath::Pi(); \n";
	#	print OUTPUTFILE "\t psiEuler = 180.*TMath::ASin(",$xz[$h],"/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); \n\n";			
	#}
	print OUTPUTFILE "\t phiEuler = 180; \n";
	print OUTPUTFILE "\t thetaEuler = 180.*TMath::ATan2(",$zy[$h],",",$zz[$h],")/TMath::Pi(); \n";
	if($xx[$h]<0) {	
		print OUTPUTFILE "\t psiEuler = 0; \n\n";
	}
	else { 		
		print OUTPUTFILE "\t psiEuler = -180; \n\n";
	}
	#print OUTPUTFILE "\t cout << \"rotAlv_",$h+1," \" << phiEuler << \" \" << thetaEuler << \" \" << psiEuler << endl; \n";
	print OUTPUTFILE "\t TGeoRotation *rotAlv_",$h+1," = new TGeoRotation(\"rotAlv",$h+1,"\",phiEuler,thetaEuler,psiEuler); \n";
        print OUTPUTFILE "\t TGeoCombiTrans *transAlvBase = new TGeoCombiTrans();\n";
        print OUTPUTFILE "\t TGeoRotation *rotPlace_",$h+1," = new TGeoRotation();\n";
	for($rot=0; $rot<4; $rot++) {
#	for($rot=0; $rot<32; $rot++) {
#          if($rot == 12 || $rot == 13 || $rot == 28 || $rot == 29){
#          if($rot == 0 || $rot == 1){
		if($rot % 2 == 0) { 			
	                print OUTPUTFILE "\t *rotAlv_",$h+1," = TGeoRotation(\"rotAlv",$h+1,"\",phiEuler,thetaEuler,psiEuler); \n";
			#print OUTPUTFILE "\t TGeoCombiTrans* transAlv_",$h+1,"_",$rot," = new TGeoCombiTrans(",$x[$h],",",$y[$h],",",$z[$h],",rotAlv_",$h+1,"); \n";
			print OUTPUTFILE "\t TGeoCombiTrans* transAlv_",$h+1,"_",$rot," = new TGeoCombiTrans(",$x[$h],",",$y[$h]+($alveoliLength[$h]/2-10.0)*$zz[$h],",",$z[$h]-($alveoliLength[$h]/2-10.0)*$zy[$h],",rotAlv_",$h+1,"); \n";
			# the shift in the alveoli center to one of the faces should be included: transDef makes the required transformation!
			print OUTPUTFILE "\t *transAlvBase =  (*transAlv_",$h+1,"_",$rot,") * (*transDef) ; \n";
#                        print OUTPUTFILE "\t rotPlace_",$h+1,"->SetAngles(",-11.25*$rot,", 0., 0.);\n";
                        print OUTPUTFILE "\t rotPlace_",$h+1,"->SetAngles(",-(120 + 90*$rot),", 0., 0.);\n";
			print OUTPUTFILE "\t *transAlv_",$h+1,"_",$rot," =  (*rotPlace_",$h+1,") * matDemo * (*transAlvBase); \n";
			print OUTPUTFILE "\t rotAlv_",$h+1,"->SetAngles(-11.25, 0., 0.);     \n";
		}
		else {
			# the shift in the alveoli center plus the rotation 
#			print OUTPUTFILE "\t rotAngle = TMath::Pi()*11.25*",$rot,"/180.;  \n"; 
			#print OUTPUTFILE "\t TGeoCombiTrans* transAlv_",$h+1,"_",$rot," = new TGeoCombiTrans(",$x[$h],"*cos(rotAngle)+",$y[$h],"*sin(rotAngle),",-$x[$h],"*sin(rotAngle)+",$y[$h],"*cos(rotAngle),",$z[$h],",rotAlv_",$h+1,"); \n";
# 			print OUTPUTFILE "\t TGeoCombiTrans* transAlv_",$h+1,"_",$rot," = new TGeoCombiTrans(",$x[$h],"*cos(rotAngle)+",($y[$h]+($alveoliLength[$h]/2-10.0)*$zz[$h]),"*sin(rotAngle),",-$x[$h],"*sin(rotAngle)+",$y[$h]+($alveoliLength[$h]/2-10.0)*$zz[$h],"*cos(rotAngle),",$z[$h]-($alveoliLength[$h]/2-10.0)*$zy[$h],",rotAlv_",$h+1,"); \n";
#			print OUTPUTFILE "\t *transAlv_",$h+1,"_",$rot," =  (*transAlv_",$h+1,"_",$rot,") * (*transDef); \n";
                        print OUTPUTFILE "\t TGeoCombiTrans* transAlv_",$h+1,"_",$rot," = new TGeoCombiTrans();\n";
                        print OUTPUTFILE "\t *transAlv_",$h+1,"_",$rot," = (*rotPlace_",$h+1,") * matDemo * (*rotAlv_",$h+1,") * (*transAlvBase);\n";
		}
		print OUTPUTFILE "\t pWorld->AddNode(Alveolus_",@alveolusNames[$h],",",$rot,",transAlv_",$h+1,"_",$rot,"); \n\n";
              }
#	}
}		
#print OUTPUTFILE "\t pWorld->AddNode(Alveolus_",1,",",999,",new TGeoCombiTrans(0,0,0,rotUni)); \n";
#print OUTPUTFILE "\t pWorld->AddNode(Alveolus_",1,",",999,",transDef); \n";
#
close(INPUTFILE);
close(INPUTFILE1); 
close(INPUTFILE2); 
close(INPUTFILE3); 
close(OUTPUTFILE);
#
####################################################################################
###################### ADDITIONAL FUNCTIONS ########################################

# restrict the number of digits after the decimal point 
#(from http://guymal.com/mycode/perl_restrict_digits/)
sub res_dec
{
  my $num=shift;#the number to work on
  my $digs_to_cut=shift;# the number of digits after
	# the decimal point to cut
	#(eg: $digs_to_cut=3 will leave
	# two digits after the decimal point)
	
  if ($num=~/\d+\.(\d){$digs_to_cut,}/)
  {
    # there are $digs_to_cut or
    # more digits after the decimal point
    $num=sprintf("%.".($digs_to_cut-1)."f", $num);
  }
  return $num;
}
#END OF PERL CODE
