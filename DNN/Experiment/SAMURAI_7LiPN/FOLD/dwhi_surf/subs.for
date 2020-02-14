      BLOCK DATA                                                        
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
      DATA F   / 1.0000000000D+00, 1.0000000000D+00, 2.0000000000D+00   
     1         , 6.0000000000D+00, 2.4000000000D+01, 1.2000000000D+02   
     2         , 7.2000000000D+02, 5.0400000000D+03, 4.0320000000D+04   
     3         , 3.6288000000D+05, 3.6288000000D+06, 3.9916800000D+07   
     4         , 4.7900160000D+08, 6.2270208000D+09, 8.7178291200D+10   
     5         , 1.3076743680D+12, 2.0922789888D+13, 3.5568742810D+14   
     6         , 6.4023737057D+15, 1.2164510041D+17, 2.4329020082D+18   
     7         , 5.1090942172D+19, 1.1240007278D+21, 2.5852016739D+22   
     8         , 6.2044840173D+23, 1.5511210043D+25, 4.0329146113D+26   
     9         , 1.0888869450D+28, 3.0488834461D+29, 8.8417619937D+30   
     $         , 2.6525285981D+32, 8.2228386542D+33/  
      END                                                               
      REAL*8 FUNCTION PHASEF(N)                                         
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      PHASEF=DFLOAT(1-2*IABS(N-2*(N/2)))
C      write(*,*)'check',PHASEF                                
      RETURN                                                            
      END                                                               
      REAL*8 FUNCTION YXFCT(M,N)                                        
