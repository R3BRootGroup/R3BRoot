      SUBROUTINE INELASTIC( PLOTFILE )  
C
C          Routine to calculate the inelastic scattering cross 
C          section and print/plot/file it according to options
C          chosen by the user.
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      PARAMETER ( MNA = 181 ) 
C 
      CHARACTER*52   TITLE
      COMMON /STUFF/ TITLE,ICON(16),ANGLE(3),HBARC,AMU,CHSQ, 
     >               DR,Z(2),ZA(2),FM(2),FMA(2),RC(3),FS(2),ECM(2), 
     >               FK(2),FK2(2),ETA(2),FMU(2),ISA,ISB,JA,JB,K, 
     >               LPLUS,MPLUS,NFF,LTRT(8),ISTRT(8),JTRT(8),IBF(8) 
C
C          The dimension 3000 must exceed LPLUS*MPLUS
C
      COMMON /BETAS/ BETAR(3000), BETAI(3000) 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      CHARACTER*(*) PLOTFILE
C
      DIMENSION PLM(3000)
C
      DIMENSION SIGPLT(MNA), SIGSAV(MNA)
C
C      EQUIVALENCE (THETAN,ANGLE(1)),(THETA1,ANGLE(2)),(DTHETA,ANGLE(3)) 
C
      THETAN=ANGLE(1)
      THETA1=ANGLE(2)
      DTHETA=ANGLE(3)

      DO 1 I=1,MNA   
      SIGSAV(I)=0.D0                                                    
    1 CONTINUE                                                          
C                                                                       
C          CALCULATE RADIAL INTEGRALS                                        
C                                                                       
      CALL RADINT                                                       
C                                                                       
C          LOOP OVER FORM FACTORS                                            
C                                                                       
      DO 80 II=1,NFF                                                    
C                                                                       
C          CALCULATE BETA COEFFICIENTS                                       
C                    
      CALL BETA (II) 
      IF( ICON(3).EQ.2 .AND. II.NE.NFF ) GO TO 80
C                                                                       
C          Calculate inelastic cross section.
C
C          Compute incoherent partial cross sections and total
C          if ICON(3)=2; compute single coherent cross section 
C          otherwise; make printer plot of each if ICON(9) is 
C          non-zero.  We add a plotfile output if non-blank 
C          filename was read in.  Use prefix labels of PARTnn, 
C          SUM, and INEL for the three cases.
C                                                                       
      NTHETA=THETAN                                                     
      NTHETA=MIN0(NTHETA,MNA)                                            
      IF( ICON(3) .EQ. 2 ) THEN
        WRITE(6,9003)                        
C        IF( PLOTFILE.NE.' ' ) WRITE(7,9013) TITLE
      ELSE
        WRITE(6,9004)                                 
C        IF( PLOTFILE.NE.' ' ) WRITE(7,9014) II, TITLE
      ENDIF
C      IF( PLOTFILE.NE.' ' ) WRITE(7,9011) NTHETA
C                                                                       
C          NORMALISATION FACTOR                                              
C                                                                       
C          ICON(10).NE.0 GIVES RELATIVISTIC KINEMATICS                       
C                                                                       
      IF( ICON(10) .NE. 0 ) THEN                                            
        FACT=2.D0*FMU(1)/(HBARC*FK(1))**2                                 
        FACT=FACT*2.D0*FMU(2)/(HBARC*FK(2))**2                            
        FACT=FACT*AMU**2
        write(*,*)'hier',FACT                                                  
      ELSE
        FACT=1.D0/(ECM(1)*ECM(2))
        WRITE(*,*)'hier',FACT                                         
      ENDIF
      FACT = FACT * FK(2)/FK(1) * 
     >            DFLOAT((ISB+1)*(JB+1)) / DFLOAT((ISA+1)*(JA+1)) 
C          
C          MULTIPLY BY 10 TO OBTAIN CROSS SECTIONS IN MB/SR                  
C        
      FACT  = 10.D0*FACT                                                   
      THETA = THETA1                                                      
