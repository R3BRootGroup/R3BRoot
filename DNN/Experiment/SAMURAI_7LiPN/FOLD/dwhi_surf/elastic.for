      SUBROUTINE ELASTIC                                                
C
C          Routine to solve the elastic scattering equations, store
C          the distorted waves, and output relevant information 
C          about the elastic scattering when requested by ICON(5).
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)                                         
C
      PARAMETER ( MNX = 900 )
      PARAMETER ( MNL = 350 )
C
      CHARACTER*52   TITLE
      COMMON /STUFF/ TITLE,ICON(16),ANGLE(3),HBARC,AMU,CHSQ, 
     >               DR,Z(2),ZA(2),FM(2),FMA(2),RC(3),FS(2),ECM(2), 
     >               FK(2),FK2(2),ETA(2),FMU(2),ISA,ISB,JA,JB,K,  
     >               LPLUS,MPLUS,NFF,LTRT(8),ISTRT(8),JTRT(8),IBF(8) 
C
      COMMON /ELSTUFF/ UR(MNX,2),  UI(MNX,2), VR(MNX,2), VI(MNX,2),
     >                 AR(0:MNX),  AI(0:MNX), FR(0:MNX), FI(0:MNX),
     >                 SIGPH(MNL), FC(MNL,2),  GC(MNL,2), 
     >                 TR(MNL),    TI(MNL) 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      DIMENSION SR(MNL), SI(MNL), SL(MNL)
C
      DR2=DR**2/12.D0     
C
      IF( ICON(5) .NE. 1 ) WRITE(6,9001)
C                                                                       
C          LOOP OVER INCOMING AND OUTGOING CHANNELS                          
C                                                                       
      DO 200 N=1,2                                                      
C                                                                       
C          CALCULATE COULOMB PHASES                                          
C                                                                       
      CALL COUPH (N)                                                    
C                                                                       
C          CALCULATE COULOMB WAVEFUNCTIONS                                   
C                                                                       
      CALL COULOM (N)                                                   
C                                                                       
C          LOOP OVER L                                                       
C                                                                       
      DO 50 L1=1,LPLUS                                                  
      L=L1-1                                                            
      FL=DFLOAT(L)                                                      
      FLFACT=FL**2+FL                                                   
C                                                                       
C          CALCULATE STARTING POINT FOR INTEGRATION                          
C
C          Minimum value is MZ = 2; Maximum leaves 20 points at end.
C                                                                       
      NM20=K-20                                                         
      M1=IDINT(DSQRT(FLFACT/12.D0))                                     
      XPO=1.D0/(2.D0*FL+1.D0)                                           
      M2=1.D-16**XPO*2.D0*FL/(2.71828182845905D0*FK(N)*DR)              
      M1=MAX0(M1,M2)+2                                                  
      MZ=MIN0(M1,NM20)                                                  
C                                                                       
C          FORM FUNCTION TO BE INTEGRATED                                    
C                                                                       
      R=0.D0                                                            
      DO 10 M=1,K                                                       
        R=R+DR                                                            
        DRR2=DR2/R**2                                                     
        AR(M)=1.D0+DR2*UR(M,N)-DRR2*FLFACT                                
        AI(M)=     DR2*UI(M,N)                                            
   10 CONTINUE                                                          
      AR( 0) = AR(1)
      AI( 0) = AI(1)
C                                                                       
C          SET WAVEFUNCTION TO ZERO (M=0 is the origin)
C                                                                       
      DO 20 M = 0, K                                                       
        FR(M) = 0.D0                                                        
        FI(M) = 0.D0                                                        
   20 CONTINUE                                                          
C                                                                       
C          CALCULATE STARTING VALUE       
C                                                                       
      FR(MZ-1) = 1.D-8              
C                                                                       
C          FOX-GOODWIN INTEGRATION   
C                                                                       
      DO 30 M=MZ,K                                                      
        BR = 12.D0-10.D0*AR(M-1)                                            
        BI =      -10.D0*AI(M-1)                                            
        CR = BR*FR(M-1)-BI*FI(M-1)-AR(M-2)*FR(M-2)+AI(M-2)*FI(M-2)   
        CI = BI*FR(M-1)+BR*FI(M-1)-AI(M-2)*FR(M-2)-AR(M-2)*FI(M-2)
        DET = AR(M)*AR(M)+AI(M)*AI(M)                                       
        FR(M) = (CR*AR(M)+CI*AI(M))/DET                                     
        FI(M) = (CI*AR(M)-CR*AI(M))/DET                                     
   30 CONTINUE                                                          
