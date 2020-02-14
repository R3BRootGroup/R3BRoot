      SUBROUTINE CG000(TJ,IA,IB,IC)                                     
C                                                                       
C          CALCULATES THE 3-J SYMBOL (A,B,C/0,0,0).                          
C          TRIANGULAR INEQUALITIES ARE ALREADY SATISFIED.                    
C          ALSO (-1)**(A+B+C)=+1 HAS BEEN SATISFIED.                         
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL1/ FL(130)                                              
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      IAB=IA+IB                                                         
   40 IG2=IAB+IC                                                        
      IG2P=IG2+2                                                        
      IG=IG2/2                                                          
      ICP=IC+1                                                          
      IACMBP=IA-IB+ICP                                                  
      IBCMAP=IB-IA+ICP                                                  
      IABMCP=IAB-IC+1                                                   
   70 IGP1=IG+1                                                         
      IGMAP1=IGP1-IA                                                    
      IGMBP1=IGP1-IB                                                    
      IGMCP1=IGP1-IC                                                    
      IGD2=IG/2                                                         
      NSIG=IG-IGD2-IGD2                                                 
      SIG=1-NSIG-NSIG                                                   
      X=FL(IGP1)-FL(IGMAP1)-FL(IGMBP1)-FL(IGMCP1)                       
      DLG=0.5*(FL(IACMBP)+FL(IBCMAP)+FL(IABMCP)-FL(IG2P))               
      YLG=X+DLG                                                         
      Y=DEXP(YLG)                                                       
      TJ=SIG*Y                                                          
      RETURN                                                            
      END                                                               
      SUBROUTINE NINEJ                                                  
C                                                                       
C          NINE-J SYMBOL SUBROUTINE (T.TAMURA - ORNL)                        
C          DEFINITION IS                                                     
C
C                 !  A B E  !                                                   
C             U9= !  C D F  !                                                   
C                 !  G H I  !                                                   
C
C     =(2K+1)*W(DAFG;KC)*W(DAHE;KB)*W(FGEH;KI) SUMMED OVER K            
C     L9(1)=2*A,L9(2)=2*B,----,L9(8)=2*H,L9(9)=2*I                      
C     THE TRIANGULAR INEQUALITIES ARE ALREADY SATISFIED                 
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL7/ U9,L9(9)                                             
      COMMON /BLNJR/ RAC,IA,IB,IC,ID,IE,IF                              
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION LT(9)                                                   
C
      U9=0.0                                                            
      DO 10 I=1,9                                                       
   10 LT(I)=L9(I)                                                       
      LMIN=LT(1)                                                        
      IMIN=1                                                            
      DO 20 I=2,9                                                       
      IF(LT(I)-LMIN) 15,20,20                                           
   15 LMIN=LT(I)                                                        
      IMIN=I                                                            
   20 CONTINUE                                                          
      KEX=0                                                             
      GOTO(110,110,110,110,150,150,170,170,190),IMIN                    
  110 MM=(IMIN-1)/2+1                                                   
      M1=MM+MM-1                                                        
      M2=M1+1                                                           
      M3=MM+4                                                           
      L1=LT(7)                                                          
      LT(7)=LT(M1)                                                      
      LT(M1)=L1                                                         
      L1=LT(8)                                                          
      LT(8)=LT(M2)                                                      
      LT(M2)=L1                                                         
      L1=LT(9)                                                          
      LT(9)=LT(M3)                                                      
      LT(M3)=L1                                                         
      IMIN=IMIN+(7-M1)                                                  
      GOTO 175                                                          
  150 KEX=1                                                             
      M1=7                                                              
      M2=8                                                              
      M3=IMIN+IMIN-9                                                    
      M4=M3+1                                                           
      GOTO 180                                                          
  170 KEX=1                                                             
  175 M1=5                                                              
      M2=6                                                              
      M3=IMIN-6                                                         
      M4=M3+2                                                           
  180 L1=LT(M1)                                                         
      LT(M1)=LT(M3)                                                     
      LT (M3)=L1                                                        
      L1=LT(M2)                                                         
      LT(M2)=LT(M4)                                                     
      LT(M4)=L1                                                         
      L1=LT(9)                                                          
      LT(9)=LT(IMIN)                                                    
      LT(IMIN)=L1                                                       
  190 IF(LT(9))200,200,300                                              
  200 IA=LT(1)                                                          
      IB=LT (2)                                                         
      IC=LT(3)                                                          
      ID=LT(4)                                                          
      IE=LT(5)                                                          
      IF=LT(7)                                                          
      R1=(IE+1)*(IF+1)                                                  
      R1=DSQRT(R1)                                                      
      IEFAD=IABS((IE+IF-IA-ID)/2)                                       
      IEFADD=IEFAD/2                                                    
      ISIG=IEFAD-IEFADD-IEFADD                                          
      S1=1-ISIG-ISIG                                                    
      CALL RAC7                                                         
      U9=RAC*S1/R1                                                      
      GOTO 370                                                          
  300 K1=IABS(LT(2)-LT(7))                                              
      K2=IABS(LT(3)-LT(5))                                              
      K3=IABS(LT(4)-LT(9))                                              
      MINRDA=MAX0(K1,K2,K3)                                             
      K1=LT(2)+LT(7)                                                    
      K2=LT(3)+LT(5)                                                    
      K3=LT(4)+LT(9)                                                    
      MAXRDA=MIN0(K1,K2,K3)                                             
      IF(MINRDA.GT.MAXRDA) GOTO 351                                     
  320 DO 350 N1=MINRDA,MAXRDA,2                                         
      RAMDA2=N1                                                         
      IA=LT(2)                                                          
      IB=LT(5)                                                          
      IC=LT(7)                                                          
      ID=LT(3)                                                          
      IE=LT(1)                                                          
      IF=N1                                                             
      CALL RAC7                                                         
      W1=(RAMDA2+1.0)*RAC                                               
      IB=LT(4)                                                          
      ID=LT(9)                                                          
      IE=LT(8)                                                          
      CALL RAC7                                                         
      W1=W1*RAC                                                         
      IA=LT(3)                                                          
      IC=LT(5)                                                          
      IE=LT(6)                                                          
      CALL RAC7                                                         
      W1=W1*RAC                                                         
  350 U9=U9+W1                                                          
  351 CONTINUE                                                          
  370 IF(KEX) 400,1000,400                                              
  400 KP=0                                                              
      DO 410 I=1,9                                                      
  410 KP=KP+LT(I)                                                       
      KPD2=KP/2                                                         
      KPD4=KPD2/2                                                       
      KSIG=KPD2-KPD4-KPD4                                               
      SIG=1-KSIG-KSIG                                                   
      U9=U9*SIG                                                         
 1000 RETURN                                                            
      END                                                               
      SUBROUTINE RAC7                                                   
