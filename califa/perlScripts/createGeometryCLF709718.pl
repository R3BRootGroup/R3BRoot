###################################################################################
#!/usr/local/bin/perl
#
# AUTHOR: Hector Alvarez Pol, 01/02/10  (hector.alvarez@usc.es)
# Modified by Ben Pietras from 707717*
# [*i.e. beware ;)]
# Last modification: 12/06/11
# 
# Program to create a TGeo file with the full CALIFA 7.07+7.17  
# See the README in this directory to understand the input files structure
###################################################################################
#
# How to: 
#	1) select the following parameters:
#	$alveolarThickness (in cm): the thickness of the alveolar structure
#	$wrappingThickness (in cm): the thickness of the wrapping material
#	@crystalLength (in cm): length of each crystal type (there are 16 crystal models)
#   2) run the perl script
#	perl createGeometryCLF709718.pl
#	3) the output file has to be compiled in R3BRoot
#
###################################################################################
# SOME CONSTRUCTION PARAMETERS!
# Thickness of the alveolar carbon fiber in cm. It only affects to the side facing the target. 
# Other sides are given by CAD model. 
# NOTE: THE WRAPPING THICKNESS REDUCES THE SIZE OF THE CRYSTAL, WHILE THE ALVEOLAR THICKNESS 
#       ONLY CHANGES THE LOCATION OF THE CRYSTAL, MOVING IT UP IN THE ALVEOLI. THIS CHANGE
#		IN POSITION INCREASES ALSO THE LATERAL ALVEOLAR MATTER THICKNESS BETWEEN CRYSTALS IN
#		DIFFERENT ALVEOLI, BUT MUCH LESS THAN THE 
#		THE NOMINAL VALUE OF ALVEOLARTHICKNESS (0.03CM) DETERMINE THE POSITION OF THE CRISTALS
#		IN THE ALVEOLI (EXCEPT OF THE MOVEMENT UP AND DOWN INCREASING OR DECREASING IT)
#
$alveolarThickness=0.03;
$wrappingThickness=0.03;
#$wrappingThickness=0.5;
# Length of the different crystals models, in cm (crystal1_A, crystal1_B, crystal2_A, ...)
@crystalLength=(19.5,19.5,19.0,19.0,18.0,18.0,17.0,17.0,16.0,16.0,14.0,14.0); #use for 709
###################################################################################
# Input files
$INPUTFILE1 = 'CLF709_AlveolarPositionsDirections.dat';
$INPUTFILE2 = 'CLF709_Dimensions_FiberBoxBarrel.dat';
$INPUTFILE3 = 'CLF709_Vertex_Crystal_all.dat';
$OUTPUTFILE = 'CLF709718_Geometry.geo';
open(INPUTFILE1);   #Open the alveoli positions and main axis directions file
open(INPUTFILE2);   #Open the dimensions of the alveoli file
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
		($x1G, $x2G, $y1G, $y2G, $z2G) =  split /\s+/, $line;
		#NOTE: Moving all dimensions to cm!!!
        push(@Alv_dx1,$x1G/10.);push(@Alv_dx2,$x2G/10.);push(@Alv_dy1,$y1G/10.);push(@Alv_dy2,$y2G/10.);push(@Alv_dz,$z2G/10.);
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
print OUTPUTFILE " //Generated ",$date , " from macro createGeometryCLF709718.pl by Hector Alvarez Pol (hector.alvarez@usc.es) \n";
print OUTPUTFILE "\n";
print OUTPUTFILE " //Alveolus\n";
# Entering in the construction of the alveoli
for ($i = 0; $i<=$#Alv_dx1; $i++) {
	print OUTPUTFILE "\t TGeoVolume *Alveolus_",$i+1," = gGeoManager->MakeTrd2(\"Alveolus_",$i+1,"\", pCarbonFibreMedium,",$Alv_dx1[$i],",",$Alv_dx2[$i],",",$Alv_dy1[$i],",",$Alv_dy2[$i],",",$Alv_dz[$i],"); \n";
	print OUTPUTFILE "\t Alveolus_",$i+1,"->SetLineColor(kBlue); \n\n";
}
print OUTPUTFILE "\n\n //Crystals with wrapping\n";
# Entering in the construction of the crystals
# Requires a little bit of computing. The CAD model has 20cm long crystals. 
# Moreover, the crystals are not fully contained in the alveols! So, some geometry is required
# to calculate the crystals vertex from the CAD data.
#Checking the output
#for ($i = 0; $i<=$#Cry_x_CAD/8; $i++) {
#	print $Cry_x_CAD[$i]*10,",  ", $Cry_y_CAD[$i]*10,",  ", $Cry_z_CAD[$i]*10,"\n";	
#}
#print "\n\n\n";
#comment above


