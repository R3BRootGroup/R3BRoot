      SUBROUTINE COUPH (N)                                              
C
C        CALCULATES COULOMB PHASE SHIFTS SIGMA(L) FROM HIGH L SERIES       
C                                                                       
C        REF:  M.ABRAMOWITZ AND I.A.STEGUN, HANDBOOK OF MATHEMATICAL       
C        FUNCTIONS, DOVER, NEW YORK (1965).                                
C        DEFINITION OF PHASE SHIFT GIVEN BY EQN 14.5.6                     
C        VALUE FOR HIGH L GIVEN BY EQN 6.1.41                              
C        RECURSION TO LOWER L VALUES BY EQN 14.5.7                         
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C                                                                       
      PARAMETER ( MNX = 900 )
      PARAMETER ( MNL = 350 )
C                                                                       
      CHARACTER*52   TITLE
      COMMON /STUFF/ TITLE,ICON(16),ANGLE(3),HBARC,AMU,CHSQ, 
     >               DR,Z(2),ZA(2),FM(2),FMA(2),RC(3),FS(2),ECM(2), 
     >               FK(2),FK2(2), EETA(2),FMU(2),ISA,ISB,JA,JB,K,  
     >               LPLUS,MPLUS,NFF,LTRT(8),ISTRT(8),JTRT(8),IBF(8) 
C
      COMMON /ELSTUFF/ UR(MNX,2),  UI(MNX,2), VR(MNX,2), VI(MNX,2),
     >                 AR(0:MNX),  AI(0:MNX), FR(0:MNX), FI(0:MNX),
     >                 SIGPH(MNL), FC(MNL,2),  GC(MNL,2), 
     >                 TR(MNL),    TI(MNL) 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DATA C1 / 360.D0/                                                 
      DATA C2 /1260.D0/                                                 
      DATA C3 /1680.D0/                                                 
      DATA C4 /1188.D0/                                                 
C
      ETA=EETA(N)                                                       
      ETASQ=ETA**2                                                      
      MAXL=LPLUS                                                        
      IF( MAXL .LT. 50 ) THEN 
        ELP=50.D0                                                         
        J=50                                                              
      ELSE
        ELP=DFLOAT(MAXL)                                                  
        J=MAXL                                                            
      ENDIF
   30 ALPHA=DATAN(ETA/ELP)                                              
      BETA=DSQRT(ETASQ+ELP**2)                                          
      Y=ALPHA*(ELP-0.5D0)+ETA*(DLOG(BETA)-1.D0)                         
     1 -DSIN(ALPHA)/(12.D0*BETA)                                        
     1 +DSIN(3.D0*ALPHA)/(C1*BETA**3)                                   
     1 -DSIN(5.D0*ALPHA)/(C2*BETA**5)                                   
     1 +DSIN(7.D0*ALPHA)/(C3*BETA**7)                                   
     1 -DSIN(9.D0*ALPHA)/(C4*BETA**9)                                   
      M=J-1                                                             
      IF( J .LE. MAXL ) SIGPH(J)=Y   
      DO 70 I=1,M                                                       
      ELP=ELP-1.D0                                                      
      J=J-1                                                             
      Y=Y-DATAN(ETA/ELP)                                                
      IF( J .LE. MAXL ) SIGPH(J)=Y 
   70 CONTINUE                                                          
      RETURN                                                            
      END                                                               
      SUBROUTINE COULOM (N)                                             
C                                                                       
C     COULOM WAVEFUNCTIONS CALCULATED AT R=RHO BY THE CONTINUED         
C     FRACTION METHOD OF STEED                                          
C     SEE BARNETT FENG STEED AND GOLDFARB COMPUTER PHYSICS COMMUN 1974  
C                                                                       
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      PARAMETER ( MNX = 900 )
      PARAMETER ( MNL = 350 )
C
      CHARACTER*52   TITLE
      COMMON /STUFF/ TITLE,ICON(16),ANGLE(3),HBARC,AMU,CHSQ, 
     >               HINT, Z(2),ZA(2),FM(2),FMA(2),RC(3),FS(2),ECM(2), 
     >               FK(2),FK2(2), EETA(2),FMU(2),ISA,ISB,JA,JB, KK,  
     >               LPLUS,MPLUS,NFF,LTRT(8),ISTRT(8),JTRT(8),IBF(8) 
C
      COMMON /ELSTUFF/ UR(MNX,2),  UI(MNX,2), VR(MNX,2), VI(MNX,2),
     >                 XR(0:MNX),  XI(0:MNX), FR(0:MNX), FI(0:MNX),
     >                 SIGPH(MNL), FC(MNL,2),  GC(MNL,2), 
     >                 TR(MNL),    TI(MNL) 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C                                                                       
C          FC AND GC ARE REGULAR AND IRREGULAR COULOMB WAVES                 
C          ARGUMENTS (J,I) ARE : J=L+1                                       
C                              : I=1  RMAX-H                                 
C                              :   2  RMAX                                   
C                                                                       
C     FCP(I) AND GCP(I) ARE TEMPORARY STORAGE FOR DERIVATIVES OF        
C     COULOMB WAVES FOR I=L+1                                           
C                                                                       
      DIMENSION FCP(MNL), GCP(MNL) 
