      SUBROUTINE FORMF (N)                                              
C
C          Routine to construct the optical potential according to
C          the information on a set of cards dictated by the options.
C
C          The actual potential cards are read by POTS.
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      PARAMETER ( MNX = 900 )
C
      CHARACTER*52   TITLE
      COMMON /STUFF/ TITLE,ICON(16),ANGLE(3),HBARC,AMU,CHSQ, 
     >               DR,Z(2),ZA(2),FM(2),FMA(2),RC(3),FS(2),ECM(2), 
     >               FK(2),FK2(2),ETA(2),FMU(2),ISA,ISB,JA,JB,K, 
     >               LPLUS,MPLUS,NFF,LTRT(8),ISTRT(8),JTRT(8),IBF(8) 
C
      COMMON /FORMS/ U(2*MNX), V(2*MNX)
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION G(4),D(4)                                 
      DATA ETA6/60.0/                                                   
C                                                                       
C          Read in CARD A of the set.              
C                                                                       
      READ (5,9000)E,FM(N),Z(N),FMA(N),ZA(N),RY,FS(N),QCD               
      E=E+QCD                                                           
      IF( N .EQ. 2 ) THEN  
        Q=E         
        E=(ECM(1)+Q)*(FM(2)+FMA(2))/FMA(2)                                
      ENDIF  
      AFACT=FMA(N)**.333333333                                          
      RC(N)=RY*AFACT                                                    
      DO 12 M = 1, 2*MNX                                                  
      U(M) = 0.0                                                          
      V(M) = 0.0                                                          
   12 CONTINUE                                                          
      RM=0.0                                                            
C                                                                       
C          ICON(10).NE.0 GIVES RELATIVISTIC KINEMATICS                       
C                                                                       
      IF( ICON(10) .NE. 0 ) THEN  
        IF( N.EQ.2 .AND. QCD.LE.0.0 ) THEN   
          E=E+(ECM(1)+Q)**2/(2.0*FMA(2)*AMU)                                
        ENDIF  
        FM1=FM(N)*AMU                                                     
        FM2=FMA(N)*AMU                                                    
        FMT=FM1+FM2                                                       
        T1=DSQRT(2.0*E*FM2+FMT**2)                                        
        W1=(FMT*FM1+FM2*E)/T1                                             
        W2=FM2*(E+FMT)/T1                                                 
        FMU(N)=W1*W2/(W1+W2)                                              
        ECM(N)=T1-FMT                                                     
        FACT=2.0*FMU(N)/HBARC**2                                          
        FMU(N)=FMU(N)/AMU                                                 
        FK2(N)=(E**2+2.0*E*FM1)*(FM2/T1)**2/HBARC**2                      
      ELSE   
        FMU(N)=FM(N)*FMA(N)/(FM(N)+FMA(N))                                
        ECM(N)=E*FMU(N)/FM(N)                                             
        FACT=2.0*FMU(N)*AMU/HBARC**2                                      
        FK2(N)=FACT*ECM(N)                                                
      ENDIF  
      FK(N)=DSQRT(DABS(FK2(N)))                                         
      ETAK=CHSQ*Z(N)*ZA(N)*FACT                                         
      ETA(N)=ETAK*0.5/FK(N)                                             
C                                                                       
C          ADD COULOMB AND KINETIC ENERGIES TO U                             
C                                                                       
      RCX=RC(N)                                                         
      IF(RCX.EQ.0.0) RCX=DR                                             
      R=0.0                                                             
      FCOU=0.5*ETAK/RCX                                                 
      DO 42 M=1, MNX  
      MK=M+M-1                                                          
      R=R+DR                                                            
      IF( R .LE. RCX ) THEN   
        FC=FCOU*(3.0-(R/RCX)**2)                                          
      ELSE   
        FC=ETAK/R                                                         
      ENDIF  
      U(MK)=U(MK)+FK2(N)-FC                                             
   42 CONTINUE                                                          
