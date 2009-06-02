#!/usr/local/bin/perl
#########################################################################
#*-- AUTHOR : Hector Alvarez-Pol     hapol@fpddux.usc.es
#*-- Date: 11/2006
#*-- Last Update: 14/11/06 by Hector Alvarez
# ----------------------------------------------------------------------
#
# Program to convert the map fields to simplest vector of magnetic field
# It is assumed a typical structure: X B_X Y B_Y Z B_Z
#
#########################################################################
$file1 = 'R3B/R3B.MAP';
#
open(INTRO1, $file1);           # Open the file
@lines = <INTRO1>;              # Read it into an array
close(INTRO1);                  # Close the file
#
for($i = 0;$i < scalar(@lines); ++$i) { 
    chomp (@lines[$i]);    #avoiding the end of line
}
#
# separating the components in the lines
#
for($i = 0;$i <scalar(@lines); ++$i) { # for all the lines defined in the file but the first one
    @linea = split(' ', @lines[$i]);
    @X[$i] = @linea[0];
    @Y[$i] = @linea[1];
    @Z[$i] = @linea[2];
    @Bx[$i] = @linea[3];
    @By[$i] = @linea[4];
    @Bz[$i] = @linea[5];
}
#
#Output file
#
$fileOut = 'code.list';
open(OUT1, ">$fileOut");        # Open for output
#
print OUT1 "  // R3B MAP - from R3B.MAP file \n";
print OUT1 "  G4double xfield[] = {";
for($i = 0;$i <scalar(@lines); ++$i) {
    print OUT1 @Bx[$i];
    if($i!=scalar(@lines)-1){
	print OUT1 "," ;
    }
    if($i%6==0){
	print OUT1 "\n";
    }
}
print OUT1 "}; \n\n\n";
print OUT1 "  G4double yfield[] = {";
for($i = 0;$i <scalar(@lines); ++$i) {
    print OUT1 @By[$i];
    if($i!=scalar(@lines)-1){
	print OUT1 "," ;
    }
    if($i%6==0){
	print OUT1 "\n";
    }
}
print OUT1 "}; \n\n\n";
print OUT1 "  G4double zfield[] = {";
for($i = 0;$i <scalar(@lines); ++$i) {
    print OUT1 @Bz[$i];
    if($i!=scalar(@lines)-1){
	print OUT1 "," ;
    }
    if($i%6==0){
	print OUT1 "\n";
    }
}
print OUT1 "}; \n\n\n";
print OUT1 "for(G4int i=0;i<numberOfPointsInGrid;i++){\n";
print OUT1 "    Bxfield[i] = xfield[i]*1E-04*tesla;\n";
print OUT1 "    Byfield[i] = yfield[i]*1E-04*tesla;\n";
print OUT1 "    Bzfield[i] = zfield[i]*1E-04*tesla;\n";
print OUT1 "}; \n";
#END OF PERL CODE
