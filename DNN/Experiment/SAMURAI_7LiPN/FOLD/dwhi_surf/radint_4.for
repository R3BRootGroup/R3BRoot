      SUBROUTINE RADINT                                                 
C                                                                       
C     CALCULATES RADIAL INTEGRAL OF OUTGOING DISTORTED WAVE * REDUCED   
C     MATRIX ELEMENT OF TRANSITION OPERATOR * INCOMING DISTORTED WAVE   
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      PARAMETER ( MNX = 900 )
      PARAMETER ( MNL = 350 )
C
C          Enlarged from 3 to 5 to permit larger LTR calculations
C          as of 16 January 1990 by J.A.Carr; old version was 
C          limited to LTR=2 by this parameter definition.
C
      PARAMETER ( MNI = 5*MNL )
C
      CHARACTER*52   TITLE
      COMMON /STUFF/ TITLE,ICON(16),ANGLE(3),HBARC,AMU,CHSQ, 
     >               DR,Z(2),ZA(2),FM(2),FMA(2),RC(3),FS(2),ECM(2), 
     >               FK(2),FK2(2),ETA(2),FMU(2),ISA,ISB,JA,JB,K,  
     >               LPLUS,MPLUS,NFF,LTRT(8),ISTRT(8),JTRT(8),IBF(8) 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION FFR(MNX), FFI(MNX), FR(MNX),  FI(MNX)
      DIMENSION UR1(MNX), UI1(MNX), UR2(MNX), UI2(MNX)   
C
C          This dimension is adequate for LTR=4 with full range
C          of partial waves.  The range is tested below for safety. 
C
      DIMENSION FINTR(MNI), FINTI(MNI)                          
C
      DATA PI /3.14159265358979D0/                                      
C                                                                       
C          LOOP OVER FORM FACTORS                                            
C                                                                       
      DO 70 II=1,NFF                                                    
      LTR=LTRT(II)                                                      
      LTR2=LTR+LTR                                                      
C                                                                       
C          READ IN FORM FACTOR                                               
C                                                                       
      READ(4) (FFR(M),FFI(M),M=1,K)
C          WRITE(*,*)'KRCO',KRCO
      KRCO=1
      DO M=1,K
       IF (M.LT.KRCO) THEN
        FFR(M)=0.
        FFI(M)=0.
       endif
      ENDDO                                  
C                                                                       
C          LOOP OVER INCOMING L                                              
C                                                                       
      INDEX=0                                                           
      DO 60 LI1=1,LPLUS                                                 
      LI=LI1-1                                                          
      LI2=LI+LI                                                         
C                                                                       
C     CALCULATE STARTING POINT FOR INCOMING WAVE                        
C                                                                       
      NM20=K-20                                                         
      FLI=DFLOAT(LI)                                                    
      M1=IDINT(DSQRT(FLI*(FLI+1.D0)/12.D0))                             
      XPO=1.D0/(2.D0*FLI+1.D0)                                          
      M2=1.D-16**XPO*2.D0*FLI/(2.71828182845905D0*FK(1)*DR)             
      M1=MAX0(M1,M2)+1                                                  
      MZ1=MIN0(M1,NM20)                                                 
C                                                                       
C     READ IN INCOMING DISTORTED WAVE/R                                 
C                                                                       
      READ(9,REC=LI1) (UR1(M),UI1(M),M=1,K)                             
C                                                                       
C          LOOP OVER OUTGOING L                                              
C                                                                       
      LOMAX1=MIN0(LPLUS,LI1+LTR)                                        
      LOMIN1=IABS(LI-LTR)+1                                             
      DO 50 LO1=LOMIN1,LOMAX1                                           
      LO=LO1-1                                                          
      LO2=LO+LO                                                         
      IF(PHASEF(LI+LO+LTR).NE.1.D0) GOTO 50                             
C                                                                       
C     CALCULATE STARTING POINT FOR OUTGOING WAVE                        
C                                                                       
      FLO=DFLOAT(LO)                                                    
      M1=IDINT(DSQRT(FLO*(FLO+1.D0)/12.D0))                             
      XPO=1.D0/(2.D0*FLO+1.D0)                                          
      M2=1.D-16**XPO*2.D0*FLO/(2.71828182845905D0*FK(2)*DR)             
      M1=MAX0(M1,M2)+1                                                  
      MZ2=MIN0(M1,NM20)                                                 
C                                                                       
C          STARTING POINT FOR INTEGRATION                                    
C                                                                       
      MZ=MAX0(MZ1,MZ2)                                                  
C                                                                       
C     READ IN OUTGOING DISTORTED WAVE*R                                 
C                                                                       
      READ(10,REC=LO1) (UR2(M),UI2(M),M=1,K)                            
C                                                                       
C     FORM INTEGRAND                                                    
C                                                                       
      SCON=PHASEF(LTR)*DSQRT(DFLOAT(LTR2+1)/(4.D0*PI))                  
     *    *VCC(LO2,LTR2,LI2,0,0)                                        
      DO 40 M=MZ,K                                                      
      AR=SCON*(FFR(M)*UR1(M)-FFI(M)*UI1(M))                             
      AI=SCON*(FFR(M)*UI1(M)+FFI(M)*UR1(M))                             
      FR(M)=UR2(M)*AR-UI2(M)*AI                                         
      FI(M)=UR2(M)*AI+UI2(M)*AR                                         
   40 CONTINUE                                                          