C
C          Output the kinematic information for this channel.
C
      WRITE(6,9010)N                                                    
      WRITE(6,9503)E,RY                                                 
      WRITE(6,9504)FM(N),FMA(N)                                         
      WRITE(6,9505)Z(N),ZA(N)                                           
      WRITE(6,9500)                                                     
      WRITE(6,9506)ECM(N),RC(N)                                         
      WRITE(6,9507)FK(N),ETA(N)                                         
      WRITE(6,9008)                                                     
C
C          Read in as many of CARD B as needed to form and 
C          return the potential in U and V of /FORMS/.
C
      CALL POTS (N,0,AFACT,FACT,E,RM)                                   
      RETURN                                                            
C
 9000 FORMAT(10F7.4)                                                    
 9008 FORMAT(21H0POTENTIAL PARAMETERS)                                  
 9010 FORMAT(9H0PARTICLE,I2,120(1H*))                                   
 9500 FORMAT(1H ,3A6,5(3X,A6,F9.4))                                     
 9503 FORMAT(18H INPUT DATA       ,9H   ELAB =,F9.4,9H   RC0  =,F9.4)   
 9504 FORMAT(18X,9H   MASSP=,F9.4,9H   MASST=,F9.4)                     
 9505 FORMAT(18X,9H   ZP   =,F9.4,9H   ZT   =,F9.4)                     
 9506 FORMAT(18H DERIVED DATA     ,9H   ECM  =,F9.4,9H   RC   =,F9.4)   
 9507 FORMAT(18X,9H   K    =,F9.4,9H   ETA  =,F9.4)                     
      END                                                               
      SUBROUTINE POTS (N,LAM,AFACT,FACT,E,RM)                           
C
C          Routine to form the scattering potential in U and
C          V demanded by the set of CARD B input by user.
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      PARAMETER ( MNX = 900 )
C
      CHARACTER*52   TITLE
      COMMON /STUFF/ TITLE,ICON(16),ANGLE(3),HBARC,AMU,CHSQ, 
     >               DR,Z(2),ZA(2),FM(2),FMA(2),RC(3),FS(2),ECM(2), 
     >               FK(2),FK2(2),ETA(2),FMU(2),ISA,ISB,JA,JB,K,  
     >               LPLUS,MPLUS,NFF,LTRT(8),ISTRT(8),JTRT(8),IBF(8) 
C
      COMMON /FORMS/ U(2*MNX), V(2*MNX)
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION UT(5),CN(16),CP(16),YLAM(16),X(8),W(8),
     1          ABETA(3),FLDF(3),LDFRM(3),G(4)  
C
      CHARACTER*18 B(0:14)
C
      DATA B(0)  / ' NX=0  THAT IS ALL' / 
      DATA B(1)  / ' NX=1 VOLUME W-S  ' / 
      DATA B(2)  / ' NX=2 SURFACE W-S ' / 
      DATA B(3)  / ' NX=3 2ND DERIV   ' / 
      DATA B(4)  / ' NX=4 L.S VOLUME  ' / 
      DATA B(5)  / ' NX=5 L.S SURFACE ' /     
      DATA B(6)  / ' NX=6 VOL*R**POWR ' /    
      DATA B(7)  / ' NX=7 SUR*R**POWR ' /   
      DATA B(8)  / ' NX=8 EXTERN FORMF' /  
      DATA B(9)  / ' NX=9 HARMONIC OSC' /  
      DATA B(10) / ' NX=10 GAUSUSIAN  ' / 
      DATA B(11) / ' NX=11 DEFORM VOL ' /    
      DATA B(12) / ' NX=12 DEFORM SURF' /   
      DATA B(13) / ' NX=13 HULTHEN    ' /  
      DATA B(14) / ' NX=14 MIXED W-S  ' /  
