      SUBROUTINE READT( FRCEFILE, FNRM1,FNRM2,FNRM3, WKLAB, KEXCH ) 
C 
C          Reads Yukawa expansion parameters of FREE T-matrix 
C 
C                  UP TO MNB RANGES ARE ALLOWED 
C 
C          Assumes input is arranged in the format of LOVE'S tables 
C 
C                either --  R, SE, TE, SO, TO  (Central) 
C                  or   --  R, E, O            (others) 
C 
C          Parameters are converted to GERAMB'S format, which is 
C          used for the calculations in INTER 
C
C          NOTE: the order in VNORM must match the order of the 
C                ranges in GCR and GCI.  Normally these go from 
C                small to large in our files.
C
C --------------------------------------------------------------------- 
C 
      IMPLICIT REAL*8 (A-H, O-Z) 
C 
      PARAMETER (MNB  =  5) 
C 
      COMMON /FORCEC / GCR(MNB,5),  GCI(MNB,5),  NCR,  NCI 
      COMMON /FORCELS/ GLSR(MNB,3), GLSI(MNB,3), NLSR, NLSI 
      COMMON /FORCETN/ GTNR(MNB,3), GTNI(MNB,3), NTNR, NTNI 
C
      COMPLEX*16      GC0X,   GC1X
      COMMON /EXCHANG/ GC0X(2),GC1X(2)
C 
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC 
C
      CHARACTER*(*) FRCEFILE
C
      COMPLEX*16 VLS, VTN
C
      DIMENSION VNORM(MNB)
C 
C          INPUT AND ECHO THE PARAMETERS 
C 
      WRITE(6,810) 
C
c      OPEN( UNIT=3, FILE=FRCEFILE, STATUS='OLD', 
c     >      FORM='FORMATTED', READONLY, IOSTAT=ICHK )
      OPEN( UNIT=3, FILE=FRCEFILE, STATUS='OLD', 
     >      FORM='FORMATTED', IOSTAT=ICHK)

C      IF(ICHK.NE.0) THEN
C        OPEN( UNIT=3, FILE='FOLD_LIBRARY:'//FRCEFILE, STATUS='OLD', 
C     >      FORM='FORMATTED', IOSTAT=ICHK )
C      ENDIF
      IF(ICHK.NE.0) STOP 'UNABLE TO FIND FORCE FILE'
C
      WRITE(6,821) 
      READ(3, * ) NCR, NLSR, NTNR 
      WRITE(6,831) NCR, NLSR, NTNR 
      IF( NCR.GT.MNB .OR. NLSR.GT.MNB .OR. NTNR.GT.MNB) THEN 
        WRITE(6,891) 
        STOP ' bad ranges in READT '
      ENDIF 
C 
      IF( NCR .EQ. 0 ) GO TO 10 
      NNIN = IABS(NCR) 
      DO 5 I = 1, NNIN 
      READ(3, * ) GCR(I,1), GCR(I,3), GCR(I,4), 
     >                       GCR(I,2), GCR(I,5) 
    5 CONTINUE 
      WRITE(6,832) ( GCR(I,1), GCR(I,3), GCR(I,4), 
     >                          GCR(I,2), GCR(I,5), I=1,NNIN ) 
C 
   10 IF( NLSR .EQ. 0 ) GO TO 30 
      NNIN = IABS(NLSR) 
      DO 25 I = 1, NNIN 
      READ(3, * ) ( GLSR(I,J), J=1,3) 
   25 CONTINUE 
      WRITE(6,833) (( GLSR(I,J), J=1,3), I=1,NNIN ) 
C 
   30 IF( NTNR .EQ. 0 ) GO TO 50 
      NNIN = IABS(NTNR) 
      DO 35 I = 1, NNIN 
      READ(3, * ) ( GTNR(I,J), J=1,3) 
   35 CONTINUE 
      WRITE(6,834) (( GTNR(I,J), J=1,3), I=1,NNIN ) 
