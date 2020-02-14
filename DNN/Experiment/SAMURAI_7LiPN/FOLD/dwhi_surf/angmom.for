      REAL*8 FUNCTION VCC(JX1,JX2,JX3,MX1,MX2)                          
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      DIMENSION FACT(0:31)                                                
      EQUIVALENCE (FACT(0),F(1))                                        
C
      COMMON /FACTRL/ F(32)                                               
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      VCC=0.0                                                           
      J1=JX1                                                            
      J2=JX2                                                            
      J3=JX3                                                            
      M1=MX1                                                            
      M2=MX2                                                            
      IF(J1.LT.J2) GO TO 20                                             
      IF(J3.LT.J2) GO TO 30                                             
      ICNTR=0                                                           
      GO TO 40                                                          
   20 IF(J3.LT.J1) GO TO 30                                             
      ICNTR=-1                                                          
      IT=J1                                                             
      J1=J2                                                             
      J2=IT                                                             
      IT=M1                                                             
      M1=M2                                                             
      M2=IT                                                             
      GO TO 40                                                          
   30 ICNTR=1                                                           
      IT=J2                                                             
      J2=J3                                                             
      J3=IT                                                             
      M2=-M1-M2                                                         
   40 CONTINUE                                                          
      JZ1=(J1+J2-J3)/2                                                  
      IF(JZ1.LT.0) GO TO 150                                            
      JZ2=(J1+J3-J2)/2                                                  
      IF(JZ2.LT.0) GO TO 150                                            
      JZ3=(J2+J3-J1)/2                                                  
      IF(JZ3.LT.0) GO TO 150                                            
      IF(J1-IABS(M1).LT.0) GO TO 150                                    
      IF(J2-IABS(M2).LT.0) GO TO 150                                    
      IF(J3-IABS(M1+M2).LT.0) GO TO 150                                 
      JT1=(J1-J3+M2)/2                                                  
      JT2=(J2-J3-M1)/2                                                  
      NUMIN=MAX0 (JT1,JT2,0)                                            
      JT3=(J1-M1)/2                                                     
      JT4=(J2+M2)/2                                                     
      NUMAX=MIN0 (JT3,JT4,JZ1)                                          
      JT5=(J2-M2)/2                                                     
      IF(NUMAX.LT.NUMIN) GO TO 150                                      
      J4=J1/2                                                           
      J5=J3/2                                                           
      PHAS=PHASEF(NUMIN)                                                
      DO 100 NU=NUMIN,NUMAX                                             
      VCC=VCC+PHAS      *(YXFCT(JT3-NU,J4)*YXFCT(NU-JT2,J5))            
     1/(FACT(JT4-NU)*FACT(NU-JT1)*FACT(JZ1-NU)*FACT(NU))                
      PHAS=-PHAS                                                        
  100 CONTINUE                                                          
      FCTOR=YXFCT(J4,(J1+M1)/2)*YXFCT(J4,JT3)*YXFCT((J1+J2+J3)/2+1,JZ2)*
     1YXFCT(J5,(J3+M1+M2)/2)*YXFCT(J5,(J3-M1-M2)/2)*FACT(JZ1)*FACT(JZ3)*
     2FACT(JT4)*FACT(JT5)*DFLOAT(J3+1)                                  
      VCC=DSQRT(FCTOR)*VCC                                              
      IF(ICNTR)120,150,110                                              
  110 VCC=VCC*DSQRT(DFLOAT(J2+1)/DFLOAT(J3+1))*PHASEF(JT3)              
      GO TO 150                                                         
  120 VCC=VCC*PHASEF(JZ1)                                               
  150 RETURN                                                            
      END                                                               
      REAL*8 FUNCTION RACAH(J1,J2,J3,J4,J5,J6)                          
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      DIMENSION FACT(0:31)                                                
      EQUIVALENCE (FACT(0),F(1))                                        
C
      COMMON /FACTRL/ F(32)                                               
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      RACAH=0.0                                                         
      Z1=DELR(J1,J2,J5)                                                 
      IF(Z1.EQ.0.0) GO TO 90                                            
      Z1=DELR(J3,J4,J5)*Z1                                              
      IF(Z1.EQ.0.0) GO TO 90                                            
      Z2=DELR(J1,J3,J6)                                                 
      IF(Z2.EQ.0.0) GO TO 90                                            
      Z2=DELR(J2,J4,J6)*Z2                                              
      IF(Z2.EQ.0.0) GO TO 90                                            
      Z1=DSQRT(Z1/Z2)*Z2                                                
      JT1=(J1+J2+J5)/2                                                  
      JT2=(J3+J4+J5)/2                                                  
      JT3=(J1+J3+J6)/2                                                  
      JT4=(J2+J4+J6)/2                                                  
      JZ1=(J1+J2+J3+J4)/2                                               
      JZ2=(J1+J4+J5+J6)/2                                               
      JZ3=(J2+J3+J5+J6)/2                                               
      NUMIN=MAX0 (JT1,JT2,JT3,JT4)                                      
      NUMAX=MIN0 (JZ1,JZ2,JZ3)                                          
      IF(NUMAX.LT.NUMIN) GO TO 90                                       
      PHASE=PHASEF(NUMIN+JZ1)*Z1                                        
      DO 80 NU=NUMIN,NUMAX                                              
      JY1=NU-JT1                                                        
      JY2=NU-JT2                                                        
      JY3=NU-JT3                                                        
      JY4=JZ1-NU                                                        
      JY5=JZ2-NU                                                        
      JY6=JZ3-NU                                                        
      FCTOR=FACT(JY1)*FACT(JY2)*FACT(JY3)*YXFCT(NU+1,NU-JT4)            
     1*FACT(JY4)*FACT(JY5)*FACT(JY6)                                    
      RACAH=RACAH+PHASE/FCTOR                                           
      PHASE=-PHASE                                                      
   80 CONTINUE                                                          
   90 RETURN                                                            
      END                                                               
      REAL*8 FUNCTION WINEJ(J1,J2,J3,J4,J5,J6,J7,J8,J9)                 
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      WINEJ=0.0                                                         
      MUMIN=MAX0(IABS(J1-J9),IABS(J2-J6),IABS(J4-J8))                   
      MUMAX=MIN0(J1+J9,J2+J6,J4+J8)                                     
      IF(MUMAX.LT.MUMIN) GO TO 40                                       
       DO 20 MU=MUMIN,MUMAX,2                                           
      PROD=RACAH(J1,J4,J9,J8,J7,MU)*RACAH(J2,J5,MU,J4,J8,J6)*           
     1     RACAH(J9,MU,J3,J2,J1,J6)*DFLOAT(MU+1)                        
      WINEJ=WINEJ+PROD                                                  
   20 CONTINUE                                                          
      WINEJ=WINEJ*PHASEF((J1+J3+J5+J8)/2+J2+J4+J9)                      
   40 RETURN                                                            
      END                                                               
