      SUBROUTINE EXCHNG     (JR,JP,JT,KFORCE,                           
     *     FJPF,FJPI,PARP,TPF,TPFM,TPI,TPIM,NPROJ,ALPP,                 
     *     IDPF,IDPI,JPX,SPX,                                           
     *     FJTF,FJTI,PART,TTF,TTFM,TTI,TTIM,NTARG,ALPT,                 
     *     IDTF,IDTI,JTX,STX)                                           
C
C          Routine to evaluate the EXCHANGE form factor using
C          proper (?) treatment of recoil, etc ....  NOT CHECKED
C
C >>> ALSO NOT UPDATED to the DECEMBER 1988 version of program !!
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      PARAMETER ( MNX = 900 ) 
C
      COMPLEX*16  FORMF
      COMMON /FF/ FORMF(MNX)           
C                                  NOTE that RR is really H
      COMMON /COOK/ RR, NR, KEXCH  
      COMMON /ID/ NID(45),LID(45),JID(45)                               
C
      COMMON /BL1/ FL(130)              
      COMMON /BL2/ P,IARRAY(10)          
      COMMON /BL3/ JARRAY(2)                                            
      COMMON /BL4/ AARRAY,KARRAY(3)                                     
      COMMON /BL5/ B(561)                                               
      COMMON /BL6/ LARRAY(2)                                            
      COMMON /BL7/ X,JA,JB,JC,JD,JE,JF,JG,JH,JI                         
      COMMON /BLNJR/ RAC,IA,IB,IC,ID,IE,IF                              
      COMMON /SUM/ MARRAY(7)                                            
      COMMON /MOSHI/ NARRAY(9)                                          
      COMMON /MOSHE/ IIARAY(9)                                          
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      COMPLEX*16 FLD
C
      DIMENSION IDPF(100),IDPI(100),JPX(100),SPX(2,100)                 
      DIMENSION IDTF(100),IDTI(100),JTX(100),STX(2,100)                 
      DATA PI /3.14159265358979D0/                                      
      DATA ONE /1.0D0/                                                  
C                                                                       
C          LOOP OVER T                                                       
C                                                                       
      DO 460 KT1=1,2                                                    
      KT=KT1-1                                                          
      T=DFLOAT(KT)                                                      
      MTMAX=2*KT+1                                                      
      DO 130 MMT=1,MTMAX                                                
      IF(KT.EQ.0) MT=0                                                  
      IF(KT.EQ.1) MT=MMT-2                                              
      FMT=DFLOAT(MT)                                                    
      TFACT=CG(TPI,TPIM,T,-FMT,TPF,TPFM)                                
     *     *CG(TTI,TTIM,T, FMT,TTF,TTFM)                                
      IF(TFACT.NE.0.D0) GOTO 135                                        
  130 CONTINUE                                                          
      GOTO 460                                                          
C              TFM already defined by TFM - TIM !!  TFACT never used
  135 CONTINUE                                                          
C                                                                       
C          LOOP OVER LFORCE                                                  
C                                                                       
      IF(KFORCE.LT.0) GOTO 400                                          
      LFMIN=KFORCE+1                                                    
      LFMAX=LFMIN                                                       
      GOTO 410                                                          
  400 LFMIN=1                                                           
      LFMAX=3                                                           
  410 DO 450 LF=LFMIN,LFMAX,2                                           
      LFORCE=LF-1                                                       
C                                                                       
C          LOOP OVER S                                                       
C                                                                       
      DO 470 KS1=1,2                                                    
      KS=KS1-1                                                          
      IF(KS.EQ.0.AND.LFORCE.GT.0) GOTO 470                              
C                                                                       
C          LOOP OVER LR                                                      
C                                                                       
      LRMIN=IABS(LFORCE-JR)+1                                           
      LRMAX=LFORCE+JR+1                                                 
      DO 300 LR1=LRMIN,LRMAX                                            
      LR=LR1-1                                                          
C                                                                       
C     CHECK TRIANGULAR RULE                                             
C                                                                       
      IF(LR.LT.IABS(JP-JT).OR.LR.GT.JP+JT) GOTO 300                     
C                                                                       
C          LOOP OVER LP                                                      
C                                                                       
      LPMIN=IABS(JP-KS)+1                                               
      LPMAX=JP+KS+1                                                     
      DO 303 LP1=LPMIN,LPMAX                                            
      LP=LP1-1                                                          
      IF(PHAS(LP).NE.PARP) GOTO 303                                     