C 
   50 WRITE(6,822) 
      READ(3, * ) NCI, NLSI, NTNI 
      WRITE(6,831) NCI, NLSI, NTNI 
      IF( NCI.GT.MNB .OR. NLSI.GT.MNB  .OR. NTNI.GT.MNB) THEN 
        WRITE(6,891) 
        STOP ' bad ranges in READT '
      ENDIF 
C 
      IF( NCI .EQ. 0 ) GO TO 70 
      NNIN = IABS(NCI) 
      DO 65 I = 1, NNIN 
      READ(3, * ) GCI(I,1), GCI(I,3), GCI(I,4), 
     >                       GCI(I,2), GCI(I,5) 
   65 CONTINUE 
      WRITE(6,832) ( GCI(I,1), GCI(I,3), GCI(I,4), 
     >                          GCI(I,2), GCI(I,5), I=1,NNIN ) 
C 
   70 IF( NLSI .EQ. 0 ) GO TO 90 
      NNIN = IABS(NLSI) 
      DO 85 I = 1,NNIN 
      READ(3, * ) ( GLSI(I,J), J=1,3) 
   85 CONTINUE 
      WRITE(6,833) (( GLSI(I,J), J=1,3), I=1,NNIN ) 
C 
   90 IF( NTNI .EQ. 0 ) GO TO 99 
      NNIN = IABS(NTNI) 
      DO 95 I = 1, NNIN 
      READ(3, * ) ( GTNI(I,J), J=1,3) 
   95 CONTINUE 
      WRITE(6,834) (( GTNI(I,J), J=1,3), I=1,NNIN ) 
C
      CLOSE( UNIT=3 )
C 
C          CONVERT R TO M AND V TO V/M 
C 
   99 N = IABS(NCR) 
      IF(  N .EQ. 0 ) GO TO 101 
      DO 100 I=1,N 
      GCR(I,1) = 1. / GCR(I,1) 
      DO 100 J=2,5 
      GCR(I,J) = GCR(I,J) / GCR(I,1) 
  100 CONTINUE 
  101 N = IABS(NCI) 
      IF( N .EQ. 0 ) GO TO 111 
      DO 110 I=1,N 
      GCI(I,1) = 1. / GCI(I,1) 
      DO 110 J=2,5 
      GCI(I,J) = GCI(I,J) / GCI(I,1) 
  110 CONTINUE 
C 
  111 N = IABS(NLSR) 
      IF( N .EQ. 0 ) GO TO 121 
      DO 120 I=1,N 
      GLSR(I,1) = 1. / GLSR(I,1) 
      DO 120 J=2,3 
      GLSR(I,J) = GLSR(I,J) / GLSR(I,1) 
  120 CONTINUE 
  121 N = IABS(NLSI) 
      IF( N .EQ. 0 ) GO TO 131 
      DO 130 I=1,N 
      GLSI(I,1) = 1. / GLSI(I,1) 
      DO 130 J=2,3 
      GLSI(I,J) = GLSI(I,J) / GLSI(I,1) 
  130 CONTINUE 
C 
  131 N = IABS(NTNR) 
      IF( N .EQ. 0 ) GO TO 141 
      DO 140 I=1,N 
      GTNR(I,1) = 1. / GTNR(I,1) 
      DO 140 J=2,3 
      GTNR(I,J) = GTNR(I,J) / GTNR(I,1) 
  140 CONTINUE 
  141 N = IABS(NTNI) 
      IF( N .EQ. 0 ) GO TO 151 
      DO 150 I=1,N 
      GTNI(I,1) = 1. / GTNI(I,1) 
      DO 150 J=2,3 
      GTNI(I,J) = GTNI(I,J) / GTNI(I,1) 
  150 CONTINUE 
