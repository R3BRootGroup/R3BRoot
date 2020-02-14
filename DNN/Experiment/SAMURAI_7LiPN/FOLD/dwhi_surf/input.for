      SUBROUTINE INPUT                                                  
C
C          Routine to read cards 2, 3, and 4; call FORMF twice to  
C          read card set 5 (incoming) and 6 (outgoing); then read
C          NFF sets of cards 7 and 8.
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
      INTEGER KRCO
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION FFR(MNX), FFI(MNX)
C
      COMPLEX*16 FFREAD(MNX)
C                                                                       
C          READ CARD SET 2                                                   
C
      IF( ICON(1) .EQ. 0 ) GO TO 5         
        READ (5,9001) ANGLE                 
        IF( ICON(1) .EQ. 1 ) GO TO 5         
          II=(ANGLE(1)-ANGLE(2))/ANGLE(3)+1.0 
          IF(II.LT.0) II=0 
          ANGLE(1)=II       
    5 CONTINUE                                                          
      WRITE(6,9010) ANGLE   
      IF( ANGLE(1) .GT. MNA ) THEN
        STOP ' -- TOO MANY ANGLES REQUESTED '
      ENDIF
C                                                                       
C          READ CARD SET 3                                                   
C
      READ (5,9002) L, NFF, ISA,ISB,JA,JB                                  
      WRITE(6,9008) L, NFF, ISA,ISB,JA,JB                                  
      IF( L .LT. 0 ) THEN 
        LC=IABS(L)  
      ELSE
        LC=0                                                              
      ENDIF
      L=IABS(L)                                                         
C
      IF( L .GE. MNL ) THEN
        STOP ' -- VALUE OF L IS TOO LARGE FOR CODE '
      ENDIF
C                              ... should use MNF = 8 ....
      IF( NFF .GT. 8 ) THEN
        IBF(3)=1  
        STOP ' -- VALUE OF NFF IS TOO BIG FOR CODE '
      ENDIF
C
      LPLUS=L+1                                                         
C                                                                       
C          READ CARD SET 4 -- input documentation changed ! 
C
      KRCO=1
      READ (5,9005) DR, NNR, KRCO                                              
      K = NNR
      RMAX = K * DR
      WRITE(6,9011) DR, K, RMAX                                              
      WRITE(6,*)'KRCO',KRCO
C
      IF( K .GT. MNX ) THEN
        STOP ' -- NUMBER OF MESH POINTS IS TOO BIG FOR CODE '
      ENDIF
C                                                                       
C          OPEN UNIT 2 FOR RADIAL MATRIX ELEMENTS                            
C                                                                       
      IUNIT=2       
      OPEN( UNIT=2, FILE='WRK002.SCR', 
     >      STATUS='NEW', 
     >      FORM='UNFORMATTED', IOSTAT=ICHK )   
      IF(ICHK.NE.0) THEN
        OPEN( UNIT=2, FILE='WRK002.SCR', 
     >      STATUS='NEW', 
     >      FORM='UNFORMATTED', IOSTAT=ICHK )   
      ENDIF
      IF(ICHK.NE.0) WRITE(6,9904) IUNIT, ICHK        
C                                                                       
C          OPEN UNIT 4 FOR FORM FACTORS                                      
C                                                                       
      IUNIT=4                                                           
      OPEN( UNIT=4, FILE='WRK004.SCR', 
     >      STATUS='NEW', 
     >      FORM='UNFORMATTED', IOSTAT=ICHK ) 
      IF(ICHK.NE.0) THEN
        OPEN( UNIT=4, FILE='WRK004.SCR', 
     >      STATUS='NEW', 
     >      FORM='UNFORMATTED', IOSTAT=ICHK ) 
      ENDIF
      IF(ICHK.NE.0) WRITE(6,9904) IUNIT, ICHK       
      WRITE(6,9504)                                                     
