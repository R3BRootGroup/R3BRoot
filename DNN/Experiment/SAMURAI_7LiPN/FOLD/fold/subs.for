      REAL*8 FUNCTION PHAS(N)                                           
C                                                                       
C          PHAS(N)=(-1.0)**N                                                 
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      PHAS=DFLOAT(1-2*IABS(N-(N/2)*2))                                  
c      write(*,*)'phase',N,PHAS,IABS(N-(N/2)*2),(N-(N/2)*2)
      RETURN                                                            
      END                                                               
      SUBROUTINE FACLOG                                                 
C                                                                       
C          FL(J)=LN(FACTORIAL(J))                                            
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL1/ FL(130)                                              
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      FL(1)=0.0                                                         
      FL(2)=0.0                                                         
      FNN=1.0                                                           
      DO 7 J=3,130                                                      
      FNN=FNN+1.0                                                       
    7 FL(J)=FL(J-1)+DLOG(FNN)                                           
      RETURN                                                            
      END                                                               
      SUBROUTINE LOGFAC                                                 
C                                                                       
C          FACT(I)=LN(FACTORIAL(I-1))                                        
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /LFAC/ FACT(500)                                           
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      FACT(1)=0.D0                                                      
      DO 10 I=2,500                                                     
   10 FACT(I)=FACT(I-1)+DLOG(DFLOAT(I-1))                               
      RETURN                                                            
      END                                                               
      REAL*8 FUNCTION BIN(N,L)                                          
C                                                                       
C          BINOMIAL COEFFICIENTS  ( N over L )
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      IF( L .GT. N ) THEN                                                      
        BIN=10.0**30   
        LP=L-N                                                            
        WRITE(6,10) LP                                                    
   10   FORMAT(/'  *** NO BIN *** (L-N)=',I5)                             
      ELSE                                                              
        XNUM=FAC(N)                                                       
        NL=N-L                                                            
        XDEN=FAC(L)*FAC(NL)                                               
        BIN=XNUM/XDEN                                                     
      ENDIF                                                             
      RETURN                                                            
      END                                                               
      REAL*8 FUNCTION DFAC(N)                                           
C                                                                       
C          DOUBLE FACTORIAL                                                  
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      IF(N) 11,12,11                                                    
   12 DFAC=1.0                                                          
      GOTO 13                                                           
   11 IF(N-2*(N/2)) 14,15,14                                            
   14 DFAC=1.0                                                          
      DO 16 I=1,N,2                                                     
   16 DFAC=I*DFAC                                                       
      RETURN                                                            
   15 DFAC=1.0                                                          
      DO 17 I=2,N,2                                                     
   17 DFAC=I*DFAC                                                       
   13 RETURN                                                            
      END                                                               
      REAL*8 FUNCTION FAC(N)                                            
C                                                                       
C          FACTORIAL                                                         
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      IF( N .EQ. 0 ) THEN 
        FAC=1.0           
        RETURN             
      ENDIF
      FAC=1.0                                                           
      DO 4 I=1,N                                                        
      FAC=I*FAC                                                         
    4 CONTINUE
      RETURN                                                            
      END                                                               
      DOUBLE PRECISION FUNCTION DELTA (A,B,C)                           
C                                                                       
C     DELTA(A,B,C)=SQRT(FACT(A+B-C)*FACT(A-B+C)*FACT(B+C-A)/            
C     FACT(A+B+C+1))   WHERE FACT(I) MEANS FACTORIAL(I)                 
C                                                                       
C     REF:  D.M.BRINK AND G.R.SATCHLER, ANGULAR MOMENTUM (2ND ED),      
C     OXFORD UNIVERSITY PRESS (1979), EQN 2.34                          
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C                                                                       
C          FACT(I)=LN(FACTORIAL(I-1))                                        
C                                                                       
      COMMON /LFAC/ FACT(500)                                           
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      ZERO=0.D0                                                         
      HALF=0.5D0                                                        
      TWO=2.D0                                                          
      PWRM9=1.D-9                                                       
      DELTA=ZERO                                                        
      IA=IDINT(TWO*A+PWRM9)                                             
      IB=IDINT(TWO*B+PWRM9)                                             
      IC=IDINT(TWO*C+PWRM9)                                             
      IG=IA+IB+IC                                                       
      IF(MOD(IG,2).NE.0) RETURN                                         
      IG=IG/2                                                           
      I=IG-IC                                                           
      IF(I.LT.0) RETURN                                                 
      J=IG-IA                                                           
      IF(J.LT.0) RETURN                                                 
      K=IG-IB                                                           
      IF(K.LT.0) RETURN                                                 
      DELTA=DEXP(HALF*(FACT(I+1)+FACT(J+1)+FACT(K+1)-FACT(IG+2)))       
      RETURN                                                            
      END                                                               
      REAL*8 FUNCTION DFLOAT(I)                                         
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      DFLOAT = DBLE(FLOAT(I))                                             
      RETURN                                                            
      END                                                               