C                                                                       
C          POINTS  FOR GAUSS LEGENDRE INTEGRATION                            
C                                                                       
      DATA X/0.0950125098,0.2816035507,0.4580167776,0.6178762444,       
     1       0.7554044083,0.8656312023,0.9445750230,0.9894009439/       
C                                                                       
C          WEIGHTS FOR GAUSS LEGENDRE INTEGRATION                            
C                                                                       
      DATA W/0.1894506104,0.1826034150,0.1691565193,0.1495959888,       
     1       0.1246289712,0.0951585116,0.0622535239,0.0271524594/       
      DATA ETA4,ETA5/ 4.0,10.0/                                         
      DATA SQRPI/1.772453851/                                           
C                                                                       
C          Read in next CARD B of this set of potential cards 
C
C          Last one has FZ = 0.
C                                                                       
   70 CONTINUE                                                          
      READ (5,9000)FZ,VR,RY,AR,VSOR,VI,RZ,AI,VSOI,PWR,WR,WS   
      NX=DABS(FZ)                                                       
      RR=RY*AFACT                                                       
      RI=RZ*AFACT                                                       
      IF(N.NE.3) GO TO 74                                               
      IF(ICON(4).EQ.2) GO TO 75                                         
   74 CONTINUE                                                          
      WRITE(6,9509) B(NX), VR,RY,AR,RR,VSOR                     
      WRITE(6,9510)        VI,RZ,AI,RI,VSOI,PWR,WR,WS                 
   75 CONTINUE                                                          
C
C      NX=1  VOLUME WOODS-SAXON                                          
C      NX=2  SURFACE WOODS-SAXON                                         
C      NX=3  SECOND DERIVATIVE WOODS-SAXON                               
C      NX=4  L.S POTENTIAL FOR WOODS-SAXON VOLUME                        
C      NX=5  L.S POTENTIAL FOR WOODS-SAXON SURFACE                       
C      NX=6  WOOD-SAXON*R**PWR                                           
C      NX=7  1ST-DER WOOD-SAXON*R**PWR                                   
C      NX=8  EXTERNAL FORM FACTOR                                        
C      NX=9  HARMONIC OSCILLATOR                                         
C      NX=10 GAUSSIAN*R**PWR                                             
C      NX=11 DEFORMED VOLUME                                             
C      NX=12 DEFORMED SURFACE                                            
C
      KFLAG=0                                                           
      IF(N-3)79,83,79                                                   
   79 CONTINUE                                                          
      VR=VR*FACT                                                        
      VI=VI*FACT
      WS=WS*FACT
      WR=WR*FACT
   83 CONTINUE                                                          
      IF(AR.NE.0.0) GO TO 85                                            
      F1=0.0                                                            
      F2=0.                                                             
      GO TO 86                                                          
   85 F2=DEXP(-DR/AR)                                                   
      F1=DEXP( RR/AR)                                                   
   86 CONTINUE                                                          
      IF(AI.NE.0.0) GO TO 95                                            
      F3=0.0                                                            
      F4=0.                                                             
      GO TO 96                                                          
   95 F4=DEXP(-DR/AI)                                                   
      F3=DEXP( RI/AI)                                                   
   96 CONTINUE                                                          
      IF(N.NE.3 ) GO TO 98                                              
      IF(E.NE.0.0) GO TO 98                                             
      IF(NX.GT.5) GO TO 98                                              
      PWR=PWR+1.0                                                       
      IF(AR.NE.0.0) VR=VR*(RR/AR)**PWR                                  
      IF(AI.NE.0.0) VI=VI*(RI/AI)**PWR                                  
   98 CONTINUE                                                          
      IF(NX.EQ.0) GO TO 2009                                            
      GO TO (100,200,300,400,500,600,700,800,900,1000,1100,1200),NX     