C 
C          compute exchange integrals  
C
  151 IF( KEXCH .EQ. 1 ) THEN
        VNORM(1) = FNRM1
        VNORM(2) = FNRM2
        VNORM(3) = FNRM3
        VNORM(4) = 1.0
        VNORM(5) = 1.0
        CALL INTER( WKLAB, 1, 0, GC0X(1),GC1X(1),VLS,VTN, VNORM )
        CALL INTER( WKLAB, 1, 1, GC0X(2),GC1X(2),VLS,VTN, VNORM )
      ELSE
        GC0X(1) = 0.D0
        GC0X(2) = 0.D0
        GC1X(1) = 0.D0
        GC1X(2) = 0.D0
      ENDIF
      WRITE(6,841) WKLAB, (VNORM(I), I=1,MNB)
      WRITE(6,842) GC0X(1),GC0X(2), GC1X(1),GC1X(2)
C
      RETURN 
C 
  810 FORMAT(// 15X, 33HYUKAWA EXPANSION OF FREE T-MATRIX / ) 
C 
  821 FORMAT(// 10H REAL PART /) 
  822 FORMAT(// 15H IMAGINARY PART /) 
C 
  831 FORMAT( 5X, 5HNCT =,I3, 7H, NLS =,I3, 7H, NTN =,I3 /) 
  832 FORMAT( 5X, 10HCENTRAL   ,5X, F7.3, 4F12.3 ) 
  833 FORMAT( 5X, 10HSPIN-ORBIT,5X, F7.3, 2F12.3 ) 
  834 FORMAT( 5X, 10HTENSOR    ,5X, F7.3, 2F12.3 ) 
C
  841 FORMAT(// ' EXCHANGE FROM WKLAB = ', F8.4, 5X, 'NORMS = ', 
     >  5F8.4 )
  842 FORMAT(// ' DELTA FUNCTION NUMBERS ARE: VC0 = ', 4F10.4 //
     >  29X, 'VC1 = ', 4F10.4 //)
C 
  891 FORMAT(/ 10X, 15HTOO MANY RANGES ) 
      END 
      SUBROUTINE INTER( Q, IEX, KT, VC0,VC1,VLS,VTN, VNORM ) 
C 
C          Returns interaction for given Q and Isospin KT 
C 
C          Calculates exchange combination based on IEX
C
C                  UP TO MNB RANGES ALLOWED 
C 
C ---------------------------------------------------------------------
C 
      IMPLICIT REAL*8 (A-H, O-Z) 
C 
      PARAMETER (MNB  =  5) 
C 
      COMMON /FORCEC / GCR(MNB,5),  GCI(MNB,5),  NCR,  NCI 
      COMMON /FORCELS/ GLSR(MNB,3), GLSI(MNB,3), NLSR, NLSI 
      COMMON /FORCETN/ GTNR(MNB,3), GTNI(MNB,3), NTNR, NTNI 
C 
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC 
C 
      COMPLEX*16 ZERO, ZONE, ZEYE 
C 
      COMPLEX*16 VC0, VC1, VLS, VTN 
      COMPLEX*16 SO, SE, TE, TO, LSE, LSO, TNE, TNO, TNEX, TNOX 
C
      DIMENSION VNORM(MNB)
      REAL QUE
C 
C          STATEMENT FUNCTIONS FOR YUKAWAS 
C 
      FC (A,B,Q) =  4.*PI*A     / (B*B+Q*Q) 
      FLS(A,B,Q) = -4.*PI*A     / (B*B+Q*Q)**2 
      FTN(A,B,Q) = 32.*PI*A*Q*Q / (B*B+Q*Q)**3 
C
      ZERO = (0.D0,0.D0)
      ZONE = (1.D0,0.D0)
      ZEYE = (0.D0,1.D0)
      PI = 3.14159265358979D0
C 
C          Set up the exchange phases.  Note that VNORM should 
C          be identically unity for the case when IEX=0
C
      IF( IEX .EQ. 0 ) THEN
        SIGN = 1.
      ELSE
        SIGN = -1.
      ENDIF
C
      VC0 = ZERO 
      VC1 = ZERO 
      VLS = ZERO 
      VTN = ZERO  
C 
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC 
C 
C          CENTRAL FORCE 
C 
      

      SO = ZERO 
      SE = ZERO 
      TE = ZERO 
      TO = ZERO 
C                                   ...REAL PART 
      N = NCR 
      IF( N .LE. 0 ) GO TO 41 
      DO 40 I = 1, N 
      IF (N.LT.0) THEN
       QUE=1.
      ELSE
       QUE=1.
      ENDIF
      SO = ZONE*FC( GCR(I,2), GCR(I,1), Q )*VNORM(1)*QUE + SO 
      SE = ZONE*FC( GCR(I,3), GCR(I,1), Q )*VNORM(1)*QUE + SE 
      TE = ZONE*FC( GCR(I,4), GCR(I,1), Q )*VNORM(1)*QUE + TE 
      TO = ZONE*FC( GCR(I,5), GCR(I,1), Q )*VNORM(1)*QUE + TO 
c      write(*,*)SO
   40 CONTINUE 
C                                    ...IMAGINARY PART 
   41 N = NCI 
      IF( N .LE. 0) GO TO 90 
      DO 50 I = 1, N 
      IF (N.LT.0) THEN
       QUE=1.
      ELSE
       QUE=1.
      ENDIF
      SO = ZEYE*FC( GCI(I,2), GCI(I,1), Q )*VNORM(1)*QUE + SO 
      SE = ZEYE*FC( GCI(I,3), GCI(I,1), Q )*VNORM(1)*QUE + SE 
      TE = ZEYE*FC( GCI(I,4), GCI(I,1), Q )*VNORM(1)*QUE + TE 
      TO = ZEYE*FC( GCI(I,5), GCI(I,1), Q )*VNORM(1)*QUE + TO 
   50 CONTINUE 
C 
   90 IF( KT .EQ. 0 ) THEN
        VC0 = (SIGN*( SO+9.*TO)+3.*SE+3.*TE) / 16. 
        VC1 = (SIGN*(-SO+3.*TO)-3.*SE+   TE) / 16. 
      ELSE
        VC0 = (SIGN*(-SO+3.*TO)+   SE-3.*TE) / 16. 
        VC1 = (SIGN*( SO+   TO)-   SE-   TE) / 16. 
        
      ENDIF
c      write(*,*)'*****'
c      write(*,*)SIGN,VNORM(1),VC1
C 

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC 
C 
C          SPIN ORBIT FORCE  --  not sure if VNORM belongs here 
C 
      LSE = ZERO 
      LSO = ZERO 
C                                    ...REAL PART 
      N = NLSR 
      IF( N .LE. 0) GO TO 141 
      DO 140 I =1 , N 
      LSE = ZONE*FLS( GLSR(I,2), GLSR(I,1), Q )*VNORM(1) + LSE 
      LSO = ZONE*FLS( GLSR(I,3), GLSR(I,1), Q )*VNORM(1) + LSO 
  140 CONTINUE 
C                                    ...IMAGINARY PART 
  141 N = NLSI 
      IF( N .LE. 0 ) GO TO 190 
      DO 150 I=1,N 
      LSE = ZEYE*FLS( GLSI(I,2), GLSI(I,1), Q )*VNORM(1) + LSE 
      LSO = ZEYE*FLS( GLSI(I,3), GLSI(I,1), Q )*VNORM(1) + LSO 
  150 CONTINUE 
C 
  190 IF( KT .EQ. 0 ) THEN
        VLS = (  SIGN*LSE+3.*LSO ) * Q*Q / 4. 
      ELSE 
        VLS = ( -SIGN*LSE+   LSO ) * Q*Q / 4. 
      ENDIF  
C 
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC 
C 
C          TENSOR FORCE  --  not sure if VNORM belongs here 
C 
      TNE = ZERO 
      TNO = ZERO 
C                                   ...REAL PART 
      N = NTNR 
      IF( N .LE. 0) GO TO 241 
      DO 240 I = 1, N 
      TNE = ZONE*FTN( GTNR(I,2), GTNR(I,1), Q )*VNORM(1)*(1.) + TNE 
      TNO = ZONE*FTN( GTNR(I,3), GTNR(I,1), Q )*VNORM(1)*(1.) + TNO 
  240 CONTINUE 
C                                    ...IMAGINARY PART 
  241 N = NTNI 
      IF( N .LE. 0) GO TO 251 
      DO 250 I = 1, N 
      TNE = ZEYE*FTN( GTNI(I,2), GTNI(I,1), Q )*VNORM(1)*(1.) + TNE 
      TNO = ZEYE*FTN( GTNI(I,3), GTNI(I,1), Q )*VNORM(1)*(1.) + TNO 
  250 CONTINUE 
C 
  251 IF( KT .EQ. 0 ) THEN
        VTN = ( SIGN*(3.*TNO)+TNE ) / 4. 
      ELSE
        VTN = ( SIGN*(   TNO)-TNE ) / 4. 
      ENDIF
C 
      RETURN 
      END 
      REAL*8 FUNCTION VNLNL(NRT,LRT,NRP,LRP,ALP,ALT,LFORCE,KS,KT)       
C                                                                       
C          CALCULATES RADIAL MATRIX ELEMENT OF INTERACTION                   
C
C  >>>  NOTE: not updated to work with INTER in December version !
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /COOK/ H,NR,KEXCH                                          
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION F(150)                                                  
      DATA PI/3.14159265358979D0/                                       
C
      ALPSQ=ALP*ALP                                                     
      ALPSQ2=ALPSQ/2.0                                                  
      ALTSQ=ALT*ALT                                                     
      ALTSQ2=ALTSQ/2.0                                                  
      LF1=LFORCE+1                                                      
      DO 3 I=1,150                                                      
      S=DFLOAT(I)*0.1                                                   
      SS1=ALPSQ*S*S                                                     
      SS2=ALTSQ*S*S                                                     
      RLRP=ANORM(NRP,LRP,ALPSQ)*AUGER(NRP,LRP,SS1)*DEXP(-ALPSQ2*S*S)    
     *    *S**LRP                                                       
      RLRT=ANORM(NRT,LRT,ALTSQ)*AUGER(NRT,LRT,SS2)*DEXP(-ALTSQ2*S*S)    
     *    *S**LRT                                                       
      X1=4.0*S                                                          
      X2=2.5*S                                                          
      X3=0.7*S                                                          
      Y1=DEXP(-X1)/X1                                                   
      Y2=DEXP(-X2)/X2                                                   
      Y3=DEXP(-X3)/X3                                                   
      GOTO(10,20,30),LF1                                                
   10 CONTINUE                                                          
      IF(KS.EQ.0.AND.KT.EQ.0) GEST= 4631.0*Y1-1781.0*Y2-7.848*Y3        
      IF(KS.EQ.0.AND.KT.EQ.1) GEST=-1518.0*Y1+ 828.4*Y2+2.616*Y3        
      IF(KS.EQ.1.AND.KT.EQ.0) GEST=  675.4*Y1+ 131.6*Y2+41.55*Y3        
      IF(KS.EQ.1.AND.KT.EQ.1) GEST=-3789.0*Y1- 827.1*Y2-0.872*Y3        
      GOTO 3                                                            
  20  CONTINUE                                                          
C     SPIN ORBIT FORCE GOES HERE                                        
      GEST = 0.
      GOTO 3                                                            
   30 X3=1.429*S                                                        
      Y3=DEXP(-X3)/X3                                                   
      IF(KT.EQ.0) GEST=(-527.1*Y2-17.32*Y3)*S*S                         
      IF(KT.EQ.1) GEST=( 244.1*Y2+3.697*Y3)*S*S                         
      IF(KS.EQ.0) GEST=0.0                                              
      IF(KS.EQ.1) GEST=DSQRT(2.D0)*GEST                                 
    3 F(I)=GEST*RLRP*RLRT*S*S                                           
      VNLNL=FNTGRL(150,0.1,F)                                           
      RETURN                                                            
      END                                                               
