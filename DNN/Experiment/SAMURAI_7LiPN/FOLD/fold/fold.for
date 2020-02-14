      PROGRAM FOLD   
C
C          Program to form double folded scattering potentials for
C          heavy-ion scattering calculations.  Based on the formalism
C          in the "spin dependence" paper by Petrovich, Philpott, 
C          Carpenter, and Carr (Nucl. Phys. A425 (1984) 609) but 
C          currently limited to only Central and Tensor forces.
C
C                                              VERSION:  08 Dec 1988
C
C          Originally assembled by Julian Cook (11/83) from Petrovich 
C          and Stanley's folding code using Fred's notes.  It has some
C          unfixed bugs related to the isospin dependence of the Z's 
C          for particular transitions not presently of interest.  Other
C          bugs have been fixed and size of code modified by J. A. Carr.
C
C          MODIFIED to add WS options, but still uses particular 
C          hard-wired version of BBML force with specific delta-
C          function exchange numbers.
C
C                                     Original version:  22 April 1988
C
C          SEVERAL MODIFICATIONS to take forces in from file and 
C          compute the exchange strength.  It also has been changed
C          to handle complex interactions and scattering potentials.
C
C          Ported to VMS system at MSU NSCL -- 21 August 1989
C
C          Options to scale densities and control printing added,
C          thereby altering the documentation.
C                                               -- 29 August 1989
C
C ---------------------------------------------------------------------
C
      IMPLICIT REAL*8 (A-H,O-Z)    
C
      PARAMETER ( MNX = 900 )
C
      COMMON /COOK/ H, NR, KEXCH                                          
C
      COMPLEX*16  FORMF
      COMMON /FF/ FORMF(MNX) 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      CHARACTER*8  FILNAM, FILEP, FILET    
      CHARACTER*48 FRCEFILE
C
      DIMENSION IDPF(100),IDPI(100),JPX(100),SPX(2,100),PDENFT(7,2,50)  
      DIMENSION IDTF(100),IDTI(100),JTX(100),STX(2,100),TDENFT(7,2,50)  
C
C          CALCULATE LOG FACTORIALS FOR ANGULAR MOMENTUM ROUTINES            
C                                                                       
      CALL FACLOG                                                       
      CALL LOGFAC                                                       
C
      WRITE(6,801)                                                       
C
C          1. Read the control parameters
C
      READ(5,811) KEXCH, KPUNCH, FILNAM                                     
      WRITE(6,812) KEXCH, KPUNCH, FILNAM                                    
      IF( KPUNCH .GT. 0 ) THEN
C        OPEN( UNIT=8, FILE=FILNAM, STATUS='NEW', FORM='UNFORMATTED')  
        OPEN( UNIT=8, FILE=FILNAM, STATUS='NEW') 
      ENDIF
C
C          2. Read number of steps, step size, E-lab, and A-proj, 
C             along with the print control options
C            
      READ(5,813) NR,H, ELAB,APROJ, IPRTR, IPRTQ, IPRTF
      WRITE(6,814) NR,H, ELAB,APROJ, IPRTR, IPRTQ, IPRTF  
C
      IF( NR .GT. MNX ) THEN
        WRITE(6,891) NR, MNX
        STOP ' -- VALUE OF NR IS TOO LARGE FOR THIS CODE '
      ENDIF
C                                                                       
C          3. Read PROJECTILE transition density  (4 to 5 cards)  
C                                                                       
      WRITE(6,815)                                                       
      CALL INPUT( FJPF,FJPI,PARP,TPF,TPFM,TPI,TPIM,NPROJ,ALPP,     
     >            IDPF,IDPI,JPX,SPX,NTYPEP,FILEP )  
C                                                                       
C          4. Read TARGET transition density   (4 to 5 cards)
C                                                                       
      WRITE(6,816)                                                      
      CALL INPUT( FJTF,FJTI,PART,TTF,TTFM,TTI,TTIM,NTARG,ALPT,     
     >            IDTF,IDTI,JTX,STX,NTYPET,FILET )  