C 
      REAL*8 K,K1,K2,K3,K4,M1,M2,M3,M4                                  
C
      DATA PLMAX1 /20000.D0/                                            
      DATA PLMAX2 /46000.D0/                                            
      DATA PACSET /999.D0/                                              
      DATA MINL,ACCUR,STEP/0,1.D-10,100.D0/                             
C
      AK1=FK(N)                                                         
      ETA=EETA(N)                                                       
      RMAX=HINT*DFLOAT(KK)                                              
      RHOMX=AK1*(RMAX-HINT)                                             
      DO 120 I=1,2                                                      
      RHO=RHOMX                                                         
      PACE=STEP                                                         
      ACC=ACCUR                                                         
      IF(PACE.LT.100.0) PACE=100.D0                                     
      IF(ACC.LT.1.D-15.OR.ACC.GT.1.D-06) ACC=1.D-06                     
      R=RHO                                                             
      KTR=1                                                             
      LMAX=LPLUS                                                        
      LMIN1=MINL+1                                                      
      XLL1=DFLOAT(MINL*LMIN1)                                           
      ETA2=ETA*ETA                                                      
      TURN=ETA+DSQRT(ETA2+XLL1)                                         
      IF(R.LT.TURN.AND.DABS(ETA).GE.1.D-06) KTR=-1                      
      KTRP=KTR                                                          
      GOTO 20                                                           
   10 R=TURN                                                            
      TF=F                                                              
      TFP=FP                                                            
      LMAX=MINL                                                         
      KTRP=1                                                            
   20 ETAR=ETA*R                                                        
      RHO2=R*R                                                          
      PL=DFLOAT(LMAX+1)                                                 
      PMX=PL+0.5D0                                                      
C                                                                       
C     CONTINUED FRACTION FOR FP(MAXL)/F(MAXL) XL IS F XLPRIME IS FP **  
C                                                                       
      FP=ETA/PL+PL/R                                                    
      DK=ETAR*2.D0                                                      
      DEL=0.D0                                                          
      D=0.D0                                                            
      F=1.D0                                                            
      K=(PL*PL-PL+ETAR)*(2.D0*PL-1.D0)                                  
      IF(PL*PL+PL+ETAR.NE.0.0) GOTO 30                                  
      R=R+1.D-06                                                        
      GOTO 20                                                           
   30 H=(PL*PL+ETA2)*(1.D0-PL*PL)*RHO2                                  
      K=K+DK+PL*PL*6.D0                                                 
      D=1.D0/(D*H+K)                                                    
      DEL=DEL*(D*K-1.D0)                                                
      IF(PL.LT.PMX) DEL=-R*(PL*PL+ETA2)*(PL+1.D0)*D/PL                  
      PL=PL+1.D0                                                        
      FP=FP+DEL                                                         
      IF(D.LT.0.0) F=-F                                                 
      IF(PL.GT.PLMAX1)   GOTO 130                                       
      IF(DABS(DEL/FP).GE.ACC) GOTO 30                                   
      FP=F*FP                                                           
      IF(LMAX.EQ.MINL) GOTO 50                                          
      FC(LMAX+1,I)=F                                                    
      FCP(LMAX+1)=FP                                                    
C                                                                       
C     DOWNWARD RECURSION TO MINL FOR F AND FP. ARRAYS GC,GCP ARE STORAGE
C                                                                       
      L=LMAX                                                            
      DO 40 LP=LMIN1,LMAX                                               
      PL=DFLOAT(L)                                                      
      GC(L+1,I)=ETA/PL+PL/R                                             
      GCP(L+1)=DSQRT(ETA2+PL*PL)/PL                                     
      FC(L,I)=(GC(L+1,I)*FC(L+1,I)+FCP(L+1))/GCP(L+1)                   
      FCP(L)=GC(L+1,I)*FC(L,I)-GCP(L+1)*FC(L+1,I)                       
   40 L=L-1                                                             
      F=FC(LMIN1,I)                                                     
      FP=FCP(LMIN1)                                                     
   50 IF(KTRP.EQ.-1) GOTO 10                                            
