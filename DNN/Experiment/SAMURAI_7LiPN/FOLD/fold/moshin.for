      SUBROUTINE MOSHIN (L1,L2,LA,LR,L,N1,N2,MNR,MN,OB)                 
C                                                                       
C     PROGRAM FOR CALCULATING OSCILLATOR BRACKETS                       
C     PROGRAM CHECKS BOTH ENERGY CONSERVATION AND TRIANGLE RELATIONS    
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL1/ FL(130)                                              
      COMMON /BL2/ P,LT2S,LP,JB1B2,JB1MB2,JB1LA1,JB2LA2,JB1MLA,         
     *   JB2MLA,JB1T2S,JB2T2S                                           
      COMMON /BL3/ NR,N                                                 
      COMMON /BL4/ R,NU1,NU2,NRN                                        
      COMMON /BL5/ B(561)                                               
      COMMON /BL6/ K,KP                                                 
      COMMON /BL7/ X,JA1T2,JA2T2,JB1T2,JB2T2,LRT2,LT2,L1T2,L2T2,LAT2    
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      NR=MNR                                                            
      N=MN                                                              
      CALL FACBIN                                                       
C                                                                       
C     NR AND LR PERTAIN TO THE RELATIVE COORDINATES                     
C     N AND L PERTAIN TO THE C.M. COORDINATES                           
C     LA=LAMBDA= TOTAL ANGULAR MOMENTUM OF SYSTEM                       
C                                                                       
      NLF1=N1+N1+L1                                                     
      NLF2=N2+N2+L2                                                     
      NLFR=NR+NR+LR                                                     
      NLF=N+N+L                                                         
      IPHI=NLF1+NLF2                                                    
      IPHIP=NLFR+NLF                                                    
      IF(IPHI-IPHIP) 65,55,65                                           
   55 JB1MB2=L1-L2                                                      
      JXX=IABS(JB1MB2)                                                  
      IF(JXX-LA)60,60,65                                                
   60 JB1B2=L1+L2                                                       
      IF(JB1B2-LA) 65,61,61                                             
   61 LRML=LR-L                                                         
      LRMLAB=IABS(LRML)                                                 
      IF(LRMLAB-LA)63,63,65                                             
   63 LRL=LR+L                                                          
      IF(LRL-LA)65,70,70                                                
   65 OB=0.0                                                            
      GOTO 220                                                          
   70 NRN=NR+N                                                          
      NRSUBN=NR-N                                                       
      LRT2=LR+LR                                                        
      LP=L+1                                                            
      LT2S=L+L                                                          
      L1T2=L1+L1                                                        
      L2T2=L2+L2                                                        
  140 IF(LR-1)150,170,170                                               
  150 OBP=0.0                                                           
      JB1T2S=L1T2                                                       
      JB2T2S=L2T2                                                       
      DO 165 LA1P=1,LP                                                  
      LA1=LA1P-1                                                        
      LA2=L-LA1                                                         
      JB1MLA=L1-LA1                                                     
      JB2MLA=L2-LA2                                                     
      JXA1=IABS(JB1MLA)                                                 
      JXA2=IABS(JB2MLA)                                                 
      KMINP=MAX0(JXA1,JXA2)+1                                           
      IF(NRSUBN)155,153,155                                             
  153 KMINPD=KMINP/2                                                    
      IF(KMINPD+KMINPD-KMINP) 155,161,161                               
  155 JB1LA1=L1+LA1                                                     
      JB2LA2=L2+LA2                                                     
      KMAXP=MIN0(JB1LA1,JB2LA2)+1                                       
      NU1=NLF1-LA1                                                      
      NU2=NLF2-LA2                                                      
      IF(KMINP.GT.KMAXP)GOTO 161                                        
      DO 160 KP=KMINP,KMAXP,2                                           
      K=KP-1                                                            
      CALL GEOMET                                                       
      CALL COMBIN                                                       
      OBP=OBP+P*R                                                       
  160 CONTINUE                                                          
  161 CONTINUE                                                          
  165 CONTINUE                                                          
      GOTO 210                                                          
  170 OBPP=0.0                                                          
      LRP=LR+1                                                          
      LRPP=LR*LRT2+LRP                                                  
      LAT2=LA+LA                                                        
      LT2=LT2S                                                          
      DO 200 JA1P=1,LRP                                                 
      JA1=JA1P-1                                                        
      JA2=LR-JA1                                                        
      JA1T2=JA1+JA1                                                     
      JA2T2=JA2+JA2                                                     
      ILRA1=LRPP+JA1T2                                                  
      GSQ=B(ILRA1)                                                      
      G=DSQRT(GSQ)                                                      
      JA1D2=JA1/2                                                       
      JA1SIG=JA1-JA1D2-JA1D2                                            
      SIGA1=1-JA1SIG-JA1SIG                                             
      GSIGA1=G*SIGA1                                                    
      L1A1P=L1+JA1+1                                                    
      L2A2=L2+JA2                                                       
      L1MA1=L1-JA1                                                      
      L2MA2=L2-JA2                                                      
      L1MA1P=IABS(L1MA1)+1                                              
      L2MA2A=IABS(L2MA2)                                                
      CXRP=0.0                                                          
      IF(L1MA1P.GT.L1A1P) GOTO 199                                      
      DO 198 JB1P=L1MA1P,L1A1P,2                                        
      JB1=JB1P-1                                                        
      JB1T2=JB1+JB1                                                     
      JB1T2S=JB1T2                                                      
      LB1=L+JB1                                                         
      LMB1=L-JB1                                                        
      LMB1AB=IABS(LMB1)                                                 
      JB2MIN=MAX0(L2MA2A,LMB1AB)+1                                      
      JB2MAX=MIN0(L2A2,LB1)+1                                           
      IF(JB2MIN.GT.JB2MAX) GOTO 196                                     
      DO 195 JB2P=JB2MIN,JB2MAX,2                                       
      JB2=JB2P-1                                                        
      JB2T2=JB2+JB2                                                     
      JB2T2S=JB2T2                                                      
      JB1B2=JB1+JB2                                                     
      JB1MB2=JB1-JB2                                                    
      CALL CG000(TJ1,JA1,JB1,L1)                                        
      CALL CG000(TJ2,JA2,JB2,L2)                                        
      CALL NINEJ                                                        
      CCX=TJ1*TJ2*X                                                     
      RP=0.0                                                            
      DO 192 LA1P=1,LP                                                  
      LA1=LA1P-1                                                        
      LA2=L-LA1                                                         
      JB1MLA=JB1-LA1                                                    
      JB2MLA=JB2-LA2                                                    
      JXA1=IABS(JB1MLA)                                                 
      JXA2=IABS(JB2MLA)                                                 
      KMINP=MAX0(JXA1,JXA2)+1                                           
      IF(NRSUBN)180,175,180                                             
  175 KMINPD=KMINP/2                                                    
      IF(KMINPD+KMINPD-KMINP)180,191,191                                
  180 JB1LA1=JB1+LA1                                                    
      JB2LA2=JB2+LA2                                                    
      KMAXP=MIN0(JB1LA1,JB2LA2)+1                                       
      NU1=NLF1-JA1-LA1                                                  
      NU2=NLF2-JA2-LA2                                                  
      IF(KMINP.GT.KMAXP) GOTO 191                                       
      DO 190 KP=KMINP,KMAXP,2                                           
      K=KP-1                                                            
      CALL GEOMET                                                       
      CALL COMBIN                                                       
      RP=RP+R*P                                                         
  190 CONTINUE                                                          
  191 CONTINUE                                                          
  192 CONTINUE                                                          
      CXRP=CXRP+CCX*RP                                                  
  195 CONTINUE                                                          
  196 CONTINUE                                                          
  198 CONTINUE                                                          
  199 CONTINUE                                                          
      OBPP=OBPP+CXRP*GSIGA1                                             
  200 CONTINUE                                                          
      XLR=LRT2+1                                                        
      XL=LT2+1                                                          
      XL1=L1T2+1                                                        
      XL2=L2T2+1                                                        
      XLRLSR=DSQRT(XLR*XL)                                              
      LD2=L/2                                                           
      LSIG=L-LD2-LD2                                                    
      SIGL=1-LSIG-LSIG                                                  
      OBP=SIGL*XL1*XL2*XLRLSR*OBPP                                      
  210 PHI=IPHI                                                          
      EX=-(PHI/2.0)                                                     
      C=2.0**EX                                                         
      CALL AFNC(AR,LR,NLFR,2)                                           
      CALL AFNC(A,L,NLF,2)                                              
      CALL AFNC(A1,L1,NLF1,2)                                           
      CALL AFNC(A2,L2,NLF2,2)                                           
      OB=(C*OBP*AR*A)/(A1*A2)                                           
  220 RETURN                                                            
      END                                                               
      SUBROUTINE FACBIN                                                 
