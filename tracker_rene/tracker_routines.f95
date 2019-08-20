module vars
 implicit none
 logical  			:: grafical_output=.true.					! determines if global grafical output is wanted
 logical            :: reference_track							! determines if text output is set during tracking
 logical            :: debug_track=.true.						! if tracking is done in debugging mode
 logical            :: get_new_derivatives						! determines if the derivatives have to be freshly calculated
 logical,parameter  :: derivative_ini_from_file=.false.			! determines if the derivatives ar freshly calculated with track data from file, special use
 logical, allocatable  :: det_hit(:)							! .true. if detector was hit during current track
 logical, allocatable  :: det_passed(:)							! .true. if xy-plane of detector was hit during current track
 character(LEN=256) :: ini_file, output_file(3), geometry_file,magnetic_field_file, derivative_file
 character(LEN=256), parameter :: track_debug_file='single_track.dat'
 character(LEN=256), parameter :: track_debug_gle_file='single_track.gle'
 double precision   :: dt, t, t_total, dt_fine, dt_coarse
 integer,parameter  :: nbr_dimensions=6							! (x,y,z,px,py,pz)
 double precision   :: x(nbr_dimensions), dx(nbr_dimensions), derivative_b_field(nbr_dimensions)
 double precision   :: x_start(nbr_dimensions), p_start(3), x_reference_step(nbr_dimensions) 
 double precision   :: x_track1(nbr_dimensions), x_track2(nbr_dimensions), x_reference(nbr_dimensions)
 double precision   :: b_field(3), v(3)							! magnetic field, velocity
 double precision   :: b_field_position_in_lab(4)				! position of magnetic field coordinate system in lab-frame (x,y,z,rotation_around_y-axis)
 double precision   :: glad_current								! magnetic field map will be multiplied with this number divided by 3584...
 double precision,parameter   :: glad_current_nominal_current=3584.d0		! current for which the field map is calculated
 double precision   :: m_amu, m, m2                				! mass of particle, AMU + SI
 double precision   :: q_e, q              						! charge of particle in elementary unit + SI 
 double precision   :: master_borders(3,2)             			! borders of master volume ((xmin,xmax),(ymin,ymax),(zmin,zmax))
 double precision   :: b_field_borders(3,2)             		! borders of magnetic field ((xmin,xmax),(ymin,ymax),(zmin,zmax))
 double precision   :: spatial_resolution             			! spatial resolution of the entire geometry, given by best detector resolution...
 double precision   :: slope_parameter(4)             			! parameters of the linear slope describing the particle track outside the magnetic field area
 double precision,parameter,dimension(3) :: norm_x = (/1.d0,0.d0,0.d0/)  ! norm-vector in x-direction ...
 double precision,parameter,dimension(3) :: norm_y = (/0.d0,1.d0,0.d0/)  ! norm-vector in y-direction ...
 double precision,parameter,dimension(3) :: norm_z = (/0.d0,0.d0,1.d0/)  ! norm-vector in z-direction ...
 double precision, parameter :: c=299792458.d0					! speed of light, SI units
 double precision, parameter :: c2=c*c	
 double precision, parameter :: e=1.60217662d-19				! elementary charge, SI units
 double precision, parameter :: amu=1.660539040d-27				! atomic mass, SI units
 double precision, allocatable :: b_field_map(:,:,:,:)			! map of magnetic field... (x,y,z,1:3) , last entry for Bx,By,Bz
 integer         , parameter :: b_field_map_resolution=5		! resolution of the field in cm !! , hence the integer ... 
 integer         , parameter :: b_field_resolution_factor=100/b_field_map_resolution		! this is to convert position in m into channels in the b-field map 
 integer  			:: nbr_steps(2), nbr_detectors, nbr_tracks=1000
 double precision, allocatable :: detector_position_in_lab(:,:)			! position of detector coordinate system in lab-frame (detector_ID:x,y,z,rotation_around_y-axis)
 double precision, allocatable :: detector_xy_plane(:,:)				! x-y-plane of detector in lab-frame (detector_ID:4) last 4 values correspond to parameters in: a1*x+a2*y+a3*z+a4=0)
 double precision, allocatable :: detector_track_interactions_lab_frame(:,:)		! intersection of track point with z=0 plane of detector in lab coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable :: detector_track_interactions_det_frame(:,:)		! intersection of track point with z=0 plane of detector in detector coordinates (detector_ID,3) last 3: x,y,z)
 double precision, allocatable :: detector_track_interactions_time(:)		! time of intersection of track point with z=0 plane of detector 
 double precision, allocatable :: detector_track_interactions_path(:)		! flightpath between starting point and intersection of track point with z=0 plane of detector 
 double precision, allocatable :: detector_track_interactions_det_frame_ref(:,:)	! intersection of track point with z=0 plane of detector in detector coordinates (detector_ID,3) last 3: x,y,z), reference track
 double precision, allocatable :: detector_track_interactions_det_frame_ref_step(:,:)	! intersection of track point with z=0 plane of detector in detector coordinates (detector_ID,3) last 3: x,y,z), reference during step-wise initialization 
 double precision, allocatable :: track_lab_frame(:,:)								! experimental track in lab coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable :: track_det_frame(:,:),track_det_frame1(:,:),track_det_frame2(:,:)		! experimental track in detector coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable :: sigma_track(:,:)		                			! absolute unctertainties of experimental track 
 double precision, allocatable :: sigma_track2(:,:)		                			! square of  sigma_track
 double precision              :: pos_target(3),sigma_target(3),sigma_target2(3)    !  target position, absolute uncertainty and it's square (x,y,z)
 logical, allocatable          :: track_hit_pattern(:),track_hit_pattern1(:),track_hit_pattern2(:)							! true, if detector provided data (detector_ID) 
 double precision, allocatable :: detector_range(:,:,:)								! range of detector in detector-frame (detector_ID:x,y,z:min,max,delta)
 double precision, allocatable :: derivative_theta(:),offset_theta(:)				! derivative and offset of x-position in detector-frame (detector_ID) when theta_yz is varied
 double precision, allocatable :: derivative_x(:),offset_x(:)				! derivative and offset of x-position in detector-frame (detector_ID) when theta_yz is varied
 double precision, allocatable :: derivative_q_over_a(:),offset_q_over_a(:)				! derivative and offset of x-position in detector-frame (detector_ID) when q/a of particle is varied
 double precision, allocatable :: derivative_p(:),offset_p(:)				! derivative and offset of x-position in detector-frame (detector_ID) when momentum of particle is varied
 integer          , parameter  :: nbr_variables=5								! variable to be fitted during tracking routine
 double precision, allocatable :: derivative_variable(:,:),offset_variable(:,:)				! derivative and offset of x-position in detector-frame (detector_ID) when any variable is varied
 double precision, allocatable :: offset_ave_x(:)								! x-position in detector-frame (detector_ID) when x or theta_yz is varied (averaged), reference track
 double precision, allocatable :: offset_ave_y(:)								! y-position in detector-frame (detector_ID)  reference track
 character (LEN=10), allocatable :: detector_name(:)					! name of detector (detector_ID)
 double precision, allocatable :: stopping_power(:)					    ! relative stopping power in detector material (detector_ID) (dE/dx / E) for kinetic energy
 integer         , parameter 	:: nbr_channels_position_spectra=1000
 integer         , parameter 	:: nbr_paramters_position_spectra=6			! mean, 1-sigma variance, 2x first channel with content > 0, last channel with content > 0, (1..4 values already converted to in m!!!, 5,6 in channels)
 integer, allocatable			:: detector_position_spectra(:,:,:)					!	spectra about xyz distribution of tracks going through detectors (detector_ID:Dimension(1..x,2..y,3..z):Channel_x,y,z)
 double precision, allocatable	:: detector_position_spectra_paramter(:,:,:)		!	spectra about xyz distribution of tracks going through detectors (detector_ID:Dimension(1..x,2..y,3..z):Paramter_type)
 double precision 				:: x_pos_target, theta_xz_target					! origin of track at target, theta in x-z plane at target
 double precision               :: det
 double precision, allocatable  :: sum_yi(:), sum_xiyi(:),sum_det_xi(:), sum_det_xixi(:), sum_det_tracks(:), det_det(:)
 double precision, allocatable  :: sum_xi(:), sum_xixi(:), sum_tracks(:)
!
!  this is for theta_yz, p, x
! 
 double precision ::  a3_matrix(3,3), a3_matrix_inv(3,3), a5_matrix(5,5), a5_matrix_inv(5,5)
 double precision, allocatable ::  a3_matrix_array(:,:,:)					    ! set of matrizes to retrieve single track parameters from detector positions
 double precision, allocatable ::  a3_matrix_inv_array(:,:,:)					! set of matrizes to retrieve single track parameters from detector positions
 double precision, allocatable ::  t3_matrix_inv_array(:,:,:)					! set of matrizes to retrieve single track parameters from detector positions, target position also constrained
 double precision, allocatable ::  a5_matrix_inv_array(:,:,:)					! set of matrizes to retrieve double track parameters from detector positions
 double precision, allocatable ::  t5_matrix_inv_array(:,:,:)					! set of matrizes to retrieve double track parameters from detector positions, target position also constrained
!
!  this is for theta_xz, y
! 
 double precision ::  a2_matrix(2,2), a2_matrix_inv(2,2)
 double precision, allocatable ::  a2_matrix_array(:,:,:)					    ! set of matrizes to retrieve single track parameters from detector positions
 double precision, allocatable ::  a2_matrix_inv_array(:,:,:)					! set of matrizes to retrieve single track parameters from detector positions
 double precision, allocatable ::  t2_matrix_inv_array(:,:,:)					! set of matrizes to retrieve single track parameters from detector positions, target position also constrained