C                                                                       
C     VOLUME WOODS SAXON                                                
C                                                                       
  100 CONTINUE                                                          
      DO 160 M=1,K                                                      
      MK=M+M-1                                                          
      F1=F1*F2                                                          
      F3=F3*F4                                                          
      U(MK  )=U(MK  )-VR*F1/(1.0+F1)                                    
      U(MK+1)=U(MK+1)-VI*F3/(1.0+F3)   
      WRITE(*,*) 'Imaginary SURFACE Potential = ',F3*4/(1.0+F3)**2
      WRITE(*,*) 'Real      SURFACE Potential = ',F1*4/(1.0+F1)**2
      U(MK+1)=U(MK+1)-WS*F3*4/(1.0+F3)**2
      U(MK  )=U(MK  )-WR*F1*4/(1.0+F1)**2
  160 CONTINUE                                                          
      GO TO 2000                                                        
C                                                                       
C     1ST DER WOODS SAXON                                               
C                                                                       
  200 CONTINUE                                                          
      DO 260 M=1,K                                                      
      MK=M+M-1                                                          
      F1=F1*F2                                                          
      F3=F3*F4                                                          
      U(MK  )=U(MK  )+VR*F1/(1.0+F1)**2                                 
      U(MK+1)=U(MK+1)+VI*F3/(1.0+F3)**2
      U(MK+1)=U(MK+1)-WS*F3*4/(1.0+F3)**2
      U(MK  )=U(MK  )-WR*F1*4/(1.0+F1)**2
  260 CONTINUE                                                          
      GO TO 2000                                                        
C                                                                       
C     2ND DER WOODS SAXON                                               
C                                                                       
  300 CONTINUE                                                          
      DO 360 M=1,K                                                      
      MK=M+M-1                                                          
      F1=F1*F2                                                          
      F3=F3*F4                                                          
      U(MK  )=U(MK  )-VR*F1*(1.0-F1)/(1.0+F1)**3                        
      U(MK+1)=U(MK+1)-VI*F3*(1.0-F3)/(1.0+F3)**3
      U(MK+1)=U(MK+1)-WS*F3*4/(1.0+F3)**2
      U(MK  )=U(MK  )-WR*F1*4/(1.0+F1)**2
  360 CONTINUE                                                          
      GO TO 2000                                                        
C                                                                       
C     L.S VOLUME WOODS SAXON                                            
C                                                                       
  400 CONTINUE                                                          
      IBF(4)=1                                                          
      IF(AR.NE.0.0) VR=VR/AR                                            
      IF(AI.NE.0.0) VI=VI/AI                                            
      R=0.0                                                             
      DO 460 M=1,K                                                      
      R=R+DR                                                            
      MK=M+M-1                                                          
      F1=F1*F2                                                          
      F3=F3*F4                                                          
      V(MK  )=V(MK  )-VR*F1/(R*(1.0+F1)**2)                             
      V(MK+1)=V(MK+1)-VI*F3/(R*(1.0+F3)**2)
      U(MK+1)=U(MK+1)-WS*F3*4/(1.0+F3)**2
      U(MK  )=U(MK  )-WR*F1*4/(1.0+F1)**2
  460 CONTINUE                                                          
      GO TO 2000                                                        
C                                                                       
C     L.S 1ST DER WOODS SAXON                                           
C                                                                       
  500 CONTINUE                                                          
      IBF(4)=1                                                          
      IF(AR.NE.0.0) VR=VR/AR                                            
      IF(AI.NE.0.0) VI=VI/AI                                            
      R=0.0                                                             
      DO 560 M=1,K                                                      
      R=R+DR                                                            
      MK=M+M-1                                                          
      F1=F1*F2                                                          
      F3=F3*F4                                                          
      V(MK  )=V(MK  )+VR*F1*(1.0-F1)/(R*(1.0+F1)**3)                    
      V(MK+1)=V(MK+1)+VI*F3*(1.0-F3)/(R*(1.0+F3)**3)
      U(MK+1)=U(MK+1)-WS*F3*4/(1.0+F3)**2
      U(MK  )=U(MK  )-WR*F1*4/(1.0+F1)**2
  560 CONTINUE                                                          
      GO TO 2000                                                        
