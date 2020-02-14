      PROGRAM DWHI
C
C          Stripped version of DWUCK produced originally by 
C          Julian Cook to work with the code FOLD for doing
C          double folding calculations of heavy-ion scattering.
C          Brought up-to-date by J. A. Carr in April 1988.
C
C                                     VERSION: 08 Dec 1988
C
C          Converted to Ridge computer and fixed various buggy 
C          details so it now handles more general case at higher
C          energy.  Checked against calculation done at Oak Ridge
C          by Fred Petrovich and published in Phys. Lett. 181 B 
C          (1986) 38.  The latter was checked against other runs
C          made with codes known to be reliable at ORNL.
C
C                                     Original Version: 09 April 1988
C
C          Updated to write plotfile and fix up some details.
C
C          Converted to run under VMS at MSU NSCL.  Requires 5000 
C          blocks of scratch space for typical case.
C                                                    -- 21 August 1989
C
C          Placed in NSCL_LIBRARY:[SCRI] area  --  29 August 1989
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      PARAMETER ( MNX = 900 )
      PARAMETER ( MNL = 350 )
      PARAMETER ( MNA = 181 )
C
      CHARACTER*52   TITLE
      COMMON /STUFF/ TITLE,ICON(16),ANGLE(3),HBARC,AMU,CHSQ, 
     >               DR,Z(2),ZA(2),FM(2),FMA(2),RC(3),FS(2),ECM(2), 
     >               FK(2),FK2(2),ETA(2),FMU(2),ISA,ISB,JA,JB,K,
     >               LPLUS,MPLUS,NFF,LTRT(8),ISTRT(8),JTRT(8),IBF(8) 
C
      COMMON /ELSTUFF/ UR(MNX,2),  UI(MNX,2), VR(MNX,2), VI(MNX,2),
     >                 AR(0:MNX),  AI(0:MNX), FR(0:MNX), FI(0:MNX),
     >                 SIGPH(MNL), FC(MNL,2),  GC(MNL,2), 
     >                 TR(MNL),    TI(MNL) 
C
      COMMON /FORMS/ UB(2*MNX), VB(2*MNX)
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      CHARACTER*8  FILNAM                                                
      CHARACTER*16 PLOTFILE
C
C          Store default ANGLE data (0 to 180 in steps of 5)                
C                                                                       
      ANGLE(1)=37.0                                                     
      ANGLE(2)=0.0                                                      
      ANGLE(3)=5.0                                                      
C
      HBARC=197.313                                                     
      AMU=931.141                                                       
      FSCON=137.036                                                     
      CHSQ=HBARC/FSCON                                                  
C
      WRITE(6,9999)                                                     
      IBF(3)=0                                                          
C                                                                       
C          READ CARD SET 1                                                   
C                                                                       
      READ(5,9802,END=20) ICON, TITLE                                    
      WRITE(6,9804)                                                     
      WRITE(6,9803) ICON, TITLE                                           
C                                                                       
C          OPEN UNIT 8 FOR MICROSCOPIC FORM FACTORS                          
C                                                                       
      IF( ICON(2) .NE. 1 ) THEN  
        READ(5,9805) FILNAM                                               
        WRITE(6,9806)FILNAM                                               
        OPEN( UNIT=8, FILE=FILNAM, STATUS='OLD',             
     >       IOSTAT=ICHK )                   
        IF( ICHK .NE. 0 ) THEN 
          WRITE(6,5) ICHK  
          STOP ' -- ERROR OPENING UNIT 8 '
        ENDIF
      ENDIF 
    5 FORMAT(///' IOSTAT =',I5///)                                      
C
C          Get everything else we need
C
      CALL INPUT                                                        
C
C          Read name of plotfile, if there is one
C
      READ(5,9807) PLOTFILE
C
      IF( PLOTFILE .NE. ' ' ) THEN
        OPEN( UNIT=7, FILE=PLOTFILE, STATUS='NEW',
     >        FORM='FORMATTED' )
        WRITE(6,9808) PLOTFILE
      ENDIF
C                                                                       
C          Calculate everything if there are no errors
C                                                                       
      IF( IBF(3) .EQ. 0 ) THEN  
        CALL ELASTIC   
        CALL INELASTIC( PLOTFILE ) 
      ENDIF
C
C          Clean up and terminate calculation
C
      IF( ICON(2) .NE. 1 ) THEN                                             
        CLOSE( UNIT=8, STATUS='KEEP' ) 
      ENDIF
   20 CLOSE( UNIT=2,  STATUS='DELETE' ) 
      CLOSE( UNIT=4,  STATUS='DELETE' )  
      CLOSE( UNIT=9,  STATUS='DELETE' )  
      CLOSE( UNIT=10, STATUS='DELETE' )   
C
      WRITE(6,899)
C
      STOP 
C
 9502 FORMAT(18H0CONTROL INTEGERS )                                     
 9802 FORMAT(16I1,4X,A52)                                              
 9803 FORMAT(1H ,16I2,4X,A52)                                          
 9804 FORMAT(33H  1 2 3 4 5 6 7 8 9 A B C D E F G,
     >  23H    RUN IDENTIFICATION )  
 9805 FORMAT(A8)                                                        
 9806 FORMAT(/' MICROSCOPIC FORM FACTOR READ FROM FILE  ',A8/)          
 9807 FORMAT(A16)
 9808 FORMAT(/ 10X, 'Plotfile written to file = ',A / 37X, 16(1H-) //)
 9999 FORMAT(1H1, ' DWUCK4-DISTORTED WAVES U.COLORADO',
     >  '-VERSION 01/FEB /1982')
C
  899 FORMAT('1' // 35(1H*), 3X, 'END OF DWHI CALCULATION', 
     >  3X, 35(1H*) //)
      END                                                               