C                                                                       
C          LOOP OVER ANGLES                                                  
C                                                                       
      DO 60 NTH=1,NTHETA                                                
      CALL LGNDR(PLM,MPLUS,LPLUS,THETA)                                 
      SIG=0.D0                                                          
C                                                                       
C          LOOP OVER M OF RELATIVE MOTION AND OUTGOING L 
C
C          (no check of triangles)
C                                                                       
C                                                                       
      DO 50 M1=1,MPLUS                                                  
        M=M1-1                                                            
        SUMR=0.D0                                                         
        SUMI=0.D0                                                         
        INDEX = M*LPLUS                                                     
        ML = M*LPLUS                                                        
        DO 40 LO1=1,LPLUS                                                 
          INDEX = INDEX+1                                                     
          ML = ML+1                                                           
          SUMR = SUMR + BETAR(INDEX)*PLM(ML)       
          SUMI = SUMI + BETAI(INDEX)*PLM(ML)     
   40   CONTINUE                                                          
        TERM=SUMR*SUMR+SUMI*SUMI                                          
        IF(M.NE.0) TERM=2.D0*TERM                                         
        SIG=SIG+TERM                                                      
   50 CONTINUE                                                          
      SIG=FACT*SIG                                                      
      IF( NTH .LE. 51 ) WRITE(6,9001) THETA,SIG   
      IF( PLOTFILE .NE. ' ' ) WRITE(7,9015) THETA, SIG
      SIGPLT(NTH) = SIG                                                   
      SIGSAV(NTH) = SIGSAV(NTH) + SIG                                       
      THETA=THETA+DTHETA                                                
   60 CONTINUE                                                          
C                                                                       
C          PLOT COHERENT CROSS SECTION IF ICON(9).NE.0 
C                                                                       
      IF( ICON(9).NE.0 .AND. ICON(3).EQ.2 ) THEN  
        CALL DWPLOT(NTHETA,ICON(9),SIGPLT,DTHETA,THETA1)                  
      ENDIF  
   80 CONTINUE                                                          
C                                                                       
C     PRINT AND PLOT INCOHERENT SUM OF CROSS SECTIONS                   
C                                                                       
      IF( ICON(3) .NE. 2 ) THEN                                         
        WRITE(6,9002)      
c        IF( PLOTFILE.NE.' ' ) THEN
c          WRITE(7,9012) TITLE
c          WRITE(7,9011) NTHETA 
c        ENDIF
        THETA=THETA1                                                      
        DO 90 NTH=1,NTHETA                                                
          IF( NTH .LE. 51 ) WRITE(6,9001) THETA,SIGSAV(NTH)   
          IF( PLOTFILE.NE.' ' ) WRITE(7,9015) THETA, SIGSAV(NTH) 
          THETA=THETA+DTHETA                                                
   90   CONTINUE                                                          
        IF( ICON(9) .NE. 0 ) THEN   
          CALL DWPLOT(NTHETA,ICON(9),SIGSAV,DTHETA,THETA1)                 
        ENDIF  
      ENDIF
      RETURN                                                            
C
 9001 FORMAT(F8.2,E14.5,7F10.5)                                         
 9002 FORMAT(41H1INCOHERENT SUM OF PARTIAL CROSS SECTIONS // 
     >  '   ANGLE     SIGMA ' /)              
 9003 FORMAT(39H1COHERENT SUM OF PARTIAL CROSS SECTIONS // 
     >  '   ANGLE     SIGMA ' /) 
 9004 FORMAT(38H1PARTIAL CROSS SECTION FOR FORM FACTOR,I3 // 
     >  '   ANGLE     SIGMA ' /)              
C
 9011 FORMAT( I5 )
 9012 FORMAT( 'SUM: ', A )
 9013 FORMAT( 'INEL: ', A )
 9014 FORMAT( 'PART', I2.2, ': ', A )
 9015 FORMAT( 0PF10.5, 1X, 1PE11.3 )
      END                                                               