C                                                                       
C     WOOD SAXON*R**PWR                                                 
C                                                                       
  600 CONTINUE                                                          
      R=0.0                                                             
      DO 660 M=1,K                                                      
      R=R+DR                                                            
      MK=M+M-1                                                          
      F1=F1*F2                                                          
      F3=F3*F4                                                          
      RPWR=R**PWR                                                       
      U(MK  )=U(MK  )-VR*F1*RPWR/(1.0+F1)                               
      U(MK+1)=U(MK+1)-VI*F3*RPWR/(1.0+F3)
      U(MK+1)=U(MK+1)-WS*F3*4/(1.0+F3)**2
      U(MK  )=U(MK  )-WR*F1*4/(1.0+F1)**2
  660 CONTINUE                                                          
      GO TO 2000                                                        
C                                                                       
C      1ST DER WOOD SAXON*R**PWR                                        
C                                                                       
  700 CONTINUE                                                          
      R=0.0                                                             
      DO 760 M=1,K                                                      
      R=R+DR                                                            
      MK=M+M-1                                                          
      F1=F1*F2                                                          
      F3=F3*F4                                                          
      RPWR=R**PWR                                                       
      U(MK  )=U(MK  )+VR*F1*RPWR/(1.0+F1)**2                            
      U(MK+1)=U(MK+1)+VI*F3*RPWR/(1.0+F3)**2
      U(MK+1)=U(MK+1)-WS*F3*4/(1.0+F3)**2
      U(MK  )=U(MK  )-WR*F1*4/(1.0+F1)**2
  760 CONTINUE                                                          
      GO TO 2000                                                        
C                                                                       
C     EXTERNAL FORM FACTOR                                              
C                                                                       
  800 CONTINUE                                                          
      READ(5,9000)F1,F2                                                 
      F3=VR                                                             
      IF(F2.NE.0.0) F3=VI                                               
      IF(F3.EQ.0.0) F3=1.0                                              
      KT=F1                                                             
      MK=F2+1.0                                                         
      DO 860 M=1,KT,5                                                   
      READ(5,9100)UT                                                    
      DO 850 I=1,5                                                      
      U(MK  )=U(MK  )+UT(I)*F3                                          
      MK=MK+2                                                           
  850 CONTINUE                                                          
  860 CONTINUE                                                          
      GO TO 2000                                                        
C                                                                       
C     HARMONIC OSCILLATOR                                               
C                                                                       
  900 CONTINUE                                                          
      READ (5,9000)FN,FL,FJ2,FSS                                        
      WRITE(6,9508)FN,FL,FJ2,FSS                                        
      F1=1.0/RY**2                                                      
      F2=F1/RY                                                          
      F3=0.5                                                            
      F4=SQRPI*0.5                                                      
      L=FL                                                              
      IF(L.EQ.0) GO TO 931                                              
      DO 930 I=1,L                                                      
      F3=F3+1.0                                                         
      F4=F4*F3                                                          
      F2=F2*F1                                                          
  930 CONTINUE                                                          
  931 CONTINUE                                                          
      NN=FN                                                             
      T1=1.0                                                            
      T2=F4                                                             
      CN(1)=(-1.0)**NN                                                  
      IF(NN.EQ.0) GO TO 941                                             
      DO 940 I=1,NN                                                     
      F3=F3+1.0                                                         
      T1=T1*DFLOAT(I)                                                   
      T2=T2*F3                                                          
      CN(I+1)=-CN(I)*F1*DFLOAT(NN+1-I)/(DFLOAT(I)*F3)                   
  940 CONTINUE                                                          
  941 CONTINUE                                                          
      T2=DSQRT(2.0*F2*T2/T1)/F4                                         
      IF(VR.NE.0.0) T2=T2*VR                                            
      KT=10.0*RY/DR                                                     
      KT=MIN0(KT,K)                                                     
      R=0.0                                                             
      F1=F1/2.0                                                         
      DO 960 M=1,KT                                                     
      MK=M+M-1                                                          
      R=R+DR                                                            
      R2=R**2                                                           
      F2=CN(1)                                                          
      IF(NN.EQ.0) GO TO 951                                             
      F3=1.0                                                            
      DO 950 I=1,NN                                                     
      F3=F3*R2                                                          
      F2=F2+CN(I+1)*F3                                                  
  950 CONTINUE                                                          
  951 CONTINUE                                                          
      U(MK  )=U(MK  )+F2*T2*DEXP(-F1*R2)*R**L                           
  960 CONTINUE                                                          
      GO TO 2000                                                        
