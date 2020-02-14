C *
C *   DENSITY.FOR
C *
C *
C *      SUBROUTINE DENSITY( J,FJF,FJI,PAR, TF,TFM,TI,TIM, 
C *     >                    NDEN,ALP, IDF,IDI,JX,SX,
C *     >                    DENFT,NTYPE,FILEN, IPRTR,IPRTQ )
C *
C *      SUBROUTINE SETRNL( FILEN ) 
C *
C *      SUBROUTINE GETRNL( N, L, J, KTZ, ALP, RNL ) 
C *
C *      SUBROUTINE MOMENT (FJI,FJF,PAR,J,NTYPE)  
C *
C *      BLOCK DATA   
C *
C *
      SUBROUTINE DENSITY( J,FJF,FJI,PAR, TF,TFM,TI,TIM, 
     >                    NDEN,ALP, IDF,IDI,JX,SX,
     >                    DENFT,NTYPE,FILEN, IPRTR,IPRTQ )
C                                                                       
C          CALCULATES TRANSITION DENSITIES                                   
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /MMNT/ DEN(7,2,150)                                        
C
      COMMON /COOK/ H,NR,KEXCH                                          
      COMMON /ID/ NID(45),LID(45),JID(45)                               
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      CHARACTER*(*) FILEN
C
      DIMENSION IDF(100),IDI(100),JX(100),SX(2,100),DENFT(7,2,50)       
      DIMENSION SUM(150),F(150), RNL1(150),RNL2(150), UU(150)  
      DIMENSION DNORM(7)
C
      DATA PI /3.14159265358979D0/                                      
C
      DO 3 I1=1,7                                   
      DO 3 I2=1,2                                   
        DO 1 I3=1,50               
          DENFT(I1,I2,I3)=0.D0                          
    1   CONTINUE
        DO 2 I3=1,150                       
          DEN(I1,I2,I3)=0.D0  
    2   CONTINUE
    3 CONTINUE 
C
C        Get WS radial shapes from file if required
C
      IF( ALP .EQ. 0. ) THEN
        CALL SETRNL( FILEN )
      ENDIF
C
      DQ=PI/(H*DFLOAT(NR))                                              
      WRITE(6,801) J       
      IF( IPRTR .NE. 0 ) WRITE(6,802)        
C                                                
C          LOOP OVER T     ----------------------------------------  
C                                                                       
      DO 290 KT1=1,2                                                    
      KT=KT1-1                                                          
      T=DFLOAT(KT)                                                      
      MTMAX=2*KT+1                                                      
      DO 200 MMT=1,MTMAX                            
        IF(KT.EQ.0) MT=0                          
        IF(KT.EQ.1) MT=MMT-2                     
        FMT=DFLOAT(MT)                           
        TFACT=CG(TI,TIM,T,FMT,TF,TFM)            
        IF( TFACT .NE. 0.D0 ) GO TO 210        
  200 CONTINUE                   
      GOTO 290                                                          
C                FMT = TFM-TIM in reality, so this is just junk
  210 CONTINUE     
C
C          Read in the norms for this T=KT set of densities
C
      READ(5,811,ERR=204) DNORM
      GOTO 206
  204 WRITE(6,205)
  205 FORMAT(' ERROR READING NORMALIZATIONS.  ASSUME THEY ARE ALL 1.')
      DO I1 = 1, 7
	DNORM(I1) = 1.0
      ENDDO
  206 CONTINUE
C                                                                       
C          LOOP OVER S     ----------------------------------------
C                                                                       
      DO 280 KS1=1,2                                                    
      KS=KS1-1                                                          
      LMIN=IABS(J-KS)+1                                                 
      LMAX=J+KS+1                                                       
C                                  
C          LOOP OVER L     ----------------------------------------
C                                          
      DO 270 LL1=LMIN,LMAX                                              
      L=LL1-1                                                           
      IF( PHAS(L) .NE. PAR ) GO TO 270        