C                                                                       
C     REPEAT FOR R=TURN IF RHO.LT.TURN                                  
C     NOW OBTAIN P+I.Q FOR MINL FROM CONTINUED FRACTION                 
C     REAL ARITHMETIC.                                                  
C                                                                       
      P=0.D0                                                            
      Q=R-ETA                                                           
      PL=0.D0                                                           
      AR=-(ETA2+XLL1)                                                   
      AI=ETA                                                            
      BR=2.D0*Q                                                         
      BI=2.D0                                                           
      WI=2.D0*ETA                                                       
      DR=BR/(BR*BR+BI*BI)                                               
      DI=-BI/(BR*BR+BI*BI)                                              
      DP=-(AR*DI+AI*DR)                                                 
      DQ=(AR*DR-AI*DI)                                                  
   60 P=P+DP                                                            
      Q=Q+DQ                                                            
      PL=PL+2.D0                                                        
      AR=AR+PL                                                          
      AI=AI+WI                                                          
      BI=BI+2.D0                                                        
      D=AR*DR-AI*DI+BR                                                  
      DI=AI*DR+AR*DI+BI                                                 
      T=1.D0/(D*D+DI*DI)                                                
      DR=T*D                                                            
      DI=-T*DI                                                          
      H=BR*DR-BI*DI-1.D0                                                
      K=BI*DR+BR*DI                                                     
      T=DP*H-DQ*K                                                       
      DQ=DP*K+DQ*H                                                      
      DP=T                                                              
      IF(PL.GT.PLMAX2)   GOTO 130                                       
      IF(DABS(DP)+DABS(DQ).GE.(DABS(P)+DABS(Q))*ACC) GOTO 60            
      P=P/R                                                             
      Q=Q/R                                                             
C                                                                       
C     SOLVE FOR FP,G,GP AND NORMALISE F AT L=MINL                       
C                                                                       
      G=(FP-P*F)/Q                                                      
      GP=P*G-Q*F                                                        
      W=1.D0/DSQRT(FP*G-F*GP)                                           
      G=W*G                                                             
      GP=W*GP                                                           
      IF(KTR.EQ.1) GOTO 80                                              
      F=TF                                                              
      FT=TFP                                                            
      LMAX=MAXL                                                         
C                                                                       
C     RUNGE-KUTTA INTEGRATION OF G(MINL) AND GP(MINL) INWARDS FROM TURN 
C     SEE FOX AND MAYERS 1968 PG 202                                    
C                                                                       
      IF(RHO.LT.0.2D0*TURN) PACE=PACSET                                 
      R3=1.D0/3.D0                                                      
      H=(RHO-TURN)/(PACE+1.D0)                                          
      H2=0.5D0*H                                                        
      I2=IDINT(PACE+1.D-03)                                             
      ETAH=ETA*H                                                        
      H2LL=H2*XLL1                                                      
      S=(ETAH+H2LL/R)/R-H2                                              
   70 RH2=R+H2                                                          
      T=(ETAH+H2LL/RH2)/RH2-H2                                          
      K1=H2*GP                                                          
      M1=S*G                                                            
      K2=H2*(GP+M1)                                                     
      M2=T*(G+K1)                                                       
      K3=H*(GP+M2)                                                      
      M3=T*(G+K2)                                                       
      M3=M3+M3                                                          
      K4=H2*(GP+M3)                                                     
      RH=R+H                                                            
      S=(ETAH+H2LL/RH)/RH-H2                                            
      M4=S*(G+K3)                                                       
      G=G+(K1+K2+K2+K3+K4)*R3                                           
      GP=GP+(M1+M2+M2+M3+M4)*R3                                         
      R=RH                                                              
      I2=I2-1                                                           
      IF(DABS(GP).GT.1.D+32) GOTO 130  
      IF(I2.GE.0) GOTO 70                                               
      W=1.D0/(FP*G-F*GP)                                                
C                                                                       
C     UPWARD RECURSION FROM GC(MINL) AND GCP(MINL),STORED VALUES ARE R,S
C     RENORMALISE FC,FCP FOR EACH L-VALUE                               
C                                                                       
   80 GC(LMIN1,I)=G                                                     
      GCP(LMIN1)=GP                                                     
      IF(LMAX.EQ.MINL) GOTO 110                                         
      DO 90 L=LMIN1,LMAX                                                
      T=GC(L+1,I)                                                       
      GC(L+1,I)=(GC(L,I)*GC(L+1,I)-GCP(L))/GCP(L+1)                     
      GCP(L+1)=GC(L,I)*GCP(L+1)-GC(L+1,I)*T                             
      FC(L+1,I)=W*FC(L+1,I)                                             
   90 FCP(L+1)=W*FCP(L+1)                                               
      FC(LMIN1,I)=W*FC(LMIN1,I)                                         
      FCP(LMIN1)=W*FCP(LMIN1)                                           
      DO 100 L=LMIN1,LMAX                                               
      GCP(L)=AK1*GCP(L)                                                 
  100 FCP(L)=AK1*FCP(L)                                                 
      GCP(LMAX+1)=AK1*GCP(LMAX+1)                                       
      FCP(LMAX+1)=AK1*FCP(LMAX+1)                                       
      GOTO 110                                                          
  110 FC(LMIN1,I)=W*F                                                   
      FCP(LMIN1)=AK1*W*FP                                               
      GCP(LMIN1)=AK1*GCP(LMIN1)                                         
  120 RHOMX=AK1*RMAX                                                    
      RETURN                                                            
  130 W=0.D0                                                            
      G=0.D0                                                            
      GP=0.D0                                                           
      WRITE(6,140)                                                      
  140 FORMAT(/44H PROBLEM IN COULOMB WAVEFUNCTION CALCULATION/)         
      GOTO 80                                                           
      END                                                               