C                                                                       
C     DEXP(-(R/R0)**2)*R**POWR                                          
C                                                                       
 1000 CONTINUE                                                          
      IF(VR.EQ.0.0) GO TO 1061                                          
      R=0.0                                                             
      DO 1060 M=1,K                                                     
      MK=M+M-1                                                          
      R=R+DR                                                            
      U(MK  )=U(MK  )-VR*DEXP(-(R/RY)**2)*R**PWR                        
 1060 CONTINUE                                                          
 1061 CONTINUE                                                          
      IF(VI.EQ.0.0) GO TO 1071                                          
      R=0.0                                                             
      DO 1070 M=1,K                                                     
      MK=M+M-1                                                          
      R=R+DR                                                            
      U(MK+1)=U(MK+1)-VI*DEXP(-(R/RZ)**2)*R**PWR                        
 1070 CONTINUE                                                          
 1071 CONTINUE                                                          
      GO TO 2000                                                        
C                                                                       
C     DEFORMED VOLUME OR SURFACE BY YLM DEXPANSION                      
C                                                                       
 1100 CONTINUE                                                          
      READ (5,9000) (ABETA(J),FLDF(J), J=1,3)                           
      WRITE(6,9512) (ABETA(J),FLDF(J), J=1,3)                           
      LMAX=LAM+1                                                        
      DO 1101 J=1,3                                                     
      LDFRM(J)=FLDF(J)                                                  
      LMAX=MAX0(LMAX,LDFRM(J)+1)                                        
 1101 CONTINUE                                                          
      T2=(-1.0)**LAM                                                    
C                                                                       
      DO 1108 I=1,8                                                     
      CN(I  )=0.0                                                       
      CN(I+8)=0.0                                                       
      P2=0.0                                                            
      P3=1.0                                                            