for ($i = 0; $i<=$#Cry_x_CAD/8; $i++) {
	for ($j = 0; $j<8; $j++) {
		if($j<4) {
			push(@Cry_x,$Cry_x_CAD[$i*8+$j]);
#			push(@Cry_y,$Cry_y_CAD[$i*8+$j]);
			push(@Cry_z,$Cry_z_CAD[$i*8+$j]);
		}
		else {			
			push(@Cry_x, $Cry_x_CAD[$i*8+$j-4] + $crystalLength[$i]*($Cry_x_CAD[$i*8+$j]-$Cry_x_CAD[$i*8+$j-4])/20.0);
#			push(@Cry_y,$crystalLength[$i]);
			push(@Cry_z, $Cry_z_CAD[$i*8+$j-4] + $crystalLength[$i]*($Cry_z_CAD[$i*8+$j]-$Cry_z_CAD[$i*8+$j-4])/20.0);
		}
	}
}
#Adding vertex points for the crystals volume after removing the wrapping part
for ($i = 0; $i<=$#Cry_x/8; $i++) {
	for ($j = 0; $j<8; $j++) {
		if ($j==0 || $j==3 || $j==4 || $j==7) {  push(@CryNW_x,$Cry_x[$i*8+$j]+$wrappingThickness); }
		else {	push(@CryNW_x,$Cry_x[$i*8+$j]-$wrappingThickness); }
		if ($j==0 || $j==1 || $j==4 || $j==5) {	 push(@CryNW_z,$Cry_z[$i*8+$j]+$wrappingThickness); }
		else {	push(@CryNW_z,$Cry_z[$i*8+$j]-$wrappingThickness); }
	}
}


#Checking the output
#for ($i = 0; $i<=$#Cry_x; $i++) {
#	print $Cry_x[$i]*10,",  ", $Cry_y[$i]*10,",  ", $Cry_z[$i]*10,"\n";
#}
#for ($i = 0; $i<=$#CryNW_x; $i++) {
#	print $CryNW_x[$i]*10,",  ", $Cry_y[$i]*10,",  ", $CryNW_z[$i]*10,"\n";
#}
#print OUTPUTFILE "\t Double_t newXCoor; Double_t newYCoor; \n";
print OUTPUTFILE "\t Double_t theta = 0.; \n";
print OUTPUTFILE "\t Double_t phi = 0.; \n\n";

for ($i = 0; $i<=$#Cry_x/8; $i++) {


if($i>10){next;}

if($i==10)
{
		print OUTPUTFILE "\t theta = 180.*TMath::ATan2(TMath::Sqrt(pow(",($Cry_x[$i*8+5]-$Cry_x[$i*8+1])/2,",2)+pow(",($Cry_z[$i*8+6]-$Cry_z[$i*8+2])/2,",2)),",$crystalLength[$i],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t phi = 180.*TMath::ATan2(",($Cry_z[$i*8+6]-$Cry_z[$i*8+2])/2,",",($Cry_x[$i*8+5]-$Cry_x[$i*8+1])/2,")/TMath::Pi(); \n";
		print OUTPUTFILE "\t TGeoVolume *CrystalWithWrapping_",($i/2)+1,"A = gGeoManager->MakeTrap(\"CrystalWithWrapping_",($i/2)+1,"A\", pWrappingMedium,",$crystalLength[$i]/2,",
		0,0,",$Cry_z[$i*8+2],", ",$Cry_x[$i*8+1],", ",$Cry_x[$i*8+1],", ",0,
		", ",($Cry_z[$i*8+6]-$Cry_z[$i*8+5])/2,", ",($Cry_x[$i*8+5]-$Cry_x[$i*8+4])/2,", ",($Cry_x[$i*8+5]-$Cry_x[$i*8+4])/2,",0); \n"; 
		#0,0 was theta,phi
		print OUTPUTFILE "\t CrystalWithWrapping_",($i/2)+1,"A->SetLineColor(kYellow); \n";
		# Now the crystal bulk... 
		print OUTPUTFILE "\t theta = 180.*TMath::ATan2(TMath::Sqrt(pow(",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+1])/2,",2)+pow(",($CryNW_z[$i*8+6]-$CryNW_z[$i*8+2])/2,",2)),",$crystalLength[$i]-2*$wrappingThickness,")/TMath::Pi(); \n";
		print OUTPUTFILE "\t phi = 180.*TMath::ATan2(",($CryNW_z[$i*8+6]-$CryNW_z[$i*8+2])/2,",",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+1])/2,")/TMath::Pi(); \n";

		print OUTPUTFILE "\t TGeoVolume *Crystal_",($i/2)+1,"A = gGeoManager->MakeTrap(\"Crystal_",($i/2)+1,"A\", pCsIMedium,",$crystalLength[$i]/2-$wrappingThickness,",
		0,0,",$CryNW_z[$i*8+2],", ",$CryNW_x[$i*8+1],", ",$CryNW_x[$i*8+1],", ",0,
		", ",($CryNW_z[$i*8+6]-$CryNW_z[$i*8+5])/2,", ",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+4])/2,", ",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+4])/2,",0); \n"; 
		#
		#print OUTPUTFILE "\t Crystal_",($i/2)+1,"A->SetLineColor(kRed); \n\n";
}

