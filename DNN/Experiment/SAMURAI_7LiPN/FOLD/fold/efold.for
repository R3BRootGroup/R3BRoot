      SUBROUTINE EFOLD(NP,LP,ANUP,NT,LT,ANUT,LR,R,ANS)                  
C                                                                       
C     CALCULATES I(R) = FOLDING INTEGRAL OF 2 HARMONIC OSCILLATOR       
C     FUNCTIONS AND A SPHERICAL BESSEL FUNCTION.                        
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMPLEX*16 ANS 
C
      DIMENSION ANSL(5)                                                 
C
      PI=3.14159265358979D0                                             
      C1=4.0*PI*DSQRT(0.5*PI)                                           
      C2=ANORM(NP,LP,ANUP)*ANORM(NT,LT,ANUT)                            
      C3=DFAC(2*LP+1)                                                   
      C4=DFAC(2*LT+1)                                                   
      ELPT=0.5*DFLOAT(LP+LT+3)                                          
      ELP=1.5+DFLOAT(LP)                                                
      ELT=1.5+DFLOAT(LT)                                                
      A1=(ANUP*ANUT)**ELPT                                              
      A2=ANUP**ELP                                                      
      A3=ANUT**ELT                                                      
      A4=(ANUP+ANUT)**ELPT                                              
      C5=A1/(A2*A3*A4)                                                  
      C=C1*C2*C3*C4*C5                                                  
      RNU=ANUP*ANUT/(ANUP+ANUT)                                         
      X=RNU*(R**2)*0.5                                                  
      NP1=NP+1                                                          
      NT1=NT+1                                                          
      SUM=0.0                                                           
      DO 100 MDO=1,NP1                                                  
      MU=MDO-1                                                          
      T1=2.0**MU*PHAS(MU)                                               
      B1=BIN(NP,MU)                                                     
      DO 100 IA=1,MDO                                                   
      IALF=IA-1                                                         
      B2=BIN(MU,IALF)                                                   
      B=B1*B2                                                           
      DO 100 MPDO=1,NT1                                                 
      MUP=MPDO-1                                                        
      T2=2.0**MUP*PHAS(MUP)                                             
      T=T1*T2                                                           
      B3=BIN(NT,MUP)                                                    
      DO 100 IB=1,MPDO                                                  
      IBET=IB-1                                                         
      B4=BIN(MUP,IBET)                                                  
      IAB=IALF+IBET                                                     
      BP=B3*B4                                                          
      ANU=RNU**IAB/(ANUP**IALF)/(ANUT**IBET)                            
      NN=(LP+LT-LR+2*IAB)/2                                             
      G=AUGER(NN,LR,X)                                                  
      NUM=LP+LT+LR+2*IAB+1                                              
      NOM=2*LR+1                                                        
      NOM1=2*LP+2*IALF+1                                                
      NOM2=2*LT+2*IBET+1                                                
      TERM=T*B*BP*ANU*G*DFAC(NUM)/DFAC(NOM)*PHAS(IAB)                   
     */DFAC(NOM1)/DFAC(NOM2)                                            
      SUM=SUM+TERM                                                      
  100 CONTINUE                                                          
      ELR=DFLOAT(LR)/2.0                                                
      Y=(2.0*X)**ELR                                                    
      ANS=C*SUM*Y/EXP(X)                                                
      RETURN                                                            
      END                                                               
      REAL*8 FUNCTION AUGER(N,L,R)                                      
C                                                                       
C          ASSOCIATED LAGUERRE POLYNOMIALS                                   
C
C                        N
C                       2  N! (2L+1)!!
C          A   = P   ------------------------
C           NL    NL   (2N+2L+1)!!
C
C          This is defined so that N starts at zero and is 
C          positive near the origin.
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      SUM=0.0                                                           
      N1=N+1                                                            
      NUM=2*L+1                                                         
      XNUM=DFAC(NUM)                                                    
      DO 100 MDO=1,N1                                                   
      M=MDO-1                                                           
      B=BIN(N,M)                                                        
      NOM=2*L+2*M+1                                                     
      XDEN=DFAC(NOM)                                                    
      RR=PHAS(M)*(2.0*R)**M                                             
  100 SUM=SUM+B*XNUM*RR/XDEN                                            
      AUGER=SUM                                                         
      RETURN                                                            
      END                                                               
      REAL*8 FUNCTION ANORM(N,L,ANU)                                    
C
C          Factor needed for H.O. radial function
C
C
C                  |   N+L+2             |1/2            L+3/2
C                  |  2      (2N+2L+1)!! |      sqrt(ANU)
C          ANORM = | ------------------  |     ----------------
C                  |    sqrt(PI)  N!     |         N
C                  |                     |        2  (2L+1)!
C
C
C          This is defined with N starting at zero, and is
C          always positive.  ANU is (ALPHA)**2 coming in.
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      NUM=2*L+2*N+1                                                     
      NOM=2*L+1                                                         
      EL=DFLOAT(L)+1.5                                                  
      PI=3.14159265358979D0                                             
      CONST=DSQRT(2.0/PI)                                               
      XNUM=CONST*DFAC(NUM)*(2.0*ANU)**EL                                
      XDEN=(2.0**N)*FAC(N)*DFAC(NOM)**2                                 
      ANORM=DSQRT(XNUM/XDEN)                                            
      RETURN                                                            
      END                                                               
      REAL*8 FUNCTION FNTGRL (N,DELTA,F)                                
C
C          Routine to integrate the function F
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      DIMENSION F(150)                                                  
C
      S=0.0                                                             
      DO 20 J=5,N,4                                                     
      M=J                                                               
   20 S=S+7.0*(F(J-4)+F(J))+32.0*(F(J-3)+F(J-1))+12.0*F(J-2)            
      IF(N-M) 50,50,25                                                  
   25 M=M+1                                                             
      DO 30 J=M,N                                                       
   30 S=S+7.84375*F(J)+20.1875*F(J-1)-8.25*F(J-2)+3.3125*F(J-3)-0.59375*
     *  F(J-4)                                                          
   50 FNTGRL=DELTA*S/22.5                                               
      RETURN                                                            
      END                                                               