C                                                                       
C     RACAH COEFFICIENT ROUTINE (T.TAMURA - ORNL)                       
C     DEFN IN EQN 36 OF RACAH, PHYS REC 62(1942)438                     
C
C     IF RAC=W(ABCD;EF) THEN IA=2*A,IB=2*B ETC                          
C     THE MAIN ROUTINE OF A PROGRAM, ANY SUBROUTINE OF WHICH            
C     IS TO CALL CLEB AND/OR RAC7, MUST HAVE THE FOLLOWING SET OF       
C     STATEMENTS AT ITS VERY BEGINNING                                  
C
C     FACLOG(1)=0.0                                                     
C     FACLOG(2)=0.0                                                     
C     FN=1.0                                                            
C     DO 10 N=3,NMAX                                                    
C     FN=FN+1.0                                                         
C  10 FACLOG(N)=FACLOG(N-1)+LOF(FN)                                     
C
C     THE TRIANGULAR INEQUALITIES ARE ALREADY SATISFIED                 
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL1/ FACLOG(130)                                          
      COMMON /BLNJR/ RAC,IA,IB,IC,ID,IE,IF                              
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION LT(6)                                                   
C
   30 LT(1)=IA                                                          
      LT(2)=ID                                                          
      LT(3)=IB                                                          
      LT(4)=IC                                                          
      LT(5)=IF                                                          
      LT(6)=IE                                                          
      LTMIN=LT(1)                                                       
      KMIN=1                                                            
      DO 40 N=2,6                                                       
      IF(LT(N)-LTMIN) 35,37,40                                          
   35 LTMIN=LT(N)                                                       
   37 KMIN=N                                                            
   40 CONTINUE                                                          
      GOTO (45,45,50,50,55,60),KMIN                                     
   45 KK=7-KMIN                                                         
      IA=LT(KK)                                                         
      ID=LT(KMIN+4)                                                     
      IE=LT(KMIN)                                                       
      IF=LT(KK-4)                                                       
      IADEF=IABS((IA+ID-IE-IF)/2)                                       
      IADEFD=IADEF/2                                                    
      ISIG1=IADEF-IADEFD-IADEFD                                         
      SIGN1=1-ISIG1-ISIG1                                               
      GOTO 70                                                           
   50 KK=9-KMIN                                                         
      IB=LT(KK)                                                         
      IC=LT(KMIN+2)                                                     
      IE=LT(KMIN)                                                       
      IF=LT(KK-2)                                                       
      IBCEF=IABS((IB+IC-IE-IF)/2)                                       
      IBCEFD=IBCEF/2                                                    
      ISIG1=IBCEF-IBCEFD-IBCEFD                                         
      SIGN1=1-ISIG1-ISIG1                                               
      GOTO 70                                                           
   55 IE=LT(5)                                                          
      IF=LT(6)                                                          
      IB=LT(4)                                                          
      IC=LT(3)                                                          
   60 SIGN1=1.0                                                         
   70 IP=IA-IB                                                          
      IQ=IC-ID                                                          
      IF(IABS(IP)-IABS(IQ)) 110,120,120                                 
  110 IT=IC                                                             
      IC=IA                                                             
      IA=IT                                                             
      IT=ID                                                             
      ID=IB                                                             
      IB=IT                                                             
      IP=IQ                                                             
  120 IF(IP) 125,130,130                                                
  125 IT=IB                                                             
      IB=IA                                                             
      IA=IT                                                             
      IT=ID                                                             
      ID=IC                                                             
      IC=IT                                                             
  130 IF(IE) 140,140,150                                                
  140 BI=IB                                                             
      DI=ID                                                             
      IBDF=(IB+ID-IF)/2                                                 
      IBDFD2=IBDF/2                                                     
      ISIG2=IBDF-IBDFD2-IBDFD2                                          
      SIGN2=1-ISIG2-ISIG2                                               
      RAC=SIGN1*SIGN2/DSQRT((BI+1.0)*(DI+1.0))                          
      GOTO 3000                                                         
  150 IABEP=(IA+IB+IE)/2+1                                              
      ICDEP=(IC+ID+IE)/2+1                                              
      IACFP=(IA+IC+IF)/2+1                                              
      IBDFP=(IB+ID+IF)/2+1                                              
      IABE=IABEP-IE                                                     
      IEAB=IABEP-IB                                                     
      IBEA=IABEP-IA                                                     
      ICDE=ICDEP-IE                                                     
      IECD=ICDEP-ID                                                     
      IDEC=ICDEP-IC                                                     
      IACF=IACFP-IF                                                     
      IFAC=IACFP-IC                                                     
      ICFA=IACFP-IA                                                     
      IBDF=IBDFP-IF                                                     
      IFBD=IBDFP-ID                                                     
      IDFB=IBDFP-IB                                                     
      IABCD1=(IA+IB+IC+ID+4)/2                                          
      IEFMAD=(IE+IF-IA-ID)/2                                            
      IEFMBC=(IE+IF-IB-IC)/2                                            
      NZMI1=1-IEFMAD                                                    
      NZMIN=MAX0(1,NZMI1)                                               
      NZMAX=MIN0(IABE,ICDE,IBDF)                                        
  180 SQLOG=FACLOG(IABE)+FACLOG(IEAB)+FACLOG(IBEA)                      
     *     +FACLOG(ICDE)+FACLOG(IECD)+FACLOG(IDEC)                      
     *     +FACLOG(IACF)+FACLOG(IFAC)+FACLOG(ICFA)                      
     *     +FACLOG(IBDF)+FACLOG(IFBD)+FACLOG(IDFB)                      
     * -FACLOG(IABEP+1)-FACLOG(ICDEP+1)-FACLOG(IACFP+1)-FACLOG(IBDFP+1) 
      SQLOG=0.5*SQLOG                                                   
      SQSS=0.0                                                          
      IF(NZMIN.GT.NZMAX) GOTO 201                                       
      DO 200 NZ=NZMIN,NZMAX                                             
      NZM1=NZ-1                                                         
      I1=IABCD1-NZM1                                                    
      I2=IABE-NZM1                                                      
      I3=ICDE-NZM1                                                      
      I4=IACF-NZM1                                                      
      I5=IBDF-NZM1                                                      
      I7=IEFMAD+NZ                                                      
      I8=IEFMBC+NZ                                                      
      SSLOG=SQLOG+FACLOG(I1)-FACLOG(I2)-FACLOG(I3)-FACLOG(I4)           
     *           -FACLOG(I5)-FACLOG(NZ)-FACLOG(I7)-FACLOG(I8)           
      NZM1D2=NZM1/2                                                     
      NZSIG=NZM1-NZM1D2-NZM1D2                                          
      ZSIG=1-NZSIG-NZSIG                                                
      SSTERM=ZSIG*DEXP(SSLOG)                                           
      SQSS=SQSS+SSTERM                                                  
  200 CONTINUE                                                          
  201 CONTINUE                                                          
      RAC=SIGN1*SQSS                                                    
 3000 CONTINUE                                                          
      IA=LT(1)                                                          
      ID=LT(2)                                                          
      IC=LT(4)                                                          
      IB=LT(3)                                                          
      IF=LT(5)                                                          
      IE=LT(6)                                                          
 4000 RETURN                                                            
      END                                                               
      REAL*8 FUNCTION CG(AJ1,AM1,AJ2,AM2,AJ,AM)               
