      SUBROUTINE DIRECT     (JR,JP,JT,KFORCE,                           
     *     FJPF,FJPI,PARP,TPF,TPFM,TPI,TPIM,PDENFT,                     
     *     FJTF,FJTI,PART,TTF,TTFM,TTI,TTIM,TDENFT,
     *     FNRM1)                     
C
C          Routine to evaluate the DIRECT-like form factors,
C          which includes treatment of ZREA terms.
C                                                                       
C          KEXCH = 0,2 DIRECT FORCE ONLY                                     
C                = 1   DIRECT FORCE + ZERO RANGE EXCHANGE (LFORCE=0)       
C          R.ZEGERS ADDED FNRM1
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      PARAMETER ( MNX = 900 ) 
      PARAMETER ( MNB =   5 )
C
      COMPLEX*16  FORMF
      COMMON /FF/ FORMF(MNX)   
C    
      COMMON /COOK/ H, NR, KEXCH   
C
      COMPLEX*16      GC0X,   GC1X
      COMMON /EXCHANG/ GC0X(2),GC1X(2)
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      COMPLEX*16 FTG(50), FTOT(50), FLD
      COMPLEX*16 VC0, VC1, VLS, VTND  
C                  ^ was VCO, changed by JSW 1/14/91
C
      DIMENSION PDENFT(7,2,50),TDENFT(7,2,50)       
      DIMENSION FTP(50),FTT(50),VNORM(MNB)   
C
      DATA PI /3.14150265358979D0/                                      
C
      RMAX = H*DFLOAT(NR)                                                 
      DQ   = PI/RMAX                                                        
C
      DO 10 I=1,MNB
      VNORM(I) = FNRM1
   10 CONTINUE
C                                                                       
C          OUTER LOOP over T     --------------------------------- 
C                                                                       
      DO 290 KT1=1,2                                                    
      KT=KT1-1                                                          
      T=DFLOAT(KT)                                                      
      MTMAX=2*KT+1                                                      
      DO 200 MMT=1,MTMAX                                                
      IF(KT.EQ.0) MT=0                                                  
      IF(KT.EQ.1) MT=MMT-2                                              
      FMT=DFLOAT(MT)                                                    
      TFACT=CG(TPI,TPIM,T,-FMT,TPF,TPFM)                                
     *     *CG(TTI,TTIM,T, FMT,TTF,TTFM)                                
      IF(TFACT.NE.0.D0)GOTO 210                                         
  200 CONTINUE                                                          
      GOTO 290                                                          
C               FMT is defined by TFM - TIM, so why the big deal?
  210 CONTINUE                                                          
C                                                                       
C          LOOP over LFORCE     -------------------------------     
C                                                                       
      IF( KFORCE .LT. 0 ) THEN      
        LFMIN=1                                                           
        LFMAX=3                                                           
      ELSE
        LFMIN=KFORCE+1                                                    
        LFMAX=LFMIN                                                       
      ENDIF                               
      DO 280 LF=LFMIN,LFMAX,2                                           
      LFORCE=LF-1                                                       
C                                                                       
C          LOOP over S      -----------------------------------   
C                                                                       
      DO 270 KS1=1,2                                                    
      KS=KS1-1                                                          
      IF(KS.EQ.0.AND.LFORCE.GT.0) GOTO 270                              
C                                                                       
C          LOOP over LR      -------------------------------
C                                                                       
      LRMIN=IABS(LFORCE-JR)+1                                           
      LRMAX=LFORCE+JR+1                                                 
      DO 260 LR1=LRMIN,LRMAX                                            
      LR=LR1-1                                                          
C                                                                       
C     CHECK TRIANGULAR RULES AND PARITY CHANGE                          
C                                                                       
      IF(LR.LT.IABS(JP-JT).OR.LR.GT.JP+JT) GOTO 260                     
      IF(PHAS(LR+JR).NE.PHAS(LFORCE)) GOTO 260                          
C                                                                       
C          LOOP over LP      ------------------------------- 
C                                                                       
      LPMIN=IABS(JP-KS)+1                                               
      LPMAX=JP+KS+1                                                     
      DO 250 LP1=LPMIN,LPMAX                                            
      LP=LP1-1                           
c      write(*,*)LFORCE,KS,LR,LP                               
      IF(PHAS(LP).NE.PARP) GOTO 250                                     
C                                                                       
C          LOOP over LT      -------------------------------   
C                                                                       
      LTMIN=IABS(JT-KS)+1                                               
      LTMAX=JT+KS+1                                                     
      DO 240 LT1=LTMIN,LTMAX                                            
      LT=LT1-1                                                          
      IF(PHAS(LT).NE.PART) GOTO 240                                     
