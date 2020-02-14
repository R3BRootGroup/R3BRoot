      PROGRAM WSAW 
C 
C          WOODS-SAXON WAVE FUNCTION  --  MODIFIED FOR ALLWRLD 
C
C          Original code calculated collective model form factors,
C          but this has been stripped out so only the Rnl(r) are
C          produced for use by a CNUU-like subroutine.
C
C                                        VERSION -- 26 April 1988
C
C          Notice that we require that Rnl(r) be normed to unity
C          in the sense that an integral over r**2 dr of R**2
C          must give one.  We also require that it be positive
C          near the origin so it goes as (-)**n as r goes to 
C          infinity.  The value of n starts at zero, so that n
C          counts the number of interior nodes in R.  I believe
C          that the original code meets all of these conditions. 
C
C          Converted to run at MSU NSCL -- 29 August 1989
C
C ---------------------------------------------------------------------
C 
      COMMON C,ETA,R,XC,BARJ,BARL,BARS 
C
      COMMON /PART/ EBIND,MP,L,NODE,IZP,XJ,XS 
      COMMON /COR / MC,IZC,V0,A,R0,RC,VSO 
      COMMON /RA/ RMESH,RMAX,NNMAX 
C
      CHARACTER*8 FILENAM
C
      DIMENSION U(1000) 
C 
      READ(5,801) RMESH, RMAX, NPUNCH, NBPUNCH, IDBUG 
      WRITE(6,802) RMESH, RMAX, NPUNCH, NBPUNCH, IDBUG
C
      IF( NPUNCH .GT. 0 ) THEN
        READ(5,803) FILENAM
        WRITE(6,804) FILENAM
        OPEN( UNIT=7, FILE=FILENAM, FORM='FORMATTED',
     >        STATUS='new' )
      ENDIF
C
      INDEX = 0
C
C          Major loop in program
C
  200 NU=1 
      INDEX = INDEX + 1
C 
      READ(5,811) TMC,TIZC, V0,A,R0,RC, VSO 
      IF( TMC .LE. 0. ) GO TO 300 
      WRITE(6,812) INDEX  
      WRITE(6,813) RMESH, RMAX 
C
      READ(5,811) EBIND, TMP, TL, TNODE, TIZP, XJ, XS 
      TL1=TL 
      WRITE(6,814) EBIND, TMP, TL, TNODE, TIZP, XJ, XS 
      WRITE(6,815) TMC, TIZC, V0, A, R0, RC, VSO 
      WRITE(6,816) TNODE, TL, XJ 
C
      MP=TMP 
      L=TL 
      NODE=TNODE 
      IZP=TIZP 
      MC=TMC 
      IZC=TIZC 
      CALL SAFU( U, VF ) 
      write(*,*)VF
      IF( IDBUG .NE. 0 ) THEN
        WRITE (6,821) (U(I), I=1,250) 
        WRITE (6,822) 
      ENDIF
C
      N4=(MIN0(NNMAX,180)+3)/4 
      DO 4 I=1,N4 
      I1=I 
      I2=N4+I1 
      I3=N4+I2 
      I4=N4+I3 
      X1=I1*RMESH 
      X2=I2*RMESH 
      X3=I3*RMESH 
      X4=I4*RMESH 
      IF( I4 .LE. NNMAX ) THEN  
        WRITE(6,823) X1,U(I1), X2,U(I2), X3,U(I3), X4,U(I4) 
      ELSE    
        WRITE(6,824) X1,U(I1), X2,U(I2), X3,U(I3) 
      ENDIF 
    4 CONTINUE 
C
      CALL RSQARE( U, TL1 ) 
      NN = NODE
      LL = L
      JJ = 2.*XJ + 0.001
      MZ = 1 - 2*IZP
      IF( NPUNCH .NE. 0 ) THEN      
        WRITE(6,832) NN, LL, JJ, MZ, INDEX, FILENAM 
        WRITE(7,833) NN, LL, JJ, MZ
        WRITE(7,834) (U(I), I=1,NBPUNCH) 
      ELSE
        WRITE(6,831) NN, LL, JJ, MZ
      ENDIF    
      GO TO 200 
C
C          Loop back up for next case
C
  300 IF( NPUNCH .GT. 0 ) THEN
        NN = -1
        WRITE(7,833) NN
        CLOSE( UNIT=7, STATUS='KEEP' )
      ENDIF
      STOP 