C                                                    
C          FORM TRANSITION DENSITY                                           
C                                                                       
      DO 220 I=1,150                                                    
        SUM(I)=0.D0                  
  220 F(I)=0.D0                                                         
      DO 240 N=1,NDEN                                                   
      IF( JX(N) .NE. J ) GO TO 240     
      N1=NID(IDF(N))
      L1=LID(IDF(N))
      J1=JID(IDF(N))
      N2=NID(IDI(N))
      L2=LID(IDI(N))
      J2=JID(IDI(N))
C                                 define SPP SNN here
      IF( FMT .EQ. 0. ) THEN
        SPP = (SX(1,N)-SX(2,N))/2.
        SNN = (SX(1,N)+SX(2,N))/2.
        SPN = 0.
        SNP = 0.
      ELSE IF( FMT .EQ. -1. ) THEN
        SPP = 0.
        SNN = 0.
        SPN = SX(2,N)
        SNP = 0.
      ELSE IF( FMT .EQ. +1. ) THEN
        SPP = 0.
        SNN = 0.
        SPN = 0.
        SNP = SX(2,N)
      ELSE
        STOP ' -- ERROR WITH FMT IN DENSITY '
      ENDIF
      IF( L.LT.IABS(L1-L2) .OR. L.GT.(L1+L2) ) GO TO 240  
      CALL GL(L,L1,L2,GLANS)                                            
      CALL GS(L,KS,J,L1,J1,L2,J2,GSANS)                            
      IF(KS.EQ.1) CALL HS(L,J,L1,J1,L2,J2,HSANS)                      
      KTP = -1
      KTN = +1
      DO 230 I=1,150
        UU(I) = 0.
  230 CONTINUE
C
C          will use UU(I) for S*RNL1*RNL2
C
      IF( SPP .NE. 0. ) THEN
        CALL GETRNL( N1,L1,J1, KTP, ALP, RNL1 )
        CALL GETRNL( N2,L2,J2, KTP, ALP, RNL2 )
        DO 231 I=1,150
          UU(I) = UU(I) + (1-2*KT)*SPP*RNL1(I)*RNL2(I)
  231   CONTINUE
      ENDIF
      IF( SNN .NE. 0. ) THEN
        CALL GETRNL( N1,L1,J1, KTN, ALP, RNL1 )
        CALL GETRNL( N2,L2,J2, KTN, ALP, RNL2 )
        DO 233 I=1,150
          UU(I) = UU(I) + SNN*RNL1(I)*RNL2(I)
  233   CONTINUE
      ENDIF
      IF( SPN .NE. 0. ) THEN
        CALL GETRNL( N1,L1,J1, KTP, ALP, RNL1 )
        CALL GETRNL( N2,L2,J2, KTN, ALP, RNL2 )
        DO 235 I=1,150
          UU(I) = UU(I) + SPN*RNL1(I)*RNL2(I)
C          write(*,*)'SPN',SPN
  235   CONTINUE
      ENDIF
      IF( SNP .NE. 0. ) THEN
        CALL GETRNL( N1,L1,J1, KTN, ALP, RNL1 )
        CALL GETRNL( N2,L2,J2, KTP, ALP, RNL2 )
        DO 237 I=1,150
          UU(I) = UU(I) + SNP*RNL1(I)*RNL2(I)
C          write(*,*)'SNP',SNP
  237   CONTINUE
      ENDIF
C
C      WRITE(*,*)GLANS,GSANS,HSANS,KS,KS1,LL1
      DO 239 I=1,150                                                    
      SUM(I) = SUM(I) + GLANS*GSANS*UU(I)  
      IF( KS .NE. 0 ) THEN   
        F(I) = F(I) + GLANS*HSANS*UU(I)
      ENDIF
  239 CONTINUE                                                          
  240 CONTINUE                                                          
C                                Store the result
      IF( KS .EQ. 0 ) IDENT=1                             
      IF( KS .EQ. 1 ) IDENT=3+L-J                       
      DO 245 I=1,150               
        SUM(I) = DNORM(IDENT)*SUM(I)    
