      SUBROUTINE AMP(LA,KS,JA,LB,JB,LR,LFORCE,JR,ANS1)                  
C                                                                       
C          CALCULATES THE COEFFICIENT A IN THE NOTES                         
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL7/ Y,IA,IB,IC,ID,IE,IF,IG,IH,II                         
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      IA=2*LA                                                           
      IB=2*KS                                                           
      IC=2*LB                                                           
      ID=2*KS                                                           
      IE=2*JA                                                           
      IF=2*JB                                                           
      IG=2*LR                                                           
      IH=2*LFORCE                                                       
      II=2*JR                                                           
      CALL NINEJ                                                        
      ANS1=PHAS(JA-LFORCE+LB+KS)*DSQRT(DFLOAT((2*LA+1)*(2*LB+1)*        
     *     (2*LR+1)*(2*JR+1)*(2*KS+1)))*Y                               
      RETURN                                                            
      END                                                               
      SUBROUTINE GS(LAB,KS,JAB,L1PT,J1PT,L2PT,J2PT,ANS23)        
C                                                                       
C          CALCULATES THE COEFFICIENT G(LSJ) IN THE NOTES                    
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BL7/ Z,IA,IB,IC,ID,IE,IF,IG,IH,II                         
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      IA=2*L1PT                                                         
      IB=2*L2PT                                                         
      IC=1                                                              
      ID=1                                                              
      IE=2*LAB                                                          
      IF=2*KS                                                           
      IG=J1PT                                                           
      IH=J2PT                                                           
      II=2*JAB                                                          
      CALL NINEJ                                                        
      ANS23 = DSQRT(DFLOAT((2*JAB+1)*(2*LAB+1)*(2*KS+1)*(J2PT+1)*2))
     >        * Z 
      RETURN                                                            
      END                                                               
      SUBROUTINE HS(LAB,JAB,L1PT,J1PT,L2PT,J2PT,ANS23)              
C                                                                       
C          CALCULATES THE COEFFICIENT H(LSJ) IN THE NOTES                    
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /BLNJR/ RAC,IA,IB,IC,ID,IE,IF                              
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      ANS23 = PHAS((2*L1PT+J1PT-1)/2-LAB-JAB)
     >       *DSQRT(DFLOAT(L2PT*(L2PT+1)*(2*L2PT+1)*(J2PT+1)
     >                                  *(2*JAB+1)*(2*LAB+1)))          
      IA=2*L1PT                                                         
      IB=2*L2PT                                                         
      IC=J1PT                                                           
      ID=J2PT                                                           
      IE=2*JAB                                                          
      IF=1                                                              
      CALL RAC7                                                         
      ANS23 = ANS23*RAC                                                   
      IA=2*LAB                                                          
      IB=2*JAB                                                          
      IC=2*L2PT                                                         
      ID=2*L2PT                                                         
      IE=2                                                              
      IF=2*L1PT                                                         
      CALL RAC7                                                         
      ANS23 = ANS23*RAC                                               
      RETURN                                                            
      END                                                               
      SUBROUTINE GL(LAB,L1PT,L2PT,ANS)                                  
C
C                                  L                                         
C          CALCULATES COEFFICIENT G (L",L') FOR DIRECT PART                  
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      DATA PI /3.14159265358979D0/                                      
      DATA ONE /1.D0/                                                   
C
      ANS=0.0                                                           
      IF(LAB.LT.IABS(L1PT-L2PT).OR.LAB.GT.L1PT+L2PT)RETURN              
      PHASE=PHAS(L1PT+L2PT+LAB)                                         
      IF(PHASE.NE.ONE) RETURN                                           
      CALL CG000(CLB,L2PT,LAB,L1PT)                                     
      CLB=PHAS(L2PT-LAB)*DSQRT(DFLOAT(2*L1PT+1))*CLB                    
      PHASE=PHAS((LAB+L2PT-L1PT)/2)                                     
      ANS=PHASE*DSQRT(DFLOAT(2*L2PT+1)/(4.0*PI))*CLB                    
      RETURN                                                            
      END                                                               