C
C          5. Read the exchange factors and force file to use.
C
      READ(5,826) FNRM1,FNRM2,FNRM3, FRCEFILE
      WRITE(6,827) FRCEFILE, FNRM1,FNRM2,FNRM3 
C
      HBARC = 197.3286D0
      XAMU = 931.5016D0
C      WKLAB = SQRT( (2.*XAMU*ELAB)/(HBARC*HBARC*APROJ) )
C    BY R. ZEGERS FNRM2 is now KA in Love-Franey description
      WKLAB=FNRM2
C
      CALL READT( FRCEFILE, FNRM1,FNRM2,FNRM3, WKLAB, KEXCH )
C                                                                       
C          6. Read number of (Jr,Jp,Jt) form factors to handle
C
      READ(5,817) NFORM                                                 
      WRITE(6,818) NFORM                                                 
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
C          Loop over the NFORM form factors requested
C                                                                       
      DO 250 N=1,NFORM                                                  
      WRITE(6,821) N         
C
C          7. Read the Jr, Jp, Jt, and part of force to use
C
      READ(5,822) JR, JP, JT, KFORCE                                       
      WRITE(6,823) JR, JP, JT, KFORCE                                       
C                                                                       
C          CALCULATE PROJECTILE TRANSITION DENSITY   
C                                                                       
      WRITE(6,824)                                                       
      CALL DENSITY( JP,FJPF,FJPI,PARP, TPF,TPFM,TPI,TPIM,
     >              NPROJ,ALPP, IDPF,IDPI,JPX,SPX, 
     >              PDENFT,NTYPEP,FILEP, IPRTR,IPRTQ )  
C                                                                       
C          CALCULATE TARGET TRANSITION DENSITY     
C                                                                       
      WRITE(6,825)                                                      
      CALL DENSITY( JT,FJTF,FJTI,PART, TTF,TTFM,TTI,TTIM, 
     >              NTARG,ALPT, IDTF,IDTI,JTX,STX,
     >              TDENFT,NTYPET,FILET, IPRTR,IPRTQ )
      DO 220 I=1,NR                                                     
      FORMF(I) = (0.D0,0.D0)                         
  220 CONTINUE
C                                                                       
C          CALCULATE DIRECT PART OF FORM FACTOR  
C          R. ZEGERS: ADDED FNRM1                         
      CALL DIRECT     (JR,JP,JT,KFORCE,                                 
     *     FJPF,FJPI,PARP,TPF,TPFM,TPI,TPIM,PDENFT,                     
     *     FJTF,FJTI,PART,TTF,TTFM,TTI,TTIM,TDENFT,
     *     FNRM1)                     
C                                                                       
C          CALCULATE EXCHANGE PART OF FORM FACTOR 
C                                                                       
      IF( KEXCH .EQ. 2 ) THEN    
        CALL EXCHNG( JR,JP,JT,KFORCE,    
     >     FJPF,FJPI,PARP,TPF,TPFM,TPI,TPIM,NPROJ,ALPP,  
     >     IDPF,IDPI,JPX,SPX,    
     >     FJTF,FJTI,PART,TTF,TTFM,TTI,TTIM,NTARG,ALPT,
     >     IDTF,IDTI,JTX,STX)   
      ENDIF  
C
C          Output the form factor to file if requested
C
C          NOTE that NR and the type of FORMF must match in any
C          other code that reads this file.
C
      IF( IPRTF .NE. 0 ) THEN
        WRITE(6,831)     
        WRITE(6,832) (FORMF(I),I=1,NR)  
      ENDIF
      IF( KPUNCH .GT. 0 ) THEN
        DO I=1,NR 
         WRITE(8,*)FORMF(I)
        ENDDO                        