C                                                                       
C          READ IN DISTORTED WAVE INFORMATION.                               
C                                                                       
C             IBF(4)=0  NO L.S POTENTIAL                                        
C                   =1  L.S POTENTIAL PRESENT                                   
C
C          FORMF reads a set of cards for incoming (set 5) and
C          outgoing (set 6) waves.
C                                                                       
      IBF(4)=0                                                          
      DO 45 N=1,2                                                       
      CALL FORMF(N)                                                     
      DO 40 M=1,K                                                       
        MK=M+M-1                                                          
        UR(M,N)=UB(MK  )                                                  
        UI(M,N)=UB(MK+1)                                                  
        VR(M,N)=VB(MK  )                                                  
        VI(M,N)=VB(MK+1)                                                  
   40 CONTINUE                                                          
C                                                                       
C     PRINT OUT K(R)**2 FOR DISTORTED WAVES                             
C                                                                       
      IF( ICON(15) .NE. 0 ) THEN 
        WRITE(6,9057)                                                     
        WRITE(6,9058)                                                     
        WRITE(6,9101)                                                     
        R=DR                                                              
        DO 54 M=1,K,5                                                     
          MK4=MIN0(M+4,K)                                                   
          WRITE(6,9052)R,(UR(J,N),UI(J,N),J=M,MK4)                          
          R=R+5.D0*DR                                                       
   54   CONTINUE                                                          
        IF( IBF(4) .NE. 0 ) THEN
          WRITE(6,9059)                                                     
          WRITE(6,9101)                                                     
          R=DR                                                              
          DO 55 M=1,K,5                                                     
            MK4=MIN0(M+4,K)                                                   
            WRITE(6,9052)R,(VR(J,N),VI(J,N),J=M,MK4)                          
            R=R+5.D0*DR                                                       
   55     CONTINUE       
        ENDIF
      ENDIF
   45 CONTINUE                                                          
C                                                                       
C          OPEN UNIT 9 FOR INCOMING DISTORTED WAVES                          
C                                                                       
      IUNIT=9                                                           
      IRECL=4*K    
      IF( IBF(4) .NE. 0 ) IRECL=(2*ISA+1)*IRECL
      IRECL=10*IRECL
      write(*,*)IRECL,'**********'                             
      ISIZE=LPLUS*(IRECL/(4*192)+1)                                     
      OPEN(  UNIT=9, FILE='WRK009.SCR',  
     >       ACCESS='DIRECT',                         
     >       FORM='UNFORMATTED',                    
     >       IOSTAT=ICHK,                           
     >       RECL=IRECL,                  
     >       STATUS='NEW')                
      IF(ICHK.NE.0) THEN
        OPEN(  UNIT=9, FILE='WRK009.SCR',  
     >       ACCESS='DIRECT',                          
     >       FORM='UNFORMATTED',                    
     >       IOSTAT=ICHK,                           
     >       RECL=IRECL,                  
     >       STATUS='NEW')                
      ENDIF
      IF(ICHK.NE.0) WRITE(6,9904)IUNIT,ICHK                             
C                                                                       
C          OPEN UNIT 10 FOR OUTGOING DISTORTED WAVES                         
C                                                                       
      IUNIT=10                                                          
      IRECL=4*K  
      IF( IBF(4) .NE. 0 ) IRECL=(2*ISB+1)*IRECL                             
      IRECL=10*IRECL
      ISIZE=LPLUS*(IRECL/(4*192)+1)                                     
      OPEN(  UNIT=10, FILE='WRK010.SCR', 
     >       ACCESS='DIRECT',                        
     >       FORM='UNFORMATTED',                     
     >       IOSTAT=ICHK,                             
     >       RECL=IRECL,      
     >       STATUS='NEW' )             
      IF(ICHK.NE.0) THEN
        OPEN(  UNIT=10, FILE='WRK010.SCR', 
     >       ACCESS='DIRECT',                        
     >       FORM='UNFORMATTED',                     
     >       IOSTAT=ICHK,                             
     >       RECL=IRECL,      
     >       STATUS='NEW')             
      ENDIF
      IF(ICHK.NE.0) WRITE(6,9904)IUNIT,ICHK                             