else{

	if($i%2==0) {
		print OUTPUTFILE "\t theta = 180.*TMath::ATan2(TMath::Sqrt(pow(",($Cry_x[$i*8+5]-$Cry_x[$i*8+1])/2,",2)+pow(",($Cry_z[$i*8+6]-$Cry_z[$i*8+2])/2,",2)),",$crystalLength[$i],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t phi = 180.*TMath::ATan2(",($Cry_z[$i*8+6]-$Cry_z[$i*8+2])/2,",",($Cry_x[$i*8+5]-$Cry_x[$i*8+1])/2,")/TMath::Pi(); \n";
		print OUTPUTFILE "\t TGeoVolume *CrystalWithWrapping_",($i/2)+1,"A = gGeoManager->MakeTrap(\"CrystalWithWrapping_",($i/2)+1,"A\", pWrappingMedium,",$crystalLength[$i]/2,",
		theta, phi,",$Cry_z[$i*8+2],", ",$Cry_x[$i*8+1],", ",$Cry_x[$i*8+1],", ",0,
		", ",($Cry_z[$i*8+6]-$Cry_z[$i*8+5])/2,", ",($Cry_x[$i*8+5]-$Cry_x[$i*8+4])/2,", ",($Cry_x[$i*8+5]-$Cry_x[$i*8+4])/2,",0); \n"; 
		#
		print OUTPUTFILE "\t CrystalWithWrapping_",($i/2)+1,"A->SetLineColor(kYellow); \n";
		# Now the crystal bulk... 
		print OUTPUTFILE "\t theta = 180.*TMath::ATan2(TMath::Sqrt(pow(",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+1])/2,",2)+pow(",($CryNW_z[$i*8+6]-$CryNW_z[$i*8+2])/2,",2)),",$crystalLength[$i]-2*$wrappingThickness,")/TMath::Pi(); \n";
		print OUTPUTFILE "\t phi = 180.*TMath::ATan2(",($CryNW_z[$i*8+6]-$CryNW_z[$i*8+2])/2,",",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+1])/2,")/TMath::Pi(); \n";
		print OUTPUTFILE "\t TGeoVolume *Crystal_",($i/2)+1,"A = gGeoManager->MakeTrap(\"Crystal_",($i/2)+1,"A\", pCsIMedium,",$crystalLength[$i]/2-$wrappingThickness,",
		theta,phi,",$CryNW_z[$i*8+2],", ",$CryNW_x[$i*8+1],", ",$CryNW_x[$i*8+1],", ",0,
		", ",($CryNW_z[$i*8+6]-$CryNW_z[$i*8+5])/2,", ",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+4])/2,", ",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+4])/2,",0); \n"; 
		#
		print OUTPUTFILE "\t Crystal_",($i/2)+1,"A->SetLineColor(kRed); \n\n";
	            }
	else {

		print OUTPUTFILE "\t theta = 180.*TMath::ATan2(TMath::Sqrt(pow(",($Cry_x[$i*8+5]-$Cry_x[$i*8+1])/2,",2)+pow(",(($Cry_z[$i*8+6]-$Cry_z[$i*8+5])/2)-$Cry_z[$i*8+2],",2)),",$crystalLength[$i],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t phi = -180.*TMath::ATan2(",(($Cry_z[$i*8+6]-$Cry_z[$i*8+5])/2)-$Cry_z[$i*8+2],",",($Cry_x[$i*8+5]-$Cry_x[$i*8+1])/2,")/TMath::Pi(); \n";
		print OUTPUTFILE "\t TGeoVolume *CrystalWithWrapping_",(($i-1)/2)+1,"B = gGeoManager->MakeTrap(\"CrystalWithWrapping_",(($i-1)/2)+1,"B\", pWrappingMedium,",$crystalLength[$i]/2,",
		theta, phi,",$Cry_z[$i*8+2],", ",$Cry_x[$i*8+1],", ",$Cry_x[$i*8+1],", ",0,
		", ",($Cry_z[$i*8+6]-$Cry_z[$i*8+5])/2,", ",($Cry_x[$i*8+5]-$Cry_x[$i*8+4])/2,", ",($Cry_x[$i*8+5]-$Cry_x[$i*8+4])/2,",0); \n"; 
		#
		print OUTPUTFILE "\t CrystalWithWrapping_",(($i-1)/2)+1,"B->SetLineColor(kGreen); \n";
		# Now the crystal bulk... 
		print OUTPUTFILE "\t theta = 180.*TMath::ATan2(TMath::Sqrt(pow(",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+1])/2,",2)+pow(",(($CryNW_z[$i*8+6]-$CryNW_z[$i*8+5])/2)-$CryNW_z[$i*8+2],",2)),",$crystalLength[$i]-2*$wrappingThickness,")/TMath::Pi(); \n";
		print OUTPUTFILE "\t phi = -180.*TMath::ATan2(",(($CryNW_z[$i*8+6]-$CryNW_z[$i*8+5])/2)-$CryNW_z[$i*8+2],",",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+1])/2,")/TMath::Pi(); \n";
		print OUTPUTFILE "\t TGeoVolume *Crystal_",(($i-1)/2)+1,"B = gGeoManager->MakeTrap(\"Crystal_",(($i-1)/2)+1,"B\", pCsIMedium,",$crystalLength[$i]/2-$wrappingThickness,",
		theta, phi,",$CryNW_z[$i*8+2],", ",$CryNW_x[$i*8+1],", ",$CryNW_x[$i*8+1],", ",0,
		", ",($CryNW_z[$i*8+6]-$CryNW_z[$i*8+5])/2,", ",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+4])/2,", ",($CryNW_x[$i*8+5]-$CryNW_x[$i*8+4])/2,",0); \n"; 
		#
		print OUTPUTFILE "\t Crystal_",(($i-1)/2)+1,"B->SetLineColor(kViolet); \n\n";
	     }
    }
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
for($rot=1;$rot<32;$rot++) {
	print OUTPUTFILE "\t TGeoRotation *rotDef",$rot," = new TGeoRotation(); \n";
	print OUTPUTFILE "\t rotDef",$rot,"->RotateZ(-11.25*",$rot,"); \n";
	print OUTPUTFILE "\t TGeoCombiTrans* transDefRot",$rot," = new TGeoCombiTrans(*noTrans,*rotDef",$rot,"); \n\n";
}
#Adding the crystal volumes inside each crystalWithWrapping volume
for ($i = 0; $i<=$#Cry_x/8; $i++) {

if($i>9)
{
		print OUTPUTFILE "\t CrystalWithWrapping_",($i/2)+1,"A->AddNode(Crystal_",($i/2)+1,"A,",1,",new TGeoCombiTrans(0,0,0,rotUni)); \n";
		print OUTPUTFILE "\t AddSensitiveVolume(Crystal_",($i/2)+1,"A);  \n";
}
else{
	if($i%2==0) {	
		print OUTPUTFILE "\t CrystalWithWrapping_",($i/2)+1,"A->AddNode(Crystal_",($i/2)+1,"A,",1,",new TGeoCombiTrans(0,0,0,rotUni)); \n";
		print OUTPUTFILE "\t AddSensitiveVolume(Crystal_",($i/2)+1,"A);  \n";
	}
	else {
		print OUTPUTFILE "\t CrystalWithWrapping_",(($i-1)/2)+1,"B->AddNode(Crystal_",(($i-1)/2)+1,"B,",1,",new TGeoCombiTrans(0,0,0,rotUni)); \n";
		print OUTPUTFILE "\t AddSensitiveVolume(Crystal_",(($i-1)/2)+1,"B);  \n";
	}
    }
}
#Next array (@alveoliType) stores the crystal types in each alveoli
print OUTPUTFILE " //Crystals inside each alveoli \n";
@alveoliType=(1,1,2,2,3,3,3,3,3,3,4,4,4,5,5,5,6,6,6);
for ($h = 0; $h<=$#Alv_dx1; $h++) {


if($h>15)
{
	print OUTPUTFILE "\t Alveolus_",$h+1,"->AddNode(CrystalWithWrapping_",$alveoliType[$h],"A,",1,",new TGeoCombiTrans(0,0,-2.97,rotUni)); \n";
}

else
 {
	# NOTE: Simulation requires the longest axis of the crystals and alveoli to be along Z! CAD defines it along Y. So...
	# ... the transformation is X->X´, Y->Z´, Z->-Y´, so it is mandatory a change in the sign of $Cry_z with respect to their expected values...
	print OUTPUTFILE "\t Alveolus_",$h+1,"->AddNode(CrystalWithWrapping_",$alveoliType[$h],"A,",1,",new TGeoCombiTrans(",$Cry_x[($alveoliType[$h]-1)*2*8+1]+($Cry_x[($alveoliType[$h]-1)*2*8+5]-$Cry_x[($alveoliType[$h]-1)*2*8+1])/4,",",$Cry_z[($alveoliType[$h]-1)*2*8+2]+($Cry_z[($alveoliType[$h]-1)*2*8+6]-$Cry_z[($alveoliType[$h]-1)*2*8+2])/4,",",-(20-$crystalLength[($alveoliType[$h]-1)*2])/2+$alveolarThickness,",rotUni)); \n";
	print OUTPUTFILE "\t Alveolus_",$h+1,"->AddNode(CrystalWithWrapping_",$alveoliType[$h],"A,",2,",new TGeoCombiTrans(",-($Cry_x[($alveoliType[$h]-1)*2*8+1]+($Cry_x[($alveoliType[$h]-1)*2*8+5]-$Cry_x[($alveoliType[$h]-1)*2*8+1])/4),",",-($Cry_z[($alveoliType[$h]-1)*2*8+2]+($Cry_z[($alveoliType[$h]-1)*2*8+6]-$Cry_z[($alveoliType[$h]-1)*2*8+2])/4),",",-(20-$crystalLength[($alveoliType[$h]-1)*2])/2+$alveolarThickness,",rotSymmetric)); \n";
	print OUTPUTFILE "\t Alveolus_",$h+1,"->AddNode(CrystalWithWrapping_",$alveoliType[$h],"B,",3,",new TGeoCombiTrans(",$Cry_x[(($alveoliType[$h]-1)*2+1)*8+1]+($Cry_x[(($alveoliType[$h]-1)*2+1)*8+5]-$Cry_x[(($alveoliType[$h]-1)*2+1)*8+1])/4,",",-($Cry_z[(($alveoliType[$h]-1)*2+1)*8+2]+((($Cry_z[(($alveoliType[$h]-1)*2+1)*8+6]-$Cry_z[(($alveoliType[$h]-1)*2+1)*8+5])/2)-$Cry_z[(($alveoliType[$h]-1)*2+1)*8+2])/2),",",-(20-$crystalLength[($alveoliType[$h]-1)*2])/2+$alveolarThickness,",rotUni)); \n";
	print OUTPUTFILE "\t Alveolus_",$h+1,"->AddNode(CrystalWithWrapping_",$alveoliType[$h],"B,",4,",new TGeoCombiTrans(",-($Cry_x[(($alveoliType[$h]-1)*2+1)*8+1]+($Cry_x[(($alveoliType[$h]-1)*2+1)*8+5]-$Cry_x[(($alveoliType[$h]-1)*2+1)*8+1])/4),",",$Cry_z[(($alveoliType[$h]-1)*2+1)*8+2]+((($Cry_z[(($alveoliType[$h]-1)*2+1)*8+6]-$Cry_z[(($alveoliType[$h]-1)*2+1)*8+5])/2)-$Cry_z[(($alveoliType[$h]-1)*2+1)*8+2])/2,",",-(20-$crystalLength[($alveoliType[$h]-1)*2])/2+$alveolarThickness,",rotSymmetric)); \n";
 }
}
#Position in space for all the alveoli
print OUTPUTFILE "\n //Positioning in space of alveoli \n";
print OUTPUTFILE "\t Double_t phiEuler=0; Double_t thetaEuler = 0; Double_t psiEuler=0; \n";
print OUTPUTFILE "\t Double_t rotAngle = 0; \t"; 
for ($h = 0; $h<=$#Alv_dx1; $h++) {

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
	#	X		[0]	[1]	[2]		X´				   X´		[0]	[3]	[6]		X		XX	XY	XZ		X
	#	Y	=	[3]	[4]	[5]		Y´    ->		   Y´	=	[1]	[4]	[7]		Y  =	YX	YY	YZ		Y
	#	Z		[6]	[7]	[8]		Z´				   Z´		[2]	[5]	[8]		Z		ZX	ZY	ZZ		Z
	#
	#	We know ZX, ZY, ZZ, XX, XY, XZ
	#
	#	so,	from fRotationMatrix[2],[5]:					phiEuler=ATan2(ZX,-ZY)						
	#		from fRotationMatrix[8] and phiEuler:			thetaEuler=ATan2(ZX/sin(phiEuler), ZZ)		
	#		from fRotationMatrix[6]:						psiEuler=ASin(XZ/sin(thetaEuler))
	#
	#	It may require a change in the sign of the elements of phiEuler 
	#############################################################################################################
	if($zy[$h]>0.) {
		print OUTPUTFILE "\t phiEuler = 180.*TMath::ATan2(",$zx[$h],",",-$zy[$h],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t thetaEuler = 180.*TMath::ATan2(",$zx[$h],"/TMath::Sin(TMath::Pi()*phiEuler/180.),",$zz[$h],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t psiEuler = 180.*TMath::ASin(",$xz[$h],"/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); \n\n";
	}	
	else {
		print OUTPUTFILE "\t phiEuler = 180.*TMath::ATan2(",-$zx[$h],",",$zy[$h],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t thetaEuler = 180.*TMath::ATan2(",$zx[$h],"/TMath::Sin(TMath::Pi()*phiEuler/180.),",$zz[$h],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t psiEuler = 180.*TMath::ASin(",$xz[$h],"/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); \n\n";			
	}
	#print OUTPUTFILE "\t cout << \"rotAlv_",$h+1,"_",$m," \" << phiEuler << \" \" << thetaEuler << \" \" << psiEuler << endl; \n";

	print OUTPUTFILE "\t TGeoRotation *rotAlv_",$h+1," = new TGeoRotation(\"rotAlv",$h+1,"\",phiEuler,thetaEuler,psiEuler); \n";
	 
	for($rot=0; $rot<32; $rot++) {		
		if($rot==0) { 			
			print OUTPUTFILE "\t TGeoCombiTrans* transAlv_",$h+1,"_",$rot," = new TGeoCombiTrans(",$x[$h],",",$y[$h],",",$z[$h],",rotAlv_",$h+1,"); \n";
			# the shift in the alveoli center to one of the faces should be included: transDef makes the required transformation!

			print OUTPUTFILE "\t *transAlv_",$h+1,"_",$rot," =  (*transAlv_",$h+1,"_",$rot,") * (*transDef) ; \n";


		}
		else {
			# the shift in the alveoli center plus the rotation 
			# for the replication around Z.

			print OUTPUTFILE "\t rotAngle = TMath::Pi()*11.25*",$rot,"/180.;  \n"; 
			print OUTPUTFILE "\t rotAlv_",$h+1,"->RotateZ(-11.25);     \n";

			print OUTPUTFILE "\t TGeoCombiTrans* transAlv_",$h+1,"_",$rot," = new TGeoCombiTrans(",$x[$h],"*cos(rotAngle)+",$y[$h],"*sin(rotAngle),",-$x[$h],"*sin(rotAngle)+",$y[$h],"*cos(rotAngle),",$z[$h],",rotAlv_",$h+1,"); \n";
			print OUTPUTFILE "\t *transAlv_",$h+1,"_",$rot," =  (*transAlv_",$h+1,"_",$rot,") * (*transDef); \n";
		}
		print OUTPUTFILE "\t pWorld->AddNode(Alveolus_",$h+1,",",$rot,",transAlv_",$h+1,"_",$rot,"); \n\n";
	}
}		
close(INPUTFILE);
close(INPUTFILE1); 
close(INPUTFILE2); 
close(INPUTFILE3); 
#close(OUTPUTFILE);

reset 'a-z';
reset 'A';
reset 'C';

###################################################################################
# SOME CONSTRUCTION PARAMETERS!
# Thickness of the alveolar carbon fiber in cm. It only affects to the side facing the target. 
# Other sides are given by CAD model. 
# NOTE: THE WRAPPING THICKNESS REDUCES THE SIZE OF THE CRYSTAL, WHILE THE ALVEOLAR THICKNESS 
#       ONLY CHANGES THE LOCATION OF THE CRYSTAL, MOVING IT UP IN THE ALVEOLI. THIS CHANGE
#		IN POSITION INCREASES ALSO THE LATERAL ALVEOLAR MATTER THICKNESS BETWEEN CRYSTALS IN
#		DIFFERENT ALVEOLI, BUT MUCH LESS THAN THE 
#		THE NOMINAL VALUE OF ALVEOLARTHICKNESS (0.03CM) DETERMINE THE POSITION OF THE CRISTALS
#		IN THE ALVEOLI (EXCEPT OF THE MOVEMENT UP AND DOWN INCREASING OR DECREASING IT)
#
$alveolarThickness=0.03;
$wrappingThickness=0.03;
#$wrappingThickness=0.5;
# Length of the different crystals models, in cm (crystal1_A, crystal1_B, crystal2_A, ...)
# Note that for the ENDCAP 7.17 there are 23 different crystals:
@crystalLength=(19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5,19.5);
###################################################################################
# Input files
$INPUTFILE1 = 'CLF717_AlveolarPositionsDirections.dat';
$INPUTFILE2 = 'CLF717_Dimensions_FiberBoxBarrel.dat';
$INPUTFILE3 = 'CLF717_Vertex_Crystals_all.dat';
open(INPUTFILE1);   #Open the alveoli positions and main axis directions file
open(INPUTFILE2);   #Open the dimensions of the alveoli file
open(INPUTFILE3);   #Open the vertex of all crystals file
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
		($x1G, $y1L, $y1S, $x2G, $y2L, $y2S, $z2G) =  split /\s+/, $line;
		#NOTE: Moving all dimensions to cm!!!
        push(@Alv_dx1,$x1G/10.);push(@Alv_dy1L,$y1L/10.);push(@Alv_dy1S,$y1S/10.);push(@Alv_dx2,$x2G/10.);push(@Alv_dy2L,$y2L/10.);push(@Alv_dy2S,$y2S/10.);push(@Alv_dz,$z2G/10.);
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
######################################################
# Working with the data to create the TGeo!
######################################################
# First the general macro structure
print OUTPUTFILE "\n";
print OUTPUTFILE " //Alveolus_EC\n";
# Entering in the construction of the alveoli
for ($i = 0; $i<=$#Alv_dx1; $i++) {

	#Defining an exception for the third alveolus. The alveolus axis is tilted, in a different way than the others...
	if($i<2){
		print OUTPUTFILE "\t TGeoVolume *Alveolus_EC_",$i+1," = gGeoManager->MakeTrap(\"Alveolus_EC_",$i+1,"\", pCarbonFibreMedium,",$Alv_dz[$i],",0,0,",$Alv_dx1[$i],",",$Alv_dy1L[$i],",",$Alv_dy1S[$i],",0,",$Alv_dx2[$i],",",$Alv_dy2L[$i],",",$Alv_dy2S[$i],",0);  \n";
	}
	else{	
		print OUTPUTFILE "\t TGeoVolume *Alveolus_EC_",$i+1," = gGeoManager->MakeTrap(\"Alveolus_EC_",$i+1,"\", pCarbonFibreMedium,",$Alv_dz[$i],",1.3,90,",$Alv_dx1[$i],",",$Alv_dy1L[$i],",",$Alv_dy1S[$i],",0,",$Alv_dx2[$i],",",$Alv_dy2L[$i],",",$Alv_dy2S[$i],",0);  \n";
		
	}
	print OUTPUTFILE "\t Alveolus_EC_",$i+1,"->SetLineColor(kBlue); \n\n";
}
print OUTPUTFILE "\n\n //Crystals with wrapping\n";
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
			#			push(@Cry_y,$Cry_y_CAD[$i*8+$j]);
			push(@Cry_z,$Cry_z_CAD[$i*8+$j]);
		}
		else {			
			push(@Cry_x, $Cry_x_CAD[$i*8+$j-4] + $crystalLength[$i]*($Cry_x_CAD[$i*8+$j]-$Cry_x_CAD[$i*8+$j-4])/20.0);
			#			push(@Cry_y,$crystalLength[$i]);
			push(@Cry_z, $Cry_z_CAD[$i*8+$j-4] + $crystalLength[$i]*($Cry_z_CAD[$i*8+$j]-$Cry_z_CAD[$i*8+$j-4])/20.0);
		}
	}
}
#Adding vertex points for the crystals volume after removing the wrapping part
for ($i = 0; $i<=$#Cry_x/8; $i++) {
	for ($j = 0; $j<8; $j++) {
		if ($j==0 || $j==3 || $j==4 || $j==7) {  push(@CryNW_x,$Cry_x[$i*8+$j]+$wrappingThickness); }
		else {	push(@CryNW_x,$Cry_x[$i*8+$j]-$wrappingThickness); }
		if ($j==0 || $j==1 || $j==4 || $j==5) {	 push(@CryNW_z,$Cry_z[$i*8+$j]+$wrappingThickness); }
		else {	push(@CryNW_z,$Cry_z[$i*8+$j]-$wrappingThickness); }
	}
}
#Checking the output
#for ($i = 0; $i<=$#Cry_x; $i++) {
#	print $Cry_x[$i]*10,",  ", $Cry_y[$i]*10,",  ", $Cry_z[$i]*10,"\n";
#}
#for ($i = 0; $i<=$#CryNW_x; $i++) {
#	print $CryNW_x[$i]*10,",  ", $Cry_y[$i]*10,",  ", $CryNW_z[$i]*10,"\n";
#}
#print OUTPUTFILE "\t Double_t newXCoor; Double_t newYCoor; \n";
print OUTPUTFILE "\t Double_t* verticesEC=new Double_t[16]; \n";
print OUTPUTFILE "\t Double_t* verticesEC2=new Double_t[16]; \n";
for ($i = 0; $i<=$#Cry_x/8; $i++) {
	for($j=0;$j<8;$j++) {
		#NOTE: The file CLF705_Vertex_Crystal_all is written following the CAD convention (the simmetry axis run along Y)
		#The output should follow the ROOT/Geant convention (the simmetry axis run along Z)
		#So, the y coordinates in the ROOT verticesEC definition are filled by the z coordinates from the CAD file
		#NOTE2: the transformation is X->X´, Y->Z´, Z->-Y´, so it is mandatory a change in the sign of $Cry_z
		print OUTPUTFILE "\t verticesEC[",$j*2,"] = ", -$Cry_x[$i*8+$j],"; verticesEC[",$j*2+1,"] = ", $Cry_z[$i*8+$j], "; \t\t\t";
		if($j==0||$j==3||$j==4||$j==7) { $newXCoor= $Cry_x[$i*8+$j]+$wrappingThickness;} 
		else { $newXCoor=$Cry_x[$i*8+$j]-$wrappingThickness;}
		if($j==0||$j==1||$j==4||$j==5) { $newYCoor=-$Cry_z[$i*8+$j]-$wrappingThickness;}
		else { $newYCoor=-$Cry_z[$i*8+$j]+$wrappingThickness;}
		print OUTPUTFILE "\t verticesEC2[",$j*2,"] = ",-$newXCoor,"; verticesEC2[",$j*2+1,"] = ",-$newYCoor,"; \n";
	}
	print OUTPUTFILE "\t TGeoVolume *CrystalWithWrapping_",$i+1," = gGeoManager->MakeArb8(\"CrystalWithWrapping_",$i+1,"\", pWrappingMedium,",$crystalLength[$i]/2,", verticesEC); \n";
	print OUTPUTFILE "\t CrystalWithWrapping_",$i+1,"->SetLineColor(kYellow); \n";
	print OUTPUTFILE "\t TGeoVolume *Crystal_",$i+1," = gGeoManager->MakeArb8(\"Crystal_",$i+1,"\", pCsIMedium,",$crystalLength[$i]/2-$wrappingThickness,", verticesEC2); \n";
	print OUTPUTFILE "\t Crystal_",$i+1,"->SetLineColor(kRed); \n\n";
}	
#Locating the crystals inside their corresponding alveoli
#print OUTPUTFILE " //Some common geometrical operations \n";
#print OUTPUTFILE "\t TGeoRotation *rotUni = new TGeoRotation();          //unitary rotation  \n";
#print OUTPUTFILE "\t TGeoTranslation* noTrans=new TGeoTranslation(\"noTrans\",0.,0.,0.); \n";
#print OUTPUTFILE "\t TGeoRotation *rotSymmetric = new TGeoRotation(); //Symmetric crystal \n";
#print OUTPUTFILE "\t rotSymmetric->RotateZ(180); \n";
# See comments below in the alveoli construction
#print OUTPUTFILE " //The right transformation describing the difference in the coordinate system in CAD and simulation \n";
#print OUTPUTFILE "\t TGeoRotation *rotDef = new TGeoRotation(); \n";
#print OUTPUTFILE "\t rotDef->RotateX(-90); \n";
print OUTPUTFILE "\t rotDef->RotateY(180); \n";
print OUTPUTFILE "\t TGeoTranslation* shiftDef1=new TGeoTranslation(\"shiftDef1\",",($Alv_dy1L[0]),",10.,",($Alv_dx1[0]),"); \n";
print OUTPUTFILE "\t TGeoTranslation* shiftDef2=new TGeoTranslation(\"shiftDef2\",",($Alv_dy1L[1]),",10.,",($Alv_dx1[1]),"); \n";
print OUTPUTFILE "\t TGeoTranslation* shiftDef3=new TGeoTranslation(\"shiftDef3\",",($Alv_dy1L[2]),",10.,",($Alv_dx1[2]),"+0.227); \n";
print OUTPUTFILE "\t TGeoCombiTrans* transDef1 = new TGeoCombiTrans(*shiftDef1,*rotDef); \n";
print OUTPUTFILE "\t TGeoCombiTrans* transDef2 = new TGeoCombiTrans(*shiftDef2,*rotDef); \n";
print OUTPUTFILE "\t TGeoCombiTrans* transDef3 = new TGeoCombiTrans(*shiftDef3,*rotDef); \n\n";
#Adding the crystal volumes inside each crystalWithWrapping volume
for ($i = 0; $i<=$#Cry_x/8; $i++) {
	print OUTPUTFILE "\t CrystalWithWrapping_",$i+1,"->AddNode(Crystal_",$i+1,",",1,",new TGeoCombiTrans(0,0,0,rotUni)); \n";
	print OUTPUTFILE "\t AddSensitiveVolume(Crystal_",$i+1,");  \n";
}
#Next array (@alveoliType) stores the crystal types in each alveoli
print OUTPUTFILE " //Crystals inside each alveoli \n";
@alveoliType=(8,8,7);
for ($h = 0; $h<=$#Alv_dx1; $h++) {
	# NOTE: Simulation requires the longest axis of the crystals and alveoli to be along Z! CAD defines it along Y. So...
	# ... the transformation is X->X´, Y->Z´, Z->-Y´, so it is mandatory a change in the sign of $Cry_z with respect to their expected values...
	if($h<2) {
		for($NCryInAlv = 0; $NCryInAlv<$alveoliType[$h]; $NCryInAlv++) {
			print OUTPUTFILE "\t Alveolus_EC_",$h+1,"->AddNode(CrystalWithWrapping_",($h*8)+$NCryInAlv+1,",",1,",new TGeoCombiTrans(",
			($Alv_dy1L[$h]),",",
			-($Alv_dx1[$h]),",",
			-(20-$crystalLength[($h*8)+$NCryInAlv])/2+$alveolarThickness,",rotUni)); \n";
		}
	}
	#Again exception due to the change in the third alveolus
	else {
		for($NCryInAlv = 0; $NCryInAlv<$alveoliType[$h]; $NCryInAlv++) {
			print OUTPUTFILE "\t Alveolus_EC_",$h+1,"->AddNode(CrystalWithWrapping_",($h*8)+$NCryInAlv+1,",",1,",new TGeoCombiTrans(",
			($Alv_dy1L[$h]),"-0.035,",
			-($Alv_dx1[$h]),"-0.175,",
			-(20-$crystalLength[($h*8)+$NCryInAlv])/2+$alveolarThickness,",rotUni)); \n";
		}
	}		
}
#Position in space for all the alveoli
print OUTPUTFILE "\n //Positioning in space of alveoli \n";
#print OUTPUTFILE "\t Double_t phiEuler=0; Double_t thetaEuler = 0; Double_t psiEuler=0; \n";
#print OUTPUTFILE "\t Double_t rotAngle = 0; \t"; 
for ($h = 0; $h<=$#Alv_dx1; $h++) {
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
	#	X		[0]	[1]	[2]		X´				   X´		[0]	[3]	[6]		X		XX	XY	XZ		X
	#	Y	=	[3]	[4]	[5]		Y´    ->		   Y´	=	[1]	[4]	[7]		Y  =	YX	YY	YZ		Y
	#	Z		[6]	[7]	[8]		Z´				   Z´		[2]	[5]	[8]		Z		ZX	ZY	ZZ		Z
	#
	#	We know ZX, ZY, ZZ, XX, XY, XZ
	#
	#	so,	from fRotationMatrix[2],[5]:					phiEuler=ATan2(ZX,-ZY)						
	#		from fRotationMatrix[8] and phiEuler:			thetaEuler=ATan2(ZX/sin(phiEuler), ZZ)		
	#		from fRotationMatrix[6]:						psiEuler=ASin(XZ/sin(thetaEuler))
	#
	#	It may require a change in the sign of the elements of phiEuler 
	#############################################################################################################
	if($zy[$h]>0.) {
		print OUTPUTFILE "\t phiEuler = 180+180.*TMath::ATan2(",$zx[$h],",",-$zy[$h],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t thetaEuler = 180.*TMath::ATan2(",$zx[$h],"/TMath::Sin(TMath::Pi()*phiEuler/180.),",$zz[$h],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t psiEuler = 180.*TMath::ASin(",$xz[$h],"/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); \n\n";
	}	
	else {
		print OUTPUTFILE "\t phiEuler = 180+180.*TMath::ATan2(",-$zx[$h],",",$zy[$h],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t thetaEuler = 180.*TMath::ATan2(",$zx[$h],"/TMath::Sin(TMath::Pi()*phiEuler/180.),",$zz[$h],")/TMath::Pi(); \n";
		print OUTPUTFILE "\t psiEuler = 180.*TMath::ASin(",$xz[$h],"/TMath::Sin(TMath::Pi()*thetaEuler/180.))/TMath::Pi(); \n\n";			
	}
	#print OUTPUTFILE "\t cout << \"rotAlv_EC_",$h+1,"_",$m," \" << phiEuler << \" \" << thetaEuler << \" \" << psiEuler << endl; \n";
	print OUTPUTFILE "\t TGeoRotation *rotAlv_EC_",$h+1," = new TGeoRotation(\"rotAlv_EC",$h+1,"\",phiEuler,thetaEuler,psiEuler); \n";
	
	for($rot=0; $rot<32; $rot++) {		
		if($rot==0) { 			
			print OUTPUTFILE "\t TGeoCombiTrans* transAlv_EC_",$h+1,"_",$rot," = new TGeoCombiTrans(",$x[$h],",",$y[$h],",",$z[$h],",rotAlv_EC_",$h+1,"); \n";
			# the shift in the alveoli center to one of the faces should be included: transDef makes the required transformation!
			print OUTPUTFILE "\t *transAlv_EC_",$h+1,"_",$rot," =  (*transAlv_EC_",$h+1,"_",$rot,") * (*transDef",$h+1,") ; \n";
		}
		else {
			# the shift in the alveoli center plus the rotation 
			print OUTPUTFILE "\t rotAngle = TMath::Pi()*11.25*",$rot,"/180.;  \n"; 
			print OUTPUTFILE "\t rotAlv_EC_",$h+1,"->RotateZ(-11.25);     \n";
			print OUTPUTFILE "\t TGeoCombiTrans* transAlv_EC_",$h+1,"_",$rot," = new TGeoCombiTrans(",$x[$h],"*cos(rotAngle)+",$y[$h],"*sin(rotAngle),",-$x[$h],"*sin(rotAngle)+",$y[$h],"*cos(rotAngle),",$z[$h],",rotAlv_EC_",$h+1,"); \n";
			print OUTPUTFILE "\t *transAlv_EC_",$h+1,"_",$rot," =  (*transAlv_EC_",$h+1,"_",$rot,") * (*transDef",$h+1,"); \n";
		}
		print OUTPUTFILE "\t pWorld->AddNode(Alveolus_EC_",$h+1,",",$rot,",transAlv_EC_",$h+1,"_",$rot,"); \n\n";
	}
}		
close(INPUTFILE);
close(INPUTFILE1); 
close(INPUTFILE2); 
close(INPUTFILE3); 
close(OUTPUTFILE);

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