!
 double precision ::  chi2_single_red, chi2_single,chi2_double_red(3), chi2_double(3)
 double precision ::  theta_yz, y_target
 integer zero_crossing, b_field_map_limits(3,2)
 character*1,parameter, dimension(3) :: ch_xyz=(/'x','y','z'/)
 integer         , parameter 	:: nbr_channels_2d_spectra=200
 integer                        :: spectrum_dthetay_dpx(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 integer                        :: spectrum_dpy_dpx(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 logical                        :: constrain_target_x, constrain_target_y       ! true, if target position is constrained during fitting
end module vars

subroutine init_from_cpp
 use vars
 implicit none
!
 call init
 call get_derivatives
 call initialize_matrizes
end

subroutine single_track_from_cpp(n_det,det_coordinates,x_positions_in,y_positions_in,z_positions_in, hit_pattern_in, &
                                 track_parameter_out, chi_parameter_out)
 use vars
 implicit none
 integer n_det, i
 logical*1 hit_pattern_in(n_det), det_coordinates
 double precision x_positions_in(n_det),track_parameter_out(6), chi_parameter_out(2)
 double precision y_positions_in(n_det)
 double precision z_positions_in(n_det)
 double precision x_local_lab(3), x_local_det(3) 
 character*1 response
!
 if (n_det == nbr_detectors) then
   if (det_coordinates) then
     track_det_frame(:,1)     = x_positions_in (:)
     track_det_frame(:,2)     = y_positions_in (:)
     track_det_frame(:,3)     = z_positions_in (:)
   else
     do i=1,nbr_detectors
       x_local_lab(1) = x_positions_in(i)
       x_local_lab(2) = y_positions_in(i)
       x_local_lab(3) = z_positions_in(i)
       call get_det_position(i,x_local_lab,x_local_det )
       track_det_frame(i,:) = x_local_det
     end do
   end if  
   track_hit_pattern        = hit_pattern_in
!
   call get_single_track_parameter_var5
!
   track_parameter_out(1:3) = x_track1(1:3)
   track_parameter_out(4:6) = x_track1(4:6)/e*c/1.d6
!
   chi_parameter_out(1)     = chi2_single   
   chi_parameter_out(2)     = chi2_single_red
!
   if (debug_track) then
     print*
     print*,'Return values to r3broot'
     print*
     print*,' Position at Target       (m):', track_parameter_out(1:3)
     print*
     print*,' Momentum at Target (MeV/c/q):', track_parameter_out(4:6)
     print*
     print*,' Momentum at Target (MeV/c/q):', chi_parameter_out
   end if
 else
   print*,'Missmatch between number of detectors called from cpp and expected inside fortran', n_det, nbr_detectors
   stop
 end if   
 if (debug_track .and. grafical_output) then
     print*
     write(*,'(A24)', advance='no') ', continue (q = quit) : '
     read(*,*) response
     if ( (response == 'q') ) stop
 end if  

end

subroutine double_track_from_cpp(n_det,x1_in,y1_in,z1_in, hit1_in,x2_in,y2_in,z2_in, hit2_in, &
                                 track1_out,track2_out, chi_parameter_out)
 use vars
 implicit none
 integer n_det, i
 logical*1 hit1_in(n_det),hit2_in(n_det)
 double precision x1_in(n_det),y1_in(n_det),z1_in(n_det)
 double precision x2_in(n_det),y2_in(n_det),z2_in(n_det)
 double precision track1_out(6),track2_out(6),chi_parameter_out(6)
!
 if (n_det == nbr_detectors) then
   track_det_frame1(:,1) = x1_in
   track_det_frame1(:,2) = y1_in
   track_det_frame1(:,3) = z1_in
   track_det_frame2(:,1) = x2_in
   track_det_frame2(:,2) = y2_in
   track_det_frame2(:,3) = z2_in
   track_hit_pattern1    = hit1_in
   track_hit_pattern2    = hit2_in
!
   call get_double_track_parameter_var5
   track1_out(1:6)      = x_track1
   track2_out(1:6)      = x_track2
   chi_parameter_out(1) = chi2_double(1) 
   chi_parameter_out(2) = chi2_double_red(1) 
   chi_parameter_out(3) = chi2_double(2) 
   chi_parameter_out(4) = chi2_double_red(2) 
   chi_parameter_out(5) = chi2_double(3) 
   chi_parameter_out(6) = chi2_double_red(3) 
!   
 else
   print*,'Missmatch between number of detectors called from cpp and expected inside fortran', n_det,    nbr_detectors
   stop
 end if   
end



subroutine called_from_cpp(n,dp)
 use vars
 implicit none
 integer n
 double precision dp
!
 print*,' fortran: ',n,dp
end


subroutine called_from_cpp2(n,dp_in, lo_in, dp_out)
 use vars
 implicit none
 integer n
 logical*1 lo_in(n)
 double precision dp_in(n),dp_out(n)
!
 print*,' fortran: ',n,dp_in
 print*,' fortran: ',n,lo_in
 dp_out = 2*dp_in+1d-8
 lo_in = .true.
 print*,' fortran: ',n,dp_out
 print*,' fortran: ',n,lo_in
 
end

subroutine initialize_matrizes
 use vars
 implicit none
 if (debug_track) print*,'start a2 matrix initialization'
 call initialize_a_t_2
 if (debug_track) print*,'start a3 matrix initialization'
 call initialize_a_t_3
 if (debug_track) print*,'start a5 matrix initialization'
! call initialize_a_t_5
 if (debug_track) print*,'finished matrix initialization'
end

subroutine calc
 use vars
 implicit none
 integer tr, d, local_nbr_tracks, det_off, chx, chy,i
 character (LEN=1) :: response
 double precision GAUSSVER, keep_track(nbr_detectors,3),dxpos(nbr_detectors),dypos(nbr_detectors)
!
 local_nbr_tracks     = nbr_tracks*100000
 local_nbr_tracks     = nbr_tracks*10
 spectrum_dthetay_dpx = 0
 spectrum_dpy_dpx = 0
!
 track_hit_pattern    = .true.
!
! open (unit=21, file='simulation_data/default/tracker_out_vary_start_angle_xz.dat')  ! get tracks data from file
! open (unit=21, file='simulation_data/default/tracker_out_vary_1_over_p.dat')  ! get tracks data from file
! open (unit=21, file='simulation_data/default/tracker_out_vary_start_position.dat')  ! get tracks data from file
! read(21,*) response                                         ! skip one line
 do tr = 1,local_nbr_tracks
   if (debug_track) then
     print*
     write(*,'(A6,I8,A24)', advance='no') 'Track ',tr,', continue (q = quit) : '
     read(*,*) response
     if ( (response == 'q') ) exit
     call get_single_track_data
   else
!     if (1000000*int(tr/1000000) == tr) then
     if (1000*int(tr/1000) == tr) then
       print*,tr , ' of ', local_nbr_tracks, ' tracks'
       if (grafical_output) call make_gle_dthetay_dpx
     end if  
   end if  
!
   call get_single_track_data
   track_hit_pattern = det_hit
   keep_track = track_det_frame
   do d=1, nbr_detectors
     dxpos(d)  = GAUSSVER(sigma_track(d,1))    !  vary x-positions of detector track in each detector
     dypos(d)  = GAUSSVER(sigma_track(d,2))    !  vary x-positions of detector track in each detector
   end do  
   track_det_frame(:,1) = dxpos(:) + keep_track(:,1)
   track_det_frame(:,2) = dxpos(:) + keep_track(:,2)
   call get_single_track_parameter_var5
!
   chy = int(1.d3*datan(x_reference_step(4)/x_reference_step(6)))+nbr_channels_2d_spectra/2            ! theta_yz, 1mrad/channel, 0 at center
   chx = int(1.d3*(x_track1(4)-x_reference_step(4))/x_reference_step(4)  )+ nbr_channels_2d_spectra/2  ! dpx/px, 0.1% / channel, 0 at center
!
   if (chx >= 0 .and. chx <= nbr_channels_2d_spectra .and. chy >= 0 .and. chy <= nbr_channels_2d_spectra) then
     spectrum_dthetay_dpx(chx,chy) = spectrum_dthetay_dpx(chx,chy) + 1
   end if  
!   
   chy = int(1.d3*(x_track1(5)-x_reference_step(5))/x_reference_step(5)  )+ nbr_channels_2d_spectra/2  ! dpy/py, 0.1% / channel, 0 at center
!
   if (chx >= 0 .and. chx <= nbr_channels_2d_spectra .and. chy >= 0 .and. chy <= nbr_channels_2d_spectra) then
     spectrum_dpy_dpx(chx,chy) = spectrum_dthetay_dpx(chx,chy) + 1
   end if  
!   
   if (debug_track) then
     print*
     print*,det_hit
     print*,'Original  :', real(x_reference_step)
     print*,'Tracked   :', real(x_track1)
     print*,'Delta     :', real(x_track1-x_reference_step)
     print*,'Delta-rel :', real(2.d0*(x_track1-x_reference_step)/(x_track1+x_reference_step+1d-100))
     print*     
   end if  
!   
 end do
 call make_gle_dthetay_dpx
 call make_gle_dpy_dpx
! close(21)
end

subroutine make_gle_dpy_dpx
 use vars
 implicit none
 character (LEN=256) :: local_output_file(3), shell_command
 integer slash_position,length, chx, chy, z_max
 real spectrum_dpy_dpx_local(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 logical backwards
!
 backwards = .true.
 slash_position = index(output_file(1),'/',backwards)
 length         = len(trim(output_file(1)))
 local_output_file(1) = output_file(1)(slash_position+1:length) // '_dpy_dpx.z'
 local_output_file(2) = trim(output_file(1)) // '_dpy_dpx.z'
 local_output_file(3) = trim(output_file(1)) // '_dpy_dpx.gle'
!
! write gle file
! 
 z_max = maxval(spectrum_dpy_dpx)
 if (z_max .eq. 0.) then
   print*,'no data in spectrum_dpy_dpx'
 else  
 spectrum_dpy_dpx_local = float(spectrum_dpy_dpx) / float(z_max)
 open (unit=30, file=local_output_file(3))
   write(30,*) 'size 16 16'
   write(30,*) 'include "color.gle"'
   write(30,*) 'amove 1 1'
   write(30,*) 'begin graph'
   write(30,*) ' size 14 14'
   write(30,*) ' xtitle "dp_{x}/p_{x} (%)" hei 0.6'
   write(30,*) ' ytitle "dp_{y}/p_{y} (%)" hei 0.6'
   write(30,*) ' title "dp_{x} vs.dp_{y}" '
!   write(30,*) ' colormap "',trim(local_output_file(1)),'" 500 500 zmin 0 zmax 1 color ! palette palette_rene '
   write(30,*) ' colormap "',trim(local_output_file(1)),'" 500 500 zmin 0 zmax 1 palette palette_rene '
   write(30,*) 'end graph'
   write(30,*) 'amove xg(xgmax)+0.3 yg(ygmin)'
   write(30,*) 'color_range_vertical zmin 0 zmax 1 zstep 0.1 format "fix 1" palette palette_rene'
   write(30,*) 'sub palette_rene z'
   write(30,*) 'local r = 1'
   write(30,*) 'local g = 1'
   write(30,*) 'local b = 1'
   write(30,*) 'if (z > 0.0) then b = 1-z*z'
   write(30,*) 'if (z > 0.0) then r = sqrt(z)'
   write(30,*) 'if (z > 0.0) then g = 0'
   write(30,*) 'return rgb(r,g,b)'
   write(30,*) 'end sub'
 close(30)
!
! write data file
! 
 open (unit=30, file=local_output_file(2))
   write(30,*) '! nx ',nbr_channels_2d_spectra+1,' ny ',nbr_channels_2d_spectra+1, 'xmin -10 xmax 10 ymin -10 ymax 10'
   do chy=0,nbr_channels_2d_spectra
     do chx=0,nbr_channels_2d_spectra
       write(30,'(F14.10)', ADVANCE='NO') spectrum_dpy_dpx_local(chx,chy)
     end do
     write(30,*)''
   end do  
 close(30)
!
! execute gle to create pdf and jpg
! 
 shell_command = 'gle -d pdf ' // trim(local_output_file(3)) // '  >/dev/null'
 if (grafical_output) call execute_command_line (shell_command)
 shell_command = 'gle -d jpg -resolution 400 ' // trim(local_output_file(3)) // '  >/dev/null'
 ! if (grafical_output) call execute_command_line (shell_command)
 end if
! 
end

subroutine make_gle_dthetay_dpx
 use vars
 implicit none
 character (LEN=256) :: local_output_file(3), shell_command
 integer slash_position,length, chx, chy, z_max
 real spectrum_dthetay_dpx_local(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 logical backwards
!
 backwards = .true.
 slash_position = index(output_file(1),'/',backwards)
 length         = len(trim(output_file(1)))
 local_output_file(1) = output_file(1)(slash_position+1:length) // '_dthetay_dpx.z'
 local_output_file(2) = trim(output_file(1)) // '_dthetay_dpx.z'
 local_output_file(3) = trim(output_file(1)) // '_dthetay_dpx.gle'
!
! write gle file
! 
 z_max = maxval(spectrum_dthetay_dpx)
 spectrum_dthetay_dpx_local = float(spectrum_dthetay_dpx) / float(z_max)
 open (unit=30, file=local_output_file(3))
   write(30,*) 'size 16 16'
   write(30,*) 'include "color.gle"'
   write(30,*) 'amove 1 1'
   write(30,*) 'begin graph'
   write(30,*) ' size 14 14'
   write(30,*) ' xtitle "dp_{x}/p_{x} (%)" hei 0.6'
   write(30,*) ' ytitle "\theta_{yz} (mrad)" hei 0.6'
   write(30,*) ' title "dp_{x} vs. \theta_{yz}" '
!   write(30,*) ' colormap "',trim(local_output_file(1)),'" 500 500 zmin 0 zmax 1 color ! palette palette_rene '
   write(30,*) ' colormap "',trim(local_output_file(1)),'" 500 500 zmin 0 zmax 1 palette palette_rene '
   write(30,*) 'end graph'
   write(30,*) 'amove xg(xgmax)+0.3 yg(ygmin)'
   write(30,*) 'color_range_vertical zmin 0 zmax 1 zstep 0.1 format "fix 1" palette palette_rene'
   write(30,*) 'sub palette_rene z'
   write(30,*) 'local r = 1'
   write(30,*) 'local g = 1'
   write(30,*) 'local b = 1'
   write(30,*) 'if (z > 0.0) then b = 1-z*z'
   write(30,*) 'if (z > 0.0) then r = sqrt(z)'
   write(30,*) 'if (z > 0.0) then g = 0'
   write(30,*) 'return rgb(r,g,b)'
   write(30,*) 'end sub'
 close(30)
!
! write data file
! 
 open (unit=30, file=local_output_file(2))
   write(30,*) '! nx ',nbr_channels_2d_spectra+1,' ny ',nbr_channels_2d_spectra+1, 'xmin -10 xmax 10 ymin -80 ymax 80'
   do chy=0,nbr_channels_2d_spectra
     do chx=0,nbr_channels_2d_spectra
       write(30,'(F14.10)', ADVANCE='NO') spectrum_dthetay_dpx_local(chx,chy)
     end do
     write(30,*)''
   end do  
 close(30)
!
! execute gle to create pdf and jpg
! 
 shell_command = 'gle -d pdf ' // trim(local_output_file(3)) // '  >/dev/null'
 if (grafical_output) call execute_command_line (shell_command)
 shell_command = 'gle -d jpg -resolution 400 ' // trim(local_output_file(3)) // '  >/dev/null'
 if (grafical_output) call execute_command_line (shell_command)
! 
end

subroutine get_double_track_parameter_var5  ! determines theta, p, x for the current set of track-detector data
 use vars
 implicit none
 integer detector, i,j,k,det_pattern_id,pattern_id
 integer, parameter :: n_var_local = 5	! e.g.  d_x, d_theta, d_1_over_p
 double precision b_vector(n_var_local), p_vector(n_var_local)   ! we will solve a*b = p
 double precision v1(nbr_detectors),v2(nbr_detectors), dth 
 logical local_hit_pattern(2*nbr_detectors)
!
! nbr_of_detectors_hit = count(track_hit_pattern)
 b_vector = 0.d0
!
! populate b, a with data from current track
! 
 v1 = (track_det_frame1(:,1) - offset_ave_x)/sigma_track2(:,1)  !  (:,1)  is the x-component
 v2 = (track_det_frame2(:,1) - offset_ave_x)/sigma_track2(:,1)  !  (:,1)  is the x-component
 do detector=1,nbr_detectors
   if (track_hit_pattern1(detector) ) then 
     b_vector(1:2)     = b_vector(1:2) + v1(detector)*derivative_variable(detector,1:2) 	
     b_vector(5)       = b_vector(5) + (v1(detector))*derivative_variable(detector,3) 
   end if
   if (track_hit_pattern2(detector) ) then 
     b_vector(3:4)     = b_vector(3:4) + v2(detector)*derivative_variable(detector,1:2) 	
     b_vector(5)       = b_vector(5) + (v2(detector))*derivative_variable(detector,3) 
   end if
 end do    
 local_hit_pattern(1:nbr_detectors)                 = track_hit_pattern1
 local_hit_pattern(1+nbr_detectors:2*nbr_detectors) = track_hit_pattern2
 pattern_id = det_pattern_id(2*nbr_detectors,local_hit_pattern)
 
! 
 p_vector = matmul(a5_matrix_inv_array(pattern_id,:,:),b_vector)
 
!
! dtheta
!
 dth           = p_vector(1)
 x_track1(4)   = x_reference(4)*dcos(dth) + x_reference(6)*dsin(dth)
 x_track1(6)   = x_reference(6)*dcos(dth) - x_reference(4)*dsin(dth)
 x_track1(5)   = x_reference(5)                    ! y-component
! 
 dth           = p_vector(3)
 x_track2(4)   = x_reference(4)*dcos(dth) + x_reference(6)*dsin(dth)
 x_track2(6)   = x_reference(6)*dcos(dth) - x_reference(4)*dsin(dth)
 x_track2(5)   = x_reference(5)                    ! y-component
!
! dp
!
 x_track1(4:6) = x_track1(4:6)/(p_vector(2)+1.d0)           !  relative change of p, length of start momentum
 x_track2(4:6) = x_track2(4:6)/(p_vector(4)+1.d0)           !  relative change of p, length of start momentum
!
! dx
!
 x_track1(1)   = x_reference(1) + p_vector(5)      ! x-position on target
 x_track1(2)   = x_reference(2)                    ! y-position on target
 x_track1(3)   = x_reference(3)                    ! z-position on target
!
 x_track2(1)   = x_reference(1) + p_vector(5)      ! x-position on target
 x_track2(2)   = x_reference(2)                    ! y-position on target
 x_track2(3)   = x_reference(3)                    ! z-position on target
!
 detector_track_interactions_det_frame(:,1) = offset_ave_x
 detector_track_interactions_det_frame(:,1) = detector_track_interactions_det_frame(:,1) + &
                                                derivative_variable(:,1)*p_vector(1)
 detector_track_interactions_det_frame(:,1) = detector_track_interactions_det_frame(:,1) + &
                                                derivative_variable(:,2)*p_vector(2)
 detector_track_interactions_det_frame(:,1) = detector_track_interactions_det_frame(:,1) + &
                                                derivative_variable(:,3)*p_vector(5)
! 
 v1        = (detector_track_interactions_det_frame(:,1) - track_det_frame1(:,1) )/sigma_track(:,1)
 chi2_double(1)      = dot_product(v1,v1)
 chi2_double_red(1)  = chi2_double(1)/(nbr_detectors - 3)
! 
 detector_track_interactions_det_frame(:,1) = offset_ave_x
 detector_track_interactions_det_frame(:,1) = detector_track_interactions_det_frame(:,1) + &
                                                derivative_variable(:,1)*p_vector(3)
 detector_track_interactions_det_frame(:,1) = detector_track_interactions_det_frame(:,1) + &
                                                derivative_variable(:,2)*p_vector(4)
 detector_track_interactions_det_frame(:,1) = detector_track_interactions_det_frame(:,1) + &
                                                derivative_variable(:,3)*p_vector(5)
! 
 v1        = (detector_track_interactions_det_frame(:,1) - track_det_frame2(:,1) )/sigma_track(:,1)
 chi2_double(2)      = dot_product(v1,v1)
 chi2_double_red(2)  = chi2_double(2)/(nbr_detectors - 3)
!
 chi2_double(3)      = chi2_double(2)+chi2_double(1)
 chi2_double_red(3)  = chi2_double(3)/(2*nbr_detectors - 5)
! 
 if (debug_track) then
   print*
   print*,'p_vector:               ',real(p_vector)
   print*
   print*,'reference track params: ',real(x_reference)
   print*,'      new track params: ',real(x_track1)
   print*
   print*,'linear track points: ',real(detector_track_interactions_det_frame(:,1))
   print*,'det values:          ',real(track_det_frame(:,1))
   print*,'diff to det values:  ',real(track_det_frame(:,1)-detector_track_interactions_det_frame(:,1))
   print*,'diff to ref track:   ',real(offset_ave_x-detector_track_interactions_det_frame(:,1))
   print*
   print*,'chi-square:          ',real(chi2_single)
   print*,'reduced chi-square:  ',real(chi2_single_red)
   print*
   x_start = x_track1
   v1      = detector_track_interactions_det_frame(:,1)
   if (grafical_output) then
     open (unit = 22, file=trim(track_debug_file))
       call single_track
     close(22)  
     call make_gle_single_track
     call make_single_track_pic
   end if  
   print*,'R-K-track points   : ',real(detector_track_interactions_det_frame(:,1))
   print*,'diff to det values:  ',real(track_det_frame(:,1)-detector_track_interactions_det_frame(:,1))
   print*,'diff to linear track:',real(v1-detector_track_interactions_det_frame(:,1))
   print*
   detector_track_interactions_det_frame(:,1) = v1
   x_start = x_reference
 end if   
end

subroutine get_single_track_parameter_var5  ! determines theta_xz, theta_yz, p, x, theta_yz, y for the current set of track-detector data
 use vars
 implicit none
 double precision theta_correction, d_track_x(nbr_detectors)
!
 call get_single_track_y
!
 theta_correction = 1.d0-1.d0/dcos(theta_yz)
!
 d_track_x = derivative_variable(1:nbr_detectors,4)*theta_correction
 if (debug_track) then
   print*,'derivative      ',derivative_variable(1:nbr_detectors,4)
   print*,'theta-correction',d_track_x
   print*,'track           ',track_det_frame(:,1)
 end if  
 track_det_frame(:,1) = track_det_frame(:,1) - d_track_x
!  
 call get_single_track_parameter_var3 
!
end


subroutine get_single_track_y  ! theta_yz, y for the current set of track-detector data
 use vars
 implicit none
 integer detector, i,j,k,det_pattern_id,pattern_id
 integer, parameter :: n_var_local = 2	! e.g.  d_y, y
 double precision b_vector(n_var_local), p_vector(n_var_local)   ! we will solve a*p = b
 double precision v1(nbr_detectors)
!
! quick & dirty fix for the y-component. Will need to change this later.
!   
 if (debug_track) then
   y_target = 0.
   do i=nbr_detectors,1,-1
     if (track_det_frame(i,2) .ne. 0. .and. track_hit_pattern(i) ) exit
   end do
   if (i.eq.0) then
     theta_yz = 0.
   else   
     theta_yz = dasin(track_det_frame(i,2)) / detector_track_interactions_path(i)
     print*,'Quick fix 0: detector',i,' ',detector_name(i)
     print*,'Quick fix 1: theta_yz, y_target', theta_yz, y_target
     theta_yz = dasin(track_det_frame(i,2)) / derivative_variable(i+nbr_detectors,4)
     print*,'Quick fix 2: theta_yz, y_target', theta_yz, y_target
   end if  
 end if    
!
 b_vector = 0.d0
!
! populate b, a with data from current track
! 
 v1 = (track_det_frame(:,2) - offset_ave_y)/sigma_track2(:,2)  !  (:,2)  is the y-component
 do detector=1,nbr_detectors
   if (track_hit_pattern(detector) ) then 
     b_vector          = b_vector + v1(detector)*derivative_variable(detector+nbr_detectors,4:3+n_var_local) 	
   end if
 end do  
!
! get the correct matrix
!
 pattern_id = det_pattern_id(nbr_detectors,track_hit_pattern)
!
! calculate p
! 
 if (constrain_target_y) then
   b_vector(n_var_local) = b_vector(n_var_local) + pos_target(2)/sigma_target2(2)
   p_vector = matmul(t2_matrix_inv_array(pattern_id,:,:),b_vector)
 else
   p_vector = matmul(a2_matrix_inv_array(pattern_id,:,:),b_vector)
 end if  
!
 theta_yz = p_vector(1)
 y_target = p_vector(2)
!
 if (debug_track) then
   print*,'2x2 Matrix fix: theta_yz, y_target', theta_yz, y_target
 end if  
end

subroutine get_single_track_parameter_var3  ! determines theta_xz, p, x for the current set of track-detector data
 use vars
 implicit none
 integer detector, i,j,k,det_pattern_id,pattern_id, nbr_detectors_hit
 integer, parameter :: n_var_local = 3	! e.g.  d_x, d_theta, d_1_over_p
 double precision b_vector(n_var_local), p_vector(n_var_local)   ! we will solve a*p = b
 double precision v1(nbr_detectors), dth 
!
! nbr_of_detectors_hit = count(track_hit_pattern)
 b_vector = 0.d0
!
! populate b, a with data from current track
! 
 v1 = (track_det_frame(:,1) - offset_ave_x)/sigma_track2(:,1)  !  (:,1)  is the x-component
 do detector=1,nbr_detectors
   if (track_hit_pattern(detector) ) then 
     b_vector     = b_vector + v1(detector)*derivative_variable(detector,1:n_var_local) 	
   end if
 end do    
!
! get the correct matrix
!
 pattern_id = det_pattern_id(nbr_detectors,track_hit_pattern)
!
! calculate p
! 
 if (constrain_target_x) then
   b_vector(n_var_local) = b_vector(n_var_local) + pos_target(1)/sigma_target2(1)
   p_vector = matmul(t3_matrix_inv_array(pattern_id,:,:),b_vector)
 else  
   p_vector = matmul(a3_matrix_inv_array(pattern_id,:,:),b_vector)
 end if  
!
! dtheta
!
 dth           = p_vector(1)
 x_track1(4)   = x_reference(4)*dcos(dth) + x_reference(6)*dsin(dth)
 x_track1(6)   = x_reference(6)*dcos(dth) - x_reference(4)*dsin(dth)
!
! dp
!
 x_track1(4:6) = x_track1(4:6)/(p_vector(2)+1.d0)           !  relative change of p, length of start momentum
 x_track1(5)   = x_track1(6)*dsin(theta_yz)                  ! y-component
!
! dx
!
 x_track1(1)   = x_reference(1) + p_vector(3)      ! x-position on target
 x_track1(2)   = y_target                          ! y-position on target
 x_track1(3)   = x_reference(3)                    ! z-position on target
!
! detector_track_interactions_det_frame(:,1) = detector_track_interactions_det_frame_ref(:,1)
 detector_track_interactions_det_frame(:,1) = offset_ave_x
 do k=1,n_var_local
   detector_track_interactions_det_frame(:,1) = detector_track_interactions_det_frame(:,1) + &
                                                derivative_variable(:,k)*p_vector(k)
 end do
! 
 v1          = (detector_track_interactions_det_frame(:,1) - track_det_frame(:,1) )/sigma_track(:,1)
 chi2_single = 0.d0
 do i=1,nbr_detectors
   if (track_hit_pattern(i)) chi2_single = v1(i)*v1(i) + chi2_single
 end do  
 nbr_detectors_hit = count(track_hit_pattern)
 if (nbr_detectors_hit >n_var_local ) then
   chi2_single_red  = chi2_single/(nbr_detectors - n_var_local)
 else
   chi2_single_red = 0.d0
 end if  
! 
 if (debug_track) then
   print*
   print*,'p_vector:               ',real(p_vector)
   print*
   print*,'reference track params: ',real(x_reference)
   print*,'      new track params: ',real(x_track1)
   print*
   print*,'track_hit_pattern: ',track_hit_pattern
   print*,'linear track points: ',real(detector_track_interactions_det_frame(:,1))
   print*,'det values:          ',real(track_det_frame(:,1))
   print*,'diff to det values:  ',real(track_det_frame(:,1)-detector_track_interactions_det_frame(:,1))
   print*,'diff to ref track:   ',real(offset_ave_x-detector_track_interactions_det_frame(:,1))
   print*
   print*,'chi-square:          ',real(chi2_single)
   print*,'reduced chi-square:  ',real(chi2_single_red)
   print*,'number of detectors: ',nbr_detectors_hit
   print*
   x_start = x_track1
   v1      = detector_track_interactions_det_frame(:,1)
   if (grafical_output) then
     open (unit = 22, file=trim(track_debug_file))
       call single_track
     close(22)  
     call make_gle_single_track
     call make_single_track_pic
   end if  
   print*,'track_hit_pattern: ',track_hit_pattern
   print*,'R-K-track points   : ',real(detector_track_interactions_det_frame(:,1))
   print*,'diff to det values:  ',real(track_det_frame(:,1)-detector_track_interactions_det_frame(:,1))
   print*,'diff to linear track:',real(v1-detector_track_interactions_det_frame(:,1))
   print*
   detector_track_interactions_det_frame(:,1) = v1
   x_start = x_reference
 end if   
end

subroutine initialize_a_t_2  ! initializes the matrizes necessary to get the linear optimization of the y-tracks
 use vars
 implicit none
 integer, parameter :: n_var_local = 2
 integer i,j,k,detector, pattern_id
 double precision e_vector(n_var_local),x_vector(n_var_local), einheit(n_var_local,n_var_local)
 logical local_hit_pattern(nbr_detectors)
!
 do pattern_id=1,2**nbr_detectors-1
   a2_matrix = 0.
   call get_hit_pattern(nbr_detectors,pattern_id,local_hit_pattern)
   do k=1, n_var_local
     do j=1, n_var_local
       do detector=1,nbr_detectors
         if (local_hit_pattern(detector) ) then
           a2_matrix(k,j) =  a2_matrix(k,j) + &
             derivative_variable(detector+nbr_detectors,j+3) * &
             derivative_variable(detector+nbr_detectors,k+3)/sigma_track2(detector,2)
         end if  
       end do  
     end do
   end do  
!
   call invert_matrix(n_var_local,a2_matrix,a2_matrix_inv)
   
   a2_matrix_inv_array(pattern_id,:,:) = a2_matrix_inv(:,:)
   a2_matrix_array(pattern_id,:,:)     = a2_matrix(:,:)
!
   a2_matrix(n_var_local,n_var_local) = a2_matrix(n_var_local,n_var_local) + 1.d0/sigma_target2(2)
!
!
   do k=1, n_var_local
     e_vector    = 0.d0
     e_vector(k) = 1.d0
     call solve_linear_equation_system(n_var_local,a2_matrix,e_vector,x_vector)
     a2_matrix_inv(:,k) = x_vector
   end do
   t2_matrix_inv_array(pattern_id,:,:) = a2_matrix_inv(:,:)
 end do  
end

subroutine initialize_a_t_3  ! initializes the matrizes necessary to get the linear optimization of the x-tracks
 use vars
 implicit none
 integer, parameter :: n_var_local = 3
 integer i,j,k,detector, pattern_id
 double precision e_vector(n_var_local),x_vector(n_var_local), einheit(n_var_local,n_var_local)
 logical local_hit_pattern(nbr_detectors)
!
 do pattern_id=1,2**nbr_detectors-1
   a3_matrix = 0.
   call get_hit_pattern(nbr_detectors,pattern_id,local_hit_pattern)
 !  print*,'a3-ini',pattern_id, local_hit_pattern
   do k=1, n_var_local
     do j=1, n_var_local
       do detector=1,nbr_detectors
         if (local_hit_pattern(detector) ) then
           a3_matrix(k,j) =  a3_matrix(k,j) + &
             derivative_variable(detector,j)*derivative_variable(detector,k)/sigma_track2(detector,1)
         end if  
       end do  
     end do
   end do  
!
   call invert_matrix(n_var_local,a3_matrix,a3_matrix_inv)
   
   a3_matrix_inv_array(pattern_id,:,:) = a3_matrix_inv(:,:)
   a3_matrix_array(pattern_id,:,:)     = a3_matrix(:,:)
!
   a3_matrix(n_var_local,n_var_local) = a3_matrix(n_var_local,n_var_local) + 1.d0/sigma_target2(1)
!
!
   do k=1, n_var_local
     e_vector    = 0.d0
     e_vector(k) = 1.d0
     call solve_linear_equation_system(n_var_local,a3_matrix,e_vector,x_vector)
     a3_matrix_inv(:,k) = x_vector
   end do
   t3_matrix_inv_array(pattern_id,:,:) = a3_matrix_inv(:,:)
 end do  
end

subroutine invert_matrix(n,a_in,a_inv_out)
 implicit none
 integer n,k
 double precision a_in(n,n),a_inv_out(n,n),e_vector(n),x_vector(n)
!
 do k=1, n
   e_vector    = 0.d0
   e_vector(k) = 1.d0
   call solve_linear_equation_system(n,a_in,e_vector,x_vector)
   a_inv_out(:,k) = x_vector
 end do
end


subroutine initialize_a_t_5  ! initializes the matrizes necessary to get the linear optimization of the tracks
 use vars
 implicit none
 integer, parameter :: n_var_local = 5
 integer i,j,k,detector, pattern_id, pattern_id1, pattern_id2, det_pattern_id
 double precision e_vector(n_var_local),x_vector(n_var_local), einheit(n_var_local,n_var_local)
 logical local_hit_pattern(2*nbr_detectors), local_hit_pattern1(nbr_detectors), local_hit_pattern2(nbr_detectors)
!
 do pattern_id1=1,2**nbr_detectors-1
  do pattern_id2=1,2**nbr_detectors-1
   a5_matrix = 0.
   pattern_id = pattern_id2 + 2**nbr_detectors * (pattern_id1 - 1)
   call get_hit_pattern(2*nbr_detectors,pattern_id,local_hit_pattern)
   call get_hit_pattern(nbr_detectors,pattern_id1,local_hit_pattern1)
   call get_hit_pattern(nbr_detectors,pattern_id2,local_hit_pattern2)
   local_hit_pattern(1:nbr_detectors) = local_hit_pattern1
   local_hit_pattern(nbr_detectors+1:2*nbr_detectors) = local_hit_pattern2
   
!   print*,'a5-ini',det_pattern_id(2*nbr_detectors,local_hit_pattern), local_hit_pattern
   do k=1, 2
     do detector=1,nbr_detectors
       if (local_hit_pattern(detector) ) then
!       
         a5_matrix(k,5) =  a5_matrix(k,5) + &
           derivative_variable(detector,3)*derivative_variable(detector,k)/sigma_track2(detector,1)
!
         do j=1,2
!
           a5_matrix(k,j) =  a5_matrix(k,j) + &
             derivative_variable(detector,j)*derivative_variable(detector,k)/sigma_track2(detector,1)
!
         end do  
       end if  
       if (local_hit_pattern(detector+nbr_detectors) ) then
!       
         a5_matrix(k+2,5) =  a5_matrix(k+2,5) + &
           derivative_variable(detector,3)*derivative_variable(detector,k)/sigma_track2(detector,1)
!
         do j=1,2
!
           a5_matrix(k+2,j+2) =  a5_matrix(k+2,j+2) + &
             derivative_variable(detector,j)*derivative_variable(detector,k)/sigma_track2(detector,1)
!
         end do  
       end if  
     end do
   end do  
   do i=1,4
     a5_matrix(5,i) =  a5_matrix(i,5)
   end do
   do detector=1,nbr_detectors
     if (local_hit_pattern(detector) ) then 
       a5_matrix(5,5)  = a5_matrix(5,5) + derivative_variable(detector,3)**2/sigma_track2(detector,1)
     end if  
     if (local_hit_pattern(detector+nbr_detectors) ) then 
       a5_matrix(5,5)  = a5_matrix(5,5) + derivative_variable(detector,3)**2/sigma_track2(detector,1)
     end if  
   end do 
!
   do k=1, n_var_local
     e_vector    = 0.d0
     e_vector(k) = 1.d0
     call solve_linear_equation_system(n_var_local,a5_matrix,e_vector,x_vector)
     a5_matrix_inv(:,k) = x_vector
   end do
   a5_matrix_inv_array(pattern_id,:,:) = a5_matrix_inv(:,:)
!
   a5_matrix(n_var_local,n_var_local) = a5_matrix(n_var_local,n_var_local) + 1.d0/sigma_target2(1)
!
!
   do k=1, n_var_local
     e_vector    = 0.d0
     e_vector(k) = 1.d0
     call solve_linear_equation_system(n_var_local,a3_matrix,e_vector,x_vector)
     a5_matrix_inv(:,k) = x_vector
   end do
   t5_matrix_inv_array(pattern_id,:,:) = a5_matrix_inv(:,:)
  end do
 end do  
end

subroutine get_single_track_data  ! at a later stage, we will here read in experimental data
 use vars
 implicit none
 double precision theta_step, t1, t2
!
 do 
  call random_number(theta_step)
  theta_step = theta_step*0.16 - 0.08
!  theta_step = 0.01
  t1         = theta_step
!
  x_reference_step    = x_reference
  x_reference_step(4) = x_reference(4)*dcos(theta_step) + x_reference(6)*dsin(theta_step)
  x_reference_step(6) = x_reference(6)*dcos(theta_step) - x_reference(4)*dsin(theta_step)
!
  call random_number(theta_step)
  theta_step = theta_step*0.16 - 0.08
!  theta_step = 0.d0
  t2         = theta_step
!
  x_start = x_reference_step
  x_reference_step(5) = x_start(5)*dcos(theta_step) + x_start(6)*dsin(theta_step)
  x_reference_step(6) = x_start(6)*dcos(theta_step) - x_start(5)*dsin(theta_step)
!
  x_start = x_reference_step
  call single_track
  if ( (det_hit(1).or.det_hit(2)) .and. (det_hit(7).or.det_hit(8)) ) exit
 end do 
 track_det_frame = detector_track_interactions_det_frame
 if (debug_track) print*,'theta_xz = ',t1*1000, 'mrad'
 if (debug_track) print*,'theta_yz = ',t2*1000, 'mrad'
end

subroutine read_single_track_data  ! at a later stage, we will here read in experimental data
 use vars
 implicit none
 integer tr, d, local_nbr_tracks, eastat
 double precision dxpos(nbr_detectors), dxpos0, GAUSSVER, vektor(3*nbr_detectors)
!
 read(21,*,iostat=eastat) vektor, dxpos0
 if (eastat /= 0) then
   rewind(21)
   read(21,*,iostat=eastat) vektor   ! skip one line
   print*,'rewind'
   read(21,*) vektor, dxpos0
 end if
 do d=1,nbr_detectors
   tr = 3*(d-1)
   track_det_frame(d,:) = vektor(tr+1:tr+3)
 end do
!
 do d=1, nbr_detectors
   dxpos(d)  = GAUSSVER(sigma_track(d,1))    !  vary x-positions of detector track in each detector
   if (debug_track) print*,dxpos(d), sigma_track(d,1)
 end do  
 track_det_frame(:,1) = dxpos(:) + track_det_frame(:,1)
!
 if (debug_track) then
   print*,'dxpos0: ', real(dxpos0)
 end if  
end

subroutine get_x_theta
 use vars
 implicit none
 integer d
 double precision sum_aizi, sum_bizi, sum_aiai, sum_aibi, sum_bibi
!
 sum_aizi = 0.
 sum_bizi = 0.
 sum_aiai = 0.
 sum_aibi = 0.
 sum_bibi = 0.
!
 do d = 1, nbr_detectors
   if (track_hit_pattern(d)) then
     sum_aizi = sum_aizi + derivative_x(d)*    (track_det_frame(d,1) - offset_ave_x(d) )
     sum_bizi = sum_bizi + derivative_theta(d)*(track_det_frame(d,1) - offset_ave_x(d) )
     sum_bibi = sum_bibi + derivative_theta(d)*derivative_theta(d)
     sum_aibi = sum_aibi + derivative_x(d)*derivative_theta(d)
     sum_aiai = sum_aiai + derivative_x(d)*derivative_x(d)
   end if
 end do
 det             = sum_aiai*sum_bibi - 2.d0*sum_aibi
 x_pos_target    = (sum_aizi*sum_bibi - sum_bizi*sum_aibi) / det
 theta_xz_target = -(sum_aizi*sum_aibi - sum_bizi*sum_aiai) / det
end 

subroutine vary_parameter(var,io_unit)
 use vars
 implicit none
 integer io_unit, var, i, j,event_id, local_nbr_tracks, d, eastatus
 double precision dx_vary, dtheta, dtheta0, max_q_over_a, dp, theta0, v0(3),v_local(3), dy_vary,p0
 double precision variable(2)
 double precision GAUSSVER, dth, theta, d_1_over_p, ra,dx1, x_local(3), x_simu(3), p_simu(3), dummy, dy1
 double precision theta_step
 character (LEN=10) :: detector_name_local
 character (LEN=80) :: d_out,o_ouT, first_line, gle_extension, first_line_tof
 character (LEN=256) :: local_output_file, simu_file(3), local_output_file_tof
 logical inside_detector_only
!
 select case(var)
   case(1)                                  ! theta_xz
     dtheta0 = 0.02d0                      ! 80 mrad is acceptance of GLAD
     inside_detector_only = .true.
     local_output_file = trim(output_file(1)) // '_vary_start_angle_xz.dat'
     local_output_file_tof = trim(output_file(1)) // '_tof_vary_start_angle_xz.dat'
     first_line =  'nbr_of_detectors*(x,y,z), det system ; dth(rad)'
     first_line_tof =  'TOF (s) ; dth(rad)'
     gle_extension = 'theta_xz_target'
   case(2)                                  ! d_1_over_p
     d_1_over_p    = 0.02d0                   ! relative change of 1/p
     inside_detector_only = .true.
     local_output_file = trim(output_file(1)) // '_vary_1_over_p.dat'
     local_output_file_tof = trim(output_file(1)) // '_tof_vary_1_over_p.dat'
     first_line =  'nbr_of_detectors*(x,y,z), det system ; d_1_over_p (relative change)'
     first_line_tof =  'TOF (s) ; d_1_over_p (relative change)'
     gle_extension = '1_over_p'
   case(3)                                  ! dx_vary
     dx_vary = 0.005d0 						! in m
     inside_detector_only = .true.
     local_output_file = trim(output_file(1)) // '_vary_start_position_x.dat'
     local_output_file_tof = trim(output_file(1)) // '_tof_vary_start_position_x.dat'
     first_line = 'nbr_of_detectors*(x,y,z), det system ; (dx,dy,dz at target in m)'
     first_line_tof =  'TOF (s) ; (dx,dy,dz at target in m)'
     gle_extension = 'x_target'
   case(4)                                  ! theta_yz
     dtheta    = 0.08d0                      ! 80 mrad is acceptance of GLAD
     inside_detector_only = .false.
     local_output_file = trim(output_file(1)) // '_vary_1_over_cos_start_angle_yz.dat'
     local_output_file_tof = trim(output_file(1)) // '_tof_vary_1_over_cos_start_angle_yz.dat'
     first_line =  'nbr_of_detectors*(x,y,z), det system ; d_cosd(theta_yz)'
     first_line_tof =  'TOF (s) ; d_cosd(theta_yz)'
     gle_extension = '1_over_cos_theta_yz_target'
   case(5)                                  ! dy_vary
     dy_vary = 0.005d0 						! in m
     inside_detector_only = .false.
     local_output_file = trim(output_file(1)) // '_vary_start_position_y.dat'
     local_output_file_tof = trim(output_file(1)) // '_tof_vary_start_position_y.dat'
     first_line = 'nbr_of_detectors*(x,y,z), det system ; (dx,dy,dz at target in m)'
     first_line_tof =  'TOF (s) ; (dx,dy,dz at target in m)'
     gle_extension = 'y_target'
   case default                                  
     print*,' do not know what to do in subroutine vary_parameter'
 end select
 if (var <= nbr_variables) then
 !
   sum_xi       = 0.
   sum_det_xi   = 0.
   sum_yi       = 0. 
   sum_xixi     = 0.
   sum_det_xixi = 0.
   sum_xiyi     = 0.
   sum_tracks   = 0.
   sum_det_tracks = 0.
   detector_position_spectra = 0.
   detector_position_spectra_paramter = 0.
!
   if (grafical_output) then
     open (unit=20,file=local_output_file)
     open (unit=23,file=local_output_file_tof)
     write(20,*) first_line
     write(23,*) first_line_tof
   end if
   write(*,*) 'vary: ', var, ' (',trim(gle_extension),')'
   if (derivative_ini_from_file .and. var<4) then   ! read track data from Michaels file
     simu_file(1) = 'simulation_data/events_vary_angle.dat'
     simu_file(2) = 'simulation_data/events_vary_p.dat'
     simu_file(3) = 'simulation_data/events_vary_x.dat'
     open (unit=21,file=simu_file(var))
       read(21,*,iostat=eastatus) event_id, local_nbr_tracks, x_simu, p_simu
       do while (eastatus  == 0)
         do i=1,local_nbr_tracks
           read(21,*) detector_name_local
           read(21,*) x_local(1)
           read(21,*) x_local(2)
           read(21,*) x_local(3)
           read(21,*) dummy
           read(21,*) dummy
           x_local = x_local/1.d2
           d=0
           if (trim(detector_name_local) == 'Fib3a') d = 1
           if (trim(detector_name_local) == 'Fib3b') d = 2
           if (trim(detector_name_local) == 'Fib10') d = 3
           if (trim(detector_name_local) == 'Fib11') d = 4
           if (trim(detector_name_local) == 'Fib12') d = 5
           if (trim(detector_name_local) == 'Fib13') d = 6
           if (trim(detector_name_local) == 'ToFD')  d = 7
           if (d==0) then
             print*,'Detector not recognized!! ', detector_name_local
             stop
           end if  
           x_start = x_reference
           select case (var)
             case(1)                                  ! dx_theta
	           dth   =datan(p_simu(1)/p_simu(3))
	           x_start(4:6) = p_simu*1.d9*e/c
!	           print*,p_simu, dsqrt(dot_product(p_simu,p_simu))
!	           print*,x_reference(4:6)
			   variable     = dth   
		     case(2)                                  ! dp_vary
   			   x_start(4:6) = p_simu*1.d9*e/c
   			   dp = dsqrt(dot_product(x_start(4:6),x_start(4:6)) / dot_product(x_reference(4:6),x_reference(4:6)) )
			   variable = 1.d0/dp -1.d0
 		     case(3)                                  ! dx_vary
			 x_start(1:3) = x_simu / 1.d0
			 dx1 = x_reference(1) - x_start(1)
			 variable = dx1
           end select
           call get_det_position(d,x_local,detector_track_interactions_det_frame(d,1:3))
		   call store_detector_track(d,variable)
         end do
         read(21,*,iostat=eastatus) event_id, local_nbr_tracks, x_simu, p_simu
       end do
     close(21)
     write(*,*) ' read all track data '
	!
	   det_det = sum_det_xixi*sum_det_tracks - sum_det_xi*sum_det_xi
	   derivative_variable(:,var) = (sum_xiyi*sum_det_tracks - sum_yi*sum_det_xi) / det_det
	   offset_variable(:,var)     = -(sum_xiyi*sum_det_xi - sum_yi*sum_det_xixi) / det_det
!       print*,'tracks:',sum_det_tracks
!       print*,derivative_variable(:,var)
!       print*,offset_variable(:,var)
   else                                 ! do internal Runge-Kutta variation
!     do j=-8,8
     do j=-0,0
       theta_step          = 1.d-2*dble(j)
	   x_reference_step    = x_reference
       x_reference_step(4) = x_reference(4)*dcos(theta_step) + x_reference(6)*dsin(theta_step)
	   x_reference_step(6) = x_reference(6)*dcos(theta_step) - x_reference(4)*dsin(theta_step)
	   x_start = x_reference_step
	   call single_track
	   detector_track_interactions_det_frame_ref_step = detector_track_interactions_det_frame
!
       do i=1,nbr_tracks*10
		 call random_number(ra)
		 ra = ra*2.0-1.d0
		 select case(var)
		   case(1)                                  ! dx_theta
	!        dth   = GAUSSVER(dtheta)
    !         dtheta  = 1.d0
    !        do while (abs(dtheta) > 0.08)
    !           dtheta = GAUSSVER(dtheta0)
     !       end do  
			dth          = ra*dtheta0
			p0           = dot_product(x_start(4:6),x_start(4:6))
			x_start(4)   = x_reference(4)*dcos(dth) + x_reference(6)*dsin(dth)
			x_start(6)   = x_reference(6)*dcos(dth) - x_reference(4)*dsin(dth)
			x_start(5)   = x_reference(5)
			p0           = p0/dot_product(x_start(4:6),x_start(4:6))
			p0           = dsqrt(p0)
			variable     = dth                      ! same variable for pos_x and pos_y
		   case(2)                                  ! dp_vary
			 dp = -2.d0
	!         do while (dp <= -1.d0)
	!           dp = GAUSSVER(d_1_over_p)
	!         end do
			dp = 1.d0 + d_1_over_p*ra
			x_start(4:6) = x_reference_step(4:6) * dp   		! dp is relative change ... Gauss around 1, always > 0
			variable = 1.d0/dp -1.d0                            ! same variable for pos_x and pos_y
		   case(3)                                  ! dx_vary
	!         dx1 = x_reference(1) + GAUSSVER(dx_vary)
			 dx1 = ra*dx_vary
			 x_start(1) = x_reference_step(1) + dx1
			 variable = dx1                                     ! same variable for pos_x and pos_y
		   case(4)                                  ! dy_theta
	!        dth   = GAUSSVER(dtheta)
			dth   =ra*dtheta
			x_start(5)   = x_reference_step(5)*dcos(dth) + x_reference_step(6)*dsin(dth)
			x_start(6)   = x_reference_step(6)*dcos(dth) - x_reference_step(5)*dsin(dth)
			x_start(4)   = x_reference_step(4)
			variable(1)  = 1.d0-1.d0/dcos(dth)                  ! variable for pos_x
 			variable(2)  = dth                                  ! variable for pos_y
		   case(5)                                  ! dy_vary
	!         dx1 = x_reference(1) + GAUSSVER(dx_vary)
			 dy1 = ra*dy_vary
			 x_start(2) = x_reference_step(2) + dy1
			 variable = dy1                                     ! same variable for pos_x and pos_y
		  end select   
		 call single_track 
!		 detector_track_interactions_det_frame = detector_track_interactions_det_frame - detector_track_interactions_det_frame_ref_step
		 call store_track(variable,inside_detector_only)
		 call store_times(variable)
         if (100*int(i/100) == int(i)) then
           write(*,'(A1)',advance='no') '.'
         end if  
	   end do  
	   write(*,*) '.'
	 end do  
    !
     write(*,*) ' '
	!
	 do i=1, nbr_detectors
  	     offset_variable(i,var) = detector_track_interactions_det_frame_ref(i,1)
  	     offset_variable(i+nbr_detectors,var) = detector_track_interactions_det_frame_ref(i,2)
  	     if (sum_tracks(i) > 100 ) then
	       derivative_variable(i,var) = sum_xiyi(i) / sum_xixi(i)
	     else
	       print*,'Detector ',i, ' not hit often enough to determine derivative ', sum_tracks(i)
	       derivative_variable(i,var) = 0.d0
	     end if  
         derivative_variable(i+nbr_detectors,var) = sum_xiyi(i+nbr_detectors) / sum_xixi(i+nbr_detectors)
	 end do  
	!
	 if (grafical_output) then
		 close(20)
		 close(23)
		 call get_detector_position_spectra_parameter
		 call make_gle_positions(trim(gle_extension))
	 end if
   end if 
   x_start = x_reference
!  
 end if
end

subroutine get_derivatives
 use vars
 implicit none
 integer io_unit, var, d
 double precision dx_vary, dtheta, max_q_over_a, dp
 character (LEN=20) :: dummy
 logical debug_track_backup
!
 io_unit = 19
! 
 m2=m*m
 reference_track = .true.
 call single_track 
!
 detector_track_interactions_det_frame_ref = detector_track_interactions_det_frame
 reference_track = .false.
 open (unit=io_unit, file=trim(derivative_file))
  if (get_new_derivatives) then 
!
   debug_track_backup = debug_track
   debug_track = .false.
!
   print*
   print*,'initialization of derivatives'
   do var=1,nbr_variables
     call vary_parameter(var,io_unit) 
   end do 
   write(io_unit,*) 'theta_xz, momentum, x_target, theta_yz, y_target'
   do d=1,nbr_detectors
     write(io_unit,*) detector_name(d), ' x'
     write(io_unit,*) derivative_variable(d,:), ' derivative'
     write(io_unit,*) offset_variable(d,:), ' offset'
     write(io_unit,*) detector_name(d), ' y'
     write(io_unit,*) derivative_variable(d+nbr_detectors,:), ' derivative'
     write(io_unit,*) offset_variable(d+nbr_detectors,:), ' offset'
   end do  
   debug_track = debug_track_backup
!
  else
   read(io_unit,*) dummy
   do d=1,nbr_detectors
     read(io_unit,*) dummy
     read(io_unit,*) derivative_variable(d,:)
     read(io_unit,*) offset_variable(d,:)
     read(io_unit,*) dummy
     read(io_unit,*) derivative_variable(d+nbr_detectors,:)
     read(io_unit,*) offset_variable(d+nbr_detectors,:)
     print*,detector_name(d),' x ',derivative_variable(d,:)
     print*,detector_name(d),' y ',derivative_variable(d+nbr_detectors,:)
   end do 
  end if  
 close(io_unit)
 offset_ave_x = 0.
 offset_ave_y = 0.
 do var = 1, nbr_variables
   offset_ave_x = offset_ave_x + offset_variable(:,var)						! x-deviations, theta
 end do
 offset_ave_x = offset_ave_x / nbr_variables
 
 offset_ave_x = detector_track_interactions_det_frame_ref(:,1)
 offset_ave_y = detector_track_interactions_det_frame_ref(:,2)
 
! print*,'offset x    :',real(offset_variable(:,1))
! print*,'offset theta:',real(offset_variable(:,2))
! print*,'offset 1/dp :',real(offset_variable(:,3))
! print*,'offset_ave  :',real(offset_ave)
! print*,'offset ref  :',real(detector_track_interactions_det_frame_ref(:,1))
! print*
! print*,'Diff offset x    :',real(offset_variable(:,1)-detector_track_interactions_det_frame_ref(:,1))
! print*,'Diff offset theta:',real(offset_variable(:,2)-detector_track_interactions_det_frame_ref(:,1))
! print*,'Diff offset 1/dp :',real(offset_variable(:,3)-detector_track_interactions_det_frame_ref(:,1))
! print*,'Diff offset_ave  :',real(offset_ave-detector_track_interactions_det_frame_ref(:,1))
! print*,'Diff offset ref  :',real(detector_track_interactions_det_frame_ref(:,1)-detector_track_interactions_det_frame_ref(:,1))
 if (grafical_output) then
   call make_gle
   call make_pics
 end if  
end

subroutine vary_p(dp_vary)
 use vars
 implicit none
 integer i, j, k, channel
 double precision dp_vary, x_save(nbr_dimensions), GAUSSVER, dp, variable(2)
 character*256 local_output_file
!
 x_save = x_start
!
 sum_xi     = 0.
 sum_yi     = 0. 
 sum_xixi   = 0.
 sum_xiyi   = 0.
 sum_tracks = 0.
 detector_position_spectra = 0.
 detector_position_spectra_paramter = 0.
!
 if (grafical_output) then
   local_output_file = trim(output_file(1)) // '_vary_p.dat'
   open (unit=20,file=local_output_file)
   write(20,*) 'nbr_of_detectors*(x,y,z), det system ; (dp ... relative change of p)'
 end if
 write(*,*) 'dp_vary '
 do i=1,nbr_tracks
   dp = -2.d0
   do while (dp <= -1.d0)
     dp = GAUSSVER(dp_vary)
   end do
   dp = 1.d0 + dp
   x_start(4:6) = x_save(4:6) * dp   ! dp is relative change ... Gauss around 1, but always positive
   call single_track 
   variable = dp
!   call store_track(variable)
 end do  
 write(*,*)
!
!! det = sum_xixi*sum_tracks - sum_xi*sum_xi
!! derivative_p = (sum_xiyi*sum_tracks - sum_yi*sum_xi) / det
!! offset_p     = -(sum_xiyi*sum_xi - sum_yi*sum_xixi) / det
!
 if (grafical_output) then
   close(20)
   call get_detector_position_spectra_parameter
   call make_gle_positions('p')
 end if
 x_start = x_save
end




subroutine vary_q_over_a(max_q_over_a)
 use vars
 implicit none
 integer i, j, k, channel
 double precision qa, GAUSSVER, m_save, max_q_over_a, x_save(nbr_dimensions), variable(2)
 character*256 local_output_file
!
 m_save = m
 x_save = x_start
!
 sum_xi     = 0.
 sum_yi     = 0. 
 sum_xixi   = 0.
 sum_xiyi   = 0.
 sum_tracks = 0.
 detector_position_spectra = 0.
 detector_position_spectra_paramter = 0.
! 
 if (grafical_output) then
   local_output_file = trim(output_file(1)) // '_vary_q_over_a.dat'
   open (unit=20,file=local_output_file)
   write(20,*) 'nbr_of_detectors*(x,y,z), det system ; q/a'
 end if
 write(*,*) 'q_over_a '
 do i=1,nbr_tracks
   call random_number(qa)
   qa = qa * max_q_over_a
   m            = q / qa
   m2           = m*m
   x_start(4:6) = m / m_save * x_save(4:6)
   call single_track 
   variable = qa/e*amu
!   call store_track(variable)
 end do  
 write(*,*)
!
!! det = sum_xixi*sum_tracks - sum_xi*sum_xi
!! derivative_q_over_a = (sum_xiyi*sum_tracks - sum_yi*sum_xi) / det
!! offset_q_over_a     = -(sum_xiyi*sum_xi - sum_yi*sum_xixi) / det
! 
 if (grafical_output) then
   close(20)
   call get_detector_position_spectra_parameter
   call make_gle_positions('particle_q_over_a')
 end if
 m  =  m_save
 m2 =  m*m
 x_start = x_save
end


subroutine store_times(variable)
 use vars
 implicit none
 double precision variable(2)
 integer j
!   
   if (grafical_output) then
!     do j=1,nbr_detectors
!       write(23,'(ES20.10)', advance='no') detector_track_interactions_time(j)
!     end do  
     write(23,*) detector_track_interactions_time, variable
   end if
end

subroutine store_track(variable,inside_detector_only)
 use vars
 implicit none
 double precision variable(2), y
 integer i,j,k, channel
 logical inside_detector,inside_detector_only

!   if (det_hit(nbr_detectors) .and. det_hit(nbr_detectors-1)) then
!     print*,'both TOFD detectors are hit!'
!     print*,detector_track_interactions_det_frame(nbr_detectors,1)
!     print*,detector_track_interactions_det_frame(nbr_detectors-1,1)
!     print*,detector_track_interactions_lab_frame(nbr_detectors,1)
!     print*,detector_track_interactions_lab_frame(nbr_detectors-1,1)
!     print*,inside_detector(nbr_detectors, detector_track_interactions_lab_frame(nbr_detectors,:))
!     print*,inside_detector(nbr_detectors-1, detector_track_interactions_lab_frame(nbr_detectors-1,:))
!     stop
!   end if
   do j=1,nbr_detectors
    if(det_hit(j) ) then
     do k=1,3
       channel = (detector_track_interactions_det_frame(j,k)-detector_range(j,k,1))/detector_range(j,k,3)* &
                  nbr_channels_position_spectra + 1
         if (channel >0 .and. channel <= nbr_channels_position_spectra) then
           detector_position_spectra(j,k,channel) = detector_position_spectra(j,k,channel) + 1
         end if  
     end do
    end if 
    if(det_hit(j) .or. .not. inside_detector_only) then
     y = detector_track_interactions_det_frame(j,1) - detector_track_interactions_det_frame_ref_step(j,1)  ! x-component of each detector
     sum_xi(j)     = sum_xi(j)   + variable(1)
     sum_xixi(j)   = sum_xixi(j) + variable(1)*variable(1)
     sum_yi(j)     = sum_yi(j)   + y		  		
     sum_xiyi(j)   = sum_xiyi(j) + y*variable(1)	
     sum_tracks(j) = sum_tracks(j) + 1.d0
    end if 
     y = detector_track_interactions_det_frame(j,2) - detector_track_interactions_det_frame_ref_step(j,2) ! y-component of each detector
     sum_xi(j+nbr_detectors)     = sum_xi(j+nbr_detectors)   + variable(2)
     sum_xixi(j+nbr_detectors)   = sum_xixi(j+nbr_detectors) + variable(2)*variable(2)
     sum_yi(j+nbr_detectors)     = sum_yi(j+nbr_detectors)   + y		  		
     sum_xiyi(j+nbr_detectors)   = sum_xiyi(j+nbr_detectors) + y*variable(2)	
     sum_tracks(j+nbr_detectors) = sum_tracks(j+nbr_detectors) + 1.d0
  end do  
!
   if (grafical_output) then
     do j=1,nbr_detectors
       write(20,'(3F15.10)', advance='no') detector_track_interactions_det_frame(j,:)
     end do  
     write(20,*) variable
   end if
!
end

subroutine store_detector_track(det_local,variable)
 use vars
 implicit none
 double precision variable(2)
 integer i,j,k, channel,det_local
!
   sum_det_xi(det_local)     = sum_det_xi(det_local)   + variable(1)
   sum_det_xixi(det_local)   = sum_det_xixi(det_local) + variable(1)*variable(1)
   sum_yi(det_local)         = sum_yi(det_local)   + detector_track_interactions_det_frame(det_local,1)		  		! x-component of each detector
   sum_xiyi(det_local)       = sum_xiyi(det_local) + detector_track_interactions_det_frame(det_local,1)*variable(1)			
   sum_det_tracks(det_local) = sum_det_tracks(det_local) +1.d0
!   
   if (10000*int(sum(sum_det_tracks)/10000) == int(sum(sum_det_tracks))) then
     write(*,'(A1)',advance='no') '.'
   end if  
end

subroutine get_detector_position_spectra_parameter
 use vars
 implicit none
 double precision sigma(3), ave(3), channel_sum
 integer i, j, k, channel, minimum, maximum

 do i=1,nbr_detectors
   ave   = 0.
   sigma = 0.
   minimum = 0
   maximum = 0
   do j=1,3
     channel_sum = sum(detector_position_spectra(i,j,:))  
     if (channel_sum > 0.) then
       do channel=1,nbr_channels_position_spectra
         if (detector_position_spectra(i,j,channel) > 0) then
           if (minimum == 0) minimum = channel
           maximum = channel
         end if  
         ave(j)   = ave(j)   + dble(channel)*dble(detector_position_spectra(i,j,channel))
         sigma(j) = sigma(j) + dble(channel)**2*dble(detector_position_spectra(i,j,channel))
       end do  
       ave(j)   = ave(j)   / channel_sum
       sigma(j) = sigma(j) / channel_sum
       sigma(j) = dsqrt( sigma(j) - ave(j)*ave(j) )
       ave(j)   = (ave(j) - 0.5d0) / dble(nbr_channels_position_spectra) * &
                  detector_range(i,j,3) + detector_range(i,j,1)
       sigma(j) = sigma(j) / dble(nbr_channels_position_spectra) * detector_range(i,j,3)
       detector_position_spectra_paramter(i,j,5) = dble(minimum)
       detector_position_spectra_paramter(i,j,6) = dble(maximum)
       detector_position_spectra_paramter(i,j,3) = (dble(minimum) - 0.5d0) / &
          dble(nbr_channels_position_spectra) * detector_range(i,j,3) + detector_range(i,j,1)
       detector_position_spectra_paramter(i,j,4) = (dble(maximum) - 0.5d0) / &
          dble(nbr_channels_position_spectra) * detector_range(i,j,3) + detector_range(i,j,1)
     end if  
   end do
   detector_position_spectra_paramter(i,:,1) = ave
   detector_position_spectra_paramter(i,:,2) = sigma
 end do
end

subroutine init
 use vars
 implicit none
 integer eastat, dot
 character*300 buffer
 integer unit_read
 double precision x1(3), x0(3),distance_detector_plane
 logical inside_b_field
 integer i, j, k  
!
 call getarg(1,BUFFER)
 if (buffer.eq.'-') then	 
	 unit_read = 5
	 print*,'Kein ini file!'
	 ini_file=''
 else 
   if (buffer.eq.'') then
     print*,'Give name of init file: '
     ini_file='tracker.ini'
!     read(*,*) ini_file
   else
     ini_file=buffer(1:len_trim(buffer))
   end if
   print*,'Init file: ',ini_file(1:40)
   unit_read = 8
   open (unit=unit_read, file=ini_file , status='old')
 end if 	 
!
 read(unit_read,*) geometry_file  
 print*,'Geometry definition: ',trim(geometry_file)
!
 read(unit_read,*) derivative_file  
 get_new_derivatives = (trim(derivative_file) == '')
 if (get_new_derivatives) then
   derivative_file = 'derivatives.out'
   print*,'NEW derivatives will be stored in: ',trim(derivative_file)
 else
   print*,'Derivatives definition: ',trim(derivative_file)
 end if 
!
 do i=1,3
   read(unit_read,*) x_start(i) , sigma_target(i)
   print*,'Start location: ',real(x_start(i)), ' +- ',real(sigma_target(i))
 end do  
 sigma_target2 = sigma_target
 pos_target    = x_start(1:3)
!
 do i=1,3
   read(unit_read,*) p_start(i)  
   print*,'Start momentum (MeV/c/u)',p_start(i)
 end do  
!
 read(unit_read,*) m_amu     ! this is now a/q
 print*,'Mass of particle (AMU): ',m_amu
 m = m_amu * amu
 print*,'Mass of particle (MeV/c2): ',m/e*c2/1d6
!
! read(unit_read,*) q_e  
 q_e = 1.d0                              ! calculate always with q == 1
 print*,'Charge of particle (e): ',q_e
 q = q_e * e
!
 x_start(4:6) = p_start(1:3)*e/c*1.d6*m_amu
!
 x_reference = x_start
!
 read(unit_read,*) spatial_resolution  
 print*,'Spatial resolution (m): ',spatial_resolution
!
 read(unit_read,*) constrain_target_x  
 if (constrain_target_x) then
   print*,'Constrain x-position on target: yes'
 else
   print*,'Constrain x-position on target: no'
 end if  
!
 read(unit_read,*) constrain_target_y  
 if (constrain_target_x) then
   print*,'Constrain y-position on target: yes'
 else
   print*,'Constrain y-position on target: no'
 end if  
!
 read(unit_read,*) debug_track  
 if (debug_track) then
   print*,'Debugging mode: yes'
 else
   print*,'Debugging mode: no'
 end if  
!
 read(unit_read,*) grafical_output  
 read(unit_read,*) output_file(1)  
 print*,'Root of output file: ',trim(output_file(1))
 output_file(2) = trim(output_file(1)) // '.dat'
 output_file(3) = trim(output_file(1)) // '.gle'
! 
 if (grafical_output) then
   print*,'Grafical output: yes'
!
   open (unit=9, file=trim(output_file(2)))
 else
   print*,'Grafical output: no' 
 end if
!
 if (unit_read .eq. 8) close(8)
 unit_read = 8
 open (unit=unit_read, file=geometry_file, status='old')
!
!  b-field
!
  read(unit_read,*) master_borders(1,1)  
  read(unit_read,*) master_borders(1,2)  
  read(unit_read,*) master_borders(2,1)  
  read(unit_read,*) master_borders(2,2)  
  read(unit_read,*) master_borders(3,1)  
  read(unit_read,*) master_borders(3,2)  
  read(unit_read,*) magnetic_field_file 
  read(unit_read,*) b_field_position_in_lab(1)  
  read(unit_read,*) b_field_position_in_lab(2)  
  read(unit_read,*) b_field_position_in_lab(3)  
  read(unit_read,*) b_field_position_in_lab(4)  
  b_field_position_in_lab(4) = b_field_position_in_lab(4)/180.d0*datan(1.d0)*4.d0                       ! conversion of deg into rad
  read(unit_read,*) glad_current 
  read(unit_read,*) b_field_borders(1,1)  
  read(unit_read,*) b_field_borders(1,2)  
  read(unit_read,*) b_field_borders(2,1)  
  read(unit_read,*) b_field_borders(2,2)  
  read(unit_read,*) b_field_borders(3,1)  
  read(unit_read,*) b_field_borders(3,2)  
!
! detectors
!
  read(unit_read,*) nbr_detectors
  allocate( detector_position_in_lab(0:nbr_detectors,4) )
  allocate( detector_xy_plane(nbr_detectors,4) )
  allocate( derivative_variable(2*nbr_detectors,nbr_variables) )
  allocate( offset_variable(2*nbr_detectors,nbr_variables) )
  allocate( detector_name(nbr_detectors) )
  allocate( sum_tracks(2*nbr_detectors) )
  allocate( sum_xi(2*nbr_detectors) )
  allocate( sum_xixi(2*nbr_detectors) )
  allocate( sum_yi(2*nbr_detectors) )
  allocate( sum_xiyi(2*nbr_detectors) )
  allocate( sum_det_xi(2*nbr_detectors) )
  allocate( sum_det_xixi(2*nbr_detectors) )
  allocate( sum_det_tracks(nbr_detectors) )
  allocate( det_det(nbr_detectors) )
  allocate( det_hit(nbr_detectors) )
  allocate( det_passed(nbr_detectors) )
  allocate( stopping_power(nbr_detectors) )
  allocate( detector_range(nbr_detectors,3,3) )
  allocate( derivative_theta(nbr_detectors) )
  allocate( offset_theta(nbr_detectors) )
  allocate( derivative_x(nbr_detectors) )
  allocate( offset_x(nbr_detectors) )
  allocate( derivative_q_over_a(nbr_detectors) )
  allocate( offset_q_over_a(nbr_detectors) )
  allocate( offset_ave_x(nbr_detectors) )
  allocate( offset_ave_y(nbr_detectors) )
  allocate( detector_track_interactions_lab_frame(nbr_detectors,3) )
  allocate( detector_track_interactions_time(nbr_detectors) )
  allocate( detector_track_interactions_path(nbr_detectors) )
  allocate( detector_track_interactions_det_frame(nbr_detectors,3) )
  allocate( detector_track_interactions_det_frame_ref(nbr_detectors,3) )
  allocate( detector_track_interactions_det_frame_ref_step(nbr_detectors,3) )
  allocate( track_det_frame(nbr_detectors,3) )
  allocate( track_det_frame1(nbr_detectors,3) )
  allocate( track_det_frame2(nbr_detectors,3) )
  allocate( track_lab_frame(nbr_detectors,3) )
  allocate( sigma_track(nbr_detectors,3) )
  allocate( sigma_track2(nbr_detectors,3) )
  allocate( track_hit_pattern(nbr_detectors) )
  allocate( track_hit_pattern1(nbr_detectors) )
  allocate( track_hit_pattern2(nbr_detectors) )
  allocate( detector_position_spectra(nbr_detectors,3,nbr_channels_position_spectra) )
  allocate( detector_position_spectra_paramter(nbr_detectors,3,nbr_paramters_position_spectra) )
  allocate( a2_matrix_inv_array(2**nbr_detectors-1,2,2) )
  allocate( a2_matrix_array(2**nbr_detectors-1,2,2) )
  allocate( t2_matrix_inv_array(2**nbr_detectors-1,2,2) )
  allocate( a3_matrix_inv_array(2**nbr_detectors-1,3,3) )
  allocate( a3_matrix_array(2**nbr_detectors-1,3,3) )
  allocate( t3_matrix_inv_array(2**nbr_detectors-1,3,3) )
  allocate( a5_matrix_inv_array( 2**(2*nbr_detectors),5,5 ) )
  allocate( t5_matrix_inv_array( 2**(2*nbr_detectors),5,5 ) )
  detector_position_spectra = 0.
  detector_position_in_lab = 0.
  detector_position_in_lab(0,:) = b_field_position_in_lab
  do i=1,nbr_detectors
    read(unit_read,*) detector_name(i)  
    do j=1,4
      read(unit_read,*) detector_position_in_lab(i,j)
    end do
    do j=1,3
      read(unit_read,*) detector_range(i,j,1:2)
    end do
    read(unit_read,*) stopping_power(i)
    do j=1,3
      read(unit_read,*) sigma_track(i,j)
    end do
    detector_position_in_lab(i,4) = detector_position_in_lab(i,4)/180.d0*datan(1.d0)*4.d0	! conversion of deg into rad
    x1 = 0.
    call get_lab_position(i,x1,x0)															! x0 is now the detector coordinate's origin in the lab frame
    call get_lab_position(i,norm_z,x1)														! x1 is now the detector coordinate's (0,0,1) point in the lab frame
    detector_xy_plane(i,1:3) = x1-x0														! vector perpendicular to the detector's xy-plane in te lab frame
    detector_xy_plane(i,4  ) = dot_product(x0,detector_xy_plane(i,1:3))
    x1 = ( detector_range(i,:,2) + detector_range(i,:,1) ) / 2.
    if ( inside_b_field(x1) ) print*, 'Detector ',trim(detector_name(i)),' is inside the B-field.'
  end do
  detector_range(:,:,3) = dabs( detector_range(:,:,2) - detector_range(:,:,1) )
 close(unit_read)
 sigma_track2 = sigma_track**2
! 
 print*,'x-borders (m): ',master_borders(1,:) 
 print*,'y-borders (m): ',master_borders(2,:) 
 print*,'z-borders (m): ',master_borders(3,:) 
!
 print*,'x-b-field-borders (m): ',b_field_borders(1,:) 
 print*,'y-b-field-borders (m): ',b_field_borders(2,:) 
 print*,'z-b-field-borders (m): ',b_field_borders(3,:) 
! 
 print*
 call b_field_init
 print*
 print*,'init done'
 print*
 
end

subroutine single_track
 use vars
 implicit none
 logical x_sign, inside_master, inside_b_field, inside_detector, inside
 double precision d_xy_det(nbr_detectors), d_xy_det_old(nbr_detectors)
 double precision distance_detector_plane, x_local(3), ekin, p_total, p(3)
 integer i, j, k  
!
 t = 0.
 x = x_start
!
 call get_velocity(x(4:6))
 dt_fine   = spatial_resolution / dsqrt(dot_product(v,v))
 if (reference_track) then
   dt_fine = dt_fine/5.d0
   print*,'Mass, Momentum: ', m, x(4:6)
   print*,'Velocity: ', v, dsqrt(dot_product(v,v))
   print*,'dt inside B-field: ', dt_fine , ' s'
   print*,'dt outside B-field: ', dt_coarse , ' s'
 end if  
 dt_coarse = dt_fine *10.d0 
 nbr_steps = 0
 detector_track_interactions_lab_frame = 0.
 detector_track_interactions_time      = 0.
 detector_track_interactions_det_frame = 0.
 det_hit     = .false.
 det_passed  = .false.
 do i=1,nbr_detectors
   d_xy_det_old(i) = distance_detector_plane( i, x(1:3) )
 end do  
 do while ( inside_master(x(1:3)) .and. t<1.d-3)
   if (inside_b_field(x(1:3)) ) then
     dt = dt_fine
     nbr_steps(1) = nbr_steps(1)+1
   else
     dt = dt_coarse
     nbr_steps(2) = nbr_steps(2)+1
   end if  
   call runge_kutta
   if (reference_track .and. grafical_output) write(9,*) x(1:3)
   if (debug_track) write(22,*) x(1:3)
   t = t + dt
   x = x + dx
!
   do i=1,nbr_detectors
     if (.not. det_passed(i)) then    										! intersection point of this detector not yet determined
       d_xy_det(i) = distance_detector_plane( i, x(1:3) )     
       if (d_xy_det(i) * d_xy_det_old(i) < 0. ) then	! sign has changed in last step
         !
         !  here we can easily include the stopping power, since we know that the detector was crossed
         !
         detector_track_interactions_lab_frame(i,:) = x(1:3) -d_xy_det(i)/(d_xy_det(i) - d_xy_det_old(i)) * dx(1:3) 
         detector_track_interactions_time(i)        = t -d_xy_det(i)/(d_xy_det(i) - d_xy_det_old(i)) * dt
         call get_det_position(i,detector_track_interactions_lab_frame(i,:),detector_track_interactions_det_frame(i,:))
         x_local = detector_track_interactions_lab_frame(i,:)
         inside = inside_detector( i,x_local )
         if (inside ) then
          if (stopping_power(i) > 0.d0) then
!           print*,detector_name(i), inside, 'will consider stopping power'
           p = x(4:6)
           ekin = dsqrt((m*c2)**2+dot_product(p,p)*c2 ) - m*c2
           ekin    = ekin*(1.d0-stopping_power(i))
           p_total = dsqrt( (ekin+m*c2)**2-(m*c2)**2)/c
           x(4:6)  = p*p_total/dsqrt(dot_product(p,p))
!           call get_velocity(x(4:6))
          end if
          det_hit(i) = .true.
         end if  
         det_passed(i) = .true.
       end if
       d_xy_det_old(i) = d_xy_det(i)
     end if
   end do
 end do
!
 if (reference_track) then
   print*,'Final:' , t,x(1:3),  inside_master(x(1:3))
   call get_velocity(x(4:6))
   print*,'Final angle:' , atan(v(1)/v(3)) * 180/3.14,' deg'
   call get_linear_slope_parameters(x_start)
   print*,'Slope incoming:' , real(slope_parameter)
   call get_linear_slope_parameters(x)
   print*,'Slope outgoing:' , real(slope_parameter)
   print*,'Outgoing angle (x):' , abs(datan(slope_parameter(1))*180/3.14),' deg'
   print*,'Number of steps (fine/coarse):' , nbr_steps 
   print*
   print*,'Interception points of reference track in det-frame: '
   print*,'detector     x (m)               y (m)             z (m)'
   do i=1,nbr_detectors
     write(*,'(A10,A2)',advance='no')detector_name(i),': '
     if (det_passed(i)) then 
       print*,real(detector_track_interactions_det_frame(i,:))
     else
       print*,'no hit'
     end if
   end do
   print*
   print*,'Interception points of reference track in lab-frame: '
   print*,'detector     x (m)               y (m)             z (m)'
   do i=1,nbr_detectors
     write(*,'(A10,A2)',advance='no')detector_name(i),': '
     if (det_passed(i)) then 
       print*,real(detector_track_interactions_lab_frame(i,:))
     else
       print*,'no hit'
     end if
   end do
   print*
   print*,'Interception time and flightpath of reference track: '
   print*,'detector  :   t (s)    ,   flightpath (m)'
   detector_track_interactions_path = detector_track_interactions_time*dsqrt(dot_product(v,v))
   do i=1,nbr_detectors
     write(*,'(A10,A2)',advance='no')detector_name(i),': '
     if (det_passed(i)) then 
       print*,real(detector_track_interactions_time(i)) , real(detector_track_interactions_path(i))
     else
       print*,'no hit'
     end if
   end do
   print*
 end if  
end

subroutine get_derivative_b_field(t_local,x_local)
 use vars
 implicit none
 double precision   :: t_local,x_local(nbr_dimensions) 
!
 call get_velocity(x_local(4:6))			! momentum -> velocity
 derivative_b_field(1:3) = v 				! (x',y',z')
!
 call  get_b_field(x_local(1:3))
!
 derivative_b_field(4) = q*(v(2)*b_field(3) - v(3)*b_field(2))   			! px' , Lorentz-Force
 derivative_b_field(5) = q*(v(3)*b_field(1) - v(1)*b_field(3))   			! py' 
 derivative_b_field(6) = q*(v(1)*b_field(2) - v(2)*b_field(1))   			! pz' 
!
end

subroutine get_velocity(p)
 use vars
 implicit none
 double precision   :: p(3) 
!
 v = p/dsqrt(m2+dot_product(p,p)/c2 )  ! relativistic expression
end 

subroutine get_linear_slope_parameters(x_local)
 use vars
 implicit none
 double precision   :: x_local(nbr_dimensions), v_local(3)
!
 v_local = x_local(4:6)/dsqrt(m2+dot_product(x_local(4:6),x_local(4:6))/c2 )  ! relativistic expression
!
! x - Parameters   x(z) = slope_parameter(1) * z + slope_parameter(2)
!  
 slope_parameter(1) = v_local(1)/v_local(3)
 slope_parameter(2) = slope_parameter(1)*x_local(3) + x_local(1)
!
! y - Parameters   y(z) = slope_parameter(3) * z + slope_parameter(4)
!  
 slope_parameter(3) = v_local(2)/v_local(3)
 slope_parameter(4) = slope_parameter(3)*x_local(3) + x_local(2)
!
end 


subroutine get_b_field(xyz_local)
 use vars
 implicit none
 double precision   :: xyz_local(3) ,xyz(3) , b(3)
 double precision 	:: d_l(3), d_h(3), f(8)
 integer h_l(3),h_h(3)
 logical inside_local
!
 b_field = 0.
   xyz = b_field_resolution_factor*xyz_local
   h_l = floor(xyz) 
   h_h = h_l + 1
   inside_local = (h_l(1) >= b_field_map_limits(1,1)) .and. (h_h(1) <= b_field_map_limits(1,2)) .and. &
          (h_l(2) >= b_field_map_limits(2,1)) .and. (h_h(2) <= b_field_map_limits(2,2)) .and. &
          (h_l(3) >= b_field_map_limits(3,1)) .and. (h_h(3) <= b_field_map_limits(3,2)) 
   if (inside_local) then
      d_l = xyz - h_l
      d_h = h_h - xyz
      f(1)    = (d_h(1)*d_h(2)*d_h(3))
      f(2)    = (d_h(1)*d_h(2)*d_l(3))
      f(3)    = (d_h(1)*d_l(2)*d_h(3))
      f(4)    = (d_l(1)*d_h(2)*d_h(3))
      f(5)    = (d_l(1)*d_l(2)*d_h(3))
      f(6)    = (d_l(1)*d_h(2)*d_l(3))
      f(7)    = (d_h(1)*d_l(2)*d_l(3))
      f(8)    = (d_l(1)*d_l(2)*d_l(3))

      f       = f / sum(f)
      
      b       = f(1)*b_field_map(h_l(1),h_l(2),h_l(3),:)
      b   = b + f(2)*b_field_map(h_l(1),h_l(2),h_h(3),:)
      b   = b + f(3)*b_field_map(h_l(1),h_h(2),h_l(3),:)
      b   = b + f(4)*b_field_map(h_h(1),h_l(2),h_l(3),:)
      b   = b + f(5)*b_field_map(h_h(1),h_h(2),h_l(3),:)
      b   = b + f(6)*b_field_map(h_h(1),h_l(2),h_h(3),:)
      b   = b + f(7)*b_field_map(h_l(1),h_h(2),h_h(3),:)
      b   = b + f(8)*b_field_map(h_h(1),h_h(2),h_h(3),:)
      b_field = b
   end if  
end 

subroutine runge_kutta
 use vars
 implicit none
 double precision k1(nbr_dimensions),k2(nbr_dimensions),k3(nbr_dimensions),k4(nbr_dimensions)
 double precision   :: t_local,x_local(nbr_dimensions) 
!
 t_local = t
 x_local = x
 call get_derivative_b_field(t_local,x_local)
 k1 = derivative_b_field
!
 t_local = t + dt/2.d0
 x_local = x + dt*k1/2.d0
 call get_derivative_b_field(t_local,x_local)
 k2 = derivative_b_field
!
! t_local = t + dt/2.d0
 x_local = x + dt*k2/2.d0
 call get_derivative_b_field(t_local,x_local)
 k3 = derivative_b_field
!
 t_local = t + dt
 x_local = x + dt*k3
 call get_derivative_b_field(t_local,x_local)
 k4 = derivative_b_field
!
 dx = (k1 + k2 + k2 + k3 + k3 + k4)*dt/6.d0
end

subroutine make_pics
 use vars
 implicit none
 character*200 sh_command
!
!! sh_command = 'gle -d png ' // trim(output_file(3)) // '  >/dev/null'
!! call execute_command_line (sh_command)
!
 sh_command = 'gle -d pdf ' // trim(output_file(3)) // '  >/dev/null'
 call execute_command_line (sh_command)
!
end 

subroutine make_single_track_pic
 use vars
 implicit none
 character*200 sh_command
!
 sh_command = 'gle -d pdf ' // trim(track_debug_gle_file) // '  >/dev/null'
 sh_command = 'gle -d pdf ' // trim(track_debug_gle_file) 
 call execute_command_line (sh_command)
!
end 

subroutine make_gle_positions(variable)
 use vars
 implicit none
 integer uc, i, j, k, det_id, koordinate,slash_position, length
 logical backwards
 character (LEN=256) local_output_file(3), shell_command
 character (LEN=10)  :: ch10
 CHARACTER(LEN=*), INTENT(IN) :: variable
 character*7 param(2)
 real off_set(2), frame_size(2), graph_size(2), axis_range(2,2),xyz
!
 uc=10
 backwards=.true.
!
 frame_size     = 20. 	! frame size (20 ; 20)
 off_set 	    = 3.	! graph-offset of drawing inside frame (3 ; 3) 
 graph_size     = 15.	! graph size (15 ; 15)	
!
 do det_id = 1,nbr_detectors				! tofd
   write(ch10,'(I3.3)') det_id
   do koordinate=1,1			! x-axis		
    if (detector_position_spectra_paramter(det_id,koordinate,6) > 0.) then
     ch10 = trim(ch10) // '_' // ch_xyz(koordinate)
     slash_position = index(output_file(1),'/',backwards)
     length         = len(output_file(1))
     local_output_file(2) = output_file(1)(slash_position+1:length)
     local_output_file(3) = trim(local_output_file(2)) // '_detector_' // trim(ch10) // '_' // trim(variable) // '.dat'
     local_output_file(1) = trim(output_file(1)) // '_detector_' // trim(ch10) // '_' // trim(variable) // '.dat'
     local_output_file(2) = trim(output_file(1)) // '_detector_' // trim(ch10) // '_' // trim(variable) // '.gle'
     open(unit=uc, file=local_output_file(1))
      do k= int(detector_position_spectra_paramter(det_id,koordinate,5)),&
           int(detector_position_spectra_paramter(det_id,koordinate,6))
        xyz = (real(k) - 0.5) / real(nbr_channels_position_spectra) * &
          detector_range(det_id,koordinate,3) + detector_range(det_id,koordinate,1)  
        write(uc,*) xyz,detector_position_spectra(det_id,koordinate,k)
      end do 
     close(uc)
     axis_range(1,:) = detector_position_spectra_paramter(det_id,koordinate,3:4)	! xmin, xmax on x-axis
!     print*,axis_range(1,:)
!     axis_range(2,1) = -3.5	! ymin on y-axis
!     axis_range(2,2) = 1.5	! ymax on y-axis
     open(unit=uc, file=local_output_file(2))
      write(uc, *) 'size ', frame_size
      write(uc, *) 'amove ', off_set
      write(uc, *) 'begin graph'
      write(uc, *) '  nobox'
      write(uc, *) '  fullsize'
      write(uc, *) '  size ', graph_size
      write(param(1:2),'(f7.5)') real(detector_position_spectra_paramter(det_id,koordinate,1:2))
      write(uc, *) '   title "',trim(detector_name(det_id)),' x_{mean} = ',     &
           param(1),', \sigma = ',param(2),' m  " hei 0.7'
      write(uc, *) '   xtitle "',ch_xyz(koordinate),'-position (m)" hei 0.6'
      write(uc, *) '   ytitle "Number of hits" hei 0.6'
      write(uc, *) '   xaxis min ',axis_range(1,1), ' max ',axis_range(1,2)
!      write(uc, *) '   yaxis min ',axis_range(2,1), ' max ',axis_range(2,2)
     ! write(uc, *) '   xaxis min 55 max 200'
     ! write(uc, *) '   yaxis max 1.2 log'
      write(uc, *) '   x2axis off'
      write(uc, *) '   y2axis off'
      write(uc, *) '   xlabels hei 0.6'
      write(uc, *) '   ylabels hei 0.6'
      write(uc, *) '   key pos tl hei 0.6'
      write(uc, *) '   data ',trim(local_output_file(3)),' d4=c1,c2 '
     ! write(uc, *) '   d4 marker fcircle msize 0.01 color steelblue' 
      write(uc, *) '   d4 line lwidth 0.03 color blue '
      write(uc, *) 'end graph'
     close(uc)
     shell_command = 'gle -d pdf ' // trim(local_output_file(2)) // '  >/dev/null'
     call execute_command_line (shell_command)
    end if
   end do
 end do    
end

subroutine make_gle
 use vars
 implicit none
 integer uc, i, j, k,slash_position,length
 real off_set(2), frame_size(2), graph_size(2), axis_range(2,2), x_target(2), x_magnet(2), x_lab(2), x_det(2)
 real x_vec(2)
 double precision x_b(3), x_l(3), x_d(3)
 character*256 data_link
 logical backwards
!
 uc=10
 frame_size(1)  = 40. 	! frame size (40 ; 20)
 frame_size(2)  = 20. 	! frame size (40 ; 20)
 off_set 	    = 3.	! graph-offset of drawing inside frame (3 ; 3) 
 graph_size(1)  = 30.	! graph size (30 ; 15)	
 graph_size(2)  = 15.	! graph size (30 ; 15)	
 axis_range(1,1) = -1.	! xmin on x-axis
 axis_range(1,2) = 9.	! xmax on x-axis
 axis_range(2,1) = -3.5	! ymin on y-axis
 axis_range(2,2) = 1.5	! ymax on y-axis
 backwards=.true.
 slash_position = index(output_file(2),'/',backwards)
 length         = len(output_file(2))
 data_link      = output_file(2)(slash_position+1:length)
 open(unit=uc, file=output_file(3))
 write(uc, *) 'size ', frame_size
 write(uc, *) 'include "shape.gle" '
 write(uc, *) 'amove ', off_set
 write(uc, *) 'begin graph'
 write(uc, *) '  nobox'
 write(uc, *) '  fullsize'
 write(uc, *) '  size ', graph_size
 write(uc, *) '   title "A track through GLAD... ',nbr_steps(1),' steps  " hei 0.7'
 write(uc, *) '   ytitle "x-position (m)" hei 0.6'
 write(uc, *) '   xtitle "z-position (m)" hei 0.6'
 write(uc, *) '   xaxis min ',axis_range(1,1), ' max ',axis_range(1,2)
 write(uc, *) '   yaxis min ',axis_range(2,1), ' max ',axis_range(2,2)
! write(uc, *) '   xaxis min 55 max 200'
! write(uc, *) '   yaxis max 1.2 log'
 write(uc, *) '   x2axis off'
 write(uc, *) '   y2axis off'
 write(uc, *) '   xlabels hei 0.6'
 write(uc, *) '   ylabels hei 0.6'
 write(uc, *) '   key pos tl hei 0.6'
 write(uc, *) '   data ',trim(data_link),' d4=c3,c1 '
! write(uc, *) '   d4 marker fcircle msize 0.01 color steelblue' 
 write(uc, *) '   d4 line lstyle 1 lwidth 0.01 color black '
 write(uc, *) 'end graph'
!
! some extra drawing...
!
 if ((axis_range(1,1)*axis_range(1,2) < 0) .and.(axis_range(2,1)*axis_range(2,2) < 0)) then ! will draw the target, if 0 is on the plot
   x_lab = 0.
   x_target=off_set + (x_lab - axis_range(:,1))*graph_size/(axis_range(:,2)-axis_range(:,1))                        ! 3+1*15/10 3+3.5*15/5
   write(uc, *) '   set color steelblue '
   write(uc, *) '   amove ', x_target
   write(uc, *) '   circle ', graph_size(1)/70, ' fill steelblue' 
   write(uc, *) '   amove ', x_target(1)-graph_size(1)/40, x_target(2)+graph_size(1)/40
   write(uc, *) '   set hei', graph_size(1)/40
   write(uc, *) '   text Target'
 end if
!
! magnetic field
!
   x_b = 0.
   call get_lab_position(0,x_b, x_l)
   x_lab(1) = x_l(3)   ! z-component goes to x-axis
   x_lab(2) = x_l(1)   ! x-component goes to y-axis
   x_magnet=off_set + (x_lab - axis_range(:,1))*graph_size/(axis_range(:,2)-axis_range(:,1))
   write(uc, *) '   set color firebrick '
   write(uc, *) '   amove ', x_magnet
   write(uc, *) '   circle ', graph_size(1)/100, ' fill firebrick' 
   write(uc, *) '   set hei', graph_size(1)/40
   write(uc, *) '   amove ', x_magnet(1)-0*graph_size(1)/10, x_magnet(2)+2*graph_size(1)/40
   write(uc, *) '   text Magnet'
   write(uc, *) '   amove ', x_magnet(1)-0*graph_size(1)/10, x_magnet(2)+1*graph_size(1)/40
   write(uc, *) '   text Reference'
   write(uc, *) '   amove ', x_magnet
   x_b(3) = 1.d0
   call get_lab_position(0,x_b, x_l)  ! ID 0 .. magnet
   x_lab(1) = x_l(3)   ! z-component goes to x-axis
   x_lab(2) = x_l(1)   ! x-component goes to y-axis
   x_vec=off_set + (x_lab - axis_range(:,1))*graph_size/(axis_range(:,2)-axis_range(:,1))
   write(uc, *) '   amove ', x_magnet
   write(uc, *) '   aline ', x_vec, ' arrow end'  
   write(uc, *) '   line_label ', x_magnet, x_vec, ' " z-axis " -0.3'  
!
! detectors
!
   do i= 1, nbr_detectors
    x_d = 0.                                  ! origin in detector frame
    x_d(1) = detector_range(i,1,1) ! minimum range in x-axis in det frame
    call get_lab_position(i,x_d, x_l)
    x_lab(1) = x_l(3)   ! z-component goes to x-axis
    x_lab(2) = x_l(1)   ! x-component goes to y-axis
    x_det=off_set + (x_lab - axis_range(:,1))*graph_size/(axis_range(:,2)-axis_range(:,1))
    x_d = 0.                                  ! origin in detector frame
    x_d(1) = detector_range(i,1,1) ! minimum range in x-axis in det frame
    x_d(3) = 0.3  ! 
    call get_lab_position(i,x_d, x_l)  
    x_lab(1) = x_l(3)   ! z-component goes to x-axis
    x_lab(2) = x_l(1)   ! x-component goes to y-axis
    x_vec=off_set + (x_lab - axis_range(:,1))*graph_size/(axis_range(:,2)-axis_range(:,1))
    write(uc, *) '   amove ', x_det
    write(uc, *) '   set color green '
    write(uc, *) '   aline ', x_vec, ' arrow end'  
    write(uc, *) '   line_label ', x_det, x_vec, ' "z" 0.1'  
    x_d = 0.                                  ! origin in detector frame
    x_d(1) = detector_range(i,1,2) ! maximum range in x-axis in det frame
    call get_lab_position(i,x_d, x_l)  
    x_lab(1) = x_l(3)   ! z-component goes to x-axis
    x_lab(2) = x_l(1)   ! x-component goes to y-axis
    x_vec=off_set + (x_lab - axis_range(:,1))*graph_size/(axis_range(:,2)-axis_range(:,1))
    write(uc, *) '   amove ', x_det
    write(uc, *) '   set color blue '
    write(uc, *) '   aline ', x_vec, ' arrow end'  
    write(uc, *) '   line_label ', x_det, x_vec, ' " ', trim(detector_name(i)),' " 0.1'  
   end do

 close(uc)
end

subroutine make_gle_single_track
 use vars
 implicit none
 integer uc, i, j, k
 real frame_size(2), graph_size(2), x_target(2), x_magnet(2), x_lab(2), x_det(2)
 real off_set_x(2),off_set_y(2), axis_range_x(2,2), axis_range_y(2,2)
 real x_vec(2), y_lab(2), y_t(2), y_det(2)
 double precision x_b(3), x_l(3), x_d(3)
!
 uc=10
 frame_size(1)  = 40. 	! frame size (40 ; 40)
 frame_size(2)  = 40. 	! frame size (40 ; 40)
 off_set_x(1)   = 3.	! graph-offset of x-track-drawing inside frame 
 off_set_x(2)   = 23.	! graph-offset of x-track-drawing inside frame 
 off_set_y(1)   = 3.	! graph-offset of y-track-drawing inside frame 
 off_set_y(2)   = 3.	! graph-offset of y-track-drawing inside frame 
 graph_size(1)  = 30.	! graph size (30 ; 15)	
 graph_size(2)  = 15.	! graph size (30 ; 15)	
 axis_range_x(1,1) = -1.	! xmin on x-axis
 axis_range_x(1,2) = 9.	! xmax on x-axis
 axis_range_x(2,1) = -3.5	! ymin on y-axis   , x-track
 axis_range_x(2,2) = 1.5	! ymax on y-axis   , x-track
 axis_range_y(1,:)  = axis_range_x(1,:)
 axis_range_y(2,1) = -2.5	! ymin on y-axis   , y-track
 axis_range_y(2,2) = 2.5	! ymax on y-axis   , y-track
 open(unit=uc, file=trim(track_debug_gle_file))
 write(uc, *) 'size ', frame_size
 write(uc, *) 'include "shape.gle" '
 write(uc, *) 'amove ', off_set_x
 write(uc, *) 'begin graph'
 write(uc, *) '  nobox'
 write(uc, *) '  fullsize'
 write(uc, *) '  size ', graph_size
 write(uc, *) '   title "x-track through GLAD... ',nbr_steps(1),' steps  " hei 0.7'
 write(uc, *) '   ytitle "x-position (m)" hei 0.6'
 write(uc, *) '   xtitle "z-position (m)" hei 0.6'
 write(uc, *) '   xaxis min ',axis_range_x(1,1), ' max ',axis_range_x(1,2)
 write(uc, *) '   yaxis min ',axis_range_x(2,1), ' max ',axis_range_x(2,2)
 write(uc, *) '   x2axis off'
 write(uc, *) '   y2axis off'
 write(uc, *) '   xlabels hei 0.6'
 write(uc, *) '   ylabels hei 0.6'
 write(uc, *) '   key pos tr hei 0.6'
 write(uc, *) '   data ',trim(track_debug_file),' d4=c3,c1 '
 write(uc, *) '   data ',trim(output_file(2)),' d5=c3,c1 '

 write(uc, *) '   d4 line lstyle 1 lwidth 0.01 color blue key "current track"'
 write(uc, *) '   d5 line lstyle 1 lwidth 0.01 color black  key "reference track" '
 write(uc, *) 'end graph'
!
 write(uc, *) 'amove ', off_set_y
 write(uc, *) 'begin graph'
 write(uc, *) '  nobox'
 write(uc, *) '  fullsize'
 write(uc, *) '  size ', graph_size
 write(uc, *) '   title "y-track through GLAD... ',nbr_steps(1),' steps  " hei 0.7'
 write(uc, *) '   ytitle "y-position (m)" hei 0.6'
 write(uc, *) '   xtitle "z-position (m)" hei 0.6'
 write(uc, *) '   xaxis min ',axis_range_y(1,1), ' max ',axis_range_y(1,2)
 write(uc, *) '   yaxis min ',axis_range_y(2,1), ' max ',axis_range_y(2,2)
 write(uc, *) '   x2axis off'
 write(uc, *) '   y2axis off'
 write(uc, *) '   xlabels hei 0.6'
 write(uc, *) '   ylabels hei 0.6'
 write(uc, *) '   key pos tr hei 0.6'
 write(uc, *) '   data ',trim(track_debug_file),' d4=c3,c2 '
 write(uc, *) '   data ',trim(output_file(2)),' d5=c3,c2 '

 write(uc, *) '   d4 line lstyle 1 lwidth 0.01 color blue key "current track"'
 write(uc, *) '   d5 line lstyle 1 lwidth 0.01 color black  key "reference track" '
 write(uc, *) 'end graph'
!
! some extra drawing...
!
 if ((axis_range_x(1,1)*axis_range_x(1,2) < 0) .and.(axis_range_x(2,1)*axis_range_x(2,2) < 0)) then ! will draw the target, if 0 is on the plot
   x_lab = 0.
   x_target=off_set_x + (x_lab - axis_range_x(:,1))*graph_size/(axis_range_x(:,2)-axis_range_x(:,1))                        ! 3+1*15/10 3+3.5*15/5
   write(uc, *) '   set color steelblue '
   write(uc, *) '   amove ', x_target
   write(uc, *) '   circle ', graph_size(1)/200, ' fill steelblue' 
   write(uc, *) '   amove ', x_target(1)-graph_size(1)/40, x_target(2)+graph_size(1)/40
   write(uc, *) '   set hei', graph_size(1)/40
   write(uc, *) '   text Target'
 end if
 if ((axis_range_y(1,1)*axis_range_y(1,2) < 0) .and.(axis_range_y(2,1)*axis_range_y(2,2) < 0)) then ! will draw the target, if 0 is on the plot
   y_lab = 0.
   y_t=off_set_y + (y_lab - axis_range_y(:,1))*graph_size/(axis_range_y(:,2)-axis_range_y(:,1))                        ! 3+1*15/10 3+3.5*15/5
   write(uc, *) '   set color steelblue '
   write(uc, *) '   amove ', y_t
   write(uc, *) '   circle ', graph_size(1)/200, ' fill steelblue' 
   write(uc, *) '   amove ', y_t(1)-graph_size(1)/40, y_t(2)+graph_size(1)/40
   write(uc, *) '   set hei', graph_size(1)/40
   write(uc, *) '   text Target'
 end if
!
! magnetic field
!
   x_b = 0.
   call get_lab_position(0,x_b, x_l)
   x_lab(1) = x_l(3)   ! z-component goes to x-axis
   x_lab(2) = x_l(1)   ! x-component goes to y-axis
   x_magnet=off_set_x + (x_lab - axis_range_x(:,1))*graph_size/(axis_range_x(:,2)-axis_range_x(:,1))
   write(uc, *) '   set color firebrick '
   write(uc, *) '   amove ', x_magnet
   write(uc, *) '   circle ', graph_size(1)/100, ' fill firebrick' 
   write(uc, *) '   set hei', graph_size(1)/40
   write(uc, *) '   amove ', x_magnet(1)-0*graph_size(1)/10, x_magnet(2)+2*graph_size(1)/40
   write(uc, *) '   text Magnet'
   write(uc, *) '   amove ', x_magnet(1)-0*graph_size(1)/10, x_magnet(2)+1*graph_size(1)/40
   write(uc, *) '   text Reference'
   write(uc, *) '   amove ', x_magnet
   x_b(3) = 1.d0
   call get_lab_position(0,x_b, x_l)  ! ID 0 .. magnet
   x_lab(1) = x_l(3)   ! z-component goes to x-axis
   x_lab(2) = x_l(1)   ! x-component goes to y-axis
   x_vec=off_set_x + (x_lab - axis_range_x(:,1))*graph_size/(axis_range_x(:,2)-axis_range_x(:,1))
   write(uc, *) '   amove ', x_magnet
   write(uc, *) '   aline ', x_vec, ' arrow end'  
   write(uc, *) '   line_label ', x_magnet, x_vec, ' " z-axis " -0.3'  
!
! detectors
!
   do i= 1, nbr_detectors
     if (track_hit_pattern(i)) then
       x_d = track_det_frame(i,:)
       call get_lab_position(i,x_d, x_l)
!
       x_lab(1) = x_l(3)   ! z-component goes to x-axis
       x_lab(2) = x_l(1)   ! x-component goes to y-axis
       x_det=off_set_x + (x_lab - axis_range_x(:,1))*graph_size/(axis_range_x(:,2)-axis_range_x(:,1))
       write(uc, *) '   amove ', x_det
       write(uc, *) '   set color green '
       write(uc, *) '   circle ', graph_size(1)/200, ' fill green' 
!
       y_lab(1) = x_l(3)   ! z-component goes to x-axis
       y_lab(2) = x_l(2)   ! y-component goes to y-axis
       y_det=off_set_y + (y_lab - axis_range_y(:,1))*graph_size/(axis_range_y(:,2)-axis_range_y(:,1))
       write(uc, *) '   amove ', y_det
       write(uc, *) '   set color green '
       write(uc, *) '   circle ', graph_size(1)/200, ' fill green' 
     end if  
   end do

 close(uc)
end


double precision function distance_detector_plane(volume_id,x_det)
 use vars
 implicit none
 double precision   :: x_det(3)
 integer volume_id
! 
 distance_detector_plane = dot_product(x_det,detector_xy_plane(volume_id,1:3)) - detector_xy_plane(volume_id,4)
end 

!
! convertes detector-coordinates into lab-coordinates
!
subroutine get_lab_position(volume_id,x_det, x_lab )
 use vars
 implicit none
 double precision   :: x_det(3), x_lab(3) 
 integer volume_id
! 
  x_lab(1) = x_det(1) * dcos(detector_position_in_lab(volume_id,4)) - x_det(3) * dsin(detector_position_in_lab(volume_id,4))
  x_lab(2) = x_det(2)
  x_lab(3) = x_det(3) * dcos(detector_position_in_lab(volume_id,4)) + x_det(1) * dsin(detector_position_in_lab(volume_id,4))
  x_lab    = x_lab + detector_position_in_lab(volume_id,1:3)
end 

!
! convertes lab-coordinates into detector-coordinates
!
subroutine get_det_position(volume_id,x_lab, x_det )
 use vars
 implicit none
 double precision   :: x_det(3), x_lab(3) , x_tmp(3)
 integer volume_id
! 
  x_tmp    = x_lab - detector_position_in_lab(volume_id,1:3)
  x_det(1) = x_tmp(1) * dcos(detector_position_in_lab(volume_id,4)) + x_tmp(3) * dsin(detector_position_in_lab(volume_id,4))
  x_det(2) = x_tmp(2)
  x_det(3) = x_tmp(3) * dcos(detector_position_in_lab(volume_id,4)) - x_tmp(1) * dsin(detector_position_in_lab(volume_id,4))
end 

!
! determines, if lab-coordinates are inside a detector
!
logical function inside_detector(volume_id, x_lab)
 use vars
 implicit none
 integer volume_id, i, j
 logical inside
 double precision  x_det(3),x_lab(3) 
!
 call get_det_position(volume_id,x_lab, x_det )
 inside = .true.
 do i=1,3
   inside = inside .and. (x_det(i) > detector_range(volume_id,i,1))
   inside = inside .and. (x_det(i) < detector_range(volume_id,i,2))
 end do  
 inside_detector = inside
end 

logical function inside_master(xyz_local)
 use vars
 implicit none
 double precision   :: xyz_local(3) 
!
 inside_master = (xyz_local(1) > master_borders(1,1)) .and. (xyz_local(1) < master_borders(1,2)) .and. &
          (xyz_local(2) > master_borders(2,1)) .and. (xyz_local(2) < master_borders(2,2)) .and. &
          (xyz_local(3) > master_borders(3,1)) .and. (xyz_local(3) < master_borders(3,2)) 
end 

logical function inside_b_field(xyz_local)
 use vars
 implicit none
 double precision   :: xyz_local(3) 
!
 inside_b_field = (xyz_local(1) > b_field_borders(1,1)) .and. (xyz_local(1) < b_field_borders(1,2)) .and. &
          (xyz_local(2) > b_field_borders(2,1)) .and. (xyz_local(2) < b_field_borders(2,2)) .and. &
          (xyz_local(3) > b_field_borders(3,1)) .and. (xyz_local(3) < b_field_borders(3,2)) 
end 

subroutine b_field_init
 use vars
 implicit none
 integer limits(3,3), ch(3), ch_l(3), ch_h(3), h_l(3), h_h(3) 
 double precision dummy(3), x_lab(3), x_b(3), x_t(3), phi, xyz(3)
 double precision b_low(3), b_high(3), b(3)
 double precision, allocatable :: b_field_map_orig(:,:,:,:)			! map of magnetic field... (x,y,z,1:3) , last entry for Bx,By,Bz
 double precision range_b_field(3,2), d_l(3), d_h(3), f(8)
 logical l1, l2, l3
 integer ii
 integer i, j, k  
!
 range_b_field = 0.
 open (unit=8, file=magnetic_field_file, status='old')
   read(8,*)
   do i=1,3
     read(8,*) limits(i,:)
   end do
   limits(:,1:2) = limits(:,1:2) / 5  ! this is fixed, see map!
   limits(:,3) = limits(:,3) + 1
   allocate(b_field_map_orig(limits(1,1)-1:limits(1,2)+1,limits(2,1)-1:limits(2,2)+1,limits(3,1)-1:limits(3,2)+1,3 ) )
   b_field_map_orig = 0.
   do i=limits(1,1),limits(1,2)
    do j=limits(2,1),limits(2,2)
     do k=limits(3,1),limits(3,2)
       read(8,*) dummy  ,  b_field
       b_field_map_orig(i,j,k,:) = b_field
       if (j==0 .and. dabs(b_field(2)) > 0.) then
!         if (i==0) print*,i,j,k,dummy,b_field
         do ii=1,3
          if (b_field(ii) <  range_b_field(ii,1) ) range_b_field(ii,1) = b_field(ii)
          if (b_field(ii) >  range_b_field(ii,2) ) range_b_field(ii,2) = b_field(ii)
         end do
       end if  
     end do
    end do
   end do
 close(8)
 do ii=1,3
   print*,'Range b-field for y==0',  range_b_field(ii,1)  , range_b_field(ii,2) 
 end do
 
 ch_l = b_field_resolution_factor*b_field_borders(:,1)
 ch_h = b_field_resolution_factor*b_field_borders(:,2)
 ch   = ch_h - ch_l +1
 allocate(b_field_map(ch_l(1):ch_h(1),ch_l(2):ch_h(2),ch_l(3):ch_h(3),3 ) )
 b_field_map = 0.
 phi = b_field_position_in_lab(4)
 print*,'phi ',phi
 do i=ch_l(1),ch_h(1)
  do j=ch_l(2),ch_h(2)
   do k=ch_l(3),ch_h(3)
! do i=0,0
!  do j=-6,-6
!   do k=ch_l(3),ch_h(3)
    x_lab(1) = dble(i) / dble(b_field_resolution_factor)								
    x_lab(2) = dble(j) / dble(b_field_resolution_factor)
    x_lab(3) = dble(k) / dble(b_field_resolution_factor)
    x_t      = x_lab - b_field_position_in_lab(1:3)							! transition
    x_b(1)   = x_t(1)*dcos(phi) + x_t(3)*dsin(phi)							! rotation around y-axis
    x_b(2)   = x_t(2)														! rotation around y-axis
    x_b(3)   = x_t(3)*dcos(phi) - x_t(1)*dsin(phi)							! rotation around y-axis
!    
    xyz = b_field_resolution_factor*x_b										! now we are in the frame of the glad-magnet
    h_l = floor(xyz)
    if ( (h_l(1) > limits(1,1)-2).and. &
         (h_l(2) > limits(2,1)-2).and. &
         (h_l(3) > limits(3,1)-2) ) then
     h_h = h_l + 1
     if ( (h_h(1) <= limits(1,2)+1) .and. (h_h(2) <= limits(2,2)+1) .and. (h_h(3) <= limits(3,2)+1) ) then ! inside boundaries
      d_l = xyz - h_l
      d_h = h_h - xyz
      
      f(1)    = (d_h(1)*d_h(2)*d_h(3))
      f(2)    = (d_h(1)*d_h(2)*d_l(3))
      f(3)    = (d_h(1)*d_l(2)*d_h(3))
      f(4)    = (d_l(1)*d_h(2)*d_h(3))
      f(5)    = (d_l(1)*d_l(2)*d_h(3))
      f(6)    = (d_l(1)*d_h(2)*d_l(3))
      f(7)    = (d_h(1)*d_l(2)*d_l(3))
      f(8)    = (d_l(1)*d_l(2)*d_l(3))

      f       = f / sum(f)
!      print*,f
!      print*,sum(f)
      if (i==0 .and.j==0 .and. k==2) print*,'b-field-orig: ',b_field_map_orig(i,j,k,:)  
      
      b       = f(1)*b_field_map_orig(h_l(1),h_l(2),h_l(3),:)
      b   = b + f(2)*b_field_map_orig(h_l(1),h_l(2),h_h(3),:)
      b   = b + f(3)*b_field_map_orig(h_l(1),h_h(2),h_l(3),:)
      b   = b + f(4)*b_field_map_orig(h_h(1),h_l(2),h_l(3),:)
      b   = b + f(5)*b_field_map_orig(h_h(1),h_h(2),h_l(3),:)
      b   = b + f(6)*b_field_map_orig(h_h(1),h_l(2),h_h(3),:)
      b   = b + f(7)*b_field_map_orig(h_l(1),h_h(2),h_h(3),:)
      b   = b + f(8)*b_field_map_orig(h_h(1),h_h(2),h_h(3),:)
!
      if (i==0 .and.j==0 .and. k==2) print*,'b-field-interpol: ',b  
 
!
      b_field_map(i,j,k,1) = b(1)*dcos(phi) - b(3)*dsin(phi)					! rotation around y-axis, now backwards
      b_field_map(i,j,k,2) = b(2)												! rotation around y-axis, now backwards
      b_field_map(i,j,k,3) = b(3)*dcos(phi) + b(1)*dsin(phi)					! rotation around y-axis, now backwards
      if (i==0 .and.j==0 .and. k==2) print*,'b-field-rotate: ',b_field_map(i,j,k,:)  
!
!       b_field_map(i,j,k,1) = 0.
!       b_field_map(i,j,k,2) = (b(2))
!       b_field_map(i,j,k,3) = 0.
!       if (j==0 .and. dabs(b(2)) > 0.) print*,i,j,k,b(2)
     end if
    end if
    if (i == ch_l(1) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         print*,'problem with b-field area, decrease x_min', i,j,k,b_field_map(i,j,k,:)
    if (i == ch_h(1) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         print*,'problem with b-field area, increase x_max', i,j,k,b_field_map(i,j,k,:)
    if (j == ch_l(2) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         print*,'problem with b-field area, decrease y_min', i,j,k,b_field_map(i,j,k,:)
    if (j == ch_h(2) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         print*,'problem with b-field area, increase y_max', i,j,k,b_field_map(i,j,k,:)
    if (k == ch_l(3) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         print*,'problem with b-field area, decrease z_min', i,j,k,b_field_map(i,j,k,:)
    if (k == ch_h(3) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         print*,'problem with b-field area, increase z_max', i,j,k,b_field_map(i,j,k,:)
   end do
  end do 
 end do
!
 b_field_map_limits(:,1) = ch_l
 b_field_map_limits(:,2) = ch_h
 b_field_map = b_field_map * glad_current / glad_current_nominal_current
end

!-- Author : Rene Reifarth
!
!User routine: Liefert Gaussverschmierung bei geg. sigma
! UM WERT NULL -> MUSS ZUM EIGENTL. WERT ADDIERT (!!) WERDEN
! EINHEITEN: GLEICHE EINHEIT WIE HALBWERTSBREITE
! EINGABE: HALB
! RUECKGABE: ENER
!
double precision function GAUSSVER(SIGMA)
    Implicit none
    double precision SIGMA,ENER,RA,Y,SHIFT,GAU,sig2,maxi, HALB
    INTEGER RECHT
    DIMENSION RA(4)

    if (SIGMA > 0.) then
!
! maxi= summe(2**-(k**2)) k,0,infinity
!
      maxi=1.564468413606d0
!     SIGMA**2=HALB**2/LN(64)
!     HALB = 2sqrt(2ln(2)) sigma
!      sig2=halb**2/alog(64.)
      sig2 = sigma*sigma
      halb = sigma * 2.35482004503d0
      GAU=0.
      Y=1.
!
!  WUERFELN BIS UNTER DER GAUSSFUNKTION
!
      DO WHILE (Y.GT.GAU)
        call random_number(RA)
        RA(1)= RA(1)*maxi
! 
!      WELCHES RECHTECK?
!
        RECHT=1 
        Y=1.
        DO WHILE (RA(1).GT.Y)
            RECHT=RECHT+1
            Y=Y+0.5**((RECHT-1)**2)
        ENDDO
!
!      WO IN DIESEM RECHTECK? LAENGE=HALB,HOEHE
!      0.5**((RECHT-1)**2)
!
        ENER=(FLOAT(RECHT-1)+RA(2))*HALB
        Y=0.5**((RECHT-1)**2)*RA(3)
!
!  GAUSSFUNKTION
!
        GAU=DEXP(-(ENER**2)/sig2/2)
      END DO
      IF (RA(4).GT.0.5) THEN
          ENER=-ENER
      END IF
      GAUSSVER = ENER
    else
      GAUSSVER = 0.
    end if    
END

!
! stand alone routine to solve the LES m*b=x  ; n is the dimension of the system
!
subroutine solve_linear_equation_system(n,m,b,x)
 implicit none
 integer, intent(in) :: n   ! rank of nxn matrix
 integer i
 double precision m_tmp(n,n), det_m, det_n(n)
 double precision, intent(out) :: x(n)
 double precision, intent(in) :: m(n,n), b(n)
!
! will solve using Cramer's rule
!
 call determinant(n,m,det_m)
! 
 m_tmp = m
 if (det_m /= 0.) then
   do i=1,n
     m_tmp(:,i) = b
     if (i>1)  m_tmp(:,i-1) = m(:,i-1)
     call determinant(n,m_tmp,det_n(i))
   end do
   x = det_n / det_m
 else
   x = 0.
 end if    

end
!
! stand alone routine to determine the determinant of a nxn matrix. 
! The select case could be reduced to the cases 1 and default, but the explicit treatment 
! of cases 2 and 3 enhances execution speed by a factor of ~5
!
recursive subroutine determinant(n,m,d) 
 implicit none
 integer, intent(in) :: n   ! rank of nxn matrix
 integer i,j
 double precision d_tmp
 double precision, intent(out) :: d
 double precision, intent(in) :: m(n,n)
 double precision :: m_red(n-1,n-1)
! 
 select case(n)
   case (1) 
     d = m(1,1)
   case (2) 
     d = m(1,1)*m(2,2) - m(2,1)*m(1,2)
   case (3) 
     d = m(1,1) * (m(2,2)*m(3,3) - m(2,3)*m(3,2))  &
       - m(1,2) * (m(2,1)*m(3,3) - m(2,3)*m(3,1))  &
       + m(1,3) * (m(2,1)*m(3,2) - m(2,2)*m(3,1))  
   case default 
     d = 0.d0
     do i=1,n
       do j=1,i-1
         m_red(:,j) = m(2:n,j)
       end do  
       do j=i+1,n
         m_red(:,j-1) = m(2:n,j)
       end do  
       call determinant(n-1, m_red,d_tmp)
       d = d_tmp * m(1,i) - d
     end do  
     if (modulo(n,2) == 0) d = -d
 end select  
!
end

!
!  converts a id-number into a detector hit pattern. inverse to the function det_pattern_id
!

subroutine get_hit_pattern(n,det_id,l)
implicit none
integer n,i, det_id, two_power, x1,x2
logical l(n)
!
 l  = .false.
 x1 = det_id
 do i=1,n-1
   two_power = 2**(n-i)
   x2 = x1 / two_power
   if (x2 > 0) then
     x1 = modulo(det_id,two_power)
     l(i) = .true.
   end if  
 end do
 if (x1 > 0) l(n) = .true. 
end

integer function det_pattern_id(n,l)
implicit none
integer n,i, pattern_id
logical l(n)
!
 pattern_id = 0
 do i=1, n-1
   if (l(i)) pattern_id = pattern_id + 1
   pattern_id = pattern_id*2
 end do  
 if (l(n)) pattern_id = pattern_id + 1
 det_pattern_id = pattern_id
end

! c) von außen kommen direkt die Parameter für 2 Spuren! Schon vorsortiert, sodass der Tracker weiß, welche Punkte zu einer Spur gehören. Dann werden beide Spuren mit gemeinsamen Punkt auf dem Target gefittet. Das ist, denke ich, der goldene Weg. Denn dann kannst du wiederum außen die Permutationen durchgehen, die Dir sinnvoll erscheinen (Kombination von Ladungen auf TOFD etc). Zurück bekommst du dann 2 x den 3er Impuls / q + ch2.

!von außen gesehen gibt es dann quasi mehrere Routinen:
!a) 1 Spur, keine Vorgaben für Target Position
!b) 1 Spur, Target Position auch eingeschränkt (die Mathematik ist etwas anders, kann ich dir gleich mal zeigen. Deshalb 2 !Routinen)
!c) 2 Spuren, gemeinsamer Target Punkt, sonst keine Einschränkungen
!d) 2 Spuren, gemeinsamer Target Punkt, Target Punkt zusätzlich eingeschränkt

!die Spuren übergibst du dann quasi aus c++ so:
!
!  int nbr_of_detectors = 7;
!  double x_positions[nbr_of_detectors] ;
!  double y_positions[nbr_of_detectors] ;
!  double z_positions[nbr_of_detectors] ;
!  bool hit_pattern[nbr_of_detectors] ;
!  double track_parameter[7] ;
!  single_track_from_cpp_(n_det,x_positions, y_positions, z_positions, hit_pattern, track_parameter)
! 
!  track paramer : 3er Impuls, chi2, position auf Target (xyz)