C                                                                       
C     B(L) IS THE BINOMIAL COEFFICIENT B(K,M) WHERE L=K*(K+1)/2+M+1     
C     WE ALLOW K TO GET AS LARGE AS 32                                  
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL1/ FL(130)                                              
      COMMON /BL5/ B(561)                                               
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      B(1)=1.0                                                          
      B(2)=1.0                                                          
      B(3)=1.0                                                          
      B(4)=1.0                                                          
      B(5)=2.0                                                          
      B(6)=1.0                                                          
      B(7)=1.0                                                          
      B(8)=3.0                                                          
      B(9)=3.0                                                          
      B(10)=1.0                                                         
      B(11)=1.0                                                         
      B(12)=4.0                                                         
      B(13)=6.0                                                         
      B(14)=4.0                                                         
      B(15)=1.0                                                         
      B(16)=1.0                                                         
      B(17)=5.0                                                         
      B(18)=10.0                                                        
      B(19)=10.0                                                        
      B(20)=5.0                                                         
      B(21)=1.0                                                         
      B(22)=1.0                                                         
      B(23)=6.0                                                         
      B(24)=15.0                                                        
      B(25)=20.0                                                        
      B(26)=15.0                                                        
      B(27)=6.0                                                         
      B(28)=1.0                                                         
      B(29)=1.0                                                         
      B(30)=7.0                                                         
      B(31)=21.0                                                        
      B(32)=35.0                                                        
      B(33)=35.0                                                        
      B(34)=21.0                                                        
      B(35)=7.0                                                         
      B(36)=1.0                                                         
      B(37)=1.0                                                         
      B(38)=8.0                                                         
      B(39)=28.0                                                        
      B(40)=56.0                                                        
      B(41)=70.0                                                        
      B(42)=56.0                                                        
      B(43)=28.0                                                        
      B(44)=8.0                                                         
      B(45)=1.0                                                         
      B(46)=1.0                                                         
      B(47)=9.0                                                         
      B(48)=36.0                                                        
      B(49)=84.0                                                        
      B(50)=126.0                                                       
      B(51)=126.0                                                       
      B(52)=84.0                                                        
      B(53)=36.0                                                        
      B(54)=9.0                                                         
      B(55)=1.0                                                         
      B(56)=1.0                                                         
      B(57)=10.0                                                        
      B(58)=45.0                                                        
      B(59)=120.0                                                       
      B(60)=210.0                                                       
      B(61)=252.0                                                       
      B(62)=210.0                                                       
      B(63)=120.0                                                       
      B(64)=45.0                                                        
      B(65)=10.0                                                        
      B(66)=1.0                                                         
      B(67)=1.0                                                         
      B(68)=11.0                                                        
      B(69)=55.0                                                        
      B(70)=165.0                                                       
      B(71)=330.0                                                       
      B(72)=462.0                                                       
      B(73)=462.0                                                       
      B(74)=330.0                                                       
      B(75)=165.0                                                       
      B(76)=55.0                                                        
      B(77)=11.0                                                        
      B(78)=1.0                                                         
      B(79)=1.0                                                         
      B(80)=12.0                                                        
      B(81)=66.0                                                        
      B(82)=220.0                                                       
      B(83)=495.0                                                       
      B(84)=792.0                                                       
      B(85)=924.0                                                       
      B(86)=792.0                                                       
      B(87)=495.0                                                       
      B(88)=220.0                                                       
      B(89)=66.0                                                        
      B(90)=12.0                                                        
      B(91)=1.0                                                         
