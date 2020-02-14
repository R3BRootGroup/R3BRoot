C *   INPUT.FOR
C *
C *      SUBROUTINE INPUT( FJF,FJI,PAR,TF,TFM,TI,TIM,NDEN,ALP, 
C *     >                  IDF,IDI,JX,SX,NTYPE,FILEN ) 
C *
C *
      SUBROUTINE INPUT( FJF,FJI,PAR,TF,TFM,TI,TIM,NDEN,ALP,         
     >                  IDF,IDI,JX,SX,NTYPE,FILEN ) 
C                                                                       
C          READS TRANSITION PARAMETERS                                       
C                                                                       
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      COMMON /ID/ NID(45),LID(45),JID(45)                               
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      INTEGER PARI,PARF                                                 
      DIMENSION IDF(100), IDI(100), JX(100), SX(2,100)    
C
      CHARACTER*(*) FILEN 
C
      CHARACTER*1 LNAME(9), LNAME1, LNAME2 
      DATA LNAME / 'S','P','D','F','G','H','I','J','K' /                  
C
C          Card A -- 
C
      READ(5,801) FJF, PARF, FJI, PARI 
      WRITE(6,802) FJF, PARF, FJI, PARI                                     
      IF( PARF .NE. PARI ) THEN
        PAR = -1.D0                                        
      ELSE
        PAR =  1.D0                                                          
      ENDIF
C                                    Card B -- 
      READ(5,803) TF, TFM, TI, TIM                                          
      WRITE(6,804) TF, TFM, TI, TIM                                         
      MTFM=IDINT(2*TFM+DSIGN(1.D-9,TFM))                                
      MTIM=IDINT(2*TIM+DSIGN(1.D-9,TIM))                                
      MT=(MTFM-MTIM)/2                                                  
      IF( MT .NE. 0 ) THEN    
        IF( TIM .GT. TF ) THEN 
          TM=TFM  
        ELSE
          TM=TIM                 
        ENDIF
        CXFACT = CG(TI,TIM,1.D0,TFM-TIM,TF,TFM) /  
     >           CG(TI,TM ,1.D0,0.D0   ,TF,TM )   
      ENDIF 
C
C          Card C -- 
C
      READ(5,805) NTYPE, KOPTN, ALP                                        
      IF( NTYPE .EQ. 1 ) THEN 
        WRITE(6,811) 
      ELSE IF( NTYPE .EQ. 2 ) THEN 
        WRITE(6,812)                                       
      ELSE IF( NTYPE .EQ. 3 ) THEN
        WRITE(6,813)                                       
      ELSE
        NTYPE = 1
        WRITE(6,811) 
      ENDIF 
      IF( KOPTN .EQ. 1 ) THEN 
        WRITE(6,814)  
      ELSE IF( KOPTN .EQ. 2 ) THEN 
        WRITE(6,815) 
      ELSE IF( KOPTN .EQ. 3 ) THEN 
        WRITE(6,816)  
      ELSE IF( KOPTN .EQ. 4 ) THEN 
        WRITE(6,817)   
      ELSE IF( KOPTN .EQ. 5 ) THEN 
        WRITE(6,818)   
      ELSE
        WRITE(6,819) KOPTN
        STOP ' -- illegal value of KOPTN '
      ENDIF
      WRITE(6,821) ALP     
      WRITE(6,822) MT       
      N=1                                                               
C                                                   Card D -- 
  160 READ(5,823) IDF(N),IDI(N),JX(N),TEMP1,TEMP2                       
      IF( IDF(N).LT.0 .OR. IDI(N).LT.0 ) GO TO 175                           
      N1=NID(IDF(N))+1                                                  
      LNAME1=LNAME(LID(IDF(N))+1)                                       
      J1=JID(IDF(N))                                                    
      N2=NID(IDI(N))+1                                                  
      LNAME2=LNAME(LID(IDI(N))+1)                                       
      J2=JID(IDI(N))                                                    
      IF( KOPTN .EQ. 1 ) THEN
        SX(1,N)=TEMP1                                                     
        SX(2,N)=TEMP2                                                     
      ELSE IF( KOPTN .EQ. 2 ) THEN
        SX(1,N)=(TEMP1+TEMP2)/DSQRT(2.D0)                                 
        SX(2,N)=(TEMP1-TEMP2)/DSQRT(2.D0)                                 
      ELSE IF( KOPTN .EQ. 3 ) THEN
        FACT=DSQRT(2.D0*DFLOAT(J1+1)*(2.D0*FJI+1.D0)/(2.D0*FJF+1.D0))     
        SX(1,N)=FACT*TEMP1                                                
        SX(2,N)=FACT*TEMP2                                                
      ELSE IF( KOPTN .EQ. 4 ) THEN
        FACT=DSQRT(DFLOAT(J1+1)*(2.D0*FJI+1.D0)/(2.D0*FJF+1.D0))          
        SX(1,N)=FACT*(TEMP1+TEMP2)                                        
        SX(2,N)=FACT*(TEMP1-TEMP2)                                        
      ELSE IF( KOPTN .EQ. 5 ) THEN
        FACT=DSQRT(2.D0*DFLOAT(J1+1)/(2.D0*FJF+1.D0))
	TM=TFM-TIM
	T=DABS(TM)     
	FACT=FACT*CG(TI,TIM,T,TM,TF,TFM)*DSQRT((2.D0*T+1.D0)/
     1					    (2.D0*TF+1.D0))
        SX(1,N)=FACT*TEMP1                                                
        SX(2,N)=FACT*TEMP2 