C                                                                       
C          CHECK MASSES                                                      
C                                                                       
      I1=DABS(FM(1)-FM(2))+0.1D0                                        
      I2=DABS(FMA(1)-FMA(2))+0.1D0                                      
      IF(I1.EQ.0.AND.I2.EQ.0) GOTO 15                                   
      IBF(3)=1                                                          
      WRITE(6,9907)                                                       
   15 CONTINUE                                                          
C                                                                       
C          FORM FACTOR LOOP                                                  
C
C          Read NFF pairs of cards 7 and 8 
C                                                                       
      WRITE(6,9503)                                                     
      MPLUS=1                                                           
      DO 35 II=1,NFF                                                    
      IBF(4)=0                                                          
      WRITE(6,9999)                                                     
      DO 10 M=1,K                                                       
        FFR(M)=0.D0                                                       
        FFI(M)=0.D0                                                       
   10 CONTINUE                                                          
      READ(5,9002)LTRT(II),ISTRT(II),JTRT(II)                           
      ISTR=ISTRT(II)                                                    
      IF(ISTR.EQ.0) JTRT(II)=LTRT(II)+LTRT(II)                          
      WRITE(6,9100) II,LTRT(II),ISTRT(II),JTRT(II)                      
      READ(5,9001) BETAR,BETAI,BETAC,FNORM                              
      WRITE(6,9004)BETAR,BETAI,BETAC,FNORM                              
C                                                                       
C          CHECK ON SPINS AND STATISTICS                                     
C
C          ALSO DEFINE MPLUS TO BE LARGEST M + 1
C                                                                       
      LTR=LTRT(II)                                                      
      JTR=JTRT(II)                                                      
      MPLUS=MAX0(MPLUS,LTR+1)                                           
      IF(PHASEF(ISA+ISB+ISTR).LT.0.D0) GOTO 33                          
      IF(PHASEF(LTR+LTR+JTR+ISTR).LT.0.D0) GOTO 33                      
      GOTO 34                                                           
   33 CONTINUE                                                          
      IBF(3)=1                                                          
      WRITE(6,9906)IS,LTR,JTR                                           
   34 CONTINUE                                                          
      AFACT=FMA(1)**(1.D0/3.D0)                                         
      RC(3)=RY*AFACT                                                    
      FACT=2.D0*FMU(1)*AMU/HBARC**2                                     
      CFACT=BETAC*FACT                                                  
      IF(ICON(3).NE.0) GOTO 8                                           
      IF(II     .NE.1) GOTO 12                                          
    8 CONTINUE                                                          
      IF(ICON(2).EQ.2) GOTO 9                                           
C                                                                       
C          COLLECTIVE FORM FACTOR (for options 0 and 1) 
C                                                                       
      FACTR=BETAR                                                       
      FACTI=BETAI                                                       
      WRITE(6,9003)                                                     
      K2=K+K                                                            
      DO 17 M=1,K2                                                      
        UB(M)=0.D0                                                        
        VB(M)=0.D0                                                        
   17 CONTINUE                                                          
      CALL POTS (3,LTRT(II),AFACT,FACT,0.D0,0.D0)                       
      DO 11 M=1,K                                                       
        MK=M+M-1                                                          
        FFR(M)=FACTR*UB(MK  )                                             
        FFI(M)=FACTI*UB(MK+1)                                             
   11 CONTINUE                                                          
      IF(ICON(2).EQ.1) GOTO 12                                          
C                                                                       
C          MICROSCOPIC FORM FACTOR (for options 0 and 2) 
C
C          Note that K must be exactly the number of points 
C          written per record by the form factor program.
C                                                                       
    9 CONTINUE
      DO M=1,K                                                          
       READ(8,*)FFREAD(M)
      ENDDO                                             
      DO 16 M=KRCO,K                                                       
        FFR(M) = FFR(M) + FNORM*DREAL(FFREAD(M))                               
        FFI(M) = FFI(M) + FNORM*DIMAG(FFREAD(M))
   16 CONTINUE                                                          
   12 CONTINUE                                                          