C                                                                       
C          LOOP OVER LT                                                      
C                                                                       
      LTMIN=IABS(JT-KS)+1                                               
      LTMAX=JT+KS+1                                                     
      DO 306 LT1=LTMIN,LTMAX                                            
      LT=LT1-1                                                          
      IF(PHAS(LT).NE.PART) GOTO 306                                    
C                                                                       
C          LOOP OVER PROJECTILE TRANSITIONS                                  
C                                                                       
      DO 310 N=1,NPROJ                                                  
      IF(JP.NE.JPX(N)) GOTO 310                                         
      N1P=NID(IDPF(N))                                                  
      L1P=LID(IDPF(N))                                                  
      J1P=JID(IDPF(N))                                                  
      N2P=NID(IDPI(N))                                                  
      L2P=LID(IDPI(N))                                                  
      J2P=JID(IDPI(N))                                                  
      SP=SPX(KT1,N)                                                     
      IF(LP.LT.IABS(L1P-L2P).OR.LP.GT.L1P+L2P) GOTO 310                 
      IF(SP.EQ.0.D0) GOTO 310                                           
C                                                                       
C          LOOP OVER TARGET TRANSITIONS                                      
C                                                                       
      DO 320 M=1,NTARG                                                  
      IF(JT.NE.JTX(M)) GOTO 320                                         
      N1T=NID(IDTF(M))                                                  
      L1T=LID(IDTF(M))                                                  
      J1T=JID(IDTF(M))                                                  
      N2T=NID(IDTI(M))                                                  
      L2T=LID(IDTI(M))                                                  
      J2T=JID(IDTI(M))                                                  
      ST=STX(KT1,M)                                                     
      IF(LT.LT.IABS(L1T-L2T).OR.LT.GT.L1T+L2T) GOTO 320                 
      IF(ST.EQ.0.D0) GOTO 320                                           
C                                                                       
C     CALCULATE G COEFFICIENTS                                          
C                                                                       
      CALL GS(LP,KS,JP,L1P,J1P,L2P,J2P,GSP)                       
      CALL GS(LT,KS,JT,L1T,J1T,L2T,J2T,GST)                       
      ANUP=2.0*ALPP**2                                                  
      ANUT=2.0*ALPT**2                                                  
      ALP=ALPP/DSQRT(2.D0)                                              
      ALT=ALPT/DSQRT(2.D0)                                              
      IROP=2*N1P+L1P+2*N2P+L2P                                          
      NCR=1+IROP/2                                                      
      IROT=2*N1T+L1T+2*N2T+L2T                                          
      NCS=1+IROT/2                                                      
C                                                                       
C     C.M. COORDINATES                                                  
C                                                                       
C                                                                       
C          LOOP OVER NCP,LCR                                                 
C                                                                       
      DO 12 MP=1,NCR                                                    
      NCP=MP-1                                                          
      LCR=IROP-2*NCP+1                                                  
C                                                                       
C          LOOP OVER LCP,NRP1                                                
C                                                                       
      DO 12 LCP1=1,LCR                                                  
      LCP=LCP1-1                                                        
      NRP1=(IROP-2*NCP-LCP)/2+1                                         
C                                                                       
C          LOOP OVER NCT,LCS                                                 
C                                                                       
      DO 12 MT=1,NCS                                                    
      NCT=MT-1                                                          
      LCS=IROT-2*NCT+1                                                  
C                                                                       
C          LOOP OVER LCT,NRT1                                                
C                                                                       
      DO 12 LCT1=1,LCS                                                  
      LCT=LCT1-1                                                        
      NRT1=(IROT-2*NCT-LCT)/2+1                                         
      TEST=PHAS(LCP+LCT+JR)                                             
      LMAX=LCP+LCT                                                      
      LMIN=IABS(LCP-LCT)                                                
      IF(JR.GT.LMAX.OR.JR.LT.LMIN) GOTO 12                              
      IF(TEST.NE.ONE) GOTO 12                                           
C                                                                       
C     RELATIVE COORDINATES                                              
C                                                                       
      RSUM=0.0                                                          
C                                                                       
C          LOOP OVER NRP,LRP                                                 
C                                                                       
      DO 17 MMP=1,NRP1                                                  
      NRP=MMP-1                                                         
      LRP=IROP-2*NCP-LCP-2*NRP                                          
      PHA=PHAS(L1P+L2P)                                                 
      PHB=PHAS(LCP+LRP)                                                 
      IF(PHA.NE.PHB) GOTO 17                                            
      LMAX2=LCP+LRP                                                     
      LMIN2=IABS(LCP-LRP)                                               
      IF(LP.GT.LMAX2.OR.LP.LT.LMIN2) GOTO 17                            
      CALL MOSHIN (L1P,L2P,LP,LRP,LCP,N1P,N2P,NRP,NCP,BKTP)             
      IF(BKTP.EQ.0.D0) GOTO 17                                          