C                                                                       
      DO 1106 M=1,LMAX                                                  
      L=M-1                                                             
      FL=L-1                                                            
      IF(L.EQ.0) GO TO 1102                                             
      P3=((2.0*FL+1.0)*X(I)*P2-FL*P1)/(FL+1.0)                          
 1102 CONTINUE                                                          
      DO 1103 J=1,3                                                     
      IF(ABETA(J).EQ.0.0) GO TO 1103                                    
      IF(L.NE.LDFRM(J)) GO TO 1103                                      
      FACTOR=P3*ABETA(J)*DSQRT(DFLOAT(L+L+1))/(SQRPI*2.0)               
      CN(I  )=CN(I  )+FACTOR                                            
      CN(I+8)=CN(I+8)+FACTOR*(-1.0)**LDFRM(J)                           
 1103 CONTINUE                                                          
      IF(L.NE.LAM) GO TO 1104                                           
      YLAM(I  )= P3*W(I)*DSQRT(DFLOAT(L+L+1))*SQRPI                     
      YLAM(I+8)=YLAM(I  )*T2                                            
 1104 CONTINUE                                                          
      P1=P2                                                             
      P2=P3                                                             
 1106 CONTINUE                                                          
 1108 CONTINUE                                                          
      DO 1170 I=1,2                                                     
      GO TO (1110,1120),I                                               
 1110 CONTINUE                                                          
      IF(VR.EQ.0.0) GO TO 1170                                          
      VX=VR                                                             
      RX=RR                                                             
      AX=AR                                                             
      F1=1.0                                                            
      IFL=-1                                                            
      GO TO 1130                                                        
 1120 CONTINUE                                                          
      IF(VI.EQ.0.0) GO TO 1170                                          
      VX=VI                                                             
      RX=RI                                                             
      AX=AI                                                             
      F1=1.0                                                            
      F2=F4                                                             
      IFL=0                                                             
 1130 CONTINUE                                                          
      DO 1135 J=1,16                                                    
      CP(J)=DEXP((1.0+CN(J))*RX/AX)                                     
 1135 CONTINUE                                                          
      IF(LAM.EQ.0) VX=VX/(SQRPI*2.0)                                    
      J=NX-10                                                           
      GO TO (1141,1151),J                                               
 1141 CONTINUE                                                          
      DO 1150 M=1,K                                                     
      MK=M+M+IFL                                                        
      VTEMP=0.0                                                         
      F1=F1*F2                                                          
      DO 1145 J=1,16                                                    
      F3=F1*CP(J)                                                       
      VTEMP=VTEMP-YLAM(J)*VX*F3/(1.0+F3)                                
 1145 CONTINUE                                                          
      U(MK  )=U(MK  )+VTEMP                                             
 1150 CONTINUE                                                          
      GO TO 1170                                                        
 1151 CONTINUE                                                          
      DO 1160 M=1,K                                                     
      MK=M+M+IFL                                                        
      VTEMP=0.0                                                         
      F1=F1*F2                                                          
      DO 1155 J=1,16                                                    
      F3=F1*CP(J)                                                       
      VTEMP=VTEMP+YLAM(J)*VX*F3/(1.0+F3)**2                             
 1155 CONTINUE                                                          
      U(MK  )=U(MK  )+VTEMP                                             
 1160 CONTINUE                                                          
 1170 CONTINUE                                                          
      GO TO 2000                                                        
 1200 CONTINUE                                                          
      GOTO 1100                                                         
 2000 CONTINUE                                                          
      IF(KFLAG.NE.0) GO TO 2009                                         
      IF(DABS(VSOR)+DABS(VSOI).EQ.0.0) GO TO 2009                       
      NX=NX+3                                                           
      IF(NX.EQ.17) GO TO 2008                                           
      IF(NX.GT.5) GO TO 2009                                            
 2008 CONTINUE                                                          
      KFLAG=1                                                           
      VR=VR*VSOR/45.2                                                   
      VI=VI*VSOI/45.2                                                   
      GO TO 83                                                          
 2009 CONTINUE                                                          
      IF(FZ.GT.0.0) GO TO 70                                            
      RETURN                                                            
C
 9000 FORMAT(12F7.4)                                                    
 9100 FORMAT(5E14.6)                                                    
 9508 FORMAT(18X,9H   NODES=,F9.4,9H   L    =,F9.4,9H   2*J  =,F9.4,
     >  9H   2*S  =,F9.4,9H   FISW =,F9.4)                                    
 9509 FORMAT(A18,9H   V RL =,F9.4,9H   R0RL =,F9.4,9H   A RL =,F9.4,
     >  9H   R RL =,F9.4,9H   VSOR =,F9.4)                                    
 9510 FORMAT(18X,9H   V IM =,F9.4,9H   R0IM =,F9.4,9H   A IM =,F9.4,9H  
     1 R IM =,F9.4,9H   VSOI =,F9.4,9H   POWR =,F9.4)                   
 9512 FORMAT(18X,9H   BETA1=,F9.4,9H   LDFR1=,F9.4,9H   BETA2=,F9.4,9H  
     1 LDFR2=,F9.4,9H   BETA3=,F9.4,9H   LDFR3=,F9.4)                   
      END                                                               