C                                                                       
C     HERE WE EXPLICITLY PUT IN BINOMIAL COEFFICIENTS FOR K UP TO 12    
C                                                                       
      DO 85 I=13,32                                                     
      J0=((I*(I+1))/2)+1                                                
      JI=J0+I                                                           
      J1=J0+1                                                           
      JIM1=JI-1                                                         
      J2=J0+2                                                           
      JIM2=JI-2                                                         
      B(J0)=1.0                                                         
      B(JI)=1.0                                                         
      XI=I                                                              
      B(J1)=XI                                                          
      B(JIM1)=XI                                                        
      B(J2)=(XI*(XI-1.0))/2.0                                           
      B(JIM2)=B(J2)                                                     
   85 CONTINUE                                                          
      DO 120 I=13,32                                                    
      ID2=I/2                                                           
      IPAR=I-ID2-ID2+1                                                  
      MMAX=(I-1)/2                                                      
  100 DO 105 M=3,MMAX                                                   
      J1=((I*(I+1))/2)+M+1                                              
      JPR2=J1-I                                                         
      JPR1=JPR2-1                                                       
      J2=J1+I-M-M                                                       
      B(J1)=B(JPR1)+B(JPR2)                                             
      B(J2)=B(J1)                                                       
  105 CONTINUE                                                          
      GOTO(110,120),IPAR                                                
  110 J=((I*(I+2))/2)+1                                                 
      JPR2=J-I                                                          
      JPR1=JPR2-1                                                       
      B(J)=B(JPR1)+B(JPR2)                                              
  120 CONTINUE                                                          
      RETURN                                                            
      END                                                               
      SUBROUTINE GEOMET                                                 
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL1/ FL(130)                                              
      COMMON /BL2/ P,LAT2,LAP,L1L2,L1ML2,L1LA1,L2LA2,L1MLA1,L2MLA2,     
     *   L1T2,L2T2                                                      
      COMMON /BL6/ K,KP                                                 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      L1L2P=L1L2+1                                                      
      L1L2ML=L1L2P-LAP                                                  
      JLX1=L1L2ML/4                                                     
      INT=(L1L2ML/2)-2*JLX1                                             
      SIG=1-INT-INT                                                     
      L1L2LA=L1L2P+LAP                                                  
      L1LAML=LAP+L1ML2                                                  
      L2LAML=LAP-L1ML2                                                  
      KP2=K+2                                                           
      L1LA1K=L1LA1+KP2                                                  
      L2LA2K=L2LA2+KP2                                                  
      L1KMLA=L1MLA1+KP                                                  
      L2KMLA=L2MLA2+KP                                                  
      JDX1=(L1KMLA+1)/2                                                 
      JDX2=(L2KMLA+1)/2                                                 
      JDK1=((K-L1MLA1)/2)+1                                             
      JDK2=((K-L2MLA2)/2)+1                                             
      JLX1=((L1LA1-K)/2)+1                                              
      JLX2=((L2LA2-K)/2)+1                                              
      KL1LA1=L1LA1K/2                                                   
      KL2LA2=L2LA2K/2                                                   
      X=0.5*(FL(L1L2LA)-FL(L1L2ML+1)+FL(L1LAML)+FL(L2LAML))             
      Y1=FL(KL1LA1)-FL(JDX1)-FL(JDK1)-FL(JLX1)                          
      Y2=FL(KL2LA2)-FL(JDX2)-FL(JDK2)-FL(JLX2)                          
      Z=FL(L1KMLA)-FL(L1LA1K)+FL(L2KMLA)-FL(L2LA2K)                     
      WLG=X+Y1+Y2+Z                                                     
      W=DEXP(WLG)                                                       
      XL1B=L1T2+1                                                       
      XL2B=L2T2+1                                                       
      XLAT2P=LAT2+1                                                     
      SRXLAP=DSQRT(XLAT2P)                                              
      P=(SIG*XL1B*XL2B*W)/SRXLAP                                        
      RETURN                                                            
      END                                                               
      SUBROUTINE COMBIN                                                 
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL4/ R,NU1,NU2,NRN                                        
      COMMON /BL5/ B(561)                                               
      COMMON /BL6/ K,KP                                                 
      COMMON /BLCD/ D,IQ                                                
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      R=0.0                                                             
      IQPMAX=MIN0(NU1,NU2)+1                                            
      IF(KP-IQPMAX) 10,10,60                                            
   10 DO 50 IQP=KP,IQPMAX,2                                             
      IQ=IQP-1                                                          
      G=2.0**IQ                                                         
      J=NRN-IQ                                                          
      M=(NU1-IQ)/2                                                      
      L=((J*(J+1))/2)+M+1                                               
      CALL DFN                                                          
      CALL AFNC(A,K,IQ,1)                                               
      R=R+G*B(L)*D*A                                                    
   50 CONTINUE                                                          
   60 RETURN                                                            
      END                                                               
      SUBROUTINE DFN                                                    
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL3/ L,K                                                  
      COMMON /BL5/ B(561)                                               
      COMMON /BLCD/ D,M                                                 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      MK=M-K                                                            
      NPMAX=MIN0(L,M)+1                                                 
      NPMIN=MAX0(0,MK)+1                                                
      D=0.0                                                             
      IF(NPMIN.GT.NPMAX) GOTO 51                                        
      DO 50 NP=NPMIN,NPMAX                                              
      N=NP-1                                                            
      J1=((L*(L+1))/2)+NP                                               
      J2=((K*(K+1))/2)+M-N+1                                            
      ND2=N/2                                                           
      NSIG=N-ND2-ND2                                                    
      SIG=1-NSIG-NSIG                                                   
      D=D+SIG*B(J1)*B(J2)                                               
   50 CONTINUE                                                          
   51 CONTINUE                                                          
      RETURN                                                            
      END                                                               
      SUBROUTINE AFNC(A,K,IQ,J)                                         
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL1/ FL(130)                                              
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      IQ1=IQ+1                                                          
      IF(K-5)50,50,200                                                  
   50 KP=K+1                                                            
      Q=IQ                                                              
      Q1=IQ1                                                            
      GOTO(110,120,130,140,150,160),KP                                  
  110 B=1.0/Q1                                                          
      GOTO 300                                                          
  120 Q2=IQ+2                                                           
      B=3.0/Q2                                                          
      GOTO 300                                                          
  130 Q3=IQ+3                                                           
      B=(5.0*Q)/(Q1*Q3)                                                 
      GOTO 300                                                          
  140 QM1=IQ-1                                                          
      Q2=IQ+2                                                           
      Q4=IQ+4                                                           
      B=(7.0*QM1)/(Q2*Q4)                                               
      GOTO 300                                                          
  150 QM2=IQ-2                                                          
      Q3=IQ+3                                                           
      Q5=IQ+5                                                           
      B=(9.0*Q*QM2)/(Q1*Q3*Q5)                                          
      GOTO 300                                                          
  160 QM1=IQ-1                                                          
      QM3=IQ-3                                                          
      Q2=IQ+2                                                           
      Q4=IQ+4                                                           
      Q6=IQ+6                                                           
      B=(11.0*QM1*QM3)/(Q2*Q4*Q6)                                       
      GOTO 300                                                          
  200 XK1=K+K+1                                                         
      C=2.0**K                                                          
      IQK=IQ+K+2                                                        
      IQKD=IQK/2                                                        
      IQMKD=IQKD-K                                                      
      YLG=FL(IQKD)-FL(IQMKD)+FL(IQ1)-FL(IQK)                            
      Y=DEXP(YLG)                                                       
      B=C*XK1*Y                                                         
  300 GOTO(310,320),J                                                   
  310 A=B                                                               
      GOTO 330                                                          
  320 BLG=DLOG(B)                                                       
      IF(B)224,222,224                                                  
  222 AP=0.0                                                            
      GOTO 228                                                          
  224 CONTINUE                                                          
      APLG=0.5*(BLG-FL(IQ1))                                            
      AP=DEXP(APLG)                                                     
  228 CONTINUE                                                          
      N=(IQ-K)/2                                                        
      ND2=N/2                                                           
      NSIG=N-ND2-ND2                                                    
      SIG=1-NSIG-NSIG                                                   
      A=SIG*AP                                                          
  330 RETURN                                                            
      END                                                               