C WAS: WRITE(8,*)(FORMF(I),I=1,NR)
      ENDIF
  250 CONTINUE                                                          
C                                                                       
C          END OF LOOP OVER JR,JP,JT,LFORCE                                  
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C                                                                       
      IF( KPUNCH .GT. 0 ) THEN 
        CLOSE( UNIT=8, STATUS='KEEP' )    
      ENDIF
      WRITE(6,899) 
      STOP                                                              
C RRRR
  801 FORMAT( 1H1 /// 1X,100(1H*) // 
     > ' DIRECT and EXCHANGE double folded potentials calculated', 
     > ' using harmonic oscillator wavefunctions.'//                         
     > ' Based on old Petrovich and Stanley code and the PPCC', 
     > ' formalism with C+T forces.'//
     > 10X, 'Original using fixed forces  --  J.Cook -- Nov 1983'//
     > 10X, 'UNIX version with bugs fixed  --  J.A.Carr -- Mar 1988'//
     > ' Generalized to higher E with complex t-matrices and Fermi', 
     > ' motion corrections to SNKE in AEA.' //
     > 10X, 'Complete and tested  --  J.A.Carr -- Dec 1988'//
     > 10X, 'VMS version with new options  --  J.A.Carr -- Aug 1989'//
     > 1X,100(1H*)///)        
C
  811 FORMAT(2I5,A8)                                                    
  812 FORMAT(' KEXCH = ',I3,5X,' KPUNCH = ',I3,5X,                      
     *' FILENAME = ',A8///)                                             
  813 FORMAT( I5, 3F10.5, 3I5 )   
  814 FORMAT(' NR = ',I5,5X,'H = ',F8.4,5X,'ELAB = ',F10.3,
     >  5X,'A(PROJ) = ',F8.1 //// ' PRINT OPTIONS: ' //
     >  16X, I2, ' FOR R-SPACE DENSITIES' //  
     >  16X, I2, ' FOR Q-SPACE DENSITIES' // 
     >  16X, I2, ' FOR FORM FACTORS' //)  
  815 FORMAT( '1 PROJECTILE TRANSITION DENSITY' / 2X,29(1H=) /)    
  816 FORMAT( '1 TARGET TRANSITION DENSITY' / 2X,25(1H=) /)     
  817 FORMAT(I5)                                                        
  818 FORMAT(// 5X, '----> ', I5, ' FORM FACTORS TO BE CALCULATED'/)
C RRRR
  821 FORMAT('1' / 1X,34(1H*), '   CALCULATE FORM FACTOR',
     >  I5,'   ' ,34(1H*) ///)                                          
  822 FORMAT(4I5)                                                       
  823 FORMAT(' JR =',I3,5X,'JP =',I3,5X,'JT =',I3,5X,'KFORCE =',I3///)  
  824 FORMAT( '  PROJECTILE TRANSITION DENSITY' / 2X,29(1H=) /)    
  825 FORMAT(/// '  TARGET TRANSITION DENSITY' / 2X,25(1H=) /)     
C
  826 FORMAT( 3F10.5,A )
  827 FORMAT( '1 Force information read from file = ', A /
     >  37X,48(1H-) /// ' Exchange norms are = ', 3F10.5 /)
C
  831 FORMAT( '1' / ' FORM FACTOR' / 1X,12(1H=) /) 
  832 FORMAT(4( ' (', 1PE11.3, ',', 1PE11.3, ') ' ))  
  833 FORMAT(5E14.6)                                                    
C
  891 FORMAT(// 5X, 'ERROR: VALUE OF NR = ', I5, 
     >  ' IS GREATER THAN MAX VALUE OF ', I5 // 
     >  5X, 'PROGRAM TERMINATED WITH MALICE' //)
  899 FORMAT(/// 35(1H*), 3X, 'END OF FOLD CALCULATION', 
     >  3X, 35(1H*) /)
C
      END                                                               