C                                                                       
C     CALCULATES CLEBSCH GORDAN COEFFICIENT                             
C                                                                       
C        REF:  D.M.BRINK AND G.R.SATCHLER, ANGULAR MOMENTUM (2ND ED),      
C        OXFORD UNIVERSITY PRESS (1979), EQNS 2.34 AND 2.35                
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
      ONE=1.D0                                                          
      TWO=2.D0                                                          
      PWRM9=1.D-9                                                       
      CG=ZERO                                                           
C                                                                       
C     CONVERT ALL ARGUMENTS TO INTEGERS TWICE CORRECT VALUE             
C                                                                       
      J1=IDINT(TWO*AJ1+PWRM9)                                           
      J2=IDINT(TWO*AJ2+PWRM9)                                           
      J =IDINT(TWO*AJ +PWRM9)                                           
      M1=IDINT(TWO*AM1+DSIGN(PWRM9,AM1))                                
      M2=IDINT(TWO*AM2+DSIGN(PWRM9,AM2))                                
      M =IDINT(TWO*AM +DSIGN(PWRM9,AM ))                                
C                                                                       
C     CHECK Z-COMPONENTS ADD UP CORRECTLY                               
C                                                                       
      IF((M1+M2).NE.M) RETURN                                           
C                                                                       
C     CHECK THAT ALL M'S ARE .LE. J'S                                   
C                                                                       
      IF(J1.LT.IABS(M1)) RETURN                                         
      IF(J2.LT.IABS(M2)) RETURN                                         
      IF(J .LT.IABS(M )) RETURN                                         