C                                                                       
C     CHECK TRIANGULAR RULES AND PARITY CHANGE                          
C                                                                       
      IF(LR.LT.IABS(LP-LT).OR.LR.GT.LP+LT) GOTO 240                     
      IF(PHAS(LP+LT).NE.PHAS(LR)) GOTO 240                              

C                                                                       
C          Calculate CONSTANT coefficient    
C                                                                       
      CALL AMP(LP,KS,JP,LT,JT,LR,LFORCE,JR,ANS)                         
      CONST=PHAS(MT+IDINT(FJPI-FJPF+1.D-9))*ANS/DSQRT(4.D0*PI)          
      CALL CG000(CLB,LP,LT,LR)                                          
      CONST=CONST*CLB*PHAS(LP-LT)*DSQRT(DFLOAT(2*LR+1))                 
      CALL CG000(CLB,JR,LR,LFORCE)                                      
      CONST=CONST*CLB*PHAS(JR-LR)*DSQRT(DFLOAT(2*LFORCE+1))             


C                                                                       
C          Calculate Fourier transform of PROJECTILE density                 
C                                                                       
      IF(KS.EQ.0) IDENT=1                                               
      IF(KS.EQ.1) IDENT=3+LP-JP                                         
      DO 300 K=1,50                                                     
      FTP(K)=PDENFT(IDENT,KT1,K)                       
c      write(*,*)FTP(K)                 
  300 CONTINUE
C                                                                       
C          Calculate Fourier transform of TARGET density                     
C                                                                       
      IF(KS.EQ.0) IDENT=1                                               
      IF(KS.EQ.1) IDENT=3+LT-JT                                    
c      write(*,*)KS,LT,JT,IDENT     
      DO 400 K=1,50                                                     
      FTT(K)=TDENFT(IDENT,KT1,K)                                        
  400 CONTINUE
C                                                                       
C          Calculate Fourier transform of INTERACTION                        
C                                                                       
      Q=0.D0                                                            
      DO 60 K=1,50                                                      
      Q=Q+DQ                              
      CALL INTER( Q, 0, KT, VC0,VC1,VLS,VTND, VNORM )
c      write(*,*)'hier'
      IF( LFORCE .EQ. 0 ) THEN
C                                                 Central
        IF( KS .EQ. 0 ) FTG(K) = VC0 + GC0X(KT+1)
        IF( KS .EQ. 1 ) FTG(K) = VC1 + GC1X(KT+1)
C                                                 Spin-Orbit 
      ELSE IF( LFORCE .EQ. 1 ) THEN
        FTG(K) = 0.
C                                            Tensor
      ELSE IF( LFORCE .EQ. 2 ) THEN
        IF( KS .EQ. 0 ) FTG(K) = 0.0                                            
        IF( KS .EQ. 1 ) FTG(K) = SQRT(2.D0) * VTND
      ENDIF
   60 CONTINUE                                                          
C                                                                       
C          Multiply Fourier transforms together with WEIGHTS                 
C                                                                       
      DO 70 K=1,50                                                      
      FTOT(K)=PI*DFLOAT(K*K)/(2.0*RMAX**3)*FTG(K)*FTP(K)*FTT(K)         
   70 CONTINUE
C                                                                       
C          Calculate FOLDING INTEGRAL at each radial value 
C                                                                       
      R=0.D0                                                            
      DO 90 I=1,NR                                                      
      R=R+H                                                             
      FLD=0.0                                                           
      Q=0.D0                                                            
      DO 80 K=1,50                                                      
      Q=Q+DQ                                                            
      FLD=FLD+FTOT(K)*BESSEL(JR,Q*R)                                    
   80 CONTINUE
c      write(*,*)'*************'
c      write(*,*)LR,LP,LT,KS,LFORCE,T,CONST,FLD
      FORMF(I)=FORMF(I)+CONST*FLD                                       
   90 CONTINUE                                                          
C                                                                       
C          END of LOOPS over LT, LP, and LR   --------------     
C                                                                       
  240 CONTINUE                                                          
  250 CONTINUE                                                          
  260 CONTINUE                                                          
C                                                                       
C     END of LOOPS over S and LFORCE      ---------------------  
C                                                                       
  270 CONTINUE                                                          
  280 CONTINUE                                                          
C                                                                       
  290 CONTINUE                                                          
C                                                                       
C     END of OUTER LOOP over T      ------------------------------
C                                                                       
      RETURN                                                            
      END                                                               