C                                                                       
C          LOOP OVER NRT,LRT                                                 
C                                                                       
      DO 13 MMT=1,NRT1                                                  
      NRT=MMT-1                                                         
      LRT=IROT-2*NCT-LCT-2*NRT                                          
      PHA=PHAS(L1T+L2T)                                                 
      PHB=PHAS(LCT+LRT)                                                 
      IF(PHA.NE.PHB) GOTO 13                                            
      LMAX3=LCT+LRT                                                     
      LMIN3=IABS(LCT-LRT)                                               
      IF(LT.GT.LMAX3.OR.LT.LT.LMIN3) GOTO 13                            
      LRPTMX=LRP+LRT                                                    
      LRPTMN=IABS(LRP-LRT)                                              
      IF(LRPTMX.LT.LFORCE.OR.LRPTMN.GT.LFORCE) GOTO 13                  
      PH=PHAS(LRP+LRT+LFORCE)                                           
      IF(PH.NE.ONE) GOTO 13                                             
      CALL MOSHIN(L1T,L2T,LT,LRT,LCT,N1T,N2T,NRT,NCT,BKTT)              
      IF(BKTT.EQ.0.D0) GOTO 13                                          
      JA=2*LRP                                                          
      JB=2*LRT                                                          
      JC=2*LCP                                                          
      JD=2*LCT                                                          
      JE=2*LFORCE                                                       
      JF=2*JR                                                           
      JG=2*LP                                                           
      JH=2*LT                                                           
      JI=2*LR                                                           
      CALL NINEJ                                                        
      TEMP=X                                                            
      CALL AMP(LP,KS,JP,LT,JT,LR,LFORCE,JR,ANS1)                        
      XX=TEMP*ANS1*DSQRT(DFLOAT(2*LR+1))                                
      CALL CG000(CLB,LRP,LRT,LFORCE)                                    
      CLB=CLB*PHAS(LRP-LRT)*DSQRT(DFLOAT(2*LFORCE+1))                   
      PHASE=PHAS(LRT+JR+LT+LCP)                                         
      PHASE=PHASE*PHAS((L1P+L2P+L1T+L2T+LCP+LCT-LFORCE)/2)              
      REL=VNLNL(NRT,LRT,NRP,LRP,ALP,ALT,LFORCE,KS,KT)                   
      REL=PHASE*XX*BKTP*BKTT*REL*CLB*(DSQRT(DFLOAT((2*LRP+1)*           
     *(2*LRT+1))))                                                      
      RSUM=RSUM+REL                                                     
   13 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER NRT,LRT                                          
C                                                                       
   17 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER NRP,LRP                                          
C                                                                       
      IF(RSUM.EQ.0.0) GOTO 12                                           
      CALL CG000(CLB,LCP,LCT,JR)                                        
      CLB=CLB*PHAS(LCP-LCT)*DSQRT(DFLOAT(2*JR+1))                       
      CM=CLB*DSQRT(DFLOAT((2*LCP+1)*(2*LCT+1))/DFLOAT(2*JR+1))/         
     *   DSQRT(4.0*PI)                                                  
      CM=CM*SP*ST*GSP*GST*RSUM*PHAS(MT+IDINT(FJPI-FJPF+1.D-9))                
      DO 21 I=1,NR                                                      
      R=RR*I                                                            
      CALL EFOLD(NCP,LCP,ANUP,NCT,LCT,ANUT,JR,R,FLD)                    
      FORMF(I)=FORMF(I)+CM*FLD                                          
   21 CONTINUE                                                          
   12 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER LCT,NRT1                                         
C          END OF LOOP OVER NCT,LCS                                          
C          END OF LOOP OVER LCP,NRP1                                         
C          END OF LOOP OVER NCP,LCR                                          
C                                                                       
  320 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER TARGET TRANSITIONS                               
C                                                                       
  310 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER PROJECTILE TRANSITIONS                           
C                                                                       
  306 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER LT                                               
C                                                                       
  303 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER LP                                               
C                                                                       
  300 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER LR                                               
C                                                                       
  470 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER S                                                
C                                                                       
  450 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER LFORCE                                           
C                                                                       
  460 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER T                                                
C                                                                       
      RETURN                                                            
      END                                                               