C                                                                       
C     INTEGRATE USING TRAPEZOIDAL METHOD                                
C                                                                       
      INDEX=INDEX+1                                                     
      IF( INDEX .GT. MNI ) THEN
        STOP ' -- EXCEEDED NUMBER OF RADIAL INTEGRALS ALLOWED '
      ENDIF
C
      SR=-0.5D0*(FR(MZ)+FR(K))                                          
      SI=-0.5D0*(FI(MZ)+FI(K))                                          
      DO 45 M=MZ,K                                                      
      SR=SR+FR(M)                                                       
      SI=SI+FI(M)                                                       
   45 CONTINUE                                                          
      FINTR(INDEX)=DR*SR                                                
      FINTI(INDEX)=DR*SI                                                
   50 CONTINUE                                                          
   60 CONTINUE                                                          
C                                                                       
C     WRITE RADIAL MATRIX ELEMENTS TO UNIT 2                            
C                                                                       
      WRITE(2) (FINTR(I),FINTI(I),I=1,MNI) 
   70 CONTINUE                                                          
      END FILE 2                                                        
      REWIND 2                                                          
      RETURN                                                            
      END                                                               
      SUBROUTINE BETA (II)                                              
C         
C          CALCULATES BETA COEFFICIENTS                                      
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      PARAMETER ( MNL = 350  )
      PARAMETER ( MNI = 5*MNL )
C
      CHARACTER*52   TITLE
      COMMON /STUFF/ TITLE,ICON(16),ANGLE(3),HBARC,AMU,CHSQ, 
     >               DR,Z(2),ZA(2),FM(2),FMA(2),RC(3),FS(2),ECM(2), 
     >               FK(2),FK2(2),ETA(2),FMU(2),ISA,ISB,JA,JB,K,  
     >               LPLUS,MPLUS,NFF,LTRT(8),ISTRT(8),JTRT(8),IBF(8) 
C
C          We check that MPLUS*LPLUS is less than 3000 here
C
      COMMON /BETAS/ BETAR(3000), BETAI(3000)
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION FINTR(MNI), FINTI(MNI)  
C
      IF( II.EQ.1 .OR. ICON(3).NE.2 ) THEN
        IMAX=MPLUS*LPLUS                                                  
        IF( IMAX .GT. 3000 ) THEN
          STOP ' -- PROBLEM EXCEEDS ALLOTED SIZE FOR BETAS '
        ENDIF
        DO 10 I=1,IMAX                                                    
        BETAR(I)=0.D0                                                     
        BETAI(I)=0.D0                                                     
   10   CONTINUE                                                          
      ENDIF
      LTR=LTRT(II)                                                      
      LTR2=LTR+LTR                                                      
C                                                                       
C          READ IN RADIAL MATRIX ELEMENTS                                    
C                                  
      READ(2) (FINTR(I),FINTI(I),I=1,MNI)                              
C                                                                       
C          LOOP OVER INCOMING L                                              
C                                                                       
      INDEX=0                                                           
      DO 40 LI1=1,LPLUS                                                 
      LI=LI1-1                                                          
      LI2=LI+LI                                                         
C                                                                       
C          LOOP OVER OUTGOING L (check triangle)
C                                                                       
      LOMAX1=MIN0(LPLUS,LI1+LTR)                                        
      LOMIN1=IABS(LI-LTR)+1                                             
      DO 30 LO1=LOMIN1,LOMAX1                                           
      LO=LO1-1                                                          
      LO2=LO+LO                                                         
      IF(PHASEF(LI+LO+LTR).NE.1.D0) GOTO 30                             
      INDEX=INDEX+1                                                     
      TEMP=PHASEF((LI-LO-LTR)/2+LTR)*DFLOAT(LO2+1)/DSQRT(DFLOAT(LTR2+1))
      TEMPR=TEMP*FINTR(INDEX)                                           
      TEMPI=TEMP*FINTI(INDEX)                                           
C                                                                       
C          LOOP OVER M OF RELATIVE MOTION (added check on triangle) 
C  
      MMAX = MIN0(LTR,LO) + 1 
      DO 20 M1=1,MMAX                                                   
      M=M1-1                                                            
      M2=M+M                                                            
      TEMP=DSQRT(YXFCT(LO+M,LO-M))*VCC(LO2,LTR2,LI2,M2,-M2)             
      ILM=LO+M*LPLUS+1                                                  
      BETAR(ILM)=BETAR(ILM)+TEMP*TEMPR                                  
      BETAI(ILM)=BETAI(ILM)+TEMP*TEMPI                                  
   20 CONTINUE                                                          
   30 CONTINUE                                                          
   40 CONTINUE                                                          
      IF(ICON(8).EQ.0) GOTO 70                                          
      IF(ICON(3).EQ.2.AND.II.NE.NFF) GOTO 70                            
C                                                                       
C     WRITE OUT BETA IF ICON(8).NE.0                                    
C                                                                       
      DO 60 LO1=1,LPLUS                                                 
      LO=LO1-1                                                          
      IMAX=LO1+(MPLUS-1)*LPLUS                                          
      WRITE(6,9902)LO,(BETAR(I),BETAI(I),I=LO1,IMAX,LPLUS)              
   60 CONTINUE                                                          
   70 CONTINUE                                                          
      RETURN                                                            
C
 9902 FORMAT(I4,10E10.3/(4X,10E10.3))                                   
      END                                                               