C                                                                       
C     COULOMB EXCITATION ADDITION TO FORM FACTOR                        
C                                                                       
      IF(RC(3).EQ.0.D0) RC(3)=DR                                        
      VFACT=3.D0*CFACT*CHSQ*Z(1)*ZA(1)*RC(3)**LTR/DFLOAT(LTR+LTR+1)     
      IF(VFACT.EQ.0.D0) GOTO 14                                         
      NMTR=LTR+1                                                        
      R=0.D0                                                            
      DO 13 M=1,K                                                       
        R=R+DR                                                            
        IF(R.LT.RC(3)) GOTO 13                                            
        FFR(M)=FFR(M)+VFACT/R**NMTR                                       
   13 CONTINUE                                                          
   14 CONTINUE                                                          
C                                                                       
C     FORM FACTOR PRINTOUT                                              
C                                                                       
      IF(ICON(4).NE.0) GOTO 3005                                        
      R=DR                                                              
      WRITE(6,9101)                                                     
      DO 3000 M=1,K,5                                                   
        MK4=MIN0(M+4,K)                                                   
        WRITE(6,9052)R,(FFR(N),FFI(N),N=M,MK4)                            
        R=R+5.D0*DR                                                       
 3000 CONTINUE                                                          
C                                                                       
C          STORE FORM FACTOR ON DISC                                         
C                                                                       
 3005 CONTINUE                                                          
      WRITE(4) (FFR(M),FFI(M),M=1,K)                                    
   35 CONTINUE                                                          
      END FILE 4                                                        
      REWIND 4                                                          
      WRITE(6,9999)                                                     
      RETURN                                                            
C
 9001 FORMAT(10F7.4)                                                    
 9002 FORMAT(18I3)                                                      
 9003 FORMAT(21H0POTENTIAL PARAMETERS )                                 
 9004 FORMAT(8H0BETAR =,F9.4,9H  BETAI =,F9.4,9H  BETAC =,F9.4,         
     19H  FNORM =,F9.4)                                                 
 9005 FORMAT( F7.4, I5, I5)
 9008 FORMAT(18H0CARD SET 3 DATA  ,9H   LMAX =,I4,9H   NFF  =,I4,       
     19H   2*SA =,I4,9H   2*SB =,I4,9H   2*JA =,I4,9H   2*JB =,I4)      
 9010 FORMAT(18H0ANGLE DATA        ,9H   THETN=,F9.4,9H   THET1=,F9.4,
     >  9H   DTHET=,F9.4)                                                   
 9011 FORMAT(18H0CARD SET 4 DATA   ,9H   DR   =,F9.4, '  K = ',I5, 
     >  9H   RMAX =,F9.4)  
 9052 FORMAT(1H ,F6.2,10E12.4)                                          
 9057 FORMAT(27H0K(R)**2 FOR DISTORTED WAVE)                            
 9058 FORMAT(13H CENTRAL PART)                                          
 9059 FORMAT(16H SPIN-ORBIT PART)                                       
 9100 FORMAT(12H0FORM FACTOR,I2,7X,6H LTR =,I4,5X,9H   2*STR=,I4,5X,9H  
     1 2*JTR=,I4)                                                       
 9101 FORMAT(132H0    R    RL,R        IM,R        RL,R+DR     IM,R+DR  
     1   RL,R+DR*2   IM,R+DR*2   RL,R+DR*3   IM,R+DR*3   RL,R+DR*4   IM,
     2R+DR*4       )                                                    
 9500 FORMAT(5E14.6)                                                    
 9503 FORMAT( '1  FORM FACTOR DATA' )                                     
 9504 FORMAT(18H PARTICLE DATA    )                                     
 9803 FORMAT(1H ,16I2,4X,13A4)                                          
 9903 FORMAT(14H0ERROR ** LTR=,I3,14H  IS TOO LARGE )                   
 9904 FORMAT(22H0ERROR IN OPENING UNIT,I3,10H  IOSTAT =,I3)             
 9906 FORMAT(1H0,28HSPIN STATISTICS NOT CORRECT ,7H 2*IS1=,I3,7H 2*IS2=,
     1I3,7H 2*IS3=,I3,7H   LTR=,I3,7H 2*JTR=,I3)                        
 9907 FORMAT(26H0MASS TRANSFER NOT ALLOWED)                             
 9999 FORMAT(1H0,130(1H*))                                              
      END                                                               