C        WRITE(*,*)I,GLANS,GSANS,HSANS,KS,KS1,LL1,SUM(I)
        DEN(IDENT,KT1,I) = SUM(I)                 
  245 CONTINUE
      Q=0.D0                                                            
      NXX = 150
      DDX = 0.1D0
      DO 246 K=1,50                                          
        Q=Q+DQ                                     
        CALL TRANSF(L,Q,DDX,NXX,SUM,ANS)             
        DENFT(IDENT,KT1,K) = ANS                       
  246 CONTINUE
C
      IF( KS .EQ. 0 ) GO TO 270                
      IDENT=6+L-J                                                       
      DO 250 I=1,150            
        F(I) = DNORM(IDENT)*F(I)
        DEN(IDENT,KT1,I) = F(I)                    
C        DEN(IDENT,KT1,I) = 0.
C        WRITE(*,*)'F',I,GLANS,GSANS,HSANS,KS,KS1,LL1,F(I)
  250 CONTINUE
      Q=0.D0                                                 
      NXX = 150
      DDX = 0.1D0
      DO 260 K=1,50                                                     
        Q=Q+DQ                         
        CALL TRANSF(L,Q,DDX,NXX,F,ANS)       
        DENFT(IDENT,KT1,K)=ANS                     
C        DENFT(IDENT,KT1,K)=0
  260 CONTINUE
  270 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER L     ------------------------------
C                                                                       
  280 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER S     ------------------------------
C                                                                       
      WRITE(6,821) KT, MT   
      WRITE(6,822) DNORM     
      LL=J-1                                                            
      LU=J+1        
      IF( IPRTR .NE. 0 ) THEN
        WRITE(6,823) J, LL,J,LU, LL,J,LU  
        WRITE(6,824)              
        R=0.D0                                         
        DO 284 I=1,150,1                                        
          R=R+0.1D0                                           
          WRITE(6,825) R, (DEN(IDENT,KT1,I), IDENT=1,7)     
  284   CONTINUE    
      ENDIF
C
      IF( IPRTQ .NE. 0 ) THEN
        WRITE(6,826) 
        WRITE(6,823) J, LL,J,LU, LL,J,LU 
        WRITE(6,824)                    
        Q=0.D0                                                    
        DO 286 I=1,50  
          Q=Q+DQ
          WRITE(6,825) Q, (DENFT(IDENT,KT1,I), IDENT=1,7)   
  286   CONTINUE         
      ENDIF 
  290 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER T     ------------------------------
C                                                                       
C          CALCULATE MOMENTS OF DENSITY                                      
C                                                                       
      CALL MOMENT(FJI,FJF,PAR,J,NTYPE)                                  
      RETURN          
C
  801 FORMAT(/ ' J =',I3 / 1X,6(1H-) /)     
  802 FORMAT(/ ' RADIAL DENSITIES FOLLOW' )   
C
  811 FORMAT( 7F10.5 )
