      REAL*8 FUNCTION BESSEL(L,X)                                       
C                                                                       
C          SPHERICAL BESSEL FUNCTION  J (X)                                  
C                                      L                                     
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      F0=DSIN(X)/X                                                      
      IF( L .EQ. 0 ) THEN  
        BESSEL=F0                                                         
        RETURN                                                            
      ENDIF
      F1=DSIN(X)/X**2-DCOS(X)/X                                         
      IF( L .EQ. 1 ) THEN  
        BESSEL=F1                                                         
        RETURN                                                            
      ENDIF
      DO 10 N=2,L                                                       
      F2=DFLOAT(2*N-1)*F1/X-F0                                          
      F0=F1                                                             
      F1=F2   
   10 CONTINUE
      BESSEL=F2                                                         
      RETURN                                                            
      END                                                               
      SUBROUTINE TRANSF (L,Q,HA,IMAX,AIN,ANS)                           
C                                                                       
C     L IS THE MULTIPOLARITY                                            
C     THIS SUBROUTINE WAS WRITTEN BY N.M.CLARKE OF KING'S COLLEGE LONDON
C     BASED ON A ROUTINE WRITTEN BY J.R.COMFORT CALLED FTFOR AND THE    
C     ROUTINE EXPAND BY P.D.KUNZ IN DWUCK5                              
C                                                                       
C     THE BESSEL FUNCTION IS EVALUATED DIRECTLY BY NUMERICAL INTEGRATION
C     OF THE DIFFERENTIAL EQUATION, SIMULTANEOUSLY WITH THE INTEGRATION 
C     OF THE FOURIER TRANSFORM.  AN ARBITARY STARTING VALUE IS USED FOR 
C     THE BESSEL FUNCTION AND NORMALISATION CARRIED OUT AT RMAX, USING  
C     THE RECURRENCE RELATIONS.  THE MODIFIED NUMEROV METHOD IS USED    
C     FOR THE NUMERICAL INTEGRATION OF THE BESSEL FUNCTION              
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      DIMENSION AIN(150)                                                
      DIMENSION S1(2),S2(2),C1(2),C2(2),WT(5)                           
      DATA ETA1/1.0D-8/                                                 
      DATA WT/1.30208333D0,2*8.68055556D-1,1.30208333D0,6.59722222D-1/  
      DATA PI /3.14159265358979D0/                                      
C                                      .... fixed NA 
      NA = 5*(IMAX/5)  
C                                                                       
C          SET INITIAL CONDITIONS FOR NUMEROV INTEGRATION                    
C                                                                       
      FLF=DFLOAT(L*(L+1))                                               
      ETA2=0.D0                                                         
      IF(L.EQ.1) ETA2=-ETA1/6.D0                                        
      FLFX=FLF/12.D0                                                    
      AINZ=0.D0                                                         
      PH=DFLOAT(1-2*IABS(L-2*(L/2)))                                    
      IF(L.EQ.0) AINZ=4.D0*(AIN(1)+AIN(3))-6.D0*AIN(2)-AIN(4)           
      SUM=0.D0                                                          
      UZ=1.D0+(Q*HA*Q*HA)/12.D0                                         
C                                                                       
C     SET MORE INITIAL CONDITIONS FOR NUMEROV INTEGRATION               
C                                                                       
      B1=0.D0                                                           
      B2=ETA1                                                           
      P1=ETA2                                                           
      P2=ETA1*(UZ-FLFX)                                                 
      IW=1                                                              
      SUM=B2*AIN(1)*WT(1)*HA                                            
C                                                                       
C     INTEGRATION LOOP                                                  
C                                                                       
      AM=1.D0                                                           
      DO 70 M=2,NA                                                      
      AM=AM+1.D0                                                        
      T1=12.D0*B2-10.D0*P2-P1                                           
      P1=P2                                                             
      P2=T1                                                             
      B1=B2                                                             
      B2=T1/(UZ-FLFX/(AM*AM))                                           
      IW=IW+1                                                           
      SUM=SUM+B2*AIN(M)*AM*HA*WT(IW)                                    
      IF(IW.EQ.5) IW=0                                                  
   70 CONTINUE                                                          
      SUM=SUM-B2*AIN(NA)*WT(5)*0.5D0*AM*HA                              
C                                                                       
C     NOTE THAT THE DO LOOP ABOVE SHOULD FINISH WITH IW=5, SO NA IS     
C     MADE A MULTIPLE OF 5                                              
C                                                                       
C     BESSEL FUNCTIONS FOR L.EQ.-1 AND L.EQ.0                           
C                                                                       
      R2=Q*AM*HA                                                        
      R1=R2-Q*HA                                                        
C                Cook had DR1=DMOD(R1,2*PI) here, Ridge choked on it
      DR1 = R1
      DR2 = R2
      S1(1)=DCOS(DR1)                                                   
      S1(2)=DSIN(DR1)                                                   
      C1(1)=-S1(2)                                                      
      C1(2)=S1(1)                                                       
      S2(1)=DCOS(DR2)                                                   
      S2(2)=DSIN(DR2)                                                   
      C2(1)=-S2(2)                                                      
      C2(2)=S2(1)                                                       
      IF(L.EQ.0) GOTO 85                                                
C                                                                       
C     RECURRENCE OF BESSEL FUNCTION FOR L.GT.0                          
C                                                                       
      FACT=-1.D0                                                        
      DO 80 IL=1,L                                                      
      FACT=FACT+2.D0                                                    
      T1=FACT*S1(2)/R1-S1(1)                                            
      S1(1)=S1(2)                                                       
      S1(2)=T1                                                          
      T1=FACT*C1(2)/R1-C1(1)                                            
      C1(1)=C1(2)                                                       
      C1(2)=T1                                                          
      T2=FACT*S2(2)/R2-S2(1)                                            
      S2(1)=S2(2)                                                       
      S2(2)=T2                                                          
      T2=FACT*C2(2)/R2-C2(1)                                            
      C2(1)=C2(2)                                                       
      C2(2)=T2                                                          
   80 CONTINUE                                                          
   85 CONTINUE                                                          
      S3=S2(1)-DFLOAT(L)*S2(2)/R2                                       
C                                                                       
C     NORMALISE BESSEL FUNCTION                                         
C                                                                       
      DET=DABS(S1(2)*C2(2)-S2(2)*C1(2))                                 
      A1=B1*C2(2)-B2*C1(2)                                              
      A2=B2*S1(2)-B1*S2(2)                                              
      DA1=DABS(A1)                                                      
      DA2=DABS(A2)                                                      
      T1=DMAX1(DA1,DA2)                                                 
      T2=DMIN1(DA1,DA2)                                                 
      T2=T2/T1                                                          
      IF(T2.LT.1.0D-12) T2=0.D0                                         
      FNORM=HA*DET/(T1*DSQRT(1.D0+T2*T2))                               
      SUM=SUM*FNORM                                                     
      ANS=4.0D0*PI*SUM/Q                                                
      RETURN                                                            
      END                                                               