C
  801 FORMAT(2F10.5,3I5) 
  802 FORMAT('1 START OF PROGRAM' ///// ' INITIAL PARAMETERS ARE:'
     >  // 5X, 'RMESH = ', F10.5, ', RMAX = ', F10.5 
     >  // 5X, 'NPUNCH = ', I5, ', NBPUNCH = ', I5,
     >   ', IDBUG = ', I5 //)
  803 FORMAT( A )
  804 FORMAT(// 10X, 'RADIAL FUNCTIONS WRITTEN TO FILE = ', A /
     >  45X, 8(1H-) //)
C
  811 FORMAT(7F10.0,I2) 
  812 FORMAT('1',14X,'INPUT PARAMETERS FOR INDEX = ', I3) 
  813 FORMAT( //,10X,'RMESH     ',F10.2,10X,'RMAX      ',F10.2) 
  814 FORMAT(// ' PARTICLE' / 
     > 10X,'BIND ENERGY',F10.2,10X,'PART MASS ',F10.0/ 
     > 10X,'ORB ANG MOM',F10.0,10X,'NODE      ',F10.0/ 
     > 10X,'PART CHARGE',F10.0,10X,'TOT ANG MOM',F10.2/ 
     > 10X,'SPIN      ',F10.2) 
  815 FORMAT(///' CORE'/10X,'CORE MASS ',F10.0,10X,'CORE CHARGE',F10.0/ 
     > 10X,'POT DEPTH ',F10.2,10X,'DIFFUSENESS',F10.2/ 
     > 10X,'CORE RADIUS',F10.2,9X,'COUL RADIUS',F10.2/ 
     > 10X,'SPIN ORB POT',F10.2) 
  816 FORMAT(/ 10X,'      NODE',5X,'ORB ANG MOM',4X,'TOT ANG MOM' 
     >  / 10X,F10.0,5X,F10.2,5X,F10.2 /) 
C
  821 FORMAT( 5(5X,E15.4) ) 
  822 FORMAT(1H1,30X,'SAFU'///) 
  823 FORMAT(4(1X,F6.2,3X,E12.5,5X)) 
  824 FORMAT(3(1X,F6.2,3X,E12.5,5X)) 
C
  831 FORMAT(/ 5X, 'N = ', I3, ', L = ', I3, ', J = ', I3, 
     >  '/2, MZ = ', I3 /)
  832 FORMAT(/ 5X, 'N = ', I3, ', L = ', I3, ', J = ', I3, 
     >  '/2, MZ = ', I3, ' WRITTEN WITH INDEX = ', I4,
     >  ' TO FILE = ', A /)
  833 FORMAT(4I5)
  834 FORMAT(5E16.8) 
C
      END 
      SUBROUTINE SAFU( U, VF ) 
C 
C          Subroutine to produce the Rnl(r)
C
      COMMON C,ETA,R,XC,BARJ,BARL,BARS 
C
      COMMON /PART/ EBIND,MP,L,NODE,IZP,XJ,XS 
      COMMON /COR/ MC,IZC,V0,A,R0,RC,VSO 
      COMMON /RA/ RMESH,RMAX,NNMAX 
C
      DIMENSION U(1000) 
C
C     WRITE (6,811) 
      TEST3=10. 
      PM=MP 
      CM=MC 
      ZP=IZP 
      ZC=IZC 
      EL=L 
      MAX=RMAX/RMESH+1. 
      NNMAX=MAX-1 
      MAXM9=MAX-9 
      MAXM10=MAX-10 
      XTEST=0. 
      YTEST=0. 
      MTEST=1 
      R=R0*CM**0.33333/A 
      PMU=PM*CM/(PM+CM)*0.023918 
      PMU2=PMU+PMU 
      ETA=ZC*ZP*1.44     *SQRT(PMU2/4./EBIND) 
      VSO=VSO          /(2.*.02398*197.3)**2 
      C=V0*.25/EBIND 
      DELV=.1*C 
      BARJ=XJ*XJ+XJ 
      BARS=XS+XS*XS 
      BARL=EL*EL+EL 
      XMESH=RMESH*2.*SQRT(EBIND*PMU2) 
      RC=RC*CM**0.333333 
      XC=RC*XMESH/RMESH 
C
C          Main loop point for calculation
C
   20 DO 3 I=1,4 
      MAXPI=MAX+I 
      X=XMESH*FLOAT(MAX+I-1) 
      PROSUM=1. 
      DO 2 J=1,10 
      PRO=1. 
      DO 1 K=1,J 
      XK=K 
      PRO=PRO*(BARL+.25-(ETA+XK-.5)**2)/XK/(-X) 
    1 CONTINUE
      PROSUM=PROSUM+PRO 
    2 CONTINUE
      U(MAXPI)=PROSUM*EXP(-.5*X)*(-1.)**NODE*X**(-ETA) 
    3 CONTINUE
      NNODE=-1 
      IF(DELV .LT.C/1000000.) GO TO 42 
      IF(ABS(ABS(TEST3)-1.).LT.1./100000.) GO TO 42 
      IF(MTEST.GT.32)  GO TO 42 
      DO 10 II=1,MAXM9 
      I=MAX+1-II 
      XI=I 
      X0=(XI-1.)*XMESH 
      X1=(XI-0.)*XMESH 
      X2=(XI+1.)*XMESH 
      X3=(XI+2.)*XMESH 
      X4=(XI+3.)*XMESH 
      U(I)=(1.+XMESH**2*1.25*POT(X1))*U(I+1)+(1.+XMESH**2*1.25*POT(X3))* 
     1U(I+3)-U(I+4)+XMESH**2*0.5*POT(X2)*U(I+2) 
      U(I)=(2.+XMESH**2/1.2*POT(X1))*U(I+1)+(-1.+XMESH**2/12.*POT(X2))* 
     1U(I+2)+XMESH**2/12.*POT(X0)*U(I) 
      IF(U(I)*U(I+1).GT.0.)GO TO 10 
      NNODE=NNODE+1 
      IF(NNODE.GE.NODE) GO TO 11 
   10 CONTINUE 
      IF(NNODE.GE.NODE-1)  GO TO 22 
C
C          Too few nodes (deeper)
C
      MTEST=MTEST+1 
      CCC=C*EBIND/.25 
      DELVC=DELV*EBIND/.25 
C     WRITE (6,812) CCC 
      C=C+DELV 
      GO TO 20 
C
C          Too many nodes (shallower)
C
   11 YTEST=1
      MTEST=MTEST+1 
      DELV=DELV/(1.+XTEST*YTEST) 
      CCC=C*EBIND/.25 
C     WRITE (6,812) CCC 
      C=C-DELV 
      GO TO 20 
C
C          Correct number of nodes found
C
   22 XMES=XMESH/128. 
      U(971)=0. 
      U(972)=XMES**(L+1) 
      IL=1 
   23 DO 36 I=3,4 
      IF(IL.EQ.3) GO TO 38 
      K=I+970 
      XI=I-1 
      X=XMES*(XI-1.) 
      U(K)=(2.+XMES**2*POT(X))*U(K-1)-U(K-2) 
   36 CONTINUE
C
  123 DO 136 I=5,25 
      IF(IL.EQ.3) GO TO 38 
      K=I+970 
      XI=I-1 
      X0=(XI-0.)*XMES 
      X1=(XI-1.)*XMES 
      X2=(XI-2.)*XMES 
      X3=(XI-3.)*XMES 
      U(K)=(1.+XMES**2*1.25*POT(X1))*U(K-1)+(1.+XMES**2*1.25*POT(X3))* 
     1U(K-3)-U(K-4)+XMES**2*0.5*POT(X2)*U(K-2) 
      U(K)=(2.+XMES**2/1.2*POT(X1))*U(K-1)+(-1.+XMES**2/12.*POT(X2))*U(K 
     1-2)+XMES**2/12.*POT(X0)*U(K) 
  136 CONTINUE 
      XMES=XMES*8. 
      IL=IL+1 
      U(972)=U(979) 
      U(973)=U(987) 
      U(974)=U(995) 
      GO TO 123 
C
   38 DO 37 I=5,25 
      K=I+970 
      XI=I-1 
      X0=(XI-0.)*XMES 
      X1=(XI-1.)*XMES 
      X2=(XI-2.)*XMES 
      X3=(XI-3.)*XMES 
      U(K)=(1.+XMES**2*1.25*POT(X1))*U(K-1)+(1.+XMES**2*1.25*POT(X3))* 
     1U(K-3)-U(K-4)+XMES**2*0.5*POT(X2)*U(K-2) 
      U(K)=(2.+XMES**2/1.2*POT(X1))*U(K-1)+(-1.+XMES**2/12.*POT(X2))*U(K 
     1-2)+XMES**2/12.*POT(X0)*U(K) 
   37 CONTINUE 
      TEST=U(11)/U(991) 
      ULDO=0.5*(U(12)-U(10))/U(11)/XMESH 
      ULDI=0.5*(U(993)-U(989))/XMESH/U(991) 
      TEST2=ULDI-ULDO 
      TEST3=ULDI/ULDO 
C
C          TEST2 = difference in log derivatives
C
      IF( TEST2 .EQ. 0. ) GO TO 42
      IF( TEST2 .GT. 0. ) THEN
C                                    deeper
        XTEST=1. 
        MTEST=MTEST+1 
        DELV=DELV/(1.+XTEST*YTEST) 
        CCC=C*EBIND/.25 
        DELVC=DELV*EBIND/.25 
C        WRITE (6,812) CCC,TEST2,TEST3 
        C=C+DELV 
      ELSE
C                                    shallower
        YTEST=1. 
        MTEST=MTEST+1 
        DELV=DELV/(1.+XTEST*YTEST) 
        CCC=C*EBIND/.25 
        DELVC=DELV*EBIND/.25 
C        WRITE (6,812) CCC,TEST2,TEST3 
        C=C-DELV 
      ENDIF
      GO TO 20 
C
C          We are done
C
   42 DO 43 I=1,11 
      J=2*I-1 
      K=J+970 
      U(I)=TEST*U(K) 
   43 CONTINUE
C                      normalize
   12 SUM=0. 
      DO 30 I=1,MAX,2 
      SUM = SUM + U(I)**2 + 4.*U(I+1)**2 + U(I+2)**2 
   30 CONTINUE
      SUM=1./3.*RMESH*SUM 
      DO 21 I=2,MAX 
      XI=I-1 
      U(I-1)=U(I)/(SQRT(SUM)*RMESH*XI) 
   21 CONTINUE
      VF=CCC 
      RETURN 
C
  811 FORMAT(/////,25X,'CONVERGENCE'//5X,'WELLDEPTH',5X,'LOG DER DIFF' 
     >  ,5X,'LOG DER RAT'// ) 
  812 FORMAT(5X,F9.5,6X,E12.5,6X,F12.7) 
      END 
      FUNCTION POT(X) 
C 
      COMMON C,ETA,R,XC,BARJ,BARL,BARS 
C
      COMMON /PART/ EBIND,MP,L,NODE,IZP,XJ,XS 
      COMMON /COR/ MC,IZC,V0,A,R0,RC,VSO 
      COMMON /RA/ RMESH,RMAX,NNMAX 
C
      FOR=EXP(X*RC/XC/A-R) 
      FORM=FOR+1. 
      FOR=FOR*C/X/A/RC*XC 
      POT=-C/FORM-VSO*(BARJ-BARL-BARS)*FOR/FORM/FORM+.25+BARL/X**2 
      IF( X .LT. XC ) THEN    
        POT=POT+ETA/2./XC*(3.-X**2/XC**2) 
      ELSE   
        POT=POT+ETA/X 
      ENDIF
      RETURN 
      END 
      SUBROUTINE RSQARE( U1, TL1 ) 
C 
C          Compute and print various moments of U1 
C
      COMMON /RA/ RMESH,RMAX,NNMAX 
C
      DIMENSION U1(1000), R11(201),R12(201),R22(201) 
C
      WRITE(6,801) 
      NMAX=10*RMAX+1 
      R11(1)=0.0 
      R22(1)=0.0 
      DO 2 J= 2, NMAX 
      K=J-1 
      R=RMESH*K 
      R11(J)=U1(K)*U1(K)*R**2 
      R22(J)=U1(K)*U1(K)*R**4 
    2 CONTINUE 
      RS11 = FNTGRL(201,.1,R11) 
      RS22 = FNTGRL(201,.1,R22) 
      RS12 = ABS(RS22/RS11)
      RS12 = SQRT(RS12)
      WRITE(6,802) RS11, RS22, RS12 
      RETURN 
C
  801 FORMAT(// 1X ) 
  802 FORMAT( 5X, 'VOLUME = ', F12.5, 5X, 'R**2 MOMENT = ', 
     >  F12.5, 5X, 'RADIUS = ', F12.5 ) 
      END 
      FUNCTION FNTGRL(N,DELTA,F) 
C 
CFNTGRL      NUMERICAL INTEGRATION SUBROUTINE 
C    
C     EXTENDED FIVE-POINT NEWTON-COTES QUADRATURE 
C     EXACT FOR FOURTH-DEGREE POLYNOMIAL 
C     REQUIRES MINIMUM OF FIVE POINTS ... NO MAXIMUM 
C        N = TOTAL NUMBER OF NET POINTS (AEQUALLY-SPACED) 
C        DELTA = INCREMENT BETWEEN NET POINTS 
C        F = INTEGRAND TABULATED AT EACH NET POINT 
C
      DIMENSION F(5) 
C 
      S=0.0 
      DO 20 J=5,N,4 
      M=J 
   20 S=S+7.*(F(J-4)+F(J))+32.*(F(J-3)+F(J-1))+12.*F(J-2) 
      IF(N-M) 50,50,25 
   25 M=M+1 
      DO 30 J=M,N 
   30 S=S+7.84375*F(J)+20.1875*F(J-1)-8.25*F(J-2)+3.3125*F(J-3)-0.59375* 
     1F(J-4) 
   50 FNTGRL=DELTA*S/22.5 
      RETURN 
      END 