C                                                                       
C     CHECK THAT J'S ARE CONSISTENT                                     
C                                                                       
      IF(J.LT.IABS(J1-J2).OR.J.GT.(J1+J2)) RETURN                       
C                                                                       
C     WHEN ALL Z-COMPONENTS ARE ZERO CG=0.0 IF AJ1+AJ2+AJ               
C     IS ODD                                                            
C                                                                       
      IF(M1.EQ.0.AND.M2.EQ.0.AND.M.EQ.0.AND.MOD((J1+J2+J)/2,2).NE.0)    
     *  RETURN                                                          
C                                                                       
C     NOW THAT ALL SELECTION RULES ARE OK DETERMINE CG                  
C                                                                       
      JT1=(J1-M1)/2                                                     
      JT2=(J-J2+M1)/2                                                   
      JT3=(J2+M2)/2                                                     
      JT4=(J-J1-M2)/2                                                   
      JT5=(J1+J2-J)/2                                                   
      NUMIN=MAX0(0,-JT2,-JT4)+1                                         
      NUMAX=MIN0(JT1,JT3,JT5)+1                                         
      IF(NUMAX.LT.NUMIN) RETURN                                         
      FCTOR=FACT((J1+M1)/2+1)+FACT(JT1+1)+FACT(JT3+1)                   
     *     +FACT((J2-M2)/2+1)+FACT((J+M)/2+1)+FACT((J-M)/2+1)           
      FCTOR=HALF*FCTOR                                                  
      DO 10 NN=NUMIN,NUMAX                                              
      NU=NN-1                                                           
      TERM=FCTOR-FACT(JT1-NU+1)-FACT(JT2+NU+1)-FACT(JT3-NU+1)           
     *     -FACT(JT4+NU+1)-FACT(NU+1)-FACT(JT5-NU+1)                    
   10 CG=CG+PHAS(NU)*DEXP(TERM)                                         
      CG=DELTA(AJ1,AJ2,AJ)*DSQRT(TWO*AJ+ONE)*CG                         
      RETURN                                                            
      END                                                               