C
C          COMPUTES N-FACT/M-FACT                                              
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      YXFCT=1.0                                                         
      NUMAX=M-N                                                         
      IF( NUMAX .EQ. 0 ) THEN
        YXFCT = 1.
        RETURN
      ELSE IF( NUMAX .GT. 0 ) THEN
        ICTRL=0                                                           
        FCTOR=N                                                           
      ELSE                                                          
        ICTRL=1                                                           
        NUMAX=-NUMAX                                                      
        FCTOR=M                                                           
      ENDIF
      IF( FCTOR .LT. 0. ) THEN
        WRITE(6,891) M, N
        STOP ' -- ERROR IN YXFCT ARGUMENTS '
      ENDIF
      DO 50 NU=1,NUMAX                                                  
      FCTOR=FCTOR+1.0                                                   
      YXFCT=YXFCT*FCTOR                                                 
   50 CONTINUE                                                          
      IF(ICTRL.EQ.0) YXFCT=1.0/YXFCT                                    
      RETURN                                                            
  891 FORMAT(// ' ERROR FOUND IN YXFCT ARGUMENTS ' //
     >  5X, 'M = ', I5, 5X, 'N = ', I5 //
     >  ' PROGRAM TERMINATED WITH MALICE' //)
      END                                                               
      REAL*8 FUNCTION DELR(J1,J2,J3)                                    
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
      JZ1=(J1+J2-J3)/2                                                  
      IF(JZ1.LT.0) GO TO 130                                            
      JZ2=(J1-J2+J3)/2                                                  
      IF(JZ2.LT.0) GO TO 130                                            
      JZ3=(J2+J3-J1)/2                                                  
      IF(JZ3.LT.0) GO TO 130                                            
      JZ4=(J1+J2+J3)/2+1                                                
      IF(JZ3.LT.JZ2) GO TO 80                                           
      IF(JZ3.LT.JZ1) GO TO 70                                           
      DELR=YXFCT(JZ4,JZ3)*FACT(JZ1)*FACT(JZ2)                           
      RETURN                                                            
   70 DELR=YXFCT(JZ4,JZ1)*FACT(JZ2)*FACT(JZ3)                           
      RETURN                                                            
   80 IF(JZ2.LT.JZ1) GO TO 70                                           
      DELR=YXFCT(JZ4,JZ2)*FACT(JZ1)*FACT(JZ3)                           
      RETURN                                                            
  130 DELR=0.0                                                          
      RETURN                                                            
      END                                                               
      SUBROUTINE LGNDR(PLM,MPLUS,LPLUS,THET)                            
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      DIMENSION PLM(3000)                                                
C
      THETA=THET /57.295779                                             
      Y=DCOS(THETA)                                                     
      Z=DSIN(THETA)                                                     
      IX=0                                                              
      DO 100 M=1,MPLUS                                                  
      LX=M-1                                                            
      L2=0                                                              
      P3=1.0                                                            
      FL1=LX                                                            
      IF(LX.EQ.0) GO TO 41                                              
      DO 40 LT=1,LX                                                     
      FL1=FL1+1.0                                                       
      P3=P3*FL1*Z/2.0                                                   
   40 CONTINUE                                                          
   41 P2=0.0                                                            
      FL2=FL1+1.0                                                       
      FL3=1.0                                                           
      DO 90 LT=1,LPLUS                                                  
      IX1=IX+LT                                                         
      IF(L2-LX)50,70,60                                                 
   50 PLM(IX1)=0.0                                                      
      GO TO 75                                                          
   60 P3=(FL2*Y*P2-FL1*P1)/FL3                                          
      FL1=FL1+1.0                                                       
      FL2=FL2+2.0                                                       
      FL3=FL3+1.0                                                       
   70 PLM(IX1)=P3                                                       
      P1=P2                                                             
      P2=P3                                                             
   75 L2=L2+1                                                           
   90 CONTINUE                                                          
      IX=IX+LPLUS                                                       
  100 CONTINUE                                                          
      RETURN                                                            
      END                                                               
      SUBROUTINE DWPLOT(NTH,NLOG,SIGPLT,DTHETA,THETA1)                  
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      PARAMETER ( MNA = 181 )
C
      DIMENSION SIGPLT(MNA) 
C
      CHARACTER*4 DSTORE(25), XDATA(4), SDATA(4), SCALE, BLANK
C
      DATA XDATA / '*   ', ' *  ', '  * ', '   *' /
      DATA SDATA / '*+++', '+*++', '++*+', '+++*' /
      DATA SCALE / '++++' /          
      DATA BLANK / '    ' /         
C
      IF( NLOG .EQ. 0 ) RETURN 
      IF( NTH .EQ. 0 ) THEN
        RETURN   
      ELSE IF( NTH .GT. 55 ) THEN
        NUMTH = 55
      ELSE
        NUMTH = NTH
      ENDIF
      MAXN=0                                                            
      DO 30 I=1,NUMTH                                                     
        IF( SIGPLT(I) .LE. 0.0 ) SIGPLT(I)=1.0E-20                            
        SIGPLT(I)=DLOG10(SIGPLT(I))+100.0                                 
        NDELT=SIGPLT(I)                                                   
        MAXN=MAX0(MAXN,NDELT+1)                                           
   30 CONTINUE                                                          
      MAXN=MAXN-NLOG                                                    
      FMAX=MAXN                                                         
      NDELT=100/NLOG                                                    
      FDELT=NDELT                                                       
C                                             Scale and top axis
      WRITE(6,9002) MAXN-100, MAXN+NLOG-100
C
      DO 40 J=1,25                                                    
        DSTORE(J)=SCALE                                                   
   40 CONTINUE                                                          
      DO 50 I=1,NLOG                                                  
        INDX=I*NDELT                                                      
        ICNT=(INDX-1)/4                                                   
        INDX=INDX-4*ICNT                                                  
        DSTORE(ICNT+1)=SDATA(INDX)                                        
   50 CONTINUE                                                          
      WRITE(6,9000)DSTORE                                               
C                                              Plot area
      DO 70 J=1,25                                                      
        DSTORE(J)=BLANK                                                   
   70 CONTINUE                                                          
      THETA=THETA1                                                      
      DO 100 I=1,NUMTH                                                    
        INDX=(SIGPLT(I)-FMAX)*FDELT+0.5                                   
        ICNT=(INDX-1)/4                                                   
        IF( ICNT .GE. 0 ) THEN
          INDX=INDX-4*ICNT                                                  
          IF(INDX.GT.0) DSTORE(ICNT+1)=XDATA(INDX)      
        ENDIF
        WRITE(6,9001)THETA,DSTORE                                         
        IF(INDX.GT.0) DSTORE(ICNT+1)=BLANK                                
        THETA=THETA+DTHETA                                                
  100 CONTINUE                                                          
C                                                Bottom axis
      DO 110 J=1,25                                                    
        DSTORE(J)=SCALE                                                   
  110 CONTINUE                                                          
      DO 150 I=1,NLOG                                                  
        INDX=I*NDELT                                                      
        ICNT=(INDX-1)/4                                                   
        INDX=INDX-4*ICNT                                                  
        DSTORE(ICNT+1)=SDATA(INDX)                                        
  150 CONTINUE                                                          
      WRITE(6,9000)DSTORE                                               
      RETURN                                                            
C
 9000 FORMAT(12H    THETA  *,25A4) 
 9001 FORMAT(1H ,F8.2 ,3H  +,25A4,1H+)  
 9002 FORMAT('1', 10X, '10**(', I3, ')', 91X, '10**(', I3, ')' /)
      END                                                               
      SUBROUTINE POLFCT(POL,SUMR,SUMI,JSX,JRX)                          
C
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8(A-H,O-Z)                                          
C
      DIMENSION SUMR(5,5),SUMI(5,5),B(2,2),C(3,3),D(3,3,3),TSUM(8)      
     1,POL(4)                                                           
C                                   
C          DATA FOR SIGMA-Y             
C                                 
C     SPIN-1/2                                                          
      DATA B/0.0,-1.0,1.0,0.0/                                          
C                                                                       
C     SPIN-1                                                            
      DATA C/0.0,-.707106781,0.0,.707106781,0.0,-.707106781,0.0,.7071067
     181,0.0/                                                           
C                                                                       
C     DATA FOR TENSOR SIGMA X SIGMA                                     
      DATA D/0.0,0.0,1.732050807,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.22474487
     11,0.0,0.0,0.0,1.224744871,0.0,0.0,0.0,.707106781,0.0,0.0,0.0,-1.41
     24213561,0.0,0.0,0.0,.707106781/                                   
C
      DO 2 I=1,4                                                        
      POL(I)=0.0                                                        
    2 CONTINUE                                                          
      JS=JSX                                                            
      JR=JRX                                                            
      DO 100 I=1,JR                                                     
      DO 90 J=1,JS                                                      
      DO 7 K=1,8                                                        
      TSUM(K)=0.0                                                       
    7 CONTINUE                                                          
      DO 80 K=1,JS                                                      
      IF(JS.GT.3) GO TO 80                                              
      GO TO (10,20,30),JS                                               
   10 TEMP=1.0                                                          
      GO TO 50                                                          
   20 TEMP=B(K,J)                                                       
      GOTO 50                                                           
   30 TEMP=C(K,J)                                                       
   50 CONTINUE                                                          
      T1=SUMR(K,I)                                                      
      T2=SUMI(K,I)                                                      
      TSUM(1)=TSUM(1)-T2*TEMP                                           
      TSUM(2)=TSUM(2)+T1*TEMP                                           
      IF(JS-3)80,71,80                                                  
   71 CONTINUE                                                          
      DO 75 L=1,3                                                       
      LL=L+L+1                                                          
      TEMP=D(K,J,L)                                                     
      TSUM(LL  )=TSUM(LL  )+T1*TEMP                                     
      TSUM(LL+1)=TSUM(LL+1)+T2*TEMP                                     
   75 CONTINUE                                                          
   80 CONTINUE                                                          
      T1=SUMR(J,I)                                                      
      T2=SUMI(J,I)                                                      
      DO 85 L=1,4                                                       
      LL=L+L+1                                                          
      POL(L)=POL(L)+TSUM(LL)*T1+TSUM(LL+1)*T2                           
   85 CONTINUE                                                          
   90 CONTINUE                                                          
  100 CONTINUE                                                          
      RETURN                                                            
      END                                                               
      REAL*8 FUNCTION DFLOAT(N)                                         
C
C
C ---------------------------------------------------------------------
C
      DFLOAT=DBLE(FLOAT(N))                                             
      RETURN                                                            
      END                                                               