C
  821 FORMAT(/' T =',I3,5X,'MT =',I3 //)        
  822 FORMAT(/'  NORMS = ', 7F12.6 /)
  823 FORMAT(/ 5X,1HL,4X,7(5X,I3,4X))        
  824 FORMAT(9H OPERATOR,8X,1H1,4X,3(5X,5HSIGMA,2X),3(7X,1HL,4X)/) 
  825 FORMAT(F10.3,7E12.3)     
  826 FORMAT(// 5X, 'MOMENTUM SPACE DENSITIES USED IN FOLDING:' /) 
C
      END                                                               
      SUBROUTINE SETRNL( FILEN )
C
C          This routine reads the radial densities from file
C          and stores them for recovery by GETRNL as needed.
C
C          N starts at zero, J is twice total angular momentum,
C          and MZ is twice isospin (+1 is neutron, -1 is proton).
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      COMMON /RNLSTUF/ NN(50),LL(50),JJ(50),MZ(50), RRR(150,50), NUMB 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      CHARACTER*(*) FILEN
C 
      OPEN( UNIT=7, FILE=FILEN, STATUS='OLD', 
     >                          FORM='FORMATTED', ERR=901 )
      INDEX = 0
  100 READ(7,801,ERR=902) N, L, J, M
c      WRITE(*,*)N,L,J,M
      IF( N .LT. 0 ) GO TO 200
      INDEX = INDEX + 1
      NN(INDEX) = N
      LL(INDEX) = L
      JJ(INDEX) = J
      MZ(INDEX) = M
      READ(7,802,ERR=903) (RRR(I,INDEX), I=1,150)
      GO TO 100
C
  200 CLOSE( UNIT=7 )
      NUMB = INDEX
      RETURN 
C
  901 WRITE(6,891)
      STOP ' -- ERROR IN SETRNL '
  902 WRITE(6,892) INDEX
      STOP ' -- ERROR IN SETRNL FILE '
  903 WRITE(6,893) INDEX
      STOP ' -- ERROR IN SETRNL FILE '
C
  801 FORMAT( 4I5 )
  802 FORMAT( 5E16.8 )
C
  891 FORMAT(// 5X, 'ERROR IN OPEN STATEMENT IN SETRNL' //)
  892 FORMAT(// 5X, 'ERROR READING N,L,J,MZ CARD FOR INDEX = ', 
     >  I5, ' IN SETRNL ' //)
  893 FORMAT(// 5X, 'ERROR READING RADIAL FUNCTION FOR INDEX = ',
     >  I5, ' IN SETRNL ' //)
      END
      SUBROUTINE GETRNL( N, L, J, KTZ, ALP, RNL )
C
C          Obtain the particular RNL requested.  ALP = 0 means
C          that we are to get the radial function from a file
C          for given N, L, J, KTZ -- normally a W-S wavefunction.
C
C             N starts at 0, and is number of interior nodes.
C
C             J is twice the total angular momentum
C
C             KTZ is twice the isospin of the particle
C
C             ( J and KTZ ignored for H.O. radial function )
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      COMMON /RNLSTUF/ NN(50),LL(50),JJ(50),MZ(50), RRR(150,50), NUMB 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION RNL(150)
C
      IF( ALP .NE. 0. ) THEN 
        ALPSQ = ALP*ALP
        ALPSQ2 = ALPSQ / 2.
        R=0.D0           
        DO 100 I = 1, 150  
        R=R+0.1D0      
        RNL(I) = ANORM(N,L,ALPSQ) * AUGER(N,L,ALPSQ*R*R) 
     >          * DEXP(-ALPSQ2*R*R) * R**L  
  100   CONTINUE
      ELSE
C
C          search on MZ, then JJ and LL, then NN to find INDEX
C
        DO 200 I = 1, NUMB
        IF( KTZ .NE. MZ(I) ) GO TO 200
        IF(  J  .NE. JJ(I) ) GO TO 200
        IF(  L  .NE. LL(I) ) GO TO 200
        IF(  N  .EQ. NN(I) ) THEN
          INDEX = I
          GO TO 201
        ENDIF
  200   CONTINUE
        WRITE(6,890) N, L, J, KTZ
        STOP ' -- FAILURE IN GETRNL '
  201   DO 250 I = 1, 150
        RNL(I) = RRR(I,INDEX)
  250   CONTINUE
      ENDIF
C
      RETURN 
C
  890 FORMAT(// 5X, ' ERROR IN GETRNL ' //
     >  5X, ' UNABLE TO MATCH N, L, J, KTZ VALUES REQUESTED ' //
     >  10X, 'N = ', I5, ', L = ', I5, ', J = ', I5, ', KTZ = ',
     >  I5 // 5X, ' CHECK FILE AND Z-COEFFICIENTS ' //)
      END
      SUBROUTINE MOMENT (FJI,FJF,PAR,J,NTYPE)                           
C                                                                       
C          CALCULATE MOMENTS OF DENSITY                                      
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /MMNT/ DEN(7,2,150)                                        
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION EMGL(2),EMGS(2)                                         
      DATA N,H,PI,FNM,EMGLN,EMGLP,EMGSN,EMGSP /150,0.1D0,               
     *     3.141592653585979D0,0.1051545D0,0.D0,1.D0,-3.826D0,5.585D0/  
C
      EMGL(1)=0.5D0*(EMGLN+EMGLP)                                       
      EMGL(2)=0.5D0*(EMGLN-EMGLP)                                       
      EMGS(1)=0.5D0*(EMGSN+EMGSP)                                       
      EMGS(2)=0.5D0*(EMGSN-EMGSP)                                       
      FJ=DFLOAT(J)                                                      
      J2=2*J                                                            
      F1=2.D0*EMGL(1)/DFLOAT(J+1)                                       
      F2=2.D0*EMGL(2)/DFLOAT(J+1)                                       
      F3=0.5D0*EMGS(1)                                                  
      F4=0.5D0*EMGS(2)                                                  
      GOTO(100,200,300),NTYPE                                           
C                                                                       
C          CALCULATES STATIC MOMENTS                                         
C                                                                       
  100 IF(J.GT.0) GOTO 130                                               
C                                                                       
C     VOLUME INTEGRALS AND RMS RADII                                    
C                                                                       
      R=0.D0                                                            
      VOL1=0.D0                                                         
      VOL2=0.D0                                                         
      VOL3=0.D0                                                         
      DO 110 I=1,N                                                      
      R=R+H                                                             
      R2=R*R                                                            
      R4=R2*R2                                                          
      VOL1=VOL1+DEN(1,1,I)*R2                                           
      VOL2=VOL2+DEN(1,2,I)*R2                                           
  110 VOL3=VOL3+DEN(1,1,I)*R4                                           
      VOL3=VOL3/VOL1                                                    
      VOL3CH=VOL3+0.76D0                                                
      VOL3=DSQRT(VOL3)                                                  
      VOL3CH=DSQRT(VOL3CH)                                              
      VOL1=4.D0*PI*H*VOL1                                               
      VOL2=4.D0*PI*H*VOL2                                               
      WRITE(6,120) VOL1,VOL2,VOL3,VOL3CH                                
  120 FORMAT(/' T = 0 VOLUME INTEGRAL = ',F10.5/                        
     *        ' T = 1 VOLUME INTEGRAL = ',F10.5/                        
     *        ' RMS MATTER RADIUS     = ',F10.5/                        
     *        ' RMS CHARGE RADIUS     = ',F10.5/)                       
      RETURN                                                            
  130 IF(PHAS(J).NE.1.D0) GOTO 170                                      
C                                                                       
C     ELECTRIC STATIC MOMENTS                                           
C                                                                       
      R=0.D0                                                            
      F=0.D0                                                            
      DO 140 I=1,N                                                      
      R=R+H                                                             
      RJ2=R**(J+2)                                                      
  140 F=F+(EMGL(1)*DEN(1,1,I)+EMGL(2)*DEN(1,2,I))*RJ2                   
      F=PHAS(-J/2)*DSQRT(4.D0*PI/(2.D0*FJ+1.D0))*                       
     *  CG(FJI,FJI,FJ,0.D0,FJI,FJI)*H*F                                 
      WRITE(6,150)J,F                                                   
  150 FORMAT(/' ELECTRIC 2**',I3,' MOMENT',F10.5/)                      
      IF(J.EQ.2) F=2.D0*F                                               
      IF(J.LE.4) WRITE(6,160)F                                          
  160 FORMAT(' .....WITH CONVENTIONAL NORMALISATION',F10.5/)            
      RETURN                                                            
C                                                                       
C     MAGNETIC STATIC MOMENTS                                           
C                                                                       
  170 R=0.D0                                                            
      F=0.D0                                                            
      DO 180 I=1,N                                                      
      R=R+H                                                             
      RJ1=R**(J+1)                                                      
  180 F=F+(F1*DEN(5,1,I)+F2*DEN(5,2,I)+F3*DEN(2,1,I)+F4*DEN(2,2,I))*RJ1 
      F=PHAS((1-J)/2)*DSQRT(4.D0*PI*FJ)*CG(FJI,FJI,FJ,0.D0,FJI,FJI)*H*F 
      WRITE(6,190)J,F                                                   
  190 FORMAT(/' MAGNETIC 2**',I3,' MOMENT',F10.5/)                      
      IF(J.EQ.3) F=-F                                                   
      IF(J.LE.3) WRITE(6,160)F                                          
      RETURN                                                            
C                                                                       
C          CALCULATE REDUCED EM TRANSITION PROBABILITIES                     
C                                                                       
  200 IF(PHAS(J).NE.PAR) GOTO 230                                       
C                                                                       
C     ELECTRIC TRANSITIONS                                              
C                                                                       
      R=0.D0                                                            
      F=0.D0                                                            
      DO 210 I=1,N                                                      
      R=R+H                                                             
      RJ2=R**(J+2)                                                      
  210 F=F+(EMGL(1)*DEN(1,1,I)+EMGL(2)*DEN(1,2,I))*RJ2                   
      F=(2.D0*FJF+1.D0)/(2.D0*FJI+1.D0)*(H*F)**2                        
      WRITE(6,220)J,F,J2                                                
  220 FORMAT(/' B(E',I2,') = ',F10.5,' E**2 FM**',I2/)                  
      RETURN                                                            
C                                                                       
C     MAGNETIC TRANSITIONS                                              
C                                                                       
  230 R=0.D0                                                            
      F=0.D0                                                            
      DO 240 I=1,N                                                      
      R=R+H                                                             
      RJ1=R**(J+1)                                                      
  240 F=F+(F1*DEN(5,1,I)+F2*DEN(5,2,I)+F3*DEN(2,1,I)+F4*DEN(2,2,I))*RJ1 
      F=FJ*(2.D0*FJ+1.D0)*(2.D0*FJF+1.D0)/(2.D0*FJI+1.D0)*(H*F)**2      
      J2M2=J2-2                                                         
      WRITE(6,250)J,F,J2M2                                              
  250 FORMAT(/' B(M',I2,') = ',F10.5,' NM**2 FM**',I2/)                 
      F=FNM**2*F                                                        
      WRITE(6,260)J,F,J2                                                
  260 FORMAT(/' B(M',I2,') = ',F10.5,' E**2 FM**2',I2/)                 
      RETURN                                                            
C                                                                       
C          CALCULATE BETA DECAY MATRIX ELEMENTS                              
C                                                                       
  300 IF(PAR.NE.1.D0) RETURN                                            
      F=0.D0                                                            
      GT=0.D0                                                           
      IF(J.NE.0) GOTO 320                                               
      R=0.D0                                                            
      DO 310 I=1,N                                                      
      R=R+H                                                             
  310 F=F+DEN(1,2,I)*R*R                                                
      F=2.D0*PI*(H*F)**2                                                
  320 IF(J.GT.1) RETURN                                                 
      IF(FJI.EQ.0.D0.AND.FJF.EQ.0.D0) GOTO 340                          
      R=0.D0                                                            
      DO 330 I=1,N                                                      
      R=R+H                                                             
  330 GT=GT+DEN(2,2,I)*R*R                                              
      GT=2.D0*PI*(2.D0*FJF+1.D0)/(2.D0*FJI+1.D0)*(H*GT)**2              
  340 WRITE(6,350) F,GT                                                 
  350 FORMAT(/' FERMI BETA DECAY MATRIX ELEMENT        =',1PE10.3/      
     *        ' GAMOW-TELLER BETA DECAY MATRIX ELEMENT =',1PE10.3/)     
      RETURN                                                            
      END                                                               
      BLOCK DATA                                                        
C
C          Define mapping between state identifies and the 
C          shell-model level quantum numbers.
C
C ---------------------------------------------------------------------
C
      COMMON /ID/ NID(45),LID(45),JID(45)                               
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DATA NID /0,0,0,1,0,0,1,1,0,0,2,1,1,0,0,2,2,1,1,0,0,3,2,2,1,1,0,0,
     *  3,3,2,2,1,1,0,0,4,3,3,2,2,1,1,0,0/                              
      DATA LID /0,1,1,0,2,2,1,1,3,3,0,2,2,4,4,1,1,3,3,5,5,0,2,2,4,4,6,6,
     *  1,1,3,3,5,5,7,7,0,2,2,4,4,6,6,8,8/                              
      DATA JID /1,1,3,1,3,5,1,3,5,7,1,3,5,7,9,1,3,5,7,9,11,1,3,5,7,9,11,
     *  13,1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,17/                    
      END                                                               