c        write(*,*)CG(TI,TIM,T,TM,TF,TFM)                                 
      ENDIF
c      IF( MT .NE. 0 ) THEN 
c        SX(1,N)=0.D0          
c        SX(2,N)=CXFACT*SX(2,N)
c        write(*,*)CXFACT 
c      ENDIF 
      WRITE(6,824)IDF(N),IDI(N),N1,LNAME1,J1,N2,LNAME2,J2,              
     *  JX(N),TEMP1,TEMP2,SX(1,N),SX(2,N)                               
      N=N+1                                                             
      GOTO 160                                                          
C                     LOOP BACK for next amplitude set 
  175 NDEN=N-1                                                          
C
C          Card E --
C
      IF( ALP .EQ. 0. ) THEN
        READ(5,831) FILEN
        WRITE(6,832) FILEN
      ELSE
        FILEN = ' '
        WRITE(6,833)
      ENDIF
      IF( MT .NE. 0 ) THEN
        WRITE(6,899) CXFACT, CXFACT*CXFACT
      ENDIF
      RETURN                                                            
C
  801 FORMAT(F10.1,A1,F9.1,A1)                                          
  802 FORMAT(' FINAL SPIN  ',F5.1,A1,'      INITIAL SPIN  ',F5.1,A1/)   
  803 FORMAT(4F10.1)                                                    
  804 FORMAT(11X,5HFINAL,14X,7HINITIAL/                                 
     *8X,1HT,9X,2HMT,9X,1HT,9X,2HMT/4F10.1)                             
  805 FORMAT(2I5,F10.0)                                                 
C
  811 FORMAT(/' STATIC DENSITY')         
  812 FORMAT(/' INELASTIC EXCITATION')    
  813 FORMAT(/' CHARGE EXCHANGE')          
  814 FORMAT(/' SPECTROSCOPIC AMPLITUDES ARE S IN ISOSPIN FORM')        
  815 FORMAT(/' SPECTROSCOPIC AMPLITUDES ARE S IN N/P FORM')            
  816 FORMAT(/' SPECTROSCOPIC AMPLITUDES ARE Z IN ISOSPIN FORM')        
  817 FORMAT(/' SPECTROSCOPIC AMPLITUDES ARE Z IN N/P FORM')            
  818 FORMAT(/' SPECTROSCOPIC AMPLITUDES ARE AS DEFINED BY WILDENTHAL')            
  819 FORMAT(/// ' ERROR: value of KOPTN = ', I5, ' illegal' //)
C
  821 FORMAT(/' OSCILLATOR CONSTANT',F10.5)                             
  822 FORMAT(/16H QUANTUM NUMBERS//                                     
     *3X,3HIDF,2X,3HIDI,6X,5HFINAL,7X,7HINITIAL,4X,1HJ,                 
     *4X,6HINPUT1,4X,6HINPUT2,4X,6HS(0,0),3X,4HS(1,I2,1H),/)            
  823 FORMAT(3I5,2F10.5)                                                
  824 FORMAT( 2I5, 2(5X,I3,A1,I2,2H/2), I5, 4F10.5 )                         
C
  831 FORMAT( A )
  832 FORMAT(// 5X, 'Read radial functions from file = ', A /
     >  39X, 16(1H-) /)
  833 FORMAT(// 5X, 'Use Harmonic Oscilator radial functions' //)
C
  899 FORMAT(// ' WARNING: method used for MT not equal to zero
     > is defective for certain'
     >  / '          cases that occur for T-lesser transitions.' 
     >  / '          CXFACT =',F9.5, ', CXFACT**2 =',F9.3//)
C
      END                                                               