C                                                                       
C     MATCH NUCLEAR WAVEFUNCTION TO COULOMB WAVEFUNCTION                
C                                                                       
C     T-MATRIX                                                          
C                                                                       
      BR = GC(L1,1)*FR(K) - FC(L1,1)*FI(K) 
     >                    - GC(L1,2)*FR(K-1) + FC(L1,2)*FI(K-1)
      BI = GC(L1,1)*FI(K) + FC(L1,1)*FR(K) 
     >                    - GC(L1,2)*FI(K-1) - FC(L1,2)*FR(K-1)
      DET = BR*BR+BI*BI                                                   
      BR =  BR/DET                                                         
      BI = -BI/DET                                                        
      CR = FC(L1,2)*FR(K-1) - FC(L1,1)*FR(K)                                
      CI = FC(L1,2)*FI(K-1) - FC(L1,1)*FI(K)                                
      TR(L1) = BR*CR - BI*CI                                                
      TI(L1) = BI*CR + BR*CI                                                
      SR(L1) = 1.0 - 2.0*TI(L1)
      SI(L1) =       2.0*TR(L1)
      SL(L1) = SQRT( SR(L1)*SR(L1) + SI(L1)*SI(L1) )
C                                                                       
C          NORMALISATION                                                     
C                                                                       
      COSPH=DCOS(SIGPH(L1))                                             
      SINPH=DSIN(SIGPH(L1))                                             
      BR=FC(L1,2)+TR(L1)*GC(L1,2)-TI(L1)*FC(L1,2)                       
      BI=         TI(L1)*GC(L1,2)+TR(L1)*FC(L1,2)                       
      DET=FR(K)*FR(K)+FI(K)*FI(K)                                       
      CR=(BR*COSPH-BI*SINPH)/DET                                        
      CI=(BI*COSPH+BR*SINPH)/DET                                        
      ANORMR=CR*FR(K)+CI*FI(K)                                          
      ANORMI=CI*FR(K)-CR*FI(K)                                          
C                                                                       
C     NORMALISE WAVEFUNCTION                                            
C                                                                       
      R=0.D0                                                            
      DO 40 M=1,K                                                       
      R=R+DR                                                            
      TEMP=FR(M)                                                        
      FR(M)=ANORMR*TEMP-ANORMI*FI(M)                                    
      FI(M)=ANORMI*TEMP+ANORMR*FI(M)                                    
C                                                                       
C          Divide by R for INCOMING waves                      
C             
      IF( N .NE. 2 ) THEN
        FR(M)=FR(M)/R                                                     
        FI(M)=FI(M)/R                                                     
C                                                                       
C          Multiply by R for OUTGOING waves              
C           
      ELSE
        FR(M)=FR(M)*R                                                     
        FI(M)=FI(M)*R              
      ENDIF
   40 CONTINUE                                                          
C                                                                       
C          WRITE NORMALISED WAVEFUNCTION TO DISC                             
C                                                                       
      WRITE(8+N,REC=L1) (FR(M),FI(M),M=1,K)                             
   50 CONTINUE                                                          
C
C          End of loop on L
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
C          Write out elastic information:  T-L when ICON is 0 or 2
C
      IF( ICON(5) .NE. 1 ) THEN
        WRITE(6,9006) 
        DO 150 L1 = 1, LPLUS, 5
          L12 = MIN0(L1+4,LPLUS)
          WRITE(6,9005) ((L11-1),TR(L11),TI(L11), L11=L1,L12) 
  150   CONTINUE
        WRITE(6,9006)
      ENDIF
C
C          S-L when ICON is 2
C
      IF( ICON(5) .EQ. 2 ) THEN
        WRITE(6,9006)
        DO 170 L1 = 1, LPLUS, 5
          L12 = MIN0(L1+4,LPLUS)
          WRITE(6,9005) ((L11-1),SR(L11),SI(L11), L11=L1,L12) 
  170   CONTINUE
        WRITE(6,9006)
        DO 180 L1 = 1, LPLUS, 5
          L12 = MIN0(L1+4,LPLUS)
          WRITE(6,9007) ((L11-1),SL(L11), L11=L1,L12) 
  180   CONTINUE
        WRITE(6,9006)
      ENDIF 
C
C          End of loop on incoming and outgoing cases
C
  200 CONTINUE                                                          
      RETURN      
C
 9001 FORMAT('1' // 5X, 'ELASTIC SCATTERING INFO:' /)
 9005 FORMAT(1X, 5(I5,' (',F7.4,', ',F7.4,') ') ) 
 9006 FORMAT( 1X )
 9007 FORMAT(1X, 5(I5,2X,F7.4,11X) ) 
      END                                                               
