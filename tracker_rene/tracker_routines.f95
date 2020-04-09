module vars
 implicit none
 logical                  :: grafical_output=.true.                    ! determines if global grafical output is wanted
 logical                  :: track_out=.false.                        ! determines single-track data are written to file
 logical            :: reference_track                               ! determines if text output is set during tracking
 logical            :: debug_track=.true.                        ! if tracking is done in debugging mode
 logical            :: get_new_derivatives                        ! determines if the derivatives have to be freshly calculated
 logical            :: get_new_y_corrections=.true.                ! determines if the y_corrections have to be freshly calculated
 logical            :: sweep_calibration=.false.                    ! determines if the geometry will get calibrated based on data from a magnetic field sweep
 logical            :: calibration_mode=.false.                    ! program is generally in calibration mode rather than production / tracking  mode
 logical,parameter  :: derivative_ini_from_file=.false.            ! determines if the derivatives ar freshly calculated with track data from file, special use
 logical, allocatable  :: det_hit(:)                            ! .true. if detector was hit during current track
 logical, allocatable  :: det_passed(:)                            ! .true. if xy-plane of detector was hit during current track
 logical, allocatable  :: det_consider(:,:)                        ! (detector,dimension) .true. dimension (x,y,z) of detector is considered during tracking. .false. if uncertainty of detector is <= 0.
 logical, allocatable  :: det_step_function(:,:)                ! (detector,dimension) .true. dimension (x,y,z) of detector is considered as step function during tracking. uncertainty is paddle width .false. if uncertainty of detector is >= 0.
 logical, allocatable  :: trigger_matrix(:)                     ! true, if a certain pattern ID is allowed for a given track
 logical, allocatable  :: trigger_always_together(:,:)            ! true, if 2 detectors have to be hit together for a given track
 logical, allocatable  :: trigger_never_together(:,:)            ! true, if 2 detectors are never hit together for a given track
 logical               :: screen_output=.false.                        ! if true, standard output is screen, otherwise logfile
 logical               :: calibration_event_by_event      ! true is calibration is done with events from experiment rather than average interception points
 logical               :: sweep_data                      ! true is calibration is done with magnetic sweep runs, .false. if done with scatter data
 character(LEN=256) :: ini_file, output_file(3), geometry_file,magnetic_field_file, derivative_file, sweep_calibration_file
 integer,parameter  :: length_ch_trigger_logic=1000
 character(LEN=length_ch_trigger_logic) :: ch_trigger_logic
 integer, parameter  :: output_unit_log = 75                        ! output unit for log file. 
 integer output_unit
 character(LEN=256), parameter :: tracker_log_file='tracker_output.log'
 character(LEN=256), parameter :: calib_file='calibration.ini'
 character(LEN=256), parameter :: track_debug_file='single_track.dat'
 character(LEN=256), parameter :: track_debug_file_d1='double_track1.dat'
 character(LEN=256), parameter :: track_debug_file_d2='double_track2.dat'
 character(LEN=256), parameter :: track_debug_gle_file='single_track.gle'
 character(LEN=256), parameter :: double_track_debug_gle_file='double_track.gle'
 integer, parameter :: unit_standard_track=9    ! output unit for reference track
 integer, parameter :: unit_debug_track=22      ! output unit for track(s) in debug mode
 double precision   :: dt, t, t_total, dt_fine, dt_coarse
 integer,parameter  :: nbr_dimensions=6                            ! (x,y,z,px,py,pz)
 double precision   :: x(nbr_dimensions), dx(nbr_dimensions), derivative_b_field(nbr_dimensions)
 double precision   :: x_start(nbr_dimensions), p_start(3), x_reference_step(nbr_dimensions) 
 double precision   :: x_track1(nbr_dimensions), x_track2(nbr_dimensions), x_reference(nbr_dimensions)
 double precision   :: b_field(3), v(3)                            ! magnetic field, velocity
 double precision   :: b_field_position_in_lab(4)                ! position of magnetic field coordinate system in lab-frame (x,y,z,rotation_around_y-axis)
 double precision   :: glad_current                                ! magnetic field map will be multiplied with this number divided by 3584...
 double precision,parameter   :: glad_current_nominal_current=3584.d0        ! current for which the field map is calculated
 double precision   :: m_amu, m, m2                                ! mass of particle, AMU + SI
 double precision   :: q_e, q                                      ! charge of particle in elementary unit + SI 
 double precision   :: master_borders(3,2)                         ! borders of master volume ((xmin,xmax),(ymin,ymax),(zmin,zmax))
 double precision   :: b_field_borders(3,2)                     ! borders of magnetic field ((xmin,xmax),(ymin,ymax),(zmin,zmax))
 double precision   :: spatial_resolution                         ! spatial resolution of the entire geometry, given by best detector resolution...
 double precision   :: max_momentum_deviation(3)                   ! constrains the possible solutions when multiple tracks are possible
 double precision, parameter   :: crossing_resolution = 1.d-3                         ! spatial resolution allowed for crossing tracks
 double precision   :: slope_parameter(4)                         ! parameters of the linear slope describing the particle track outside the magnetic field area
 double precision,parameter,dimension(3) :: norm_x = (/1.d0,0.d0,0.d0/)  ! norm-vector in x-direction ...
 double precision,parameter,dimension(3) :: norm_y = (/0.d0,1.d0,0.d0/)  ! norm-vector in y-direction ...
 double precision,parameter,dimension(3) :: norm_z = (/0.d0,0.d0,1.d0/)  ! norm-vector in z-direction ...
 double precision, parameter :: c=299792458.d0                    ! speed of light, SI units
 double precision, parameter :: c2=c*c    
 double precision, parameter :: e=1.60217662d-19                ! elementary charge, SI units
 double precision, parameter :: amu=1.660539040d-27                ! atomic mass, SI units
 double precision, allocatable :: b_field_map(:,:,:,:)                ! map of magnetic field... (x,y,z,1:3) , last entry for Bx,By,Bz
 double precision, allocatable :: b_field_map_1amp(:,:,:,:)            ! map of magnetic field for a current of 1 Amp ... (x,y,z,1:3) , last entry for Bx,By,Bz
 integer         , parameter :: b_field_map_resolution=5        ! resolution of the field in cm !! , hence the integer ... 
 integer         , parameter :: b_field_resolution_factor=100/b_field_map_resolution        ! this is to convert position in m into channels in the b-field map 
 integer              :: nbr_steps(2), nbr_detectors, nbr_tracks=1000
 double precision, allocatable :: detector_position_in_lab(:,:)            ! position of detector coordinate system in lab-frame (detector_ID:x,y,z,rotation_around_y-axis)
 double precision, allocatable :: detector_xy_plane(:,:)                ! x-y-plane of detector in lab-frame (detector_ID:4) last 4 values correspond to parameters in: a1*x+a2*y+a3*z+a4=0)
 double precision, allocatable :: detector_track_interactions_lab_frame(:,:)        ! intersection of track point with z=0 plane of detector in lab coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable :: detector_track_interactions_lab_frame_ref(:,:)        ! intersection of track point with z=0 plane of detector in lab coordinates (detector_ID,3) last 3: x,y,z) , reference track
 double precision, allocatable :: detector_track_interactions_det_frame(:,:)        ! intersection of track point with z=0 plane of detector in detector coordinates (detector_ID,3) last 3: x,y,z)
 double precision, allocatable :: detector_track_interactions_time(:)        ! time of intersection of track point with z=0 plane of detector 
 double precision, allocatable :: detector_track_interactions_path(:)        ! flightpath between starting point and intersection of track point with z=0 plane of detector 
 double precision, allocatable :: detector_track_interactions_det_frame_ref(:,:)    ! intersection of track point with z=0 plane of detector in detector coordinates (detector_ID,3) last 3: x,y,z), reference track
 double precision, allocatable :: detector_track_interactions_det_frame_ref_step(:,:)    ! intersection of track point with z=0 plane of detector in detector coordinates (detector_ID,3) last 3: x,y,z), reference during step-wise initialization 
 double precision, allocatable :: track_lab_frame(:,:)                                ! experimental track in lab coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable :: track_det_frame(:,:),track_det_frame1(:,:),track_det_frame2(:,:)        ! experimental track in detector coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable :: track_det_frame_y_corrected(:,:)        ! experimental track in detector coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable :: track_det_frame1_y_corrected(:,:),track_det_frame2_y_corrected(:,:)        ! experimental track in detector coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable :: sigma_track(:,:)                                    ! absolute unctertainties of experimental track 
 double precision, allocatable :: sigma_track2(:,:)                                    ! square of  sigma_track
 double precision              :: pos_target(3),sigma_target(3),sigma_target2(3), pos_target_original(3)    !  target position, absolute uncertainty and it's square (x,y,z)
 logical, allocatable          :: track_hit_pattern(:),track_hit_pattern1(:),track_hit_pattern2(:)                            ! true, if detector provided data (detector_ID) 
 logical, allocatable          :: track_hit_pattern_used(:),track1_hit_pattern_used(:),track2_hit_pattern_used(:)                            ! true, if detector provided data (detector_ID) 
 logical, allocatable    :: track_hit_pattern_from_chi2(:),track1_hit_pattern_from_chi2(:),track2_hit_pattern_from_chi2(:)                            ! true, if detector current track SHOULD go through this detector (detector_ID) 
 logical, allocatable    :: paddle_hit(:,:),paddle_hit_1(:,:),paddle_hit_2(:,:)            ! true, if detector is set to "step function" and track goes through the corresponding paddle (detector_id,dimension)
 double precision, allocatable :: detector_range(:,:,:)                                ! range of detector in detector-frame (detector_ID:x,y,z:min,max,delta)
 double precision, allocatable :: derivative_theta(:),offset_theta(:)                ! derivative and offset of x-position in detector-frame (detector_ID) when theta_yz is varied
 double precision, allocatable :: derivative_x(:),offset_x(:)                ! derivative and offset of x-position in detector-frame (detector_ID) when theta_yz is varied
 double precision, allocatable :: derivative_q_over_a(:),offset_q_over_a(:)                ! derivative and offset of x-position in detector-frame (detector_ID) when q/a of particle is varied
 double precision, allocatable :: derivative_p(:),offset_p(:)                ! derivative and offset of x-position in detector-frame (detector_ID) when momentum of particle is varied
 integer          , parameter  :: nbr_variables=5                                ! variable to be fitted during tracking routine
 double precision, allocatable :: derivative_variable(:,:),offset_variable(:,:)                ! derivative and offset of x-position in detector-frame (detector_ID) when any variable is varied
 double precision, allocatable :: offset_ave_x(:)                                ! x-position in detector-frame (detector_ID) when x or theta_yz is varied (averaged), reference track
 double precision, allocatable :: offset_ave_y(:)                                ! y-position in detector-frame (detector_ID)  reference track
 integer, parameter            :: length_detector_name = 10
 character (LEN=length_detector_name), allocatable :: detector_name(:)                    ! name of detector (detector_ID)
 double precision, allocatable :: stopping_power(:)                        ! relative stopping power in detector material (detector_ID) (dE/dx / E) for kinetic energy
 integer         , parameter     :: nbr_channels_position_spectra=10000
 integer         , parameter     :: nbr_paramters_position_spectra=6            ! mean, 1-sigma variance, 2x first channel with content > 0, last channel with content > 0, (1..4 values already converted to in m!!!, 5,6 in channels)
 integer                        :: max_dim_pos_spectra=3
 integer, allocatable            :: detector_position_spectra(:,:,:)                    !    spectra about xyz distribution of tracks going through detectors (detector_ID:Dimension(1..x,2..y,3..z):Channel_x,y,z)
 double precision, allocatable    :: detector_position_spectra_paramter(:,:,:)        !    spectra about xyz distribution of tracks going through detectors (detector_ID:Dimension(1..x,2..y,3..z):Paramter_type)
 double precision                 :: x_pos_target, theta_xz_target                    ! origin of track at target, theta in x-z plane at target
 double precision               :: det
 double precision, allocatable  :: sum_yi(:), sum_xiyi(:), sum_xi(:), sum_xixi(:), sum_tracks(:)
!
!  this is for theta_yz, p, x
! 
 double precision ::  a3_matrix(3,3), a3_matrix_inv(3,3), a5_matrix(5,5), a5_matrix_inv(5,5)
 double precision, allocatable ::  a3_matrix_inv_array(:,:,:)                    ! set of matrizes to retrieve single track parameters from detector positions
 double precision, allocatable ::  t3_matrix_inv_array(:,:,:)                    ! set of matrizes to retrieve single track parameters from detector positions, target position also constrained
 double precision, allocatable ::  a5_matrix_inv_array(:,:,:)                    ! set of matrizes to retrieve double track parameters from detector positions
 double precision, allocatable ::  t5_matrix_inv_array(:,:,:)                    ! set of matrizes to retrieve double track parameters from detector positions, target position also constrained
 logical,          allocatable ::  t5_matrix_inv_ok(:,:)                          ! ok, if matrix-inversion was inverted (:1) and successfully inverted (:,2)
 logical,          allocatable ::  a5_matrix_inv_ok(:,:)                          ! ok, if matrix-inversion was inverted (:1) and successfully inverted (:,2)
 logical,          allocatable ::  t3_matrix_inv_ok(:)                          ! ok, if matrix-inversion was successful
 logical,          allocatable ::  a3_matrix_inv_ok(:)                          ! ok, if matrix-inversion was successful
!
!  this is for theta_xz, y
! 
 double precision ::  a2_matrix(2,2), a2_matrix_inv(2,2)
 double precision, allocatable ::  y_a2_matrix_inv_array(:,:,:)                    ! set of matrizes to retrieve single track parameters from detector positions
 double precision, allocatable ::  y_t2_matrix_inv_array(:,:,:)                    ! set of matrizes to retrieve single track parameters from detector positions, target position also constrained
 double precision, allocatable ::  y_a3_matrix_inv_array(:,:,:)                    ! set of matrizes to retrieve single track parameters from detector positions
 double precision, allocatable ::  y_t3_matrix_inv_array(:,:,:)                    ! set of matrizes to retrieve single track parameters from detector positions, target position also constrained
 logical,          allocatable ::  y_a2_matrix_inv_ok(:)                          ! ok, if matrix-inversion was successful
 logical,          allocatable ::  y_t2_matrix_inv_ok(:)                          ! ok, if matrix-inversion was successful
 logical,          allocatable ::  y_t3_matrix_inv_ok(:,:)                          ! ok, if matrix-inversion was inverted (:1) and successfully inverted (:,2)
 logical,          allocatable ::  y_a3_matrix_inv_ok(:,:)                          ! ok, if matrix-inversion was inverted (:1) and successfully inverted (:,2)
!
 double precision ::  chi2_single(2), chi2_double(6)
 double precision ::  theta_yz, y_target, theta_yz1, theta_yz2
 integer zero_crossing, b_field_map_limits(3,2)
 character*1,parameter, dimension(3) :: ch_xyz=(/'x','y','z'/)
 integer, parameter :: length_ch_tf=5
 character(LEN=length_ch_tf), parameter :: ch_true='tTtTt',ch_false='fFfFf'
! 
 integer         , parameter     :: nbr_functions_y_correction=20                 ! 3: only 3rd deg x, 6: 3rd ded. in x and 1st deg in y  8: 3rd ded. in x and 2nd deg in y  8: 3rd ded. in x and 3rd deg in y  
 double precision, allocatable ::  coefficients_y_correction(:,:)               ! coefficients to correct y, if theta_xz is varied (nbr_detector,basic_functions_y_correction)
 double precision, allocatable ::  y_correction(:),y_correction1(:),y_correction2(:)
!
 double precision ::  dx_b_calibration(3)                                            ! used only for calibration purposes
!
 double precision ::  p_vector_y2(2), p_vector_y3(3),    p_vector_x3(3),    p_vector_x5(5)                                                ! raw-results of linearisation
!
 integer         , parameter     :: nbr_channels_2d_spectra=200
 integer                        :: spectrum_dthetay_dpx(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 integer                        :: spectrum_dthetay_dpy(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 integer                        :: spectrum_dthetax_dpx(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 integer                        :: spectrum_dthetax_dpy(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 integer                        :: spectrum_dpy_dpx(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 logical                        :: constrain_target_x, constrain_target_y       ! true, if target position is constrained during fitting
!
 integer*8 counter(6)                                                           ! some global counting, different internal uses 
 character(LEN=256), parameter :: geometry_outpout_file='geometry_position_optimized.ini'
end module vars

module calib
 integer                         :: max_nbr_events_run                   ! max. number of events (contains several hits) to be considered per run
 integer                         :: max_nbr_hits_per_run                 ! max. number of detector hits to be stored per run
 integer                         :: max_nbr_hits_per_event               ! max. number of detector hits per event as found in data
 integer                         :: nbr_runs                             ! number of B-field settings
 integer                          :: max_nbr_good_events                     ! maximum number of valid events to be treated. Will be set autimatically to the minimum number of good events available between all runs
 integer, allocatable            :: nbr_events_in_run(:)                 ! number of events in current run loaded into memory
 integer, allocatable            :: nbr_hits_in_event(:,:)               ! number of hits in current event loaded into memory
 integer, allocatable            :: current_event(:), next_event(:)      ! position of current and next event of run in event-arrays
 integer, allocatable            :: detector_id(:,:), event_id(:,:)      ! detector-ID and event-ID of hit in event-array
 double precision, allocatable   :: x_pos(:,:), y_pos(:,:)               ! x,y position of hit in detector coordinates in event-array
 double precision, allocatable   :: b_field_sweep(:)                     ! magnetic field (per run)
 logical, allocatable            :: optimize_detector(:)                 ! true if detector needs to be position-optimized
 logical, allocatable            :: locked_pair(:,:,:)                   ! true if distance between detector pair is locked during position-optimization (det,det,dimesion)
 integer, parameter              :: nbr_dimensions_optimize = 3          ! number of dimensions to optimize. for the moment 3 - x,y,z
 logical                          :: write_to_calib_histos = .false.         ! if true, data will be written into position calibration spectra
 logical                         :: momentum_optimization                 ! if true, all optimizations will be done according to the initial momentum rather than chi2 deviations from a reference track        
 double precision                :: p_incoming(3)                         ! incoming momentum assuming scattering
 double precision                :: m_b , m_t                             ! mass of moving particle, mass of target particle
 double precision                :: chi_treshold                         ! chi2-cut off to be counted as good event
 double precision,allocatable    :: track_ref_det_frame(:,:,:)           ! track interactions for each sweep run (run,det_id,dimension)
 double precision, allocatable   :: derivative_variable_m(:,:),derivative_variable_p(:,:),derivative_variable_ref(:,:)                ! derivative x-position in detector-frame (detector_ID) when any variable is varied
 double precision, allocatable   :: detector_track_interactions_lab_frame_m(:,:)        ! intersection of track point with z=-derivative_step plane of detector in lab coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable   :: detector_track_interactions_lab_frame_p(:,:)        ! intersection of track point with z=+derivative_step plane of detector in lab coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable   :: detector_track_interactions_det_frame_m(:,:)        ! intersection of track point with z=-derivative_step plane of detector in det coordinates (detector_ID,3) last 3: x,y,z
 double precision, allocatable   :: detector_track_interactions_det_frame_p(:,:)        ! intersection of track point with z=+derivative_step plane of detector in det coordinates (detector_ID,3) last 3: x,y,z
 character (LEN=3),parameter, dimension(nbr_dimensions_optimize) :: ch_direction=(/'.X.','.Y.','.Z.'/) 
 double precision                :: derivative_step = 2.d-3
 double precision, allocatable   :: sum_xiyi_p(:), sum_xixi_p(:),sum_xiyi_m(:), sum_xixi_m(:)
 double precision, allocatable   :: detector_track_interactions_det_frame_ref_p(:,:)    ! intersection of track point with z=+derivative_step plane of detector in detector coordinates (detector_ID,3) last 3: x,y,z), reference during step-wise initialization 
 double precision, allocatable   :: detector_track_interactions_det_frame_ref_m(:,:)    ! intersection of track point with z=-derivative_step plane of detector in detector coordinates (detector_ID,3) last 3: x,y,z), reference during step-wise initialization 
 double precision, allocatable   :: coefficients_y_correction_p(:,:), coefficients_y_correction_m(:,:)               ! coefficients to correct y, if theta_xz is varied (nbr_detector,basic_functions_y_correction)
 double precision, allocatable   :: coefficients_y_correction_ref(:,:)               ! coefficients to correct y, if theta_xz is varied (nbr_detector,basic_functions_y_correction)
!
end module calib

subroutine init_from_cpp
 use vars
 implicit none
!
 call init
 call initialize_trigger_logic
 call b_field_init
 if (sweep_calibration) then
   call make_sweep_calibration
 else  
   call get_derivatives
   call initialize_y_corrections
   call initialize_matrizes
 end if  
end


subroutine multi_track_extended_output_from_cpp(array_size,n_points,det_coordinates,double_track, local_target_position, &
                                detector_id_in, charge_in, x_positions_in,y_positions_in,z_positions_in, &
                                 track_parameter_out, chi_parameter_out,track1_hit_pattern_out,track2_hit_pattern_out)
!
 use vars
 implicit none
 logical*1 track1_hit_pattern_out(2*nbr_detectors),track2_hit_pattern_out(2*nbr_detectors)                            ! true, if detector was used for track (detector_ID) 
 integer n_points, array_size
 logical*1 det_coordinates, double_track
 integer detector_id_in(array_size), charge_in(array_size)
 double precision x_positions_in(array_size)
 double precision y_positions_in(array_size)
 double precision z_positions_in(array_size)
 double precision track_parameter_out(12), chi_parameter_out(6)
 double precision local_target_position(3)
 integer i
 character*1 c1
!
 
 call multi_track_from_cpp(array_size,n_points,det_coordinates,double_track, local_target_position, &
                                detector_id_in, charge_in, x_positions_in,y_positions_in,z_positions_in, &
                                 track_parameter_out, chi_parameter_out)
!
 do i=1,2*nbr_detectors
   track1_hit_pattern_out(i) = track1_hit_pattern_used(i)  
   track2_hit_pattern_out(i) = track2_hit_pattern_used(i)  
 end do
! 
 if (debug_track) then
   write(*,*) 'Acknowledge to start next event'
   read(*,*) c1
 end if
end

subroutine multi_track_from_cpp(array_size,n_points,det_coordinates,double_track, local_target_position, &
                                detector_id_in, charge_in, x_positions_in,y_positions_in,z_positions_in, &
                                 track_parameter_out, chi_parameter_out)
 use vars
 implicit none
 integer i,j, d, new_hit, det_pattern_id, pattern_id, d1, d2
 integer n_det_track_old
 integer n_det_track, n_det_track1, n_det_track2
 integer n_points, array_size
 logical*1 det_coordinates, double_track,  left, right, new_combination
 logical all_done, all_done1, all_done2, skip_combination, skip
 logical track_ok,track1_ok,track2_ok
 integer detector_id_in(array_size), charge_in(array_size)
 double precision x_positions_in(array_size)
 double precision y_positions_in(array_size)
 double precision z_positions_in(array_size)
 double precision track_parameter_out(12), chi_parameter_out(6)
 double precision x_local(3), local_target_position(3), x_local_det(3), x_local_lab(3), delta
 integer nbr_hits_per_detector(nbr_detectors)
 integer track_hit_numbers(nbr_detectors),track_hit_numbers1(nbr_detectors),track_hit_numbers2(nbr_detectors)
 integer track1_hit_numbers_return(nbr_detectors),track2_hit_numbers_return(nbr_detectors), track_hit_numbers_return(nbr_detectors)
 double precision track_points_det_frame(nbr_detectors,n_points,3)
 double precision track_points_lab_frame(nbr_detectors,n_points,3)
 double precision chi2_local,chi2_local_old
 integer track_points_charge(nbr_detectors,n_points), charge1, charge2, charge
!
 if (debug_track) then
   write(output_unit,*) 'multi: sorted events'
   write(output_unit,*) 'array_size',array_size
   write(output_unit,*) 'n_points',n_points
 end if
!
 if (constrain_target_x) then
   pos_target(1) = local_target_position(1)
 else
   pos_target(1) = pos_target_original(1)
 end if    
!
 if (constrain_target_y) then
   pos_target(2) = local_target_position(2)
 else
   pos_target(2) = pos_target_original(2)
 end if    
!
 nbr_hits_per_detector  = 0
 track_hit_numbers_return = 0
 track1_hit_numbers_return = 0
 track2_hit_numbers_return = 0
 n_det_track_old        = 0
 track_points_det_frame = 0.d0
 track_points_lab_frame = 0.d0
 track_points_charge    = 0
 chi_parameter_out      = 1.d35
 chi2_local_old         = 1.d35
 do i=1,n_points
   d                        = detector_id_in(i) + 1
   nbr_hits_per_detector(d) = nbr_hits_per_detector(d) + 1
   x_local(1)               = x_positions_in(i)
   x_local(2)               = y_positions_in(i)
   x_local(3)               = z_positions_in(i)
   if (det_coordinates) then
     x_local_det = x_local
     call get_lab_position(d,x_local,x_local_lab ) 
   else
     x_local_lab = x_local
     call get_det_position(d,x_local,x_local_det )  
   end if
   !
   ! do some sorting within a given detector: sort according to x in lab frame. x ascending!
   !
   new_hit = nbr_hits_per_detector(d)
   do j=nbr_hits_per_detector(d)-1,1,-1
     if (x_local_lab(1) < track_points_lab_frame(d,j,1) )  then       ! will flip entries
       track_points_det_frame(d,j+1,:) = track_points_det_frame(d,j,:)
       track_points_lab_frame(d,j+1,:) = track_points_lab_frame(d,j,:)
       track_points_charge(d,j+1)      = track_points_charge(d,j)
       new_hit = j
     else 
       exit  
     end if
   end do   
   track_points_det_frame(d,new_hit,:) = x_local_det
   track_points_lab_frame(d,new_hit,:) = x_local_lab
   track_points_charge(d,new_hit)      = charge_in(i)
 end do  
 if (debug_track) then
   write(output_unit,*) 'multi: sorted events'
   do d=1,nbr_detectors 
     print*
     write(output_unit,*) detector_name(d)
     do i=1,nbr_hits_per_detector(d)
       write(output_unit,*) track_points_charge(d,i), track_points_det_frame(d,i,:)
     end do
   end do
 end if
!
 counter            = 0
! 
! start looping & tracking
!
 if (double_track) then
   track_hit_numbers1 = 0
   all_done1          = .false.
   do
!
!  get hit-pattern of current track1
!
     do d=1,nbr_detectors
       track_hit_pattern1(d) = (track_hit_numbers1(d) > 0)        
     end do
!
!  do some checks on the track1
!   
!  request certain detector combinations
!
     pattern_id = det_pattern_id(nbr_detectors,track_hit_pattern1)
     track1_ok  = trigger_matrix(pattern_id) 
!
!  check, if track CAN make sense (no crossings...)
!
     n_det_track1 = 0
     charge1 = 0
     if (track1_ok) then
       left  = .true.
       right = .true.
       do d=1,nbr_detectors
         if (track_hit_pattern1(d)) then
           if (det_consider(d,1)) n_det_track1 = n_det_track1 + 1
           delta = track_points_lab_frame(d,track_hit_numbers1(d),1) - detector_track_interactions_lab_frame_ref(d,1)
           left  = left  .and. (delta > -crossing_resolution)                                                 ! interception point is left of reference track
           right = right .and. (delta <  crossing_resolution)                                                                            ! interception point is right of reference track
         end if
         track1_ok  = track1_ok .and. (left .or. right)                                                                ! all interception points are either left or right
         if (track_hit_pattern1(d) .and. track1_ok) then
           track_det_frame1(d,:) = track_points_det_frame(d,track_hit_numbers1(d),:)            ! store interception points for later tracking
           if (track_points_charge(d,track_hit_numbers1(d)) /= 0) then
             if (charge1>0 .and. charge1 /=track_points_charge(d,track_hit_numbers1(d))) then
               track1_ok = .false.
               write(output_unit,*) 'Multi hit routine, doube track 1, charge missmatch!',&
               charge1,track_points_charge(d,track_hit_numbers1(d))
             end if
             charge1=track_points_charge(d,track_hit_numbers1(d))
           end if  
         end if  
         if (.not.track1_ok) exit
       end do
     end if  
     track_hit_numbers2 = 0
      
     if (track1_ok) then
       counter(1) = counter(1) + 1                                                                ! counter for all double track1 candidates
       all_done2 = .false.
!
       do
!
!  get hit-pattern of current track2
!
         do d=1,nbr_detectors
           track_hit_pattern2(d) = (track_hit_numbers2(d) > 0)        
         end do
!
!  do some checks on the track2
!   
!  request certain detector combinations
!
         pattern_id = det_pattern_id(nbr_detectors,track_hit_pattern2)
         track2_ok  = trigger_matrix(pattern_id) 
!
!  check, if track CAN make sense (no crossings...)
!
         n_det_track2 = 0
         charge2 = 0
         if (track2_ok) then
           counter(3) = counter(3) + 1                                                            
           left  = .true.
           right = .true.
           do d=1,nbr_detectors
             if (track_hit_pattern2(d)) then
               if (det_consider(d,1)) n_det_track2 = n_det_track2 + 1
               delta = track_points_lab_frame(d,track_hit_numbers2(d),1) - detector_track_interactions_lab_frame_ref(d,1)
               left  = left  .and. (delta > -crossing_resolution)                                                 ! interception point is left of reference track
               right = right .and. (delta <  crossing_resolution)                                                                            ! interception point is right of reference track
             end if
             track2_ok  = track2_ok .and. (left .or. right)                                                                ! all interception points are either left or right
             if (track_hit_pattern2(d) .and. track2_ok) then
               track_det_frame2(d,:) = track_points_det_frame(d,track_hit_numbers2(d),:)            ! store interception points for later tracking
               if (track_points_charge(d,track_hit_numbers2(d)) /= 0) then
                 if (charge2>0 .and. charge2 /=track_points_charge(d,track_hit_numbers2(d))) then
                   track2_ok = .false.
         !          write(output_unit,*) 'Multi hit routine, doube track 2, charge missmatch!',&
         !          charge2,track_points_charge(d,track_hit_numbers2(d))
                 end if
                 charge2=track_points_charge(d,track_hit_numbers2(d))
               end if  
             end if  
             if (.not.track2_ok) exit
           end do
         end if  
         if (track2_ok) then
           counter(4) = counter(4) + 1                                                            
           right = .true.
           do d=1,nbr_detectors
             if (track_hit_pattern2(d) .and.track_hit_pattern1(d) ) then
               right = right .and. (track_hit_numbers2(d) > track_hit_numbers1(d))                        ! interception point is right of track1
             end if
             track2_ok  = track2_ok .and. right                                                                            ! all interception points are right
             if (.not. track2_ok) then
               exit
             end if  
           end do
         end if  
!         
         if (track2_ok) then    ! let's track...
           counter(2) = counter(2)+1
!
           call get_double_track_parameter_var8
!
           n_det_track = n_det_track1 + n_det_track2
           chi2_local  = chi2_double(5) + chi2_double(6)
           new_combination = (n_det_track > n_det_track_old)
           new_combination = new_combination .or. ((n_det_track == n_det_track_old) .and. chi2_local < chi2_local_old)

           if (any(.not.paddle_hit_1) .or. any(.not.paddle_hit_2) ) new_combination = .false.
           do i=1,3
             if (max_momentum_deviation(i) > 0.d0) then
               if (dabs(x_track1(i+3) - x_reference(i+3)) > max_momentum_deviation(i) ) new_combination = .false.
               if (dabs(x_track2(i+3) - x_reference(i+3)) > max_momentum_deviation(i) ) new_combination = .false.
             end if
           end do


           if (new_combination) then
             if (charge1 < charge2) then
               track_parameter_out(1:3)   = x_track1(1:3)                    ! starting position in lab frame (m)
               track_parameter_out(4:6)   = x_track1(4:6)/e*c/1.d6*dble(charge1)        ! momentum (MeV/c)
               track_parameter_out(7:9)   = x_track2(1:3)                    ! starting position in lab frame (m)
               track_parameter_out(10:12) = x_track2(4:6)/e*c/1.d6*dble(charge2)        ! momentum (MeV/c)
               chi_parameter_out(1:4)     = chi2_double(1:4)  
               track1_hit_pattern_used(1:nbr_detectors) = track_hit_pattern1
               track2_hit_pattern_used(1:nbr_detectors) = track_hit_pattern2
               track1_hit_pattern_used(nbr_detectors+1:2*nbr_detectors) = track1_hit_pattern_from_chi2
               track2_hit_pattern_used(nbr_detectors+1:2*nbr_detectors) = track2_hit_pattern_from_chi2
               track1_hit_numbers_return                = track_hit_numbers1
               track2_hit_numbers_return                = track_hit_numbers2
             else
               track_parameter_out(1:3)   = x_track2(1:3)                    ! starting position in lab frame (m)
               track_parameter_out(4:6)   = x_track2(4:6)/e*c/1.d6*dble(charge2)        ! momentum (MeV/c)
               track_parameter_out(7:9)   = x_track1(1:3)                    ! starting position in lab frame (m)
               track_parameter_out(10:12) = x_track1(4:6)/e*c/1.d6*dble(charge1)        ! momentum (MeV/c)
               chi_parameter_out(1:2)     = chi2_double(3:4)  
               chi_parameter_out(3:4)     = chi2_double(1:2)  
               track1_hit_pattern_used(1:nbr_detectors) = track_hit_pattern2
               track2_hit_pattern_used(1:nbr_detectors) = track_hit_pattern1
               track1_hit_pattern_used(nbr_detectors+1:2*nbr_detectors) = track2_hit_pattern_from_chi2
               track2_hit_pattern_used(nbr_detectors+1:2*nbr_detectors) = track1_hit_pattern_from_chi2
               track1_hit_numbers_return                = track_hit_numbers2
               track2_hit_numbers_return                = track_hit_numbers1
             end if           
!
             chi_parameter_out(5:6)     = chi2_double(5:6)  
             n_det_track_old            = n_det_track
             chi2_local_old             = chi2_local
             if (debug_track) then
               write(output_unit,*) '  n_det_track_old, chi2        ',n_det_track_old, chi2_double
               write(output_unit,*) '  track_parameter_out_double 1  ',track_parameter_out(1:6)
               write(output_unit,*) '  track_parameter_out_double 2  ',track_parameter_out(7:12)
               write(output_unit,*) 'x_track1',x_track1
               write(output_unit,*) 'x_track2',x_track2
             end if  
           end if  

         end if  
!
!  get next track2 candidate
!
         do d1=1,nbr_detectors
           skip_combination = .false.
           if (track_hit_numbers2(d1) == 0) then
             do d2=d1+1,nbr_detectors
               skip = (trigger_always_together(d1,d2).and.track_hit_numbers2(d2)==0)
               skip = (skip.or.(trigger_never_together(d1,d2).and.track_hit_numbers2(d2)>0))
               skip_combination = skip_combination .or. skip
               if (skip_combination) exit
             end do 
           end if
           if (skip_combination) then 
             all_done2 = (d1 == nbr_detectors)
           else  
             track_hit_numbers2(d1) = track_hit_numbers2(d1) + 1
             if (track_hit_numbers2(d1) == track_hit_numbers1(d1) ) then
               track_hit_numbers2(d1) = track_hit_numbers2(d1) + 1
             end if  
             if (track_hit_numbers2(d1) > nbr_hits_per_detector(d1) ) then
               track_hit_numbers2(d1) = 0
               all_done2 = (d1 == nbr_detectors)
             else
               exit
             end if  
           end if
         end do
         counter(6) = counter(6)+1
         if (all_done2) exit
       end do  
     end if  

!
!  get next track1 candidate
!
     do d1=1,nbr_detectors
       skip_combination = .false.
       if (track_hit_numbers1(d1) < nbr_hits_per_detector(d1)) then
         if (track_hit_numbers1(d1) == 0) then
           do d2=d1+1,nbr_detectors
             skip = (trigger_always_together(d1,d2).and.track_hit_numbers1(d2)==0)
             skip = (skip.or.(trigger_never_together(d1,d2).and.track_hit_numbers1(d2)>0))
             skip_combination = skip_combination .or. skip
             if (skip_combination) exit
           end do 
         end if
         if (skip_combination) then 
           all_done1 = (d1 == nbr_detectors)
         else  
           track_hit_numbers1(d1) = track_hit_numbers1(d1) + 1  
           exit
         end if  
       else
         track_hit_numbers1(d1) = 0
         all_done1 = (d1 == nbr_detectors)
       end if
     end do
     counter(5) = counter(5)+1
     if (all_done1) exit
!       write(output_unit,*) 'multi: track_hit_numbers1', track_hit_numbers1,' *** ',nbr_hits_per_detector
!       write(output_unit,*) 'multi-counter', counter
   end do
!
!  single-track
!   
 else
  track_hit_numbers  = 0
  all_done           = .false.
  do
   counter(1) = counter(1) + 1                                                                ! counter for all track candidates
!
!  get hit-pattern of current track
!
   do d=1,nbr_detectors
     track_hit_pattern(d) = (track_hit_numbers(d) > 0)        
   end do
!
!  do some checks on the track
!   
!  request certain detector combinations
!
   pattern_id = det_pattern_id(nbr_detectors,track_hit_pattern)
   track_ok   = trigger_matrix(pattern_id) 
   if (debug_track) then
      do d=1,nbr_detectors
        write(output_unit,*) ' track_hit_pattern   ',detector_name(d), track_hit_pattern(d)
      end do  
      write(output_unit,*) ' checked tigger logik will go on with tracking T/F   ',track_ok
   end if  

!
!  check, if track CAN make sense (no crossings...)
!
   n_det_track = 0
   charge = 0
   if (track_ok) then
     left  = .true.
     right = .true.
     do d=1,nbr_detectors
       if (track_hit_pattern(d)) then
         if (det_consider(d,1)) then
           n_det_track = n_det_track + 1
           delta = track_points_lab_frame(d,track_hit_numbers(d),1) - detector_track_interactions_lab_frame_ref(d,1)
           left  = left  .and. (delta > -crossing_resolution)                                 ! interception point is left of reference track
           right = right .and. (delta <  crossing_resolution)                                  ! interception point is right of reference track
         end if  
         if (det_step_function(d,1)) then
!           n_det_track = n_det_track + 1
           delta = track_points_lab_frame(d,track_hit_numbers(d),1) - detector_track_interactions_lab_frame_ref(d,1)
           left  = left  .and. (delta > -sigma_track(d,i)-crossing_resolution)                                 ! interception point is left of reference track
           right = right .and. (delta <  sigma_track(d,i)+crossing_resolution)                                  ! interception point is right of reference track
         end if  
       end if
  !     track_ok  = track_ok .and. (left .or. right)                                                                ! all interception points are either left or right
       if (.not. track_ok) then
         if (debug_track) then
           write(output_unit,*) ' left/right check failed   ',track_ok
         end if  
         exit
       else
         track_det_frame(d,:) = track_points_det_frame(d,track_hit_numbers(d),:)            ! store interception points for later tracking
         if (track_hit_pattern(d) .and. track_points_charge(d,track_hit_numbers(d)) /= 0) then
           if (charge>0 .and. charge /=track_points_charge(d,track_hit_numbers(d))) then
             track_ok = .false.
             if (debug_track) then
               write(output_unit,*) ' charge check failed   ',track_ok,&
                 charge,track_points_charge(d,track_hit_numbers(d))
             end if  
           end if
           charge=track_points_charge(d,track_hit_numbers(d))
         end if  
       end if  
     end do
   end if  
   if (debug_track) then
      write(output_unit,*) ' further checks of track, will go on with tracking T/F   ',track_ok
   end if  
!
!  do something with the track (e.g. call tracker)
!   
   if (track_ok) then
     counter(2) = counter(2) + 1                                                            ! counter for reasonable track candidates
!
     call get_single_track_parameter_var5
!
     new_combination = (n_det_track > n_det_track_old)
     new_combination = new_combination .or. ((n_det_track == n_det_track_old) .and. chi2_single(1) < chi_parameter_out(1))
     
     if (any(.not.paddle_hit)) then
   !    new_combination = .false.
       if (debug_track) then
         write(output_unit,*) 'some detectors are not hit, but required to be hit'
         do d = 1,nbr_detectors
           do i=1,3
             write(output_unit,*) detector_name(d), 'dimension',i, paddle_hit(d,i)
           end do  
         end do
       end if
     end if  
     do i=1,3
       if (max_momentum_deviation(i) > 0.d0) then
         if (dabs(x_track1(i+3) - x_reference(i+3)) > max_momentum_deviation(i) ) new_combination = .false.
         if (debug_track) then
           write(output_unit,*) 'tracked momentum outside range', i, x_track1(i+3),max_momentum_deviation(i)
         end if
       end if
     end do

     if (new_combination) then
       track_parameter_out(1:3) = x_track1(1:3)                              ! starting position in lab frame (m)
       track_parameter_out(4:6) = x_track1(4:6)/e*c/1.d6*dble(charge)        ! momentum (MeV/c)
!
       chi_parameter_out(1:2)   = chi2_single   
       n_det_track_old          = n_det_track
       track_hit_pattern_used(1:nbr_detectors) = track_hit_pattern
       track_hit_pattern_used(nbr_detectors+1:2*nbr_detectors) = track_hit_pattern_from_chi2
       track_hit_numbers_return                = track_hit_numbers
       if (debug_track) then
         write(output_unit,*) '  n_det_track_old, chi2, track_parameter_out_single   ',&
           n_det_track_old, chi2_single,track_parameter_out(1:6)
         write(output_unit,*) 'x_track1',x_track1
       end if  
     end if  
!
   end if  
!
!  get next track candidate
!
   do d1=1,nbr_detectors
     skip_combination = .false.
     if (track_hit_numbers(d1) < nbr_hits_per_detector(d1)) then
       if (track_hit_numbers(d1) == 0) then
         do d2=d1+1,nbr_detectors
           skip = (trigger_always_together(d1,d2).and.track_hit_numbers(d2)==0)
           skip = (skip.or.(trigger_never_together(d1,d2).and.track_hit_numbers(d2)>0))
           skip_combination = skip_combination .or. skip
           if (skip_combination) exit
         end do 
       end if
       if (skip_combination) then 
         all_done = (d1 == nbr_detectors)
       else  
         if (debug_track) then
           write(output_unit,*) '  track_hit_numbers(d1), nbr_hits_per_detector(d1), d1   ',&
             track_hit_numbers(d1), nbr_hits_per_detector(d1), d1
           write(output_unit,*) 'x_track1',x_track1
         end if  
         track_hit_numbers(d1) = track_hit_numbers(d1) + 1  
         exit
       end if  
     else
       track_hit_numbers(d1) = 0
       all_done = (d1 == nbr_detectors)
     end if
   end do
   counter(6) = counter(6)+1
   if (all_done) exit
  end do
 end if

 if (debug_track) then
   write(output_unit,*)  'Multi: all combinations done'
 end if  

!
 n_points     = 0
 n_det_track1 = 0 
 n_det_track2 = 0
! 
 if (double_track) then
   do d = 1, nbr_detectors
     if (track1_hit_numbers_return(d) > 0) then
       n_points                 = n_points + 1
       n_det_track1 = n_det_track1 + 1
       detector_id_in(n_points) = d - 1 ! fortran -> cpp
       charge_in(n_points)      = track_points_charge(d,track1_hit_numbers_return(d))
       x_positions_in(n_points) = track_points_det_frame(d,track1_hit_numbers_return(d),1)
       y_positions_in(n_points) = track_points_det_frame(d,track1_hit_numbers_return(d),2)
       z_positions_in(n_points) = track_points_det_frame(d,track1_hit_numbers_return(d),3)
     end if
   end do
   charge                       = maxval(charge_in(1:n_det_track1))
   charge_in(1:n_det_track1)    = charge
   do d = 1, nbr_detectors
     if (track2_hit_numbers_return(d) > 0) then
       n_points                 = n_points + 1
       n_det_track2 = n_det_track2 + 1
       detector_id_in(n_points) = d - 1 ! fortran -> cpp
       charge_in(n_points)      = track_points_charge(d,track2_hit_numbers_return(d))
       x_positions_in(n_points) = track_points_det_frame(d,track2_hit_numbers_return(d),1)
       y_positions_in(n_points) = track_points_det_frame(d,track2_hit_numbers_return(d),2)
       z_positions_in(n_points) = track_points_det_frame(d,track2_hit_numbers_return(d),3)
     end if
   end do
   charge                                              = maxval(charge_in(1+n_det_track1:n_det_track1+n_det_track2))
   charge_in(1+n_det_track1:n_det_track1+n_det_track2) = charge
 else
   do d = 1, nbr_detectors
     if (track_hit_numbers_return(d) > 0) then
       n_points                 = n_points + 1
       if (debug_track) then
         write(output_unit,*)  'Multi: d',d, detector_name(d)
         write(output_unit,*)  'Multi: n_points,track_hit_numbers_return(d)',n_points,track_hit_numbers_return(d)
         write(output_unit,*)  'Multi: track_points_charge(d,track_hit_numbers_return(d))',&
         track_points_charge(d,track_hit_numbers_return(d))
         write(output_unit,*)  'Multi: track_points_det_frame(d,track_hit_numbers_return(d),:)',&
         track_points_det_frame(d,track_hit_numbers_return(d),:)
         
       end if  
       detector_id_in(n_points) = d - 1 ! fortran -> cpp
       charge_in(n_points)      = track_points_charge(d,track_hit_numbers_return(d))
       x_positions_in(n_points) = track_points_det_frame(d,track_hit_numbers_return(d),1)
       y_positions_in(n_points) = track_points_det_frame(d,track_hit_numbers_return(d),2)
       z_positions_in(n_points) = track_points_det_frame(d,track_hit_numbers_return(d),3)
     end if
   end do
   charge                = maxval(charge_in(1:n_points))
   charge_in(1:n_points) = charge
 end if
!
 if (debug_track) then
   write(output_unit,*)  'Multi: number of data points:',n_points
   write(output_unit,*)  'Multi: some counting of events:',counter
 end if  





! write(output_unit,*)  'Multi: some counting of events:',counter
end

subroutine single_track_from_cpp(n_det,det_coordinates,local_target_position, &
                                x_positions_in,y_positions_in,z_positions_in, hit_pattern_in, &
                                 track_parameter_out, chi_parameter_out)
 use vars
 implicit none
 integer n_det, i
 logical*1 hit_pattern_in(n_det), det_coordinates
 double precision x_positions_in(n_det),track_parameter_out(6), chi_parameter_out(2)
 double precision y_positions_in(n_det)
 double precision z_positions_in(n_det)
 double precision x_local_lab(3), x_local_det(3), local_target_position(3)
 character*1 response
!
 if (constrain_target_x) then
   pos_target(1) = local_target_position(1)
 else
   pos_target(1) = pos_target_original(1)
 end if    
!
 if (constrain_target_y) then
   pos_target(2) = local_target_position(2)
 else
   pos_target(2) = pos_target_original(2)
 end if    
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
   if (debug_track) then
     write(output_unit,*) 
     write(output_unit,*) 'Values from R3B-root'
     write(output_unit,*) 
     write(output_unit,*) ' Nbr-detectors:', n_det
     write(output_unit,*) 
     write(output_unit,*) ' det_coordinates:', det_coordinates
     write(output_unit,*) 
     write(output_unit,*) ' x-positions (m):', x_positions_in
     write(output_unit,*) 
     write(output_unit,*) ' y-positions (m):', y_positions_in
     write(output_unit,*) 
     write(output_unit,*) ' z-positions (m):', z_positions_in
     write(output_unit,*) 
     write(output_unit,*) ' hit-pattern:', hit_pattern_in
   end if
   track_hit_pattern        = hit_pattern_in
!
   call get_single_track_parameter_var5
!
   track_parameter_out(1:3) = x_track1(1:3)                    ! starting position in lab frame (m)
   track_parameter_out(4:6) = x_track1(4:6)/e*c/1.d6        ! momentum (MeV/c)
!
   chi_parameter_out        = chi2_single   
!
   if (debug_track) then
     write(output_unit,*) 
     write(output_unit,*) 'Return values to r3broot'
     write(output_unit,*) 
     write(output_unit,*) ' Position at Target       (m):', track_parameter_out(1:3)
     write(output_unit,*) 
     write(output_unit,*) ' Momentum at Target (MeV/c/q):', track_parameter_out(4:6)
     write(output_unit,*) 
     write(output_unit,*) ' Momentum at Target (MeV/c/q):', chi_parameter_out
   end if
 else
   print*,'Missmatch between number of detectors called from cpp and expected inside fortran', n_det, nbr_detectors
   write(output_unit,*) 'Missmatch between number of detectors called from cpp and expected inside fortran',&
     n_det, nbr_detectors
   stop
 end if   
 if (debug_track .and. grafical_output) then
     print*
     write(*,'(A24)', advance='no') ', continue (q = quit) : '
     read(*,*) response
     if ( (response == 'q') ) stop
 end if  

end

subroutine double_track_from_cpp(n_det, det_coordinates, local_target_position,x1_in,y1_in,z1_in, hit1_in,&
                                 x2_in,y2_in,z2_in, hit2_in, &
                                 track1_out,track2_out, chi_parameter_out)
 use vars
 implicit none
 integer n_det, i
 logical*1 hit1_in(n_det),hit2_in(n_det), det_coordinates
 double precision x1_in(n_det),y1_in(n_det),z1_in(n_det)
 double precision x2_in(n_det),y2_in(n_det),z2_in(n_det)
 double precision track1_out(6),track2_out(6),chi_parameter_out(6)
 double precision x_local_lab(3), x_local_det(3) , local_target_position(3)
!
 if (constrain_target_x) then
   pos_target(1) = local_target_position(1)
 else
   pos_target(1) = pos_target_original(1)
 end if    
!
 if (constrain_target_y) then
   pos_target(2) = local_target_position(2)
 else
   pos_target(2) = pos_target_original(2)
 end if    
!
 if (n_det == nbr_detectors) then
  if (det_coordinates) then
   track_det_frame1(:,1) = x1_in
   track_det_frame1(:,2) = y1_in
   track_det_frame1(:,3) = z1_in
   track_det_frame2(:,1) = x2_in
   track_det_frame2(:,2) = y2_in
   track_det_frame2(:,3) = z2_in
  else
   do i=1,nbr_detectors
     x_local_lab(1) = x1_in(i)
     x_local_lab(2) = y1_in(i)
     x_local_lab(3) = z1_in(i)
     call get_det_position(i,x_local_lab,x_local_det )
     track_det_frame1(i,:) = x_local_det
     x_local_lab(1) = x2_in(i)
     x_local_lab(2) = y2_in(i)
     x_local_lab(3) = z2_in(i)
     call get_det_position(i,x_local_lab,x_local_det )
     track_det_frame2(i,:) = x_local_det
   end do
  end if  
    
   track_hit_pattern1    = hit1_in
   track_hit_pattern2    = hit2_in
!
   call get_double_track_parameter_var8
!
   track1_out(1:3) = x_track1(1:3)                    ! starting position in lab frame (m)
   track1_out(4:6) = x_track1(4:6)/e*c/1.d6            ! momentum (MeV/c)
   track2_out(1:3) = x_track2(1:3)                    ! starting position in lab frame (m)
   track2_out(4:6) = x_track2(4:6)/e*c/1.d6            ! momentum (MeV/c)
!
   chi_parameter_out = chi2_double
!
   if (debug_track) then
     write(output_unit,*) 'FORTRAN, track1:',real(track1_out)   
     write(output_unit,*) 'FORTRAN, track2:',real(track2_out)   
     write(output_unit,*) 'FORTRAN, chi2  :',real(chi_parameter_out)
   end if     
!   
 else
   print*,'Missmatch between number of detectors called from cpp and expected inside fortran', n_det,    nbr_detectors
   write(output_unit,*) 'Missmatch between number of detectors called from cpp and expected inside fortran',&
    n_det,    nbr_detectors
   stop
 end if   
end


subroutine initialize_matrizes
 use vars
 implicit none
 if (debug_track) write(output_unit,*) 'start y_a2 matrix initialization'
 call initialize_y_a_t_2
! if (debug_track) write(output_unit,*) 'start y_a3 matrix initialization'
! call initialize_y_a_t_3
 if (debug_track) write(output_unit,*) 'start a3 matrix initialization'
 call initialize_a_t_3
! if (debug_track) write(output_unit,*) 'start a5 matrix initialization'
! call initialize_a_t_5
 if (debug_track) write(output_unit,*) 'finished matrix initialization'
end

subroutine special_event
 use vars
 implicit none
 integer eastat, d,ql,n_points
 double precision xl, yl, zl
 integer, parameter :: array_size=100
 integer detector_id_in(array_size), charge_in(array_size)
 double precision x_positions_in(array_size),y_positions_in(array_size),z_positions_in(array_size)
 double precision track_parameter_out(12), chi_parameter_out(6)
 logical*1 det_coordinates,double_track, ghost_event
 logical*1 hit1(2*nbr_detectors),hit2(2*nbr_detectors)
!
 zl = 0.d0
 n_points = 0
 ghost_event = .false.
 open(unit=76,file='special_event2.txt')
   read(76,*,iostat=eastat) d, xl, yl, ql
   do
     if (eastat /= 0) exit
!     print*,d, xl, yl, ql
     n_points = n_points + 1
     if (trim(geometry_file) .eq. 'geometry_9d.ini')  then
       detector_id_in(n_points) = d+1
     else  
       detector_id_in(n_points) = d
     end if  
     charge_in(n_points)      = ql
     x_positions_in(n_points) = xl
     y_positions_in(n_points) = yl
     z_positions_in(n_points) = zl
     read(76,*,iostat=eastat) d, xl, yl, ql
   end do
 close(76)
 if (trim(geometry_file) .eq. 'geometry_9d.ini' .and. ghost_event)  then
   n_points                 = n_points + 1
   detector_id_in(n_points) = 0
   charge_in(n_points)      = 0
   x_positions_in(n_points) = 0.
   y_positions_in(n_points) = 0.d0
   z_positions_in(n_points) = 0.d0
 end if   

 if (trim(geometry_file) .eq. 'geometry_9d_micha.ini' .and. ghost_event)  then
   n_points                 = n_points + 1
   detector_id_in(n_points) = nbr_detectors-1
   charge_in(n_points)      = 0
   x_positions_in(n_points) = 0.
   y_positions_in(n_points) = 0.d0
   z_positions_in(n_points) = 0.d0
 end if   

 det_coordinates = .true.
 double_track    = .true.
! call multi_track_from_cpp(array_size,n_points,det_coordinates,double_track, pos_target_original, &
!                                detector_id_in, charge_in, x_positions_in,y_positions_in,z_positions_in, &
!                                 track_parameter_out, chi_parameter_out)  
 call multi_track_extended_output_from_cpp(array_size,n_points,det_coordinates,double_track, pos_target_original, &
                                detector_id_in, charge_in, x_positions_in,y_positions_in,z_positions_in, &
                                 track_parameter_out, chi_parameter_out, hit1,hit2)  
 print*,'success'
 print*,'n_points:',n_points
! print*,'charge_in:',charge_in(1:n_points)
 print*,'Counter from multi-call:',counter
 print*, track_parameter_out(1:3)                           
 print*, track_parameter_out(4:6)*e/c*1.d6                            
 print*, track_parameter_out (7:9)                          
 print*, track_parameter_out (10:12)*e/c*1.d6 
 print*, chi_parameter_out                           
 print*, hit1(1:nbr_detectors)                           
 print*, hit2(1:nbr_detectors)                           
 
end

subroutine calc
 use vars
 implicit none
 integer tr, d, local_nbr_tracks, chx, chy,i
 integer ch_dpx, ch_dpy, ch_thetax, ch_thetay
 character (LEN=1) :: response
 double precision x1_local(6), x2_local(6)
 integer, parameter :: array_size=100
 integer n_points
 logical*1 det_coordinates,double_track
 double precision x_positions_in(array_size),y_positions_in(array_size),z_positions_in(array_size)
 double precision track_parameter_out(12), chi_parameter_out(6)
 integer detector_id_in(array_size), charge_in(array_size)
!
 local_nbr_tracks     = nbr_tracks*100000
 local_nbr_tracks     = nbr_tracks*10
 local_nbr_tracks     = 10
 local_nbr_tracks     = 100
 spectrum_dthetay_dpx = 0
 spectrum_dthetay_dpy = 0
 spectrum_dthetax_dpx = 0
 spectrum_dthetax_dpy = 0
 spectrum_dpy_dpx = 0
!
 track_hit_pattern    = .true.
!
! call special_event
! stop 
 
!
 do tr = 1,local_nbr_tracks
!   if (debug_track) then
     write(*,'(A6,I8,A24)', advance='no') 'Track ',tr,', continue (q = quit) : '
     read(*,*) response
     if ( (response == 'q') ) exit
!   else
!     if (1000000*int(tr/1000000) == tr) then
     if (100*int(tr/100) == tr) then
       print*,tr , ' of ', local_nbr_tracks, ' tracks'
       if (grafical_output) call make_gle_dthetay_dpx
     end if  
!   end if  
!
   print*,'.........................................................................',tr
   call get_single_track_data
   track_hit_pattern = det_hit
   track_det_frame1   = track_det_frame
   track_hit_pattern1 = track_hit_pattern
   x1_local = x_reference_step
   call get_single_track_parameter_var5
   print*,'Simulation    1 : ', real(x1_local)
   print*,'Linearisation 1 : ', real(x_track1)
   print*,'chi2-single     : ', real(chi2_single)
   print*
!  
   call get_single_track_data
   track_hit_pattern = det_hit  
   track_det_frame2   = track_det_frame
   track_hit_pattern2 = track_hit_pattern
   x2_local = x_reference_step
   call get_single_track_parameter_var5
   print*,'Simulation    2 : ', real(x2_local)
   print*,'Linearisation 2 : ', real(x_track1)
   print*,'chi2-single     : ', real(chi2_single)
!   
   print*
   call get_double_track_parameter_var8
   print*,'Double-Fit:'
   print*,'Simulation    1 : ', real(x1_local)
   print*,'Double-Track  1 : ', real(x_track1)
   print*,'Simulation    2 : ', real(x2_local)
   print*,'Double-Track  2 : ', real(x_track2)
   print*,'chi2-double     : ', real(chi2_double),real(chi2_double(5)+chi2_double(6))
   print*
   
!
!   keep_track = track_det_frame
!   do d=1, nbr_detectors
!     dxpos(d)  = GAUSSVER(sigma_track(d,1))    !  vary x-positions of detector track in each detector
!     dypos(d)  = GAUSSVER(sigma_track(d,2))    !  vary x-positions of detector track in each detector
!   end do  
 !  track_det_frame(:,1) = dxpos(:) + keep_track(:,1)
 !  track_det_frame(:,2) = dypos(:) + keep_track(:,2)
   
   det_coordinates = .true.
   double_track    = .true.
   n_points        = 0
!
   do d = 1, nbr_detectors
     if (track_hit_pattern1(d) ) then
       n_points = n_points + 1
       detector_id_in(n_points) = d-1
       charge_in(n_points)      = 1
       x_positions_in(n_points) = track_det_frame1(d,1)
       y_positions_in(n_points) = track_det_frame1(d,2)
       z_positions_in(n_points) = track_det_frame1(d,3)
     end if
   end do
   do d = 1, nbr_detectors
     if (track_hit_pattern2(d) ) then
       n_points = n_points + 1
       detector_id_in(n_points) = d-1
       charge_in(n_points)      = 1
       x_positions_in(n_points) = track_det_frame2(d,1)
       y_positions_in(n_points) = track_det_frame2(d,2)
       z_positions_in(n_points) = track_det_frame2(d,3)
     end if
   end do
   if (trim(geometry_file) .eq. 'geometry_9_micha.ini') then
     n_points = n_points + 1
     detector_id_in(n_points) = nbr_detectors-1
     charge_in(n_points)      = 1
     x_positions_in(n_points) = 0.d0
     y_positions_in(n_points) = 0.d0
     z_positions_in(n_points) = 0.d0
   end if  

   do i=1,1
!   do i=1,10000
     call multi_track_from_cpp(array_size,n_points,det_coordinates,double_track, pos_target_original, &
                                detector_id_in, charge_in, x_positions_in,y_positions_in,z_positions_in, &
                                 track_parameter_out, chi_parameter_out)   
     print*
   
     print*,'Multi         1 : ',real(track_parameter_out(1:6) *e/c*1.d6)
     print*,'Multi         2 : ',real(track_parameter_out(7:12) *e/c*1.d6 )
     print*,'Chi2            : ',real(chi_parameter_out ),real(chi_parameter_out(5)+chi_parameter_out(6) )
   
     print*
   end do                              
!
   ch_dpx    = int(1.d3*(x_track1(4)-x_reference_step(4))/x_reference_step(4)  )+ nbr_channels_2d_spectra/2  ! dpx/px, 0.1% / channel, 0 at center
   ch_dpy    = int(1.d3*(x_track1(5)-x_reference_step(5))/x_reference_step(5)  )+ nbr_channels_2d_spectra/2  ! dpy/py, 0.1% / channel, 0 at center
   ch_thetax = int(1.d3*datan(x_reference_step(5)/x_reference_step(6)))+nbr_channels_2d_spectra/2            ! theta_yz, 1mrad/channel, 0 at center 
   ch_thetay = int(1.d3*datan(x_reference_step(4)/x_reference_step(6)))+nbr_channels_2d_spectra/2            ! theta_xz, 1mrad/channel, 0 at center
!   
   chy = ch_thetay
   chx = ch_dpx
!
   if (chx >= 0 .and. chx <= nbr_channels_2d_spectra .and. chy >= 0 .and. chy <= nbr_channels_2d_spectra) then
     spectrum_dthetay_dpx(chx,chy) = spectrum_dthetay_dpx(chx,chy) + 1
   end if  
!   
   chx = ch_dpx
   chy = ch_dpy
!
   if (chx >= 0 .and. chx <= nbr_channels_2d_spectra .and. chy >= 0 .and. chy <= nbr_channels_2d_spectra) then
     spectrum_dpy_dpx(chx,chy) = spectrum_dthetay_dpx(chx,chy) + 1
   end if  
!
   chy = ch_thetay
   chx = ch_dpy
!
   if (chx >= 0 .and. chx <= nbr_channels_2d_spectra .and. chy >= 0 .and. chy <= nbr_channels_2d_spectra) then
     spectrum_dthetay_dpy(chx,chy) = spectrum_dthetay_dpy(chx,chy) + 1
   end if  
!
   chy = ch_thetax
   chx = ch_dpy
!
   if (chx >= 0 .and. chx <= nbr_channels_2d_spectra .and. chy >= 0 .and. chy <= nbr_channels_2d_spectra) then
     spectrum_dthetax_dpy(chx,chy) = spectrum_dthetax_dpy(chx,chy) + 1
   end if  
!
   chy = ch_thetax
   chx = ch_dpx
!
   if (chx >= 0 .and. chx <= nbr_channels_2d_spectra .and. chy >= 0 .and. chy <= nbr_channels_2d_spectra) then
     spectrum_dthetax_dpx(chx,chy) = spectrum_dthetax_dpx(chx,chy) + 1
   end if  
!   
   if (debug_track) then
     print*
     write(output_unit,*) det_hit
     write(output_unit,*) 'Original  :', real(x_reference_step)
     write(output_unit,*) 'Tracked   :', real(x_track1)
     write(output_unit,*) 'Delta     :', real(x_track1-x_reference_step)
     write(output_unit,*) 'Delta-rel :', real(2.d0*(x_track1-x_reference_step)/(x_track1+x_reference_step+1d-100))
     print*     
   end if  
!   
 end do
 call make_gle_dthetay_dpx
 call make_gle_dthetay_dpy
 call make_gle_dthetax_dpx
 call make_gle_dthetax_dpy
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
   write(output_unit,*) 'no data in spectrum_dpy_dpx'
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

subroutine make_gle_dthetax_dpx
 use vars
 implicit none
 character (LEN=256) :: local_output_file(3), shell_command
 integer slash_position,length, chx, chy, z_max
 real spectrum_dthetax_dpx_local(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 logical backwards
!
 backwards = .true.
 slash_position = index(output_file(1),'/',backwards)
 length         = len(trim(output_file(1)))
 local_output_file(1) = output_file(1)(slash_position+1:length) // '_dthetax_dpx.z'
 local_output_file(2) = trim(output_file(1)) // '_dthetax_dpx.z'
 local_output_file(3) = trim(output_file(1)) // '_dthetax_dpx.gle'
!
! write gle file
! 
 z_max = maxval(spectrum_dthetax_dpx)
 spectrum_dthetax_dpx_local = float(spectrum_dthetax_dpx) / float(z_max)
 open (unit=30, file=local_output_file(3))
   write(30,*) 'size 16 16'
   write(30,*) 'include "color.gle"'
   write(30,*) 'amove 1 1'
   write(30,*) 'begin graph'
   write(30,*) ' size 14 14'
   write(30,*) ' xtitle "dp_{x}/p_{x} (%)" hei 0.6'
   write(30,*) ' ytitle "\theta_{xz} (mrad)" hei 0.6'
   write(30,*) ' title "dp_{x} vs. \theta_{xz}" '
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
       write(30,'(F14.10)', ADVANCE='NO') spectrum_dthetax_dpx_local(chx,chy)
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


subroutine make_gle_dthetax_dpy
 use vars
 implicit none
 character (LEN=256) :: local_output_file(3), shell_command
 integer slash_position,length, chx, chy, z_max
 real spectrum_dthetax_dpy_local(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 logical backwards
!
 backwards = .true.
 slash_position = index(output_file(1),'/',backwards)
 length         = len(trim(output_file(1)))
 local_output_file(1) = output_file(1)(slash_position+1:length) // '_dthetax_dpy.z'
 local_output_file(2) = trim(output_file(1)) // '_dthetax_dpy.z'
 local_output_file(3) = trim(output_file(1)) // '_dthetax_dpy.gle'
!
! write gle file
! 
 z_max = maxval(spectrum_dthetax_dpy)
 spectrum_dthetax_dpy_local = float(spectrum_dthetax_dpy) / float(z_max)
 open (unit=30, file=local_output_file(3))
   write(30,*) 'size 16 16'
   write(30,*) 'include "color.gle"'
   write(30,*) 'amove 1 1'
   write(30,*) 'begin graph'
   write(30,*) ' size 14 14'
   write(30,*) ' xtitle "dp_{y}/p_{y} (%)" hei 0.6'
   write(30,*) ' ytitle "\theta_{xz} (mrad)" hei 0.6'
   write(30,*) ' title "dp_{y} vs. \theta_{xz}" '
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
       write(30,'(F14.10)', ADVANCE='NO') spectrum_dthetax_dpy_local(chx,chy)
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

subroutine make_gle_dthetay_dpy
 use vars
 implicit none
 character (LEN=256) :: local_output_file(3), shell_command
 integer slash_position,length, chx, chy, z_max
 real spectrum_dthetay_dpy_local(0:nbr_channels_2d_spectra,0:nbr_channels_2d_spectra)
 logical backwards
!
 backwards = .true.
 slash_position = index(output_file(1),'/',backwards)
 length         = len(trim(output_file(1)))
 local_output_file(1) = output_file(1)(slash_position+1:length) // '_dthetay_dpy.z'
 local_output_file(2) = trim(output_file(1)) // '_dthetay_dpy.z'
 local_output_file(3) = trim(output_file(1)) // '_dthetay_dpy.gle'
!
! write gle file
! 
 z_max = maxval(spectrum_dthetay_dpy)
 spectrum_dthetay_dpy_local = float(spectrum_dthetay_dpy) / float(z_max)
 open (unit=30, file=local_output_file(3))
   write(30,*) 'size 16 16'
   write(30,*) 'include "color.gle"'
   write(30,*) 'amove 1 1'
   write(30,*) 'begin graph'
   write(30,*) ' size 14 14'
   write(30,*) ' xtitle "dp_{y}/p_{y} (%)" hei 0.6'
   write(30,*) ' ytitle "\theta_{yz} (mrad)" hei 0.6'
   write(30,*) ' title "dp_{y} vs. \theta_{yz}" '
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
       write(30,'(F14.10)', ADVANCE='NO') spectrum_dthetay_dpy_local(chx,chy)
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

subroutine get_single_track_parameter_var5  ! determines theta_xz, theta_yz, p, x, y for the current set of track-detector data
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
   write(output_unit,*) 'derivative      ',derivative_variable(1:nbr_detectors,4)
   write(output_unit,*) 'theta-correction',d_track_x
   write(output_unit,*) 'track           ',track_det_frame(:,1)
 end if  
 track_det_frame_y_corrected(:,1) = track_det_frame(:,1) - d_track_x
!  
 call get_single_track_parameter_var3 
!
end



subroutine get_double_track_parameter_var5  ! determines 2x(theta_xz, p) + x for the current set of track-detector data
 use vars
 implicit none
 integer detector, det_pattern_id,pattern_id
 integer, parameter :: n_var_local = 5    ! e.g.  d_x, d_theta, d_1_over_p
 double precision b_vector(n_var_local)   ! we will solve a*b = p
 double precision v1(nbr_detectors),v2(nbr_detectors), dth 
 logical local_hit_pattern(2*nbr_detectors)
!
! nbr_of_detectors_hit = count(track_hit_pattern)
 b_vector = 0.d0
!
! populate b, a with data from current track
! 
 v1 = (track_det_frame1_y_corrected(:,1) - offset_ave_x)/sigma_track2(:,1)  !  (:,1)  is the x-component
 v2 = (track_det_frame2_y_corrected(:,1) - offset_ave_x)/sigma_track2(:,1)  !  (:,1)  is the x-component
 do detector=1,nbr_detectors
   if (track_hit_pattern1(detector) .and. det_consider(detector,1) ) then 
     b_vector(1:2)     = b_vector(1:2) + v1(detector)*derivative_variable(detector,1:2)     
     b_vector(5)       = b_vector(5) + (v1(detector))*derivative_variable(detector,3) 
   end if
   if (track_hit_pattern2(detector) .and. det_consider(detector,1)  ) then 
     b_vector(3:4)     = b_vector(3:4) + v2(detector)*derivative_variable(detector,1:2)     
     b_vector(5)       = b_vector(5) + (v2(detector))*derivative_variable(detector,3) 
   end if
 end do    
 local_hit_pattern(1:nbr_detectors)                 = track_hit_pattern1 .and. det_consider(:,1) 
 local_hit_pattern(1+nbr_detectors:2*nbr_detectors) = track_hit_pattern2 .and. det_consider(:,1) 
 pattern_id = det_pattern_id(2*nbr_detectors,local_hit_pattern)
 
! 
 if (constrain_target_x) then
   b_vector(n_var_local) = b_vector(n_var_local) + pos_target(1)/sigma_target2(1)
   if (.not.t5_matrix_inv_ok(pattern_id,1)) call initialize_single_a_t_5(pattern_id)
   p_vector_x5 = matmul(t5_matrix_inv_array(pattern_id,:,:),b_vector)
   if (.not.t5_matrix_inv_ok(pattern_id,2)) write(output_unit,*) 't5_matrix not available'
 else
   if (.not.a5_matrix_inv_ok(pattern_id,1)) call initialize_single_a_t_5(pattern_id)
   p_vector_x5 = matmul(a5_matrix_inv_array(pattern_id,:,:),b_vector)  
   if (.not.a5_matrix_inv_ok(pattern_id,2)) write(output_unit,*) 'a5_matrix not available'
 end if  
 

!
! dtheta
!
 dth           = p_vector_x5(1)
 x_track1(4)   = x_reference(4)*dcos(dth) + x_reference(6)*dsin(dth)
 x_track1(6)   = x_reference(6)*dcos(dth) - x_reference(4)*dsin(dth)
 x_track1(5)   = x_track1(6)*dsin(theta_yz1)                  ! y-component
! 
 dth           = p_vector_x5(3)
 x_track2(4)   = x_reference(4)*dcos(dth) + x_reference(6)*dsin(dth)
 x_track2(6)   = x_reference(6)*dcos(dth) - x_reference(4)*dsin(dth)
 x_track2(5)   = x_track2(6)*dsin(theta_yz2)                  ! y-component
!
! dp
!
 x_track1(4:6) = x_track1(4:6)/(p_vector_x5(2)+1.d0)           !  relative change of p, length of start momentum
 x_track2(4:6) = x_track2(4:6)/(p_vector_x5(4)+1.d0)           !  relative change of p, length of start momentum
!
! dx
!
 x_track1(1)   = x_reference(1) + p_vector_x5(5)      ! x-position on target
! x_track1(2)   = x_reference(2)                    ! y-position on target 
 x_track1(2)   = y_target                          ! y-position on target
 x_track1(3)   = x_reference(3)                    ! z-position on target
!
 x_track2(1)   = x_reference(1) + p_vector_x5(5)      ! x-position on target
! x_track2(2)   = x_reference(2)                    ! y-position on target
 x_track2(2)   = y_target                          ! y-position on target
 x_track2(3)   = x_reference(3)                    ! z-position on target
!
 call get_double_chi2
! 
 if (debug_track) then
   print*
   write(output_unit,*) 'p_vector:               ',real(p_vector_x5)
   print*
   write(output_unit,*) 'reference track  params: ',real(x_reference)
   write(output_unit,*) '      new track1 params: ',real(x_track1)
   write(output_unit,*) '      new track2 params: ',real(x_track2)
   print*
   if (grafical_output) then
     track_out = .true.
     x_start = x_track1
     open (unit = unit_debug_track, file=trim(track_debug_file_d1))
       call single_track
     close(unit_debug_track)  
     x_start = x_track2
     open (unit = unit_debug_track, file=trim(track_debug_file_d2))
       call single_track
     close(unit_debug_track)  
     call make_gle_double_track
     call make_double_track_pic
     track_out = .false.
   end if  
   print*
   x_start = x_reference
 end if   
end

subroutine get_double_track_parameter_var8  ! determines 2x (theta_xz, theta_yz, p) and x,y for the current set of track-detector data
 use vars
 implicit none
 double precision theta_correction, d_track_x(nbr_detectors)
!
 call get_double_track_y   
! 
 theta_correction = 1.d0-1.d0/dcos(theta_yz1)
 d_track_x = derivative_variable(1:nbr_detectors,4)*theta_correction
 track_det_frame1_y_corrected(:,1) = track_det_frame1(:,1) - d_track_x
!
 theta_correction = 1.d0-1.d0/dcos(theta_yz2)
 d_track_x = derivative_variable(1:nbr_detectors,4)*theta_correction
 track_det_frame2_y_corrected(:,1) = track_det_frame2(:,1) - d_track_x
!  
 call get_double_track_parameter_var5 
!
end


subroutine get_double_track_y  ! theta_yz, y for the current set of track-detector data
 use vars
 implicit none
 integer detector, det_pattern_id,pattern_id
 integer, parameter :: n_var_local = 3    ! e.g.  d_y, y
 double precision b_vector(n_var_local)  ! we will solve a*p = b
 double precision v1(nbr_detectors),v2(nbr_detectors)
 logical local_hit_pattern(2*nbr_detectors)
!
 b_vector = 0.d0
!
! populate b, a with data from current track
! 
 call get_y_corrections(track_det_frame1(:,1),track_det_frame1(:,2))
 y_correction1 = y_correction
 v1 = (track_det_frame1(:,2) - y_correction1 - offset_ave_y)/sigma_track2(:,2)  !  (:,2)  is the y-component
! 
 call get_y_corrections(track_det_frame2(:,1),track_det_frame2(:,2))
 y_correction2 = y_correction
 v2 = (track_det_frame2(:,2) - y_correction2 - offset_ave_y)/sigma_track2(:,2)  !  (:,2)  is the y-component
! 
 do detector=1,nbr_detectors
   if (track_hit_pattern1(detector) .and. det_consider(detector,2) ) then 
     b_vector(1)     = b_vector(1) + v1(detector)*derivative_variable(detector+nbr_detectors,4)     
     b_vector(3)     = b_vector(3) + v1(detector)*derivative_variable(detector+nbr_detectors,5) 
   end if
   if (track_hit_pattern2(detector) .and. det_consider(detector,2) ) then 
     b_vector(2)     = b_vector(2) + v2(detector)*derivative_variable(detector+nbr_detectors,4)     
     b_vector(3)     = b_vector(3) + v2(detector)*derivative_variable(detector+nbr_detectors,5) 
   end if
 end do    
!
! get the correct matrix
!
 local_hit_pattern(1:nbr_detectors)                 = track_hit_pattern1 .and. det_consider(:,2)
 local_hit_pattern(1+nbr_detectors:2*nbr_detectors) = track_hit_pattern2 .and. det_consider(:,2)
 pattern_id = det_pattern_id(2*nbr_detectors,local_hit_pattern)
!
! calculate p
! 
 if (constrain_target_y) then
   b_vector(n_var_local) = b_vector(n_var_local) + pos_target(2)/sigma_target2(2)
   if (.not.y_t3_matrix_inv_ok(pattern_id,1)) call initialize_single_y_a_t_3(pattern_id)
   p_vector_y3 = matmul(y_t3_matrix_inv_array(pattern_id,:,:),b_vector)
   if (.not.y_t3_matrix_inv_ok(pattern_id,2)) write(output_unit,*) 'y_t3_matrix not available'
 else
   if (.not.y_a3_matrix_inv_ok(pattern_id,1)) call initialize_single_y_a_t_3(pattern_id)
   p_vector_y3 = matmul(y_a3_matrix_inv_array(pattern_id,:,:),b_vector)
   if (.not.y_a3_matrix_inv_ok(pattern_id,2)) write(output_unit,*) 'y_a3_matrix not available'
 end if  
 
!
 theta_yz1 = p_vector_y3(1)
 theta_yz2 = p_vector_y3(2)
 y_target  = p_vector_y3(3)
!
 if (debug_track) then
   write(output_unit,*) 'b_vector - d',b_vector
   write(output_unit,*) '2x2 Matrix fix: theta_yz, y_target', theta_yz, y_target
 end if  
end


subroutine get_single_track_y  ! theta_yz, y for the current set of track-detector data
 use vars
 implicit none
 integer detector,det_pattern_id,pattern_id
 integer, parameter :: n_var_local = 2    ! e.g.  d_y, y
 double precision b_vector(n_var_local)   ! we will solve a*p = b
 double precision v1(nbr_detectors)
!
 b_vector = 0.d0
!
! populate b, a with data from current track
! 
 call get_y_corrections(track_det_frame(:,1),track_det_frame(:,2))
 v1 = (track_det_frame(:,2) - y_correction - offset_ave_y)/sigma_track2(:,2)  !  (:,2)  is the y-component
!
 do detector=1,nbr_detectors
   if (track_hit_pattern(detector) .and. det_consider(detector,2) ) then 
     b_vector          = b_vector + v1(detector)*derivative_variable(detector+nbr_detectors,4:5)     
   end if
 end do  
!
! get the correct matrix
!
 pattern_id = det_pattern_id(nbr_detectors,(track_hit_pattern.and.det_consider(:,2)))
!
! calculate p
! 
 if (constrain_target_y) then
   b_vector(n_var_local) = b_vector(n_var_local) + pos_target(2)/sigma_target2(2)
   p_vector_y2 = matmul(y_t2_matrix_inv_array(pattern_id,:,:),b_vector)
 else
   p_vector_y2 = matmul(y_a2_matrix_inv_array(pattern_id,:,:),b_vector)
 end if  
 if (.not.y_a2_matrix_inv_ok(pattern_id)) write(output_unit,*) 'y_a2_matrix not available'
!
 theta_yz = p_vector_y2(1)
 y_target = p_vector_y2(2)
!
 if (debug_track) then
   write(output_unit,*) 'b_vector - s',b_vector
   write(output_unit,*) '2x2 Matrix fix: theta_yz, y_target', theta_yz, y_target
 end if  
end

subroutine get_single_track_parameter_var3  ! determines theta_xz, p, x for the current set of track-detector data
 use vars
 implicit none
 integer detector, i,det_pattern_id,pattern_id
 integer, parameter :: n_var_local = 3    ! e.g.  d_x, d_theta, d_1_over_p
 double precision b_vector(n_var_local)  ! we will solve a*p = b
 double precision v1(nbr_detectors), dth 
!
! nbr_of_detectors_hit = count(track_hit_pattern)
 b_vector = 0.d0
!
! populate b, a with data from current track
! 
 v1 = (track_det_frame_y_corrected(:,1) - offset_ave_x)/sigma_track2(:,1)  !  (:,1)  is the x-component
 do detector=1,nbr_detectors
   if (track_hit_pattern(detector) .and. det_consider(detector,1) ) then 
     b_vector     = b_vector + v1(detector)*derivative_variable(detector,1:n_var_local)     
   end if
 end do    
!
! get the correct matrix
!
 pattern_id = det_pattern_id(nbr_detectors,(track_hit_pattern.and.det_consider(:,1)))
!
! calculate p
! 
 if (constrain_target_x) then
   b_vector(n_var_local) = b_vector(n_var_local) + pos_target(1)/sigma_target2(1)
   p_vector_x3 = matmul(t3_matrix_inv_array(pattern_id,:,:),b_vector)
 else
   p_vector_x3 = matmul(a3_matrix_inv_array(pattern_id,:,:),b_vector)  
 end if  
!
 if (.not.a3_matrix_inv_ok(pattern_id)) write(output_unit,*) 'a3_matrix not available'
 if (p_vector_x3(2) .eq. -1.d0)  then
   p_vector_x3(2) = -0.9999999
   write(output_unit,*) 'p_vector_x3 corrected', p_vector_x3
 end if  
 if (dabs(p_vector_x3(1)) .le. 1.d-5)  then
   write(output_unit,*) 'p_vector_x3(1) = 0', p_vector_x3
   write(output_unit,*) 'b_vector', b_vector
   write(output_unit,*) 
   write(output_unit,*) track_hit_pattern
   write(output_unit,*) det_consider(:,1)
   write(output_unit,*) real(track_det_frame_y_corrected(:,1))
   write(output_unit,*) 
   
   
   do i=1,3
     write(output_unit,*) (a3_matrix_inv_array(pattern_id,i,:))
   end do
 end if  
!
! dtheta
!
 dth           = p_vector_x3(1)
 x_track1(4)   = x_reference(4)*dcos(dth) + x_reference(6)*dsin(dth)
 x_track1(6)   = x_reference(6)*dcos(dth) - x_reference(4)*dsin(dth)
!
! dp
!
 x_track1(4:6) = x_track1(4:6)/(p_vector_x3(2)+1.d0)           !  relative change of p, length of start momentum
 x_track1(5)   = x_track1(6)*dsin(theta_yz)                  ! y-component
!
! dx
!
 x_track1(1)   = x_reference(1) + p_vector_x3(3)      ! x-position on target
 x_track1(2)   = y_target                          ! y-position on target
 x_track1(3)   = x_reference(3)                    ! z-position on target
!
 call get_single_chi2
! 
 if (debug_track) then
   print*
   write(output_unit,*) 'p_vector:               ',(p_vector_x3)
   write(output_unit,*) 'b_vector:               ',(b_vector)
   print*
   write(output_unit,*) 'reference track params: ',real(x_reference)
   write(output_unit,*) '    new s-track params: ',real(x_track1)
   print*
   write(output_unit,*) 'track_hit_pattern: ',track_hit_pattern
   write(output_unit,*) 'Matrix-invesrion results: ' ,a3_matrix_inv_ok(pattern_id),t3_matrix_inv_ok(pattern_id)
   write(output_unit,*) a3_matrix_inv_array(pattern_id,:,:)
   write(output_unit,*) 'linear track points: ',real(detector_track_interactions_det_frame(:,1))
   write(output_unit,*) 'det values:          ',real(track_det_frame(:,1))
   write(output_unit,*) 'diff to det values:  ',real(track_det_frame(:,1)-detector_track_interactions_det_frame(:,1))
   write(output_unit,*) 'diff to ref track:   ',real(offset_ave_x-detector_track_interactions_det_frame(:,1))
   write(output_unit,*)
   write(output_unit,*) 'chi-square:          ',real(chi2_single)
   write(output_unit,*)
   x_start = x_track1
   v1      = detector_track_interactions_det_frame(:,1)
   if (grafical_output) then
     open (unit = unit_debug_track, file=trim(track_debug_file))
       track_out = .true.
       call single_track
       track_out = .false.
     close(unit_debug_track)  
     call make_gle_single_track
     call make_single_track_pic
   end if  
   write(output_unit,*) 'track_hit_pattern: ',track_hit_pattern
   write(output_unit,*) 'R-K-track points   : ',real(detector_track_interactions_det_frame(:,1))
   write(output_unit,*) 'diff to det values:  ',real(track_det_frame(:,1)-detector_track_interactions_det_frame(:,1))
   write(output_unit,*) 'diff to linear track:',real(v1-detector_track_interactions_det_frame(:,1))
   print*
   detector_track_interactions_det_frame(:,1) = v1
   x_start = x_reference
 end if   
end

subroutine get_double_chi2
 use vars
 implicit none
 double precision ch2_x, ch2_y,d_target
!
!  Track 1
!
 p_vector_x3(1:2)  = p_vector_x5(1:2)
 p_vector_x3(3)    = p_vector_x5(5)
 p_vector_y2(1)    = p_vector_y3(1)
 p_vector_y2(2)    = p_vector_y3(3)
 y_correction      = y_correction1
 track_hit_pattern = track_hit_pattern1
 track_det_frame   = track_det_frame1
! x_start           = x_track1
!
 call get_single_chi2 
!
 chi2_double(1:2)             = chi2_single
 track1_hit_pattern_from_chi2 = track_hit_pattern_from_chi2
 paddle_hit_1                 = paddle_hit
!
!  Track 2
!
 p_vector_x3(1:2)  = p_vector_x5(3:4)
 p_vector_x3(3)    = p_vector_x5(5)
 p_vector_y2(1)    = p_vector_y3(2)
 p_vector_y2(2)    = p_vector_y3(3)
 y_correction      = y_correction2
 track_hit_pattern = track_hit_pattern2
 track_det_frame   = track_det_frame2
! x_start           = x_track2
!
 call get_single_chi2 
!
 chi2_double(3:4)             = chi2_single
 track2_hit_pattern_from_chi2 = track_hit_pattern_from_chi2
 paddle_hit_2                 = paddle_hit
!
 chi2_double(5:6) = chi2_double(1:2) + chi2_double(3:4) 
 if (constrain_target_x) then
   d_target = pos_target(1) - p_vector_x5(5)
   ch2_x    = d_target**2/sigma_target2(1)
   chi2_double(5) = chi2_double(5) - ch2_x
 end if  
 if (constrain_target_y) then
   d_target = pos_target(2) - p_vector_y3(3)
   ch2_y    = d_target**2/sigma_target2(2)
   chi2_double(6) = chi2_double(6) - ch2_y
 end if  
end

subroutine get_single_chi2_runge_kutta
 use vars
 implicit none
 integer i
 double precision v1(nbr_detectors,2), d_target
 double precision ch2_x, ch2_y
! 
 detector_track_interactions_det_frame = 0.d0
 call single_track
! 
 v1(:,1:2)  = (detector_track_interactions_det_frame(:,1:2) - track_det_frame(:,1:2) )/sigma_track(:,1:2)
 chi2_single = 0.d0
 ch2_x = 0.d0
 ch2_y = 0.d0
 do i=1,nbr_detectors
   if (track_hit_pattern(i) ) then
     if (det_consider(i,1) ) then
       ch2_x = v1(i,1)*v1(i,1) + ch2_x
       if (debug_track) then
         write(output_unit,*) detector_name(i), ' pos-x: simu, tracker; chi2-x :', track_det_frame(i,1),&
         detector_track_interactions_det_frame(i,1),ch2_x
       end if  
     end if  
     if (det_consider(i,2) ) then
       ch2_y = v1(i,2)*v1(i,2) + ch2_y
       if (debug_track) then
         write(output_unit,*) detector_name(i), ' pos-y: simu, tracker; chi2-y :', track_det_frame(i,2),&
         detector_track_interactions_det_frame(i,2),ch2_y
       end if  
     end if  
   end if  
 end do  
 if (constrain_target_x) then
   d_target = pos_target(1) - p_vector_x3(3)
   ch2_x    = ch2_x  + d_target**2/sigma_target2(1)
 end if  
 if (constrain_target_y) then
   d_target = pos_target(2) - p_vector_y2(2)
   ch2_y    = ch2_y  + d_target**2/sigma_target2(2)
 end if  
 chi2_single(1) = ch2_x
 chi2_single(2) = ch2_y
!
end



subroutine get_single_chi2
 use vars
 implicit none
 integer i,k
 double precision v1(nbr_detectors,2), d_target
 double precision theta_correction, d_track_x(nbr_detectors) , ch2_x, ch2_y
 logical ok
! 
 theta_correction = 1.d0-1.d0/dcos(p_vector_y2(1))
!
 d_track_x = derivative_variable(1:nbr_detectors,4)*theta_correction
!
 detector_track_interactions_det_frame(:,1) = offset_ave_x + d_track_x
 detector_track_interactions_det_frame(:,2) = offset_ave_y + y_correction
!
 do k=1,3
   detector_track_interactions_det_frame(:,1) = detector_track_interactions_det_frame(:,1) + &
                                                derivative_variable(1:nbr_detectors,k)*p_vector_x3(k)
 end do
! 
 do k=1,2
   detector_track_interactions_det_frame(:,2) = detector_track_interactions_det_frame(:,2) + &
                                                derivative_variable(nbr_detectors+1:2*nbr_detectors,k+3)*p_vector_y2(k)
 end do
!
 v1(:,1:2)  = detector_track_interactions_det_frame(:,1:2) - track_det_frame(:,1:2) 
!
 do i=1,nbr_detectors
   ok = .true.
   do k=1,2   ! x,y
     ok = ok .and. (detector_track_interactions_det_frame(i,k) > detector_range(i,k,1) )
     ok = ok .and. (detector_track_interactions_det_frame(i,k) < detector_range(i,k,2) )
   end do
   track_hit_pattern_from_chi2(i) = ok
 end do
!
 chi2_single = 0.d0
 ch2_x = 0.d0
 ch2_y = 0.d0
 paddle_hit = .true.
 do i=1,nbr_detectors
   if (track_hit_pattern(i) ) then
     !
     ! paddle hit - is only important, if step-function...
     !
     do k=1,2
       if (det_step_function(i,k)) paddle_hit(i,k) = (dabs(v1(i,k)) < sigma_track(i,k))
     end do
     ! chi2
     if (det_consider(i,1) ) then
       ch2_x = v1(i,1)*v1(i,1)/sigma_track2(i,1) + ch2_x
       if (debug_track) then
         write(output_unit,*) detector_name(i), ' pos-x: simu, tracker; chi2-x :', track_det_frame(i,1),&
         detector_track_interactions_det_frame(i,1),ch2_x
       end if  
     end if  
     if (det_consider(i,2) ) then
       ch2_y = v1(i,2)*v1(i,2)/sigma_track2(i,2) + ch2_y
       if (debug_track) then
         write(output_unit,*) detector_name(i), ' pos-y: simu, tracker; chi2-y :', track_det_frame(i,2),&
         detector_track_interactions_det_frame(i,2),ch2_y
       end if  
     end if  
   end if  
 end do  
 if (constrain_target_x) then
   d_target = pos_target(1) - p_vector_x3(3)
   ch2_x    = ch2_x  + d_target**2/sigma_target2(1)
 end if  
 if (constrain_target_y) then
   d_target = pos_target(2) - p_vector_y2(2)
   ch2_y    = ch2_y  + d_target**2/sigma_target2(2)
 end if  
! 
 chi2_single(1) = ch2_x
 chi2_single(2) = ch2_y
!
end

subroutine initialize_single_y_a_t_3(pattern_id)  ! initializes the matrizes necessary to get the linear optimization of 2 y-tracks
 use vars
 implicit none
 integer, parameter :: n_var_local = 3
 integer detector, pattern_id, pattern_id1, pattern_id2
 logical local_hit_pattern(2*nbr_detectors), local_hit_pattern1(nbr_detectors), local_hit_pattern2(nbr_detectors),ok
!
   a3_matrix = 0.d0

   pattern_id1 = pattern_id / 2**nbr_detectors + 1
   pattern_id2 = pattern_id - 2**nbr_detectors * (pattern_id1 - 1)
   
   call get_hit_pattern(2*nbr_detectors,pattern_id,local_hit_pattern)
   call get_hit_pattern(nbr_detectors,pattern_id1,local_hit_pattern1)
   call get_hit_pattern(nbr_detectors,pattern_id2,local_hit_pattern2)
   local_hit_pattern(1:nbr_detectors) = local_hit_pattern1
   local_hit_pattern(nbr_detectors+1:2*nbr_detectors) = local_hit_pattern2
!   
   do detector=1,nbr_detectors
     if (local_hit_pattern(detector) ) then 
       a3_matrix(1,1) =  a3_matrix(1,1) + derivative_variable(detector+nbr_detectors,4)**2/sigma_track2(detector,2)
       a3_matrix(3,3)  = a3_matrix(3,3) + derivative_variable(detector+nbr_detectors,5)**2/sigma_track2(detector,2)
       a3_matrix(3,1) =  a3_matrix(3,1) + derivative_variable(detector+nbr_detectors,4) &
                                          *derivative_variable(detector+nbr_detectors,5)/sigma_track2(detector,2)
     end if  
     if (local_hit_pattern(detector+nbr_detectors) ) then 
       a3_matrix(2,2) =  a3_matrix(2,2) + derivative_variable(detector+nbr_detectors,4)**2/sigma_track2(detector,2)
       a3_matrix(3,3)  = a3_matrix(3,3) + derivative_variable(detector+nbr_detectors,5)**2/sigma_track2(detector,2)
       a3_matrix(3,2) =  a3_matrix(3,2) + derivative_variable(detector+nbr_detectors,4) &
                                          *derivative_variable(detector+nbr_detectors,5)/sigma_track2(detector,2)
     end if  
   end do 
!
   call invert_matrix(n_var_local,a3_matrix,a3_matrix_inv,ok)
   y_a3_matrix_inv_array(pattern_id,:,:) = a3_matrix_inv(:,:)
   y_a3_matrix_inv_ok(pattern_id,1)      = .true.
   y_a3_matrix_inv_ok(pattern_id,2)      = ok
!
   a3_matrix(n_var_local,n_var_local) = a3_matrix(n_var_local,n_var_local) + 1.d0/sigma_target2(2)
   call invert_matrix(n_var_local,a3_matrix,a3_matrix_inv,ok)
   y_t3_matrix_inv_array(pattern_id,:,:) = a3_matrix_inv(:,:)
   y_t3_matrix_inv_ok(pattern_id,1)      = .true.
   y_t3_matrix_inv_ok(pattern_id,2)      = ok

end

subroutine initialize_y_a_t_3  ! initializes the matrizes necessary to get the linear optimization of 2 y-tracks
 use vars
 implicit none
 integer pattern_id, pattern_id1, pattern_id2
!
 do pattern_id1=1,2**nbr_detectors-1
  do pattern_id2=1,2**nbr_detectors-1
   pattern_id = pattern_id2 + 2**nbr_detectors * (pattern_id1 - 1)
   call initialize_single_y_a_t_3(pattern_id)  
  end do
 end do  
end

subroutine initialize_y_a_t_2  ! initializes the matrizes necessary to get the linear optimization of the y-tracks
 use vars
 implicit none
 integer, parameter :: n_var_local = 2
 integer j,k,detector, pattern_id
 logical local_hit_pattern(nbr_detectors),ok
!
 do pattern_id=1,2**nbr_detectors-1
   a2_matrix = 0.
   call get_hit_pattern(nbr_detectors,pattern_id,local_hit_pattern)
   do k=1, n_var_local
     do j=1, n_var_local
       do detector=1,nbr_detectors
         if (local_hit_pattern(detector)) then
           a2_matrix(k,j) =  a2_matrix(k,j) + &
             derivative_variable(detector+nbr_detectors,j+3) * &
             derivative_variable(detector+nbr_detectors,k+3)/sigma_track2(detector,2)
         end if  
       end do  
     end do
   end do  
!
   call invert_matrix(n_var_local,a2_matrix,a2_matrix_inv,ok)
   y_a2_matrix_inv_array(pattern_id,:,:) = a2_matrix_inv(:,:)
   y_a2_matrix_inv_ok(pattern_id)        = ok
!
   a2_matrix(n_var_local,n_var_local) = a2_matrix(n_var_local,n_var_local) + 1.d0/sigma_target2(2)
!
   call invert_matrix(n_var_local,a2_matrix,a2_matrix_inv,ok)
   y_t2_matrix_inv_array(pattern_id,:,:) = a2_matrix_inv(:,:)
   y_t2_matrix_inv_ok(pattern_id)        = ok
 end do  
end

subroutine initialize_a_t_3  ! initializes the matrizes necessary to get the linear optimization of the x-tracks
 use vars
 implicit none
 integer, parameter :: n_var_local = 3
 integer j,k,detector, pattern_id
 logical local_hit_pattern(nbr_detectors),ok
!
 do pattern_id=1,2**nbr_detectors-1
   a3_matrix = 0.
   call get_hit_pattern(nbr_detectors,pattern_id,local_hit_pattern)
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
   call invert_matrix(n_var_local,a3_matrix,a3_matrix_inv,ok)
   a3_matrix_inv_array(pattern_id,:,:) = a3_matrix_inv(:,:)
   a3_matrix_inv_ok(pattern_id)        = ok
!
   a3_matrix(n_var_local,n_var_local) = a3_matrix(n_var_local,n_var_local) + 1.d0/sigma_target2(1)
!
   call invert_matrix(n_var_local,a3_matrix,a3_matrix_inv,ok)
   t3_matrix_inv_array(pattern_id,:,:) = a3_matrix_inv(:,:)
   t3_matrix_inv_ok(pattern_id)        = ok
 end do  
end

subroutine invert_matrix(n,a_in,a_inv_out,ok)
 implicit none
 integer n
 double precision a_in(n,n),a_inv_out(n,n), a_gauss(n,n)
 logical ok
!
! call invert_matrix_lgs(n,a_in,a_lgs)
 call invert_matrix_gauss(n,a_in,a_gauss,ok)
 a_inv_out = a_gauss
! a_inv_out = a_lgs
end

subroutine invert_matrix_gauss(n,a_in,a_inv_out,ok)
 implicit none
 integer n,k,l
 double precision a(n,n),a_in(n,n),a_inv_out(n,n), f
 logical ok
!
 a_inv_out = 0.d0
 a         = a_in
 do k=1, n
   a_inv_out(k,k) = 1.d0
 end do
!
 ok = .true.
 do k=1,n
   f = a(k,k)
   if (f == 0.d0) then
     ok = .false.
     a_inv_out = 0.d0
     exit
   end if  
   a(k,:)         =  a(k,:)      / f
   a_inv_out(k,:) =  a_inv_out(k,:) / f
   do l=1,n
     if (l /= k) then
       f = a(l,k)
       a_inv_out(l,:) =  a_inv_out(l,:) - f * a_inv_out(k,:)
       a(l,:)         =  a(l,:)         - f * a(k,:)
     end if
   end do
 end do
end

subroutine invert_matrix_lgs(n,a_in,a_inv_out)
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


subroutine initialize_a_t_5  ! initializes the matrizes necessary to get the linear optimization of 2 tracks
 use vars
 implicit none
 integer pattern_id, pattern_id1, pattern_id2
!
 do pattern_id1=1,2**nbr_detectors-1
   do pattern_id2=1,2**nbr_detectors-1
     pattern_id = pattern_id2 + 2**nbr_detectors * (pattern_id1 - 1)
     call initialize_single_a_t_5(pattern_id)
   end do
 end do  

end


subroutine initialize_single_a_t_5(pattern_id)
 use vars
 implicit none
 integer, parameter :: n_var_local = 5
 integer i,j,k,detector, pattern_id, pattern_id1, pattern_id2
 logical local_hit_pattern(2*nbr_detectors), local_hit_pattern1(nbr_detectors), local_hit_pattern2(nbr_detectors)
 logical ok
!

   a5_matrix = 0.
   pattern_id1 = pattern_id / 2**nbr_detectors + 1
   pattern_id2 = pattern_id - 2**nbr_detectors * (pattern_id1 - 1)
   call get_hit_pattern(2*nbr_detectors,pattern_id,local_hit_pattern)
   call get_hit_pattern(nbr_detectors,pattern_id1,local_hit_pattern1)
   call get_hit_pattern(nbr_detectors,pattern_id2,local_hit_pattern2)
   local_hit_pattern(1:nbr_detectors) = local_hit_pattern1
   local_hit_pattern(nbr_detectors+1:2*nbr_detectors) = local_hit_pattern2
!
   do k=1, 2
     do detector=1,nbr_detectors
       if (local_hit_pattern(detector)  ) then
!       
         a5_matrix(k,n_var_local) =  a5_matrix(k,n_var_local) + &
           derivative_variable(detector,3)*derivative_variable(detector,k)/sigma_track2(detector,1)
!
         do j=1,2
!
           a5_matrix(k,j) =  a5_matrix(k,j) + &
             derivative_variable(detector,j)*derivative_variable(detector,k)/sigma_track2(detector,1)
!
         end do  
       end if  
       if (local_hit_pattern(detector+nbr_detectors)  ) then
!       
         a5_matrix(k+2,n_var_local) =  a5_matrix(k+2,n_var_local) + &
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
   do i=1,n_var_local-1
     a5_matrix(n_var_local,i) =  a5_matrix(i,n_var_local)
   end do
   do detector=1,nbr_detectors
     if (local_hit_pattern(detector)) then 
       a5_matrix(n_var_local,n_var_local)  = a5_matrix(n_var_local,n_var_local) &
                                           + derivative_variable(detector,3)**2/sigma_track2(detector,1)
     end if  
     if (local_hit_pattern(detector+nbr_detectors) ) then 
       a5_matrix(n_var_local,n_var_local)  = a5_matrix(n_var_local,n_var_local) &
                                           + derivative_variable(detector,3)**2/sigma_track2(detector,1)
     end if  
   end do 
!
   call invert_matrix(n_var_local,a5_matrix,a5_matrix_inv,ok)
   a5_matrix_inv_ok(pattern_id,1)      = .true.
   a5_matrix_inv_ok(pattern_id,2)      = ok
   a5_matrix_inv_array(pattern_id,:,:) = a5_matrix_inv(:,:)
!
   a5_matrix(n_var_local,n_var_local) = a5_matrix(n_var_local,n_var_local) + 1.d0/sigma_target2(1)
!
   call invert_matrix(n_var_local,a5_matrix,a5_matrix_inv,ok)
   t5_matrix_inv_ok(pattern_id,1)      = .true.
   t5_matrix_inv_ok(pattern_id,2)      = ok
   t5_matrix_inv_array(pattern_id,:,:) = a5_matrix_inv(:,:)
end

subroutine get_single_track_data  ! at a later stage, we will here read in experimental data
 use vars
 implicit none
 double precision theta_step, t1, t2
!
 do 
  call random_number(theta_step)
  theta_step = theta_step*0.16 - 0.08
!  theta_step = 0.01d0
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
  det_hit = .false.
  call single_track
  if ( (det_hit(1).or.det_hit(2)) .and. (det_hit(7).or.det_hit(8)) .and. &
    (det_hit(3).or.det_hit(4).or.det_hit(5).or.det_hit(6) )) exit
 end do 
 track_det_frame = detector_track_interactions_det_frame
 if (debug_track) write(output_unit,*) 'theta_xz = ',t1*1000, 'mrad'
 if (debug_track) write(output_unit,*) 'theta_yz = ',t2*1000, 'mrad'
end

subroutine read_single_track_data  ! at a later stage, we will here read in experimental data
 use vars
 implicit none
 integer tr, d, eastat
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

subroutine vary_parameter(var)
 use vars
 use calib
 implicit none
 integer var, i, j,local_nbr_tracks
 double precision dx_vary, dtheta, dtheta0, dp, dy_vary,p0
 double precision variable(2)
 double precision dth, d_1_over_p, ra,dx1, dy1
 double precision theta_step
 character (LEN=length_detector_name) :: dummy_ch
 character (LEN=80) :: first_line, gle_extension, first_line_tof
 character (LEN=256) :: local_output_file, local_output_file_tof
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
     dx_vary = 0.005d0                         ! in m
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
     dy_vary = 0.005d0                         ! in m
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
   sum_xi         = 0.
   sum_yi         = 0. 
   sum_xixi       = 0.
   sum_xiyi       = 0.
   sum_tracks     = 0.
   detector_position_spectra = 0.
   detector_position_spectra_paramter = 0.
!
   if (grafical_output .and. .not.calibration_mode) then
     open (unit=20,file=local_output_file)
     open (unit=23,file=local_output_file_tof)
     write(20,*) first_line
     write(23,*) first_line_tof
   end if
   if (calibration_mode) then
     local_nbr_tracks     = nbr_tracks 
     inside_detector_only = .false.
     sum_xixi_p   = 0.
     sum_xiyi_p   = 0.
     sum_xixi_m   = 0.
     sum_xiyi_m   = 0.
   else
     write(*,*) 'vary: ', var, ' (',trim(gle_extension),')'
     local_nbr_tracks = nbr_tracks*10
   end if
!   do j=-8,8
   do j=-0,0
     theta_step          = 1.d-2*dble(j)
     x_reference_step    = x_reference
     x_reference_step(4) = x_reference(4)*dcos(theta_step) + x_reference(6)*dsin(theta_step)
     x_reference_step(6) = x_reference(6)*dcos(theta_step) - x_reference(4)*dsin(theta_step)
     x_start = x_reference_step
     call single_track
     detector_track_interactions_det_frame_ref_step = detector_track_interactions_det_frame
!
     do i=1,local_nbr_tracks
       call random_number(ra)
       ra = ra*2.0-1.d0
       select case(var)
           case(1)                                  ! dx_theta
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
            dp = 1.d0 + d_1_over_p*ra
            x_start(4:6) = x_reference_step(4:6) * dp           ! dp is relative change ... Gauss around 1, always > 0
            variable = 1.d0/dp -1.d0                            ! same variable for pos_x and pos_y
           case(3)                                  ! dx_vary
             dx1 = ra*dx_vary
             x_start(1) = x_reference_step(1) + dx1
             variable = dx1                                     ! same variable for pos_x and pos_y
           case(4)                                  ! dy_theta
            dth   =ra*dtheta
            x_start(5)   = x_reference_step(5)*dcos(dth) + x_reference_step(6)*dsin(dth)
            x_start(6)   = x_reference_step(6)*dcos(dth) - x_reference_step(5)*dsin(dth)
            x_start(4)   = x_reference_step(4)
            variable(1)  = 1.d0-1.d0/dcos(dth)                  ! variable for pos_x
             variable(2)  = dth                                  ! variable for pos_y
           case(5)                                  ! dy_vary
             dy1 = ra*dy_vary
             x_start(2) = x_reference_step(2) + dy1
             variable = dy1                                     ! same variable for pos_x and pos_y
       end select   
       call single_track 
       call store_track(variable,inside_detector_only)
       if (.not.calibration_mode) call store_times(variable)
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
     if (calibration_mode .or. sum_tracks(i) > 100 ) then
       derivative_variable(i,var) = sum_xiyi(i) / sum_xixi(i)
     else
       print*,'Detector ',i, detector_name(i), ' not hit often enough to determine derivative ', &
                ' (',trim(gle_extension),')',real(sum_tracks(i))
       print*
       print*,'Please acknowledge (any entry will continue)'
       read(*,*) dummy_ch
       print*
       derivative_variable(i,var) = 0.d0
     end if  
     derivative_variable(i+nbr_detectors,var) = sum_xiyi(i+nbr_detectors) / sum_xixi(i+nbr_detectors)
     if (calibration_mode) then
       derivative_variable_m(i,var) = sum_xiyi_m(i) / sum_xixi_m(i)
       derivative_variable_p(i,var) = sum_xiyi_p(i) / sum_xixi_p(i)
       derivative_variable_m(i+nbr_detectors,var) = sum_xiyi_m(i+nbr_detectors) / sum_xixi_m(i+nbr_detectors)
       derivative_variable_p(i+nbr_detectors,var) = sum_xiyi_p(i+nbr_detectors) / sum_xixi_p(i+nbr_detectors)
     end if
   end do  
    !
   if (grafical_output .and. .not.calibration_mode) then
         close(20)
         close(23)
         call get_detector_position_spectra_parameter
         call make_gle_positions(trim(gle_extension))
   end if
   x_start = x_reference
!  
 end if
end

subroutine initialize_y_corrections
 use vars
 use calib
 implicit none
 logical ok
 integer d, var, var2
 double precision dy_theta, dx_theta, theta_max, dy_th, dx_th
 double precision dy_interactions(nbr_detectors), incr
 double precision y_interactions_0(nbr_detectors), y_interactions_0_m(nbr_detectors), y_interactions_0_p(nbr_detectors)
 double precision x_interactions(nbr_detectors), y_interactions(nbr_detectors)
 double precision vector(nbr_detectors,nbr_functions_y_correction)
 double precision vector_p(nbr_detectors,nbr_functions_y_correction)
 double precision vector_m(nbr_detectors,nbr_functions_y_correction)
 double precision matrix(nbr_detectors,nbr_functions_y_correction,nbr_functions_y_correction)
 double precision matrix_p(nbr_detectors,nbr_functions_y_correction,nbr_functions_y_correction)
 double precision matrix_m(nbr_detectors,nbr_functions_y_correction,nbr_functions_y_correction)
 double precision inv_matrix(nbr_functions_y_correction,nbr_functions_y_correction)
 double precision :: basic_functions_y_correction
!
 reference_track = .false.
     print*,'y-correction 1'

  if (get_new_y_corrections) then 
    dy_theta  = 0.0025d0   ! go in steps of 2.5 mrad
    dx_theta  = 0.0025d0   ! go in steps of 2.5 mrad
    theta_max = 0.08d0    ! maximum spread, corresponds to acceptance of GLAD
    dy_th     = 0.d0
    dy_th     = 0
    vector    = 0.d0
    matrix    = 0.d0
    vector_p    = 0.d0
    matrix_p    = 0.d0
    vector_m    = 0.d0
    matrix_m    = 0.d0
    do while (dy_th <= theta_max)
      dx_th = 0.d0
      x_reference_step = x_reference
      x_reference_step(4) = x_reference(4) 
      x_reference_step(5) = x_reference(5)*dcos(dy_th) + x_reference(6)*dsin(dy_th)
      x_reference_step(6) = x_reference(6)*dcos(dy_th) - x_reference(5)*dsin(dy_th)
      x_start             = x_reference_step  
      call single_track 
!
      dx_th = -theta_max
      y_interactions_0 = detector_track_interactions_det_frame(:,2)
      if (calibration_mode) then
        y_interactions_0_m = detector_track_interactions_det_frame_m(:,2)
        y_interactions_0_p = detector_track_interactions_det_frame_p(:,2)
      end if
      do while (dx_th <= theta_max)
        x_start(4)   = x_reference_step(4)*dcos(dx_th) + x_reference_step(6)*dsin(dx_th)
        x_start(5)   = x_reference_step(5)
        x_start(6)   = x_reference_step(6)*dcos(dx_th) - x_reference_step(4)*dsin(dx_th)
        call single_track
!
        x_interactions  = detector_track_interactions_det_frame(:,1) 
        y_interactions  = detector_track_interactions_det_frame(:,2) 
        dy_interactions = y_interactions - y_interactions_0
        do d=1,nbr_detectors
          do var = 1,nbr_functions_y_correction
            incr = dy_interactions(d)*basic_functions_y_correction(var,x_interactions(d),y_interactions(d))
            vector(d,var) = vector(d,var) + incr
            do var2 = 1, nbr_functions_y_correction
              incr = basic_functions_y_correction(var,x_interactions(d),y_interactions(d))&
                     *basic_functions_y_correction(var2,x_interactions(d),y_interactions(d))
              matrix(d,var,var2) = matrix(d,var,var2) + incr
            end do       
          end do
        end do
        if (calibration_mode) then
          x_interactions  = detector_track_interactions_det_frame_p(:,1) 
          y_interactions  = detector_track_interactions_det_frame_p(:,2) 
          dy_interactions = y_interactions - y_interactions_0_p
          do d=1,nbr_detectors
            do var = 1,nbr_functions_y_correction
              incr = dy_interactions(d)*basic_functions_y_correction(var,x_interactions(d),y_interactions(d))
              vector_p(d,var) = vector_p(d,var) + incr
              do var2 = 1, nbr_functions_y_correction
                incr = basic_functions_y_correction(var,x_interactions(d),y_interactions(d))&
                       *basic_functions_y_correction(var2,x_interactions(d),y_interactions(d))
                matrix_p(d,var,var2) = matrix_p(d,var,var2) + incr
              end do       
            end do
          end do
!        
          x_interactions  = detector_track_interactions_det_frame_m(:,1) 
          y_interactions  = detector_track_interactions_det_frame_m(:,2) 
          dy_interactions = y_interactions - y_interactions_0_m
          do d=1,nbr_detectors
            do var = 1,nbr_functions_y_correction
              incr = dy_interactions(d)*basic_functions_y_correction(var,x_interactions(d),y_interactions(d))
              vector_m(d,var) = vector_m(d,var) + incr
              do var2 = 1, nbr_functions_y_correction
                incr = basic_functions_y_correction(var,x_interactions(d),y_interactions(d))&
                       *basic_functions_y_correction(var2,x_interactions(d),y_interactions(d))
                matrix_m(d,var,var2) = matrix_m(d,var,var2) + incr
              end do       
            end do
          end do
        end if
        dx_th = dx_th + dx_theta
      end do
!
      dy_th = dy_th + dy_theta
    end do
!
    do d = 1, nbr_detectors
      call invert_matrix(nbr_functions_y_correction,matrix(d,:,:),inv_matrix,ok)
      coefficients_y_correction(d,:)=matmul(vector(d,:),inv_matrix)
      if (calibration_mode) then
        coefficients_y_correction_ref(d,:) = coefficients_y_correction(d,:)
        call invert_matrix(nbr_functions_y_correction,matrix_p(d,:,:),inv_matrix,ok)
        coefficients_y_correction_p(d,:)=matmul(vector_p(d,:),inv_matrix)
        call invert_matrix(nbr_functions_y_correction,matrix_m(d,:,:),inv_matrix,ok)
        coefficients_y_correction_m(d,:)=matmul(vector_m(d,:),inv_matrix)
      end if  
    end do  
  end if
  print*,'y-correction 3' 
end   

subroutine get_y_corrections(x_local, y_local)
 use vars
 implicit none
 integer var, d
 double precision :: basic_functions_y_correction
 double precision :: x_local(nbr_detectors), y_local(nbr_detectors)
! 
 y_correction = 0.d0
 do d=1, nbr_detectors
   do var=1,nbr_functions_y_correction
     y_correction(d) = y_correction(d) + coefficients_y_correction(d,var)* &
                           basic_functions_y_correction(var,x_local(d),y_local(d))
   end do
 end do
end

subroutine get_derivatives
 use vars
 use calib
 implicit none
 integer io_unit, var, d
 character (LEN=20) :: dummy
 logical debug_track_backup
!
 io_unit = 19
! 
 m2=m*m
 if (.not.calibration_mode) reference_track = .true.
 reference_track = .true.
 call single_track 
!
 detector_track_interactions_det_frame_ref = detector_track_interactions_det_frame
 detector_track_interactions_lab_frame_ref = detector_track_interactions_lab_frame
 reference_track = .false.
 if (calibration_mode) then
   detector_track_interactions_det_frame_ref_m = detector_track_interactions_det_frame_m
   detector_track_interactions_det_frame_ref_p = detector_track_interactions_det_frame_p
   do var=1,nbr_variables
     call vary_parameter(var) 
   end do 
   derivative_variable_ref = derivative_variable
 else
   open (unit=io_unit, file=trim(derivative_file))
     if (get_new_derivatives) then 
!    
       debug_track_backup = debug_track
       debug_track = .false.
!    
       print*
       print*,'initialization of derivatives'
       do var=1,nbr_variables
         call vary_parameter(var) 
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
       end do 
     end if  
   close(io_unit)
 end if
 offset_ave_x = 0.
 offset_ave_y = 0.
 do var = 1, nbr_variables
   offset_ave_x = offset_ave_x + offset_variable(:,var)                        ! x-deviations, theta
 end do
 offset_ave_x = offset_ave_x / nbr_variables
 
 offset_ave_x = detector_track_interactions_det_frame_ref(:,1)
 offset_ave_y = detector_track_interactions_det_frame_ref(:,2)
 
 if (grafical_output.and..not.calibration_mode) then
   call make_gle
   call make_pics
 end if  
end

subroutine store_times(variable)
 use vars
 implicit none
 double precision variable(2)
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
 use calib
 implicit none
 double precision variable(2), y, local_det_frame(3)
 integer j,k, channel
 logical inside_detector_only

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
   if (calibration_mode) then
     local_det_frame = detector_track_interactions_det_frame_m(j,:)
     y = local_det_frame(1) - detector_track_interactions_det_frame_ref_m(j,1)  ! x-component of each detector
     sum_xixi_m(j)   = sum_xixi_m(j) + variable(1)*variable(1)
     sum_xiyi_m(j)   = sum_xiyi_m(j) + y*variable(1)    
     y = local_det_frame(2) - detector_track_interactions_det_frame_ref_m(j,2)  ! y-component of each detector
     sum_xixi_m(j+nbr_detectors)   = sum_xixi_m(j+nbr_detectors) + variable(1)*variable(1)
     sum_xiyi_m(j+nbr_detectors)   = sum_xiyi_m(j+nbr_detectors) + y*variable(1)    
!     
     local_det_frame = detector_track_interactions_det_frame_p(j,:)
     y = local_det_frame(1) - detector_track_interactions_det_frame_ref_p(j,1)  ! x-component of each detector
     sum_xixi_p(j)   = sum_xixi_p(j) + variable(1)*variable(1)
     sum_xiyi_p(j)   = sum_xiyi_p(j) + y*variable(1)    
     y = local_det_frame(2) - detector_track_interactions_det_frame_ref_p(j,2)  ! y-component of each detector
     sum_xixi_p(j+nbr_detectors)   = sum_xixi_p(j+nbr_detectors) + variable(1)*variable(1)
     sum_xiyi_p(j+nbr_detectors)   = sum_xiyi_p(j+nbr_detectors) + y*variable(1)    
   end if  
 end do  
!
   if (grafical_output.and..not.calibration_mode) then
     do j=1,nbr_detectors
       write(20,'(3F15.10)', advance='no') detector_track_interactions_det_frame(j,:)
     end do  
     write(20,*) variable
   end if
!
end

subroutine get_detector_position_spectra_parameter
 use vars
 use calib
 implicit none
 double precision sigma, ave, channel_sum, d_range, d_min
 integer i, j, channel, minimum, maximum
!
 do i=1,nbr_detectors
   do j=1,max_dim_pos_spectra
     ave   = 0.
     sigma = 0.
     minimum = 0
     maximum = 0
     if (sweep_calibration) then
       d_range = detector_range(i,1,3)
       d_min   = detector_range(i,1,1)
     else
       d_range = detector_range(i,j,3)
       d_min   = detector_range(i,j,1)
     end if    
     channel_sum = sum(detector_position_spectra(i,j,:))  
     if (channel_sum > 0.) then
       do channel=1,nbr_channels_position_spectra
         if (detector_position_spectra(i,j,channel) > 0) then
           if (minimum == 0) minimum = channel
           maximum = channel
         end if  
         ave   = ave   + dble(channel)*dble(detector_position_spectra(i,j,channel))
         sigma = sigma + dble(channel)**2*dble(detector_position_spectra(i,j,channel))
       end do  
       ave   = ave   / channel_sum
       sigma = sigma / channel_sum
       sigma = dsqrt( sigma - ave*ave )
       ave   = (ave - 0.5d0) / dble(nbr_channels_position_spectra) * &
                  d_range + d_min
       sigma = sigma / dble(nbr_channels_position_spectra) * d_range 
       detector_position_spectra_paramter(i,j,5) = dble(minimum)
       detector_position_spectra_paramter(i,j,6) = dble(maximum)
       detector_position_spectra_paramter(i,j,3) = (dble(minimum) - 0.5d0) / &
          dble(nbr_channels_position_spectra) * d_range + d_min
       detector_position_spectra_paramter(i,j,4) = (dble(maximum) - 0.5d0) / &
          dble(nbr_channels_position_spectra) * d_range + d_min
     end if  
     detector_position_spectra_paramter(i,j,1) = ave
     detector_position_spectra_paramter(i,j,2) = sigma
   end do
 end do
end

subroutine allocate_arrays
 use vars
 implicit none
!
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
  allocate( det_hit(nbr_detectors) )
  allocate( det_passed(nbr_detectors) )
  allocate( det_consider(nbr_detectors,3) )
  det_consider      = .false.
  allocate( det_step_function(nbr_detectors,3) )
  det_step_function = .false.
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
  allocate( detector_track_interactions_lab_frame_ref(nbr_detectors,3) )
  allocate( detector_track_interactions_time(nbr_detectors) )
  allocate( detector_track_interactions_path(nbr_detectors) )
  allocate( detector_track_interactions_det_frame(nbr_detectors,3) )
  allocate( detector_track_interactions_det_frame_ref(nbr_detectors,3) )
  allocate( detector_track_interactions_det_frame_ref_step(nbr_detectors,3) )
  allocate( track_det_frame(nbr_detectors,3) )
  allocate( track_det_frame1(nbr_detectors,3) )
  allocate( track_det_frame2(nbr_detectors,3) )
  allocate( track_det_frame_y_corrected(nbr_detectors,3) )
  allocate( track_det_frame1_y_corrected(nbr_detectors,3) )
  allocate( track_det_frame2_y_corrected(nbr_detectors,3) )
  allocate( track_lab_frame(nbr_detectors,3) )
  allocate( sigma_track(nbr_detectors,3) )
  sigma_track = 0.d0
  allocate( sigma_track2(nbr_detectors,3) )
  allocate( track_hit_pattern(nbr_detectors) )
  allocate( track_hit_pattern1(nbr_detectors) )
  allocate( track_hit_pattern2(nbr_detectors) )
  allocate( track_hit_pattern_used(2:nbr_detectors) )
  allocate( track1_hit_pattern_used(2:nbr_detectors) )
  allocate( track2_hit_pattern_used(2:nbr_detectors) )
  allocate( track_hit_pattern_from_chi2(nbr_detectors) )
  allocate( track1_hit_pattern_from_chi2(nbr_detectors) )
  allocate( track2_hit_pattern_from_chi2(nbr_detectors) )
  allocate( paddle_hit(nbr_detectors,3) )
  allocate( paddle_hit_1(nbr_detectors,3) )
  allocate( paddle_hit_2(nbr_detectors,3) )
  allocate( detector_position_spectra(nbr_detectors,max_dim_pos_spectra,nbr_channels_position_spectra) )
  allocate( detector_position_spectra_paramter(nbr_detectors,max_dim_pos_spectra,nbr_paramters_position_spectra) )
  allocate( y_a2_matrix_inv_ok(2**nbr_detectors-1) )
  allocate( y_t2_matrix_inv_ok(2**nbr_detectors-1) )
  allocate( y_a3_matrix_inv_ok(2**(2*nbr_detectors),2) )
  y_a3_matrix_inv_ok = .false.
  allocate( y_t3_matrix_inv_ok(2**(2*nbr_detectors),2) )
  y_t3_matrix_inv_ok = .false.
  allocate( a3_matrix_inv_ok(2**nbr_detectors-1) )
  allocate( t3_matrix_inv_ok(2**nbr_detectors-1) )
  allocate( a5_matrix_inv_ok( 2**(2*nbr_detectors),2 ) )
  a5_matrix_inv_ok = .false.
  allocate( t5_matrix_inv_ok( 2**(2*nbr_detectors),2 ) )
  t5_matrix_inv_ok = .false.
  allocate( y_a2_matrix_inv_array(2**nbr_detectors-1,2,2) )
  allocate( y_t2_matrix_inv_array(2**nbr_detectors-1,2,2) )
  allocate( y_a3_matrix_inv_array(2**(2*nbr_detectors),3,3) )
  allocate( y_t3_matrix_inv_array(2**(2*nbr_detectors),3,3) )
  allocate( a3_matrix_inv_array(2**nbr_detectors-1,3,3) )
  allocate( t3_matrix_inv_array(2**nbr_detectors-1,3,3) )
  allocate( a5_matrix_inv_array( 2**(2*nbr_detectors),5,5 ) )
  allocate( t5_matrix_inv_array( 2**(2*nbr_detectors),5,5 ) )
  allocate( coefficients_y_correction(nbr_detectors,nbr_functions_y_correction) )
  allocate( y_correction(nbr_detectors) )
  allocate( y_correction1(nbr_detectors) )
  allocate( y_correction2(nbr_detectors) )
  allocate( trigger_matrix(2**nbr_detectors-1) )
  allocate( trigger_always_together(nbr_detectors,nbr_detectors) )
  allocate( trigger_never_together(nbr_detectors,nbr_detectors) )
end

subroutine init
 use vars
 implicit none
 integer eastat
 character*300 buffer
 integer unit_read, buffer_counter
 double precision x1(3)
 logical inside_b_field
 integer i, j 
!
 buffer_counter = 1
 call getarg(buffer_counter,BUFFER)
 if (buffer.eq.'-calibration'.or.buffer.eq.'--calibration') then     
   screen_output     = .true.
   sweep_calibration = .true.
   calibration_mode  = .true.
   buffer_counter    = buffer_counter + 1
 end if
 call getarg(buffer_counter,BUFFER)
 if (buffer.eq.'-screen'.or.buffer.eq.'--screen') then     
   screen_output     = .true.
   buffer_counter    = buffer_counter + 1
 end if
!
 call getarg(buffer_counter,BUFFER)
!
 if (screen_output) then 
   output_unit = 6
 else
   output_unit = output_unit_log
   open (unit=output_unit_log, file=tracker_log_file)
 end if  
!
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
   if (.not. sweep_calibration) write(output_unit,*) 'Init file: ',ini_file(1:40)
   unit_read = 8
   open (unit=unit_read, file=ini_file , status='old')
 end if      
!
 read(unit_read,*) geometry_file  
 if (.not. sweep_calibration) write(output_unit,*) 'Geometry definition: ',trim(geometry_file)
!
 read(unit_read,*) derivative_file  
 get_new_derivatives = (trim(derivative_file) == '')
 if (calibration_mode) get_new_derivatives = .true.
 if (get_new_derivatives) then
   derivative_file = 'derivatives.out'
   if (.not. calibration_mode) write(output_unit,*) 'NEW derivatives will be stored in: ',trim(derivative_file)
 else
   if (.not. calibration_mode) write(output_unit,*) 'Derivatives definition: ',trim(derivative_file)
 end if 
!
 do i=1,3
   read(unit_read,*) x_start(i) , sigma_target(i)
   if (.not. sweep_calibration) write(output_unit,*) 'Start location: ',real(x_start(i)), ' +- ',real(sigma_target(i))
 end do  
 sigma_target2       = sigma_target
 pos_target          = x_start(1:3)
 pos_target_original = x_start(1:3)
!
 do i=1,3
   read(unit_read,'(A100)') buffer
   read(buffer,*,iostat=eastat) p_start(i)  , max_momentum_deviation(i)
   if (eastat /= 0) then
     read(buffer,*,iostat=eastat) p_start(i)
     max_momentum_deviation(i) = 0.d0
   end if  
   if (eastat /= 0) then
     p_start(i) = 0.d0
   end if  
   if (.not. sweep_calibration) write(output_unit,*) 'Start momentum (MeV/c/u)',p_start(i)
   if (.not. sweep_calibration) write(output_unit,*) 'Deviation of tracking momentum from start momentum (MeV/c/u)',p_start(i)
 end do  
!
 read(unit_read,*) m_amu     ! this is now a/q
 if (.not. sweep_calibration) write(output_unit,*) 'Mass of particle (AMU): ',m_amu
 m = m_amu * amu
 if (.not. sweep_calibration) write(output_unit,*) 'Mass of particle (MeV/c2): ',m/e*c2/1d6
!  
! read(unit_read,*) q_e  
 q_e = 1.d0                              ! calculate always with q == 1
 if (.not. sweep_calibration) write(output_unit,*) 'Charge of particle (e): ',q_e
 q = q_e * e
!
 x_start(4:6)           = p_start(1:3)*e/c*1.d6*m_amu
 max_momentum_deviation = max_momentum_deviation*e/c*1.d6*m_amu
!
 x_reference = x_start
!
 read(unit_read,*) spatial_resolution  
 if (.not. sweep_calibration) write(output_unit,*) 'Spatial resolution (m): ',spatial_resolution
!
 read(unit_read,*) constrain_target_x  
 if (constrain_target_x) then
   if (.not. sweep_calibration) write(output_unit,*) 'Constrain x-position on target: yes'
 else
   if (.not. sweep_calibration) write(output_unit,*) 'Constrain x-position on target: no'
 end if  
!
 read(unit_read,*) constrain_target_y  
 if (constrain_target_x) then
   if (.not. sweep_calibration) write(output_unit,*) 'Constrain y-position on target: yes'
 else
   if (.not. sweep_calibration) write(output_unit,*) 'Constrain y-position on target: no'
 end if  
!
 read(unit_read,*) debug_track  
 if (debug_track) then
!   screen_output = .true.
   if (.not. sweep_calibration) write(output_unit,*) 'Debugging mode: yes'
 else
   if (.not. sweep_calibration) write(output_unit,*) 'Debugging mode: no'
 end if  
!
 read(unit_read,*) grafical_output  
 read(unit_read,*) output_file(1)  
 if (.not. sweep_calibration) write(output_unit,*) 'Root of output file: ',trim(output_file(1))
 output_file(2) = trim(output_file(1)) // '.dat'
 output_file(3) = trim(output_file(1)) // '.gle'
! 
 if (grafical_output) then
   if (.not. sweep_calibration) write(output_unit,*) 'Grafical output: yes'
!
 else
   if (.not. sweep_calibration) write(output_unit,*) 'Grafical output: no' 
 end if
!
 if (sweep_calibration) write(output_unit,*) 'B-field sweep calibration of geometry ', trim(sweep_calibration_file)
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
!
  call allocate_arrays
!  
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
      det_consider(i,j) = (sigma_track(i,j) .gt. 0.d0)
      det_step_function(i,j) = (sigma_track(i,j) .lt. 0.d0)
    end do
    detector_position_in_lab(i,4) = detector_position_in_lab(i,4)/180.d0*datan(1.d0)*4.d0    ! conversion of deg into rad
    x1 = ( detector_range(i,:,2) + detector_range(i,:,1) ) / 2.
    if ( inside_b_field(x1) .and. .not. sweep_calibration) &
      write(output_unit,*)  'Detector ',trim(detector_name(i)),' is inside the B-field.'
  end do
  detector_range(:,:,3) = dabs( detector_range(:,:,2) - detector_range(:,:,1) )
 read (unit_read,*) ch_trigger_logic
 close(unit_read)
 sigma_track2 = sigma_track**2
 sigma_track  = dabs(sigma_track)
! 
 if (.not. sweep_calibration) write(output_unit,*) 'x-borders (m): ',master_borders(1,:) 
 if (.not. sweep_calibration) write(output_unit,*) 'y-borders (m): ',master_borders(2,:) 
 if (.not. sweep_calibration) write(output_unit,*) 'z-borders (m): ',master_borders(3,:) 
!
 if (.not. sweep_calibration) write(output_unit,*) 'x-b-field-borders (m): ',b_field_borders(1,:) 
 if (.not. sweep_calibration) write(output_unit,*) 'y-b-field-borders (m): ',b_field_borders(2,:) 
 if (.not. sweep_calibration) write(output_unit,*) 'z-b-field-borders (m): ',b_field_borders(3,:) 
! 
 write(output_unit,*)
 write(output_unit,*) 'init done, will now do some geometry checks'
 write(output_unit,*)
 call geometry_check
end

subroutine geometry_check
 use vars
 implicit none
 integer d1, d2
 double precision x_det(3), x_lab(3)
!
 write(output_unit,*) 'Positions (x,y,z) in m and angle in rad in lab-frame:'
 do d1=1,nbr_detectors
   write(output_unit,*) detector_name(d1),real(detector_position_in_lab(d1,:))
 end do
 write(output_unit,*)
 write(output_unit,*) 'Origin of detectors in other detectors frames:'
 write(output_unit,*)
 do d1=1,nbr_detectors
   write(output_unit,*) 'Origin of detector ', detector_name(d1) ,' in frame of detector ...'
   do d2 = 1,nbr_detectors
     x_lab = detector_position_in_lab(d1,1:3)
     call get_det_position(d2,x_lab,x_det)    
     write(output_unit,*) detector_name(d2), real(x_det)
   end do  
 end do
 write(output_unit,*)
 x_det(1) = 13.6d-3
 x_det(1) = 0.d0
 x_det(2) = 0.d0
 x_det(3) = 55.d-3
 write(output_unit,*) 'Convert det-frame-point ' ,real(x_det),' into lab frame from'
 do d1=1,nbr_detectors
   call get_lab_position(d1,x_det,x_lab)
   write(output_unit,*) detector_name(d1),(x_lab),'  ( ',real(detector_position_in_lab(d1,1:3)),' )'
 end do
! stop
end

subroutine get_xy_plane(d)
 use vars
 implicit none
 integer d
 double precision x1(3), x0(3)
!
    x1 = 0.
    call get_lab_position(d,x1,x0)                                                            ! x0 is now the detector coordinate's origin in the lab frame
    call get_lab_position(d,norm_z,x1)                                                        ! x1 is now the detector coordinate's (0,0,1) point in the lab frame
    detector_xy_plane(d,1:3) = x1-x0                                                        ! vector perpendicular to the detector's xy-plane in the lab frame
    detector_xy_plane(d,4  ) = dot_product(x0,detector_xy_plane(d,1:3))
end

subroutine single_track
 use vars
 use calib
 implicit none
 logical inside_master, inside_b_field, inside_detector, inside
 double precision d_xy_det(nbr_detectors), d_xy_det_old(nbr_detectors)
 double precision distance_detector_plane, x_local(3), ekin, p_total, p(3)
 integer i, local_nbr_steps
!
 t = 0.
 x = x_start
!
 call get_velocity(x(4:6))
 dt_fine   = spatial_resolution / dsqrt(dot_product(v,v))
 if (reference_track .and. grafical_output) open (unit=unit_standard_track, file=trim(output_file(2)))
 if (reference_track) then
   dt_fine = dt_fine/5.d0
   write(output_unit,*) 'Mass, Momentum: ', m, x(4:6)
   write(output_unit,*) 'Velocity: ', v, dsqrt(dot_product(v,v))
   write(output_unit,*) 'dt inside B-field: ', dt_fine , ' s'
   write(output_unit,*) 'dt outside B-field: ', dt_coarse , ' s'
 end if  
 dt_coarse = dt_fine *10.d0 
 nbr_steps = 0
 local_nbr_steps = sum(nbr_steps)
 detector_track_interactions_lab_frame = 0.
 detector_track_interactions_time      = 0.
 detector_track_interactions_det_frame = 0.
 det_hit     = .false.
 det_passed  = .false.
 do i=1,nbr_detectors
   call get_xy_plane(i)
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
   if (modulo(local_nbr_steps,10) == 0) then
     if (reference_track .and. grafical_output) write(unit_standard_track,*) x(1:3)
     if (track_out) write(unit_debug_track,*) x(1:3)
   end if  
   local_nbr_steps = sum(nbr_steps)
   t = t + dt
   x = x + dx
!
   do i=1,nbr_detectors
     if (.not. det_passed(i).or.calibration_mode) then                                            ! intersection point of this detector not yet determined
       d_xy_det(i) = distance_detector_plane( i, x(1:3) )     
       if (d_xy_det(i) * d_xy_det_old(i) < 0. ) then    ! sign has changed in last step
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
           p = x(4:6)
           ekin = dsqrt((m*c2)**2+dot_product(p,p)*c2 ) - m*c2
           ekin    = ekin*(1.d0-stopping_power(i))
           p_total = dsqrt( (ekin+m*c2)**2-(m*c2)**2)/c
           x(4:6)  = p*p_total/dsqrt(dot_product(p,p))
          end if
          det_hit(i) = .true.
         end if 
         det_passed(i) = .true.
       end if
       if (calibration_mode) then
         if ((d_xy_det(i)-derivative_step ) * (d_xy_det_old(i)-derivative_step) < 0.d0 ) then    ! sign has changed in last step
           detector_track_interactions_lab_frame_p(i,:) = x(1:3) &
                    -(d_xy_det(i)-derivative_step )/(d_xy_det(i) - d_xy_det_old(i)) * dx(1:3) 
           call get_det_position(i,detector_track_interactions_lab_frame_p(i,:),detector_track_interactions_det_frame_p(i,:))         
         end if
         if ((d_xy_det(i)+derivative_step ) * (d_xy_det_old(i)+derivative_step) < 0.d0 ) then    ! sign has changed in last step
           detector_track_interactions_lab_frame_m(i,:) = x(1:3) &
                    -(d_xy_det(i)+derivative_step )/(d_xy_det(i) - d_xy_det_old(i)) * dx(1:3) 
           call get_det_position(i,detector_track_interactions_lab_frame_m(i,:),detector_track_interactions_det_frame_m(i,:))         
         end if
       end if
       d_xy_det_old(i) = d_xy_det(i)
     end if  
   end do
 end do

!
 if (reference_track .and. grafical_output) close(unit_standard_track)
!
 if (reference_track) then
   write(output_unit,*) 'Final:' , t,x(1:3),  inside_master(x(1:3))
   call get_velocity(x(4:6))
   write(output_unit,*) 'Final angle:' , atan(v(1)/v(3)) * 180/3.14,' deg'
   call get_linear_slope_parameters(x_start)
   write(output_unit,*) 'Slope incoming:' , real(slope_parameter)
   call get_linear_slope_parameters(x)
   write(output_unit,*) 'Slope outgoing:' , real(slope_parameter)
   write(output_unit,*) 'Outgoing angle (x):' , abs(datan(slope_parameter(1))*180/3.14),' deg'
   write(output_unit,*) 'Number of steps (fine/coarse):' , nbr_steps 
   print*
   write(output_unit,*) 'Interception points of reference track in det-frame: '
   write(output_unit,*) 'detector     x (m)               y (m)             z (m)'
   do i=1,nbr_detectors
     write(output_unit,'(A10,A2)',advance='no')detector_name(i),': '
     if (det_passed(i)) then 
       write(output_unit,*) real(detector_track_interactions_det_frame(i,:))
     else
       write(output_unit,*) 'no hit'
     end if
   end do
   print*
   write(output_unit,*) 'Interception points of reference track in lab-frame: '
   write(output_unit,*) 'detector     x (m)               y (m)             z (m)'
   do i=1,nbr_detectors
     write(output_unit,'(A10,A2)',advance='no')detector_name(i),': '
     if (det_passed(i)) then 
       write(output_unit,*) real(detector_track_interactions_lab_frame(i,:))
     else
       write(output_unit,*) 'no hit'
     end if
   end do
   print*
   if (calibration_mode) then
     write(output_unit,*) 'Interception points of reference track in lab-frame +  ', derivative_step
     write(output_unit,*) 'detector     x (m)               y (m)             z (m)'
     do i=1,nbr_detectors
       write(output_unit,'(A10,A2)',advance='no')detector_name(i),': '
       if (det_passed(i)) then 
         write(output_unit,*) real(detector_track_interactions_lab_frame_p(i,:))
       else
         write(output_unit,*) 'no hit'
       end if
     end do
     print*
     write(output_unit,*) 'Interception points of reference track in lab-frame -  ', derivative_step
     write(output_unit,*) 'detector     x (m)               y (m)             z (m)'
     do i=1,nbr_detectors
       write(output_unit,'(A10,A2)',advance='no')detector_name(i),': '
       if (det_passed(i)) then 
         write(output_unit,*) real(detector_track_interactions_lab_frame_m(i,:))
       else
         write(output_unit,*) 'no hit'
       end if
     end do
     print*
   end if  
   write(output_unit,*) 'Interception time and flightpath of reference track: '
   write(output_unit,*) 'detector  :   t (s)    ,   flightpath (m)'
   detector_track_interactions_path = detector_track_interactions_time*dsqrt(dot_product(v,v))
   do i=1,nbr_detectors
     write(output_unit,'(A10,A2)',advance='no')detector_name(i),': '
     if (det_passed(i)) then 
       write(output_unit,*) real(detector_track_interactions_time(i)) , real(detector_track_interactions_path(i))
     else
       write(output_unit,*) 'no hit'
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
 call get_velocity(x_local(4:6))            ! momentum -> velocity
 derivative_b_field(1:3) = v                 ! (x',y',z')
!
 call  get_b_field(x_local(1:3))
!
 derivative_b_field(4) = q*(v(2)*b_field(3) - v(3)*b_field(2))               ! px' , Lorentz-Force
 derivative_b_field(5) = q*(v(3)*b_field(1) - v(1)*b_field(3))               ! py' 
 derivative_b_field(6) = q*(v(1)*b_field(2) - v(2)*b_field(1))               ! pz' 
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
 double precision     :: d_l(3), d_h(3), f(8)
 integer h_l(3),h_h(3)
 logical inside_local
!
 b_field = 0.
   xyz = b_field_resolution_factor*(xyz_local+dx_b_calibration)
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

subroutine make_double_track_pic
 use vars
 implicit none
 character*200 sh_command
!
 sh_command = 'gle -d pdf ' // trim(double_track_debug_gle_file) // '  >/dev/null'
 sh_command = 'gle -d pdf ' // trim(double_track_debug_gle_file) 
 call execute_command_line (sh_command)
!
end 

subroutine make_gle_positions(variable)
 use vars
 implicit none
 integer uc, k, det_id, koordinate,slash_position, length
 logical backwards
 character (LEN=256) local_output_file(3), shell_command
 character (LEN=length_detector_name)  :: ch10
 CHARACTER(LEN=*), INTENT(IN) :: variable
 character*7 param(2)
 real off_set(2), frame_size(2), graph_size(2), axis_range(2,2),xyz
!
 uc=10
 backwards=.true.
!
 frame_size     = 20.     ! frame size (20 ; 20)
 off_set         = 3.    ! graph-offset of drawing inside frame (3 ; 3) 
 graph_size     = 15.    ! graph size (15 ; 15)    
!
 do det_id = 1,nbr_detectors                ! tofd
   write(ch10,'(I3.3)') det_id
   do koordinate=1,1            ! x-axis        
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
     axis_range(1,:) = detector_position_spectra_paramter(det_id,koordinate,3:4)    ! xmin, xmax on x-axis
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
 integer uc, i, slash_position,length
 real off_set(2), frame_size(2), graph_size(2), axis_range(2,2), x_target(2), x_magnet(2), x_lab(2), x_det(2)
 real x_vec(2)
 double precision x_b(3), x_l(3), x_d(3)
 character*256 data_link
 logical backwards
!
 uc=10
 frame_size(1)  = 40.     ! frame size (40 ; 20)
 frame_size(2)  = 20.     ! frame size (40 ; 20)
 off_set         = 3.    ! graph-offset of drawing inside frame (3 ; 3) 
 graph_size(1)  = 30.    ! graph size (30 ; 15)    
 graph_size(2)  = 15.    ! graph size (30 ; 15)    
 axis_range(1,1) = -1.    ! xmin on x-axis
 axis_range(1,2) = 9.    ! xmax on x-axis
 axis_range(2,1) = -3.5    ! ymin on y-axis
 axis_range(2,2) = 1.5    ! ymax on y-axis
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

subroutine make_gle_double_track
 use vars
 implicit none
 integer uc, i
 real frame_size(2), graph_size(2), x_target(2), x_magnet(2), x_lab(2), x_det(2)
 real off_set_x(2),off_set_y(2), axis_range_x(2,2), axis_range_y(2,2)
 real x_vec(2), y_lab(2), y_t(2), y_det(2)
 double precision x_b(3), x_l(3), x_d(3)
!
 uc=10
 frame_size(1)  = 40.     ! frame size (40 ; 40)
 frame_size(2)  = 40.     ! frame size (40 ; 40)
 off_set_x(1)   = 3.    ! graph-offset of x-track-drawing inside frame 
 off_set_x(2)   = 23.    ! graph-offset of x-track-drawing inside frame 
 off_set_y(1)   = 3.    ! graph-offset of y-track-drawing inside frame 
 off_set_y(2)   = 3.    ! graph-offset of y-track-drawing inside frame 
 graph_size(1)  = 30.    ! graph size (30 ; 15)    
 graph_size(2)  = 15.    ! graph size (30 ; 15)    
 axis_range_x(1,1) = -1.    ! xmin on x-axis
 axis_range_x(1,2) = 9.    ! xmax on x-axis
 axis_range_x(2,1) = -3.5    ! ymin on y-axis   , x-track
 axis_range_x(2,2) = 1.5    ! ymax on y-axis   , x-track
 axis_range_y(1,:)  = axis_range_x(1,:)
 axis_range_y(2,1) = -2.5    ! ymin on y-axis   , y-track
 axis_range_y(2,2) = 2.5    ! ymax on y-axis   , y-track
 open(unit=uc, file=trim(double_track_debug_gle_file))
 write(uc, *) 'size ', frame_size
 write(uc, *) 'include "shape.gle" '
 write(uc, *) 'amove ', off_set_x
 write(uc, *) 'begin graph'
 write(uc, *) '  nobox'
 write(uc, *) '  fullsize'
 write(uc, *) '  size ', graph_size
! write(uc, *) '   title "x-track through GLAD... ',nbr_steps(1),' steps  " hei 0.7'
 write(uc, *) '   title "x-track through GLAD... \chi_{x}^{2} = ',real(chi2_double(5)),' " hei 0.7'
 write(uc, *) '   ytitle "x-position (m)" hei 0.6'
 write(uc, *) '   xtitle "z-position (m)" hei 0.6'
 write(uc, *) '   xaxis min ',axis_range_x(1,1), ' max ',axis_range_x(1,2)
 write(uc, *) '   yaxis min ',axis_range_x(2,1), ' max ',axis_range_x(2,2)
 write(uc, *) '   x2axis off'
 write(uc, *) '   y2axis off'
 write(uc, *) '   xlabels hei 0.6'
 write(uc, *) '   ylabels hei 0.6'
 write(uc, *) '   key pos tr hei 0.6'
 write(uc, *) '   data ',trim(track_debug_file_d1),' d3=c3,c1 '
 write(uc, *) '   data ',trim(track_debug_file_d2),' d4=c3,c1 '
 write(uc, *) '   data ',trim(output_file(2)),' d5=c3,c1 '

 write(uc, *) '   d3 line lstyle 1 lwidth 0.01 color red key "current track 1"'
 write(uc, *) '   d4 line lstyle 1 lwidth 0.01 color blue key "current track 2"'
 write(uc, *) '   d5 line lstyle 1 lwidth 0.01 color black  key "reference track" '
 write(uc, *) 'end graph'
!
 write(uc, *) 'amove ', off_set_y
 write(uc, *) 'begin graph'
 write(uc, *) '  nobox'
 write(uc, *) '  fullsize'
 write(uc, *) '  size ', graph_size
! write(uc, *) '   title "y-track through GLAD... ',nbr_steps(1),' steps  " hei 0.7'
 write(uc, *) '   title "y-track through GLAD... \chi_{y}^{2} = ',real(chi2_double(6)),' " hei 0.7'
 write(uc, *) '   ytitle "y-position (m)" hei 0.6'
 write(uc, *) '   xtitle "z-position (m)" hei 0.6'
 write(uc, *) '   xaxis min ',axis_range_y(1,1), ' max ',axis_range_y(1,2)
 write(uc, *) '   yaxis min ',axis_range_y(2,1), ' max ',axis_range_y(2,2)
 write(uc, *) '   x2axis off'
 write(uc, *) '   y2axis off'
 write(uc, *) '   xlabels hei 0.6'
 write(uc, *) '   ylabels hei 0.6'
 write(uc, *) '   key pos tr hei 0.6'
 write(uc, *) '   data ',trim(track_debug_file_d1),' d31=c3,c2 '
 write(uc, *) '   data ',trim(track_debug_file_d2),' d41=c3,c2 '
 write(uc, *) '   data ',trim(output_file(2)),' d51=c3,c2 '

 write(uc, *) '   d31 line lstyle 1 lwidth 0.01 color red key "current track 1"'
 write(uc, *) '   d41 line lstyle 1 lwidth 0.01 color blue key "current track 2"'
 write(uc, *) '   d51 line lstyle 1 lwidth 0.01 color black  key "reference track" '
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
     if (track_hit_pattern1(i)) then
       x_d = track_det_frame1(i,:)
       call get_lab_position(i,x_d, x_l)
!
       x_lab(1) = x_l(3)   ! z-component goes to x-axis
       x_lab(2) = x_l(1)   ! x-component goes to y-axis
       x_det=off_set_x + (x_lab - axis_range_x(:,1))*graph_size/(axis_range_x(:,2)-axis_range_x(:,1))
       write(uc, *) '   amove ', x_det
       write(uc, *) '   set color red '
       write(uc, *) '   circle ', graph_size(1)/200, ' fill red' 
!
       y_lab(1) = x_l(3)   ! z-component goes to x-axis
       y_lab(2) = x_l(2)   ! y-component goes to y-axis
       y_det=off_set_y + (y_lab - axis_range_y(:,1))*graph_size/(axis_range_y(:,2)-axis_range_y(:,1))
       write(uc, *) '   amove ', y_det
       write(uc, *) '   set color red '
       write(uc, *) '   circle ', graph_size(1)/200, ' fill red' 
     end if  
     if (track_hit_pattern2(i)) then
       x_d = track_det_frame2(i,:)
       call get_lab_position(i,x_d, x_l)
!
       x_lab(1) = x_l(3)   ! z-component goes to x-axis
       x_lab(2) = x_l(1)   ! x-component goes to y-axis
       x_det=off_set_x + (x_lab - axis_range_x(:,1))*graph_size/(axis_range_x(:,2)-axis_range_x(:,1))
       write(uc, *) '   amove ', x_det
       write(uc, *) '   set color blue '
       write(uc, *) '   circle ', graph_size(1)/200, ' fill blue' 
!
       y_lab(1) = x_l(3)   ! z-component goes to x-axis
       y_lab(2) = x_l(2)   ! y-component goes to y-axis
       y_det=off_set_y + (y_lab - axis_range_y(:,1))*graph_size/(axis_range_y(:,2)-axis_range_y(:,1))
       write(uc, *) '   amove ', y_det
       write(uc, *) '   set color blue '
       write(uc, *) '   circle ', graph_size(1)/200, ' fill blue' 
     end if  
   end do

 close(uc)
end

subroutine make_gle_single_track
 use vars
 implicit none
 integer uc, i
 real frame_size(2), graph_size(2), x_target(2), x_magnet(2), x_lab(2), x_det(2)
 real off_set_x(2),off_set_y(2), axis_range_x(2,2), axis_range_y(2,2)
 real x_vec(2), y_lab(2), y_t(2), y_det(2)
 double precision x_b(3), x_l(3), x_d(3)
!
 uc=10
 frame_size(1)  = 40.     ! frame size (40 ; 40)
 frame_size(2)  = 40.     ! frame size (40 ; 40)
 off_set_x(1)   = 3.    ! graph-offset of x-track-drawing inside frame 
 off_set_x(2)   = 23.    ! graph-offset of x-track-drawing inside frame 
 off_set_y(1)   = 3.    ! graph-offset of y-track-drawing inside frame 
 off_set_y(2)   = 3.    ! graph-offset of y-track-drawing inside frame 
 graph_size(1)  = 30.    ! graph size (30 ; 15)    
 graph_size(2)  = 15.    ! graph size (30 ; 15)    
 axis_range_x(1,1) = -1.    ! xmin on x-axis
 axis_range_x(1,2) = 9.    ! xmax on x-axis
 axis_range_x(2,1) = -3.5    ! ymin on y-axis   , x-track
 axis_range_x(2,2) = 1.5    ! ymax on y-axis   , x-track
 axis_range_y(1,:)  = axis_range_x(1,:)
 axis_range_y(2,1) = -2.5    ! ymin on y-axis   , y-track
 axis_range_y(2,2) = 2.5    ! ymax on y-axis   , y-track
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
! convertes vector in detector-coordinates into vector in lab-coordinates
!
subroutine get_lab_vector(volume_id,v_det, v_lab )
 use vars
 implicit none
 double precision   :: v_det(3), v_lab(3) 
 integer volume_id
! 
  v_lab(1) = v_det(1) * dcos(detector_position_in_lab(volume_id,4)) - v_det(3) * dsin(detector_position_in_lab(volume_id,4))
  v_lab(2) = v_det(2)
  v_lab(3) = v_det(3) * dcos(detector_position_in_lab(volume_id,4)) + v_det(1) * dsin(detector_position_in_lab(volume_id,4))
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
 integer volume_id, i
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
 double precision b(3)
 double precision, allocatable :: b_field_map_orig(:,:,:,:)            ! map of magnetic field... (x,y,z,1:3) , last entry for Bx,By,Bz
 double precision range_b_field(3,2), d_l(3), d_h(3), f(8)
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
   if (.not. sweep_calibration) write(output_unit,*) 'Range b-field for y==0',  range_b_field(ii,1)  , range_b_field(ii,2) 
 end do
 
 ch_l = b_field_resolution_factor*b_field_borders(:,1)
 ch_h = b_field_resolution_factor*b_field_borders(:,2)
 ch   = ch_h - ch_l +1
 allocate(b_field_map(ch_l(1):ch_h(1),ch_l(2):ch_h(2),ch_l(3):ch_h(3),3 ) )
 allocate(b_field_map_1amp(ch_l(1):ch_h(1),ch_l(2):ch_h(2),ch_l(3):ch_h(3),3 ) )
 
 
 b_field_map = 0.
 phi =detector_position_in_lab(0,4)
 if (.not. sweep_calibration) write(output_unit,*) 'phi ',phi
 do i=ch_l(1),ch_h(1)
  do j=ch_l(2),ch_h(2)
   do k=ch_l(3),ch_h(3)
! do i=0,0
!  do j=-6,-6
!   do k=ch_l(3),ch_h(3)
    x_lab(1) = dble(i) / dble(b_field_resolution_factor)                                
    x_lab(2) = dble(j) / dble(b_field_resolution_factor)
    x_lab(3) = dble(k) / dble(b_field_resolution_factor)
    x_t      = x_lab - detector_position_in_lab(0,1:3)                            ! transition
    x_b(1)   = x_t(1)*dcos(phi) + x_t(3)*dsin(phi)                            ! rotation around y-axis
    x_b(2)   = x_t(2)                                                        ! rotation around y-axis
    x_b(3)   = x_t(3)*dcos(phi) - x_t(1)*dsin(phi)                            ! rotation around y-axis
!    
    xyz = b_field_resolution_factor*x_b                                        ! now we are in the frame of the glad-magnet
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
      if (i==0 .and.j==0 .and. k==2 .and. .not. sweep_calibration ) &
        write(output_unit,*) 'b-field-orig: ',b_field_map_orig(i,j,k,:)  
      
      b       = f(1)*b_field_map_orig(h_l(1),h_l(2),h_l(3),:)
      b   = b + f(2)*b_field_map_orig(h_l(1),h_l(2),h_h(3),:)
      b   = b + f(3)*b_field_map_orig(h_l(1),h_h(2),h_l(3),:)
      b   = b + f(4)*b_field_map_orig(h_h(1),h_l(2),h_l(3),:)
      b   = b + f(5)*b_field_map_orig(h_h(1),h_h(2),h_l(3),:)
      b   = b + f(6)*b_field_map_orig(h_h(1),h_l(2),h_h(3),:)
      b   = b + f(7)*b_field_map_orig(h_l(1),h_h(2),h_h(3),:)
      b   = b + f(8)*b_field_map_orig(h_h(1),h_h(2),h_h(3),:)
!
      if (i==0 .and.j==0 .and. k==2 .and. .not. sweep_calibration) &
       write(output_unit,*) 'b-field-interpol: ',b  
 
!
      b_field_map(i,j,k,1) = b(1)*dcos(phi) - b(3)*dsin(phi)                    ! rotation around y-axis, now backwards
      b_field_map(i,j,k,2) = b(2)                                                ! rotation around y-axis, now backwards
      b_field_map(i,j,k,3) = b(3)*dcos(phi) + b(1)*dsin(phi)                    ! rotation around y-axis, now backwards
      if (i==0 .and.j==0 .and. k==2 .and. .not. sweep_calibration) &
       write(output_unit,*) 'b-field-rotate: ',b_field_map(i,j,k,:)  
!
     end if
    end if
    if (i == ch_l(1) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         write(output_unit,*) 'problem with b-field area, decrease x_min', i,j,k,b_field_map(i,j,k,:)
    if (i == ch_h(1) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         write(output_unit,*) 'problem with b-field area, increase x_max', i,j,k,b_field_map(i,j,k,:)
    if (j == ch_l(2) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         write(output_unit,*) 'problem with b-field area, decrease y_min', i,j,k,b_field_map(i,j,k,:)
    if (j == ch_h(2) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         write(output_unit,*) 'problem with b-field area, increase y_max', i,j,k,b_field_map(i,j,k,:)
    if (k == ch_l(3) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         write(output_unit,*) 'problem with b-field area, decrease z_min', i,j,k,b_field_map(i,j,k,:)
    if (k == ch_h(3) .and. dot_product(b_field_map(i,j,k,:),b_field_map(i,j,k,:)) >0.) &
         write(output_unit,*) 'problem with b-field area, increase z_max', i,j,k,b_field_map(i,j,k,:)
   end do
  end do 
 end do
!
 b_field_map_limits(:,1) = ch_l
 b_field_map_limits(:,2) = ch_h
 b_field_map_1amp = b_field_map / glad_current_nominal_current
 b_field_map      = b_field_map_1amp * glad_current
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
    double precision SIGMA,ENER,RA,Y,GAU,sig2,maxi, HALB
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
!
! these functions will be used in linear combinations to describe the y-dependence on the angle theta_xz in all detectors
!
double precision function basic_functions_y_correction(n,x,y)
 implicit none
 integer n
 double precision x,y
 double precision basic_functions_y_correction_deg5
!
 basic_functions_y_correction = basic_functions_y_correction_deg5(n,x,y)
end 

double precision function basic_functions_y_correction_deg5(n,x,y)
 implicit none
 integer n
 double precision x,y,z
!
 select case(n)
   case(5)
     z = x*x*x*x*x
   case(4)
     z = x*x*x*x
   case(3)
     z = x*x*x
   case(2)
     z = x*x
   case(1)
     z = x
   case(6)
     z = x*x*x*x*y
   case(7)
     z = x*x*x*y
   case(8)
     z = x*x*y
   case(9)
     z = x*y
   case(10)
     z = y
   case(11)
     z = x*x*x*y*y
   case(12)
     z = x*x*y*y
   case(13)
     z = x*y*y
   case(14)
     z = y*y
   case(15)
     z = x*x*y*y*y
   case(16)
     z = x*y*y*y
   case(17)
     z = y*y*y
   case(18)
     z = x*y*y*y*y
   case(19)
     z = y*y*y*y
   case(20)
     z = y*y*y*y*y
   case default 
     z=0.
 end select
 basic_functions_y_correction_deg5 = z
end


double precision function basic_functions_y_correction_deg3(n,x,y)
 implicit none
 integer n
 double precision x,y,z, y_local
!
 if (y>=0.) then
   y_local = y
 else
   y_local = -y
 end if    
 select case(n)
   case(1)
     z = x*x*x
   case(2)
     z = x*x
   case(3)
     z = x
   case(4)
     z = x*x*y_local
   case(5)
     z = x*y_local
   case(6)
     z = y_local
   case(7)
     z = x*y_local*y_local
   case(8)
     z = y_local*y_local
   case(9)
     z = y_local*y_local*y_local
   case default 
     z=0.
 end select
 if (y>=0.) then
   basic_functions_y_correction_deg3 = z
 else
   basic_functions_y_correction_deg3 = -z
 end if    
 
end


subroutine rotate_vector_x(v_in,theta,v_out)
 implicit none
 double precision v_in(3),theta,v_out(3)
!
  v_out(1) = v_in(1)*dcos(theta) + v_in(3)*dsin(theta)
  v_out(2) = v_in(2)
  v_out(3) = v_in(3)*dcos(theta) - v_in(1)*dsin(theta)
end 

subroutine rotate_vector_y(v_in,theta,v_out)
 implicit none
 double precision v_in(3),theta,v_out(3)
!
  v_out(1) = v_in(1)
  v_out(2) = v_in(2)*dcos(theta) + v_in(3)*dsin(theta)
  v_out(3) = v_in(3)*dcos(theta) - v_in(2)*dsin(theta)
end 

!
! some routines for trigger logic
!

subroutine replace_det_name_in_logic(d,tf,ch_in)
 use vars
 implicit none 
 integer d, pos_name, l_replace
 character (LEN=length_ch_trigger_logic) :: ch_before, ch_behind, ch_in
 logical tf
!
 pos_name = index(ch_in,trim(detector_name(d)) )
 do while (pos_name > 0)
   if (pos_name > 1) then
     ch_before = ch_in(1:pos_name-1)
   else
     ch_before = ''
   end if    
   l_replace = len_trim(detector_name(d))
   ch_behind = ch_in(pos_name+l_replace:length_ch_trigger_logic)
   if (tf) then
     ch_in = trim(ch_before) // trim(ch_true) // trim(ch_behind)
   else
     ch_in = trim(ch_before) // trim(ch_false) // trim(ch_behind)
   end if  
   pos_name = index(ch_in,trim(detector_name(d)) )
 end do
end

recursive subroutine reduce_logic_string(in_string, reduced_string)
 use vars
 implicit none
 character (LEN=length_ch_trigger_logic) :: in_string,a_string, reduced_string
 character (LEN=length_ch_trigger_logic) :: b_string,c_string
 character (LEN=length_ch_tf) :: l_string,r_string
 integer pos_l, pos_r
 logical, parameter :: backwards=.true.
!
!  reduce parenthesis
! 
 a_string = in_string
 pos_l = index(a_string,'(',backwards)
 do while (pos_l > 0)
   pos_r = index(a_string(pos_l+1:length_ch_trigger_logic),')')+pos_l
   b_string = a_string(pos_l+1:pos_r-1)
   call reduce_logic_string(b_string, c_string)
   a_string = a_string(1:pos_l-1) // trim(c_string) //trim(a_string(pos_r+1:length_ch_trigger_logic) )
   pos_l = index(a_string,'(',backwards)
 end do
!
!  reduce .NOT.
! 
 pos_l = index(a_string,'.NOT.')
 do while (pos_l > 0)
   pos_r=pos_l+5
   r_string = a_string(pos_r:pos_r+length_ch_tf) 
   if (r_string .eq. ch_true) then
     a_string = a_string(1:pos_l-1) // ch_false // trim(a_string(pos_r+length_ch_tf:length_ch_trigger_logic) )
   else 
     a_string = a_string(1:pos_l-1) // ch_true // trim(a_string(pos_r+length_ch_tf:length_ch_trigger_logic) )
   end if  
   pos_l = index(a_string,'.NOT.')
 end do
!
!  reduce .AND.
! 
 pos_l = index(a_string,'.AND.')
 do while (pos_l > 0)
   pos_r=pos_l+5
   pos_l=pos_l-length_ch_tf
   r_string = a_string(pos_r:pos_r+length_ch_tf) 
   l_string = a_string(pos_l:pos_l-1+length_ch_tf) 
   if ((r_string .eq. ch_true) .and. (l_string .eq. ch_true)) then
     a_string = a_string(1:pos_l-1) // ch_true // trim(a_string(pos_r+length_ch_tf:length_ch_trigger_logic) )
   else 
     a_string = a_string(1:pos_l-1) // ch_false // trim(a_string(pos_r+length_ch_tf:length_ch_trigger_logic) )
   end if  
    pos_l = index(a_string,'.AND.')
 end do
!
!  reduce .OR.
! 
 pos_l = index(a_string,'.OR.')
 do while (pos_l > 0)
   pos_r=pos_l+4
   pos_l=pos_l-length_ch_tf
   r_string = a_string(pos_r:pos_r+length_ch_tf) 
   l_string = a_string(pos_l:pos_l-1+length_ch_tf) 
   if ((r_string .eq. ch_false) .and. (l_string .eq. ch_false)) then
     a_string = a_string(1:pos_l-1) // ch_false // trim(a_string(pos_r+length_ch_tf:length_ch_trigger_logic) )
   else 
     a_string = a_string(1:pos_l-1) // ch_true // trim(a_string(pos_r+length_ch_tf:length_ch_trigger_logic) )
   end if  
    pos_l = index(a_string,'.OR.')
 end do
!
!  reduce .EQV.
! 
 pos_l = index(a_string,'.EQV.')
 do while (pos_l > 0)
   pos_r=pos_l+5
   pos_l=pos_l-length_ch_tf
   r_string = a_string(pos_r:pos_r+length_ch_tf) 
   l_string = a_string(pos_l:pos_l-1+length_ch_tf) 
   if (r_string .eq. l_string ) then
     a_string = a_string(1:pos_l-1) // ch_true // trim(a_string(pos_r+length_ch_tf:length_ch_trigger_logic) )
   else 
     a_string = a_string(1:pos_l-1) // ch_false // trim(a_string(pos_r+length_ch_tf:length_ch_trigger_logic) )
   end if  
    pos_l = index(a_string,'.EQV.')
 end do
!
!  reduce .NEQV.
! 
 pos_l = index(a_string,'.NEQV.')
 do while (pos_l > 0)
   pos_r=pos_l+6
   pos_l=pos_l-length_ch_tf
   r_string = a_string(pos_r:pos_r+length_ch_tf) 
   l_string = a_string(pos_l:pos_l-1+length_ch_tf) 
   if (r_string .eq. l_string ) then
     a_string = a_string(1:pos_l-1) // ch_false // trim(a_string(pos_r+length_ch_tf:length_ch_trigger_logic) )
   else 
     a_string = a_string(1:pos_l-1) // ch_true // trim(a_string(pos_r+length_ch_tf:length_ch_trigger_logic) )
   end if  
    pos_l = index(a_string,'.NEQV.')
 end do

 reduced_string = a_string
end 
 

subroutine initialize_trigger_logic
 use vars
 implicit none
 integer d, pos_in_string,pattern_id, d1, d2
 logical backwards, hit_pattern_local(nbr_detectors), trig_true
 character (LEN=length_ch_trigger_logic) :: reduced_string, ch_trigger_logic_local
!
 backwards = .true.
 hit_pattern_local = .true.
! slash_position = index(output_file(1),'/',backwards)
!
 write(output_unit,*) 'Entered trigger logic init'
 write(output_unit,*) trim(ch_trigger_logic)

 do pattern_id=1,2**nbr_detectors-1
   ch_trigger_logic_local = ch_trigger_logic
   call get_hit_pattern(nbr_detectors,pattern_id,hit_pattern_local)
!
! replace detector names with TRUE or FALSE characters
!
   do d=1,nbr_detectors
     call replace_det_name_in_logic(d,hit_pattern_local(d),ch_trigger_logic_local)
   end do
!
! remove spaces
!
   pos_in_string = index(trim(ch_trigger_logic_local),' ')
   do while (pos_in_string>0)
     ch_trigger_logic_local=trim(ch_trigger_logic_local(1:pos_in_string-1)) // &
                trim(ch_trigger_logic_local(pos_in_string+1:length_ch_trigger_logic))
     pos_in_string = index(trim(ch_trigger_logic_local),' ')
   end do
!
! reduce to a single TRUE / FALSE character
!
   call reduce_logic_string(ch_trigger_logic_local, reduced_string)
!
! 
! 
   if ((trim(reduced_string).eq.ch_true).or.(trim(reduced_string).eq.ch_false) ) then
     trig_true = (trim(reduced_string) .eq. ch_true) 
   else
     write(output_unit,*) '!!something wrong with trigger logic!! ',  trim(reduced_string)
     stop
   end if   
   trigger_matrix(pattern_id) = trig_true
 end do
!
! no trigger_matrix is done. Will now check for some cross correlations
! 
 trigger_always_together = .true.
 trigger_never_together  = .true.
 do pattern_id=1,2**nbr_detectors-1
   ch_trigger_logic_local = ch_trigger_logic
   call get_hit_pattern(nbr_detectors,pattern_id,hit_pattern_local)
   do d1 = 1, nbr_detectors
     do d2 = 1, nbr_detectors
       if (trigger_matrix(pattern_id) .and. d1 /= d2) then
         trig_true = hit_pattern_local(d1) .eqv. hit_pattern_local(d2)
         trigger_always_together(d1,d2) = trigger_always_together(d1,d2).and.trig_true
         trig_true = .not.(hit_pattern_local(d1) .and. hit_pattern_local(d2))
         trigger_never_together(d1,d2)  = trigger_never_together(d1,d2) .and.trig_true
       end if
     end do
   end do
 end do  
 print*
 print*,'Always together: '
 if (.not.screen_output) write(output_unit,*) 'Always together: '
 print*
 if (.not.screen_output) write(output_unit,*)
 do d1 = 1, nbr_detectors
   write(*,'(A10,A3)', advance='no') detector_name(d1),' : '
   if (.not.screen_output) write(output_unit,'(A10,A3)', advance='no') detector_name(d1),' : '
   do d2 = 1, nbr_detectors
     if (trigger_always_together(d1,d2) .and. d1 /= d2) then
       write(*,'(A10,A1)', advance='no') detector_name(d2),' '  
       if (.not.screen_output) write(output_unit,'(A10,A1)', advance='no') detector_name(d2),' '  
     end if
   end do
   write(*,*)
   if (.not.screen_output) write(output_unit,*)
 end do
 write(*,*)
 if (.not.screen_output) write(output_unit,*)
 print*,'Never together: '
 if (.not.screen_output) write(output_unit,*) 'Never together: '
 write(*,*)
 do d1 = 1, nbr_detectors
   write(*,'(A10,A3)', advance='no') detector_name(d1),' : '
   if (.not.screen_output) write(output_unit,'(A10,A3)', advance='no') detector_name(d1),' : '
   do d2 = 1, nbr_detectors
     if (trigger_never_together(d1,d2).and. d1 /= d2) then
       write(*,'(A10,A1)', advance='no') detector_name(d2),' '  
       if (.not.screen_output) write(output_unit,'(A10,A1)', advance='no') detector_name(d2),' '  
     end if
   end do
   write(*,*)
   if (.not.screen_output) write(output_unit,*) 
 end do
 write(*,*)
 if (.not.screen_output) write(output_unit,*)
end

!
! a few routines for geometry calibration
!

subroutine make_sweep_calibration
 use vars
 use calib
 implicit none
!
  m_b = 16.d0 
  m_t = 16.d0 
  print*,'initialize_sweep_calibration called'
  call initialize_sweep_calibration
  print*,'initialize_sweep_calibration done'
  if (calibration_event_by_event) then
!    call make_calib_histos
!    call make_event_sweep_b_field_position_calibration
    momentum_optimization = .true.
!    momentum_optimization = .false.
    if (momentum_optimization) call initialize_scatter_run_matrizes
    print*,'test - was here'
    call read_sweep_events
    call make_event_sweep_detektor_position_calibration
!    call make_calib_histos
  else
    call make_sweep_detector_position_calibration
  !  call make_sweep_b_field_position_calibration
  !  call make_sweep_incoming_beam_calibration
  end if
  call write_new_geometry_file
end


!
!  optimized detector positions according to chi2 deviations from reference curves
!
subroutine make_event_sweep_detektor_position_calibration
 use vars
 use calib
 implicit none
 integer i,j,d,d1
 logical all_detectors_done
 logical dimension_free(nbr_detectors,nbr_dimensions_optimize),detector_free(nbr_detectors)
 double precision qs_0, qs_old, qs_vary, qs_max
 double precision detector_position_in_ref(nbr_detectors,4)
 double precision d_det_pos_lab(3,4), d_det_pos_det(3), d_det_pos_lab_max(4)
 double precision spatial_resolution_local
 double precision detector_position_in_lab_orig(nbr_detectors,4)
 double precision null_vector(3), distance_to_zero(2, nbr_detectors),distance_detector_plane
 integer sum_old, sum_0, sum_vary, sum_max, direction_max
 character*20 output_line
!
 detector_position_in_lab_orig = detector_position_in_lab(1:nbr_detectors,:)
 qs_0             = 1.d10
 sum_0            = 0
 null_vector      = 0.d0
 spatial_resolution_local = spatial_resolution*5.d0
! spatial_resolution_local = spatial_resolution*1.d1
! spatial_resolution_local = spatial_resolution*1.d2
 output_line = ' '
   qs_0             = 1.d10
   sum_0            = 0
!
 detector_free        = .true.
 dimension_free       = .true.
 print*
 do d = 1, nbr_detectors
   do d1 = d+1, nbr_detectors
     if ( all(locked_pair(d,d1,:)) ) detector_free(d1) = .false. 
     do i=1,nbr_dimensions_optimize
        if ( locked_pair(d,d1,i) ) then
          dimension_free(d1,i) = .false.  
          if (.not.optimize_detector(d1) ) dimension_free(d,i) = .false.   ! dimension locked to a non-optimized detector
        end if  
     end do
   end do
 end do  
!
 do d = 1, nbr_detectors
  if (optimize_detector(d) ) then
   do i = 1, nbr_dimensions_optimize
     if ( dimension_free(d,i) )  then
       print*,'Detektor ',detector_name(d), ' free in dimension ',i
     else  
       print*,'Detektor ',detector_name(d), ' locked in dimension ',i
     end if 
   end do   
  end if 
 end do  
 print* 
 do d = 1, nbr_detectors
   if (optimize_detector(d) ) then
     if ( detector_free(d) )  then
       print*,'Detektor ',detector_name(d), ' will be freely optimized'
     else  
       print*,'Detektor ',detector_name(d), ' is locked locked, but optimized'
     end if  
   end if  
 end do  
!
 do d = 1, nbr_detectors
   call get_xy_plane(d)
   distance_to_zero(1,d) = distance_detector_plane(d,null_vector)
 end do  
!
 max_nbr_good_events = max_nbr_events_run+1
!
 if (momentum_optimization) then
   chi_treshold= 1e-19  
 else
   chi_treshold= 5.d1 
 end if
! 
 call calculate_quadratic_deviations(qs_vary,sum_vary)
 do i=1,5
   call calculate_quadratic_deviations(qs_vary,sum_vary)
   print*,sum(nbr_events_in_run(1:nbr_runs) ), qs_vary,sum_vary,chi_treshold
   if (100*sum_vary < sum(nbr_events_in_run(1:nbr_runs) ) ) then
     chi_treshold = 2.d0*chi_treshold
   else
     if (2*sum_vary > sum(nbr_events_in_run(1:nbr_runs) ) ) then
       chi_treshold = chi_treshold/2.d0
     else
       exit
     end if  
   end if
 end do
 print*,'Final chi2-threshold:',sum(nbr_events_in_run(1:nbr_runs) ), qs_vary,sum_vary,chi_treshold
 print*,'Max number good events',max_nbr_good_events
 print*,'(please ackowledge)'
 read(*,*) j
!
 qs_0 = 1.d31
 do j=1,8
   output_line(j:j) = '*' 
   print*
   print*,'Spatial resolution: ',real(spatial_resolution_local)*1e3,' mm'
   !
   spatial_resolution_local = spatial_resolution_local/2.d0
   call calculate_quadratic_deviations(qs_0,sum_0)
   if (qs_0 > 1.d30) sum_0 = 0
   do 
     all_detectors_done = .true.
     write(output_unit,'(A1)',advance='no') ' '
     do d=1,nbr_detectors !!! loop over d 
       if ( optimize_detector(d) .and. detector_free(d) ) then
         qs_old  = qs_0
         sum_old = sum_0
         write(output_unit,'(A1)',advance='no') '.'
         detector_position_in_ref(:,:) = detector_position_in_lab(1:nbr_detectors,:)
!
         d_det_pos_lab = 0.d0
!         
         sum_max = sum_old
         qs_max  = qs_old
         do i=1,3
           if (dimension_free(d,i) ) then                                                           
             d_det_pos_det = 0.d0
             d_det_pos_det(i) = spatial_resolution_local
             call get_lab_vector(d,d_det_pos_det, d_det_pos_lab(i,1:3) )
             detector_position_in_lab(d,:) = detector_position_in_ref(d,:) - d_det_pos_lab(i,:)
             do d1 = d+1,nbr_detectors
               if (locked_pair(d,d1,i)) detector_position_in_lab(d1,:) = detector_position_in_ref(d1,:) - d_det_pos_lab(i,:)
             end do
             call calculate_quadratic_deviations(qs_vary,sum_vary)
!             write(output_unit,*) '   - ',i,detector_name(d), qs_vary, sum_vary
             if (qs_vary > 1.d30) sum_vary = 0
             if (sum_vary>sum_max .or. (sum_vary==sum_max .and. qs_vary<qs_max) ) then
               d_det_pos_lab_max = -d_det_pos_lab(i,:)
               sum_max           = sum_vary
               qs_max            = qs_vary
               direction_max     = i
             end if  
             detector_position_in_lab(d,:) = detector_position_in_ref(d,:) + d_det_pos_lab(i,:)
             do d1 = d+1,nbr_detectors
               if (locked_pair(d,d1,i)) detector_position_in_lab(d1,:) = detector_position_in_ref(d1,:) + d_det_pos_lab(i,:)
             end do
             call calculate_quadratic_deviations(qs_vary,sum_vary)
!             write(output_unit,*) '   + ',i,detector_name(d), qs_vary, sum_vary
             if (qs_vary > 1.d30) sum_vary = 0
             if (sum_vary>sum_max .or. (sum_vary==sum_max .and.qs_vary<qs_max)) then
               d_det_pos_lab_max = d_det_pos_lab(i,:)
               sum_max           = sum_vary
               qs_max            = qs_vary
               direction_max     = i
             end if  
           end if
         end do
!
         detector_position_in_lab(1:nbr_detectors,:) = detector_position_in_ref(:,:)
         if (sum_max > sum_0 .or. (sum_0==sum_max .and. qs_0>qs_max)) then
           detector_position_in_lab(d,1:3) = detector_position_in_lab(d,1:3) + d_det_pos_lab_max(1:3)
           do d1 = d+1,nbr_detectors
             if (locked_pair(d,d1,direction_max)) then
               detector_position_in_lab(d1,1:3) = detector_position_in_lab(d1,1:3) + d_det_pos_lab_max(1:3)
             end if  
           end do
!
           qs_0  = qs_max
           sum_0 = sum_max
           all_detectors_done = .false.
         end if
       end if      
     end do
     if (all_detectors_done) exit
   end do
   write(output_unit,*)
   write(output_unit,*) output_line, qs_0, sum_0
 end do   
 print*
 print*
 do d=1,nbr_detectors !!! loop over d 
   if ( optimize_detector(d) )then
!     write(output_unit,*) 
!     write(output_unit,*) '***************************************************************************'
!     write(output_unit,*) 'recommended x,y,z position for ',detector_name(d),' (m): ', real(detector_position_in_lab(d,1:3))
!     write(output_unit,*) '        old x,y,z position for ',detector_name(d),' (m): ', real(detector_position_in_lab_orig(d,1:3))
     write(output_unit,*) '      shift x,y,z position for ',detector_name(d),' (m): ', &
                         real(detector_position_in_lab(d,1:3)-detector_position_in_lab_orig(d,1:3))
   end if                      
 end do
 print*
 do d = 1, nbr_detectors
   call get_xy_plane(d)
   distance_to_zero(2,d) = distance_detector_plane(d,null_vector)
 !  print*,detector_name(d), distance_to_zero(1,d),distance_to_zero(2,d),distance_to_zero(1,d)-distance_to_zero(2,d)
 end do  

end

subroutine make_event_sweep_b_field_position_calibration
 use vars
 use calib
 implicit none
 integer i,j
 logical break_out
 double precision qs_0, qs_old, norm, qs_vary
 double precision dx_b_0(3),d_qs(3),dx_local(3)
 double precision d_hits(3), d_hits_m(3), d_hits_p(3)
 double precision spatial_resolution_local
 integer sum_old, sum_0, sum_vary
 character*100 output_line
!
 dx_b_calibration = 0.d0
 spatial_resolution_local = spatial_resolution*1.d1
 output_line = 'dx_b_calibration '
 do j=1,10
   qs_0             = 1.d10
   sum_0            = 0
   spatial_resolution_local = spatial_resolution_local/2.d0
   output_line = trim(output_line) // '* ' 
   print*,'*************************************************************************'
   print*,'Spatial resolution: ',real(spatial_resolution_local)*1e3,' mm'
   do  
     qs_old  = qs_0
     sum_old = sum_0
     call calculate_quadratic_deviations(qs_0,sum_0)
     write(output_unit,'(A1)',advance='no') '.'
     dx_b_0   = dx_b_calibration
     do i=1,3
       dx_b_calibration    = dx_b_0
       dx_b_calibration(i) = dx_b_0(i) - spatial_resolution_local
       call calculate_quadratic_deviations(qs_vary,sum_vary)
       d_hits_m(i) = - (sum_vary - sum_0)
       d_qs(i)   = -(qs_vary - qs_0)
!       write(output_unit,*) sum_vary,  d_hits_m(i)
     end do
     do i=1,3
       dx_b_calibration    = dx_b_0
       dx_b_calibration(i) = dx_b_0(i) + spatial_resolution_local
       call calculate_quadratic_deviations(qs_vary,sum_vary)
       d_hits_p(i) = (sum_vary - sum_0) 
       d_qs(i) = qs_vary - qs_0 + d_qs(i)
!       write(output_unit,*) sum_vary,  d_hits_p(i)
     end do
     dx_b_calibration = dx_b_0
     break_out = (d_hits_m(1)*d_hits_p(1) < 0.d0)
     break_out = (d_hits_m(2)*d_hits_p(2) < 0.d0) .and. break_out
     break_out = (d_hits_m(3)*d_hits_p(3) < 0.d0) .and. break_out
     if (sum_old >= sum_0) then
       break_out = .true.
       dx_b_calibration    = dx_b_0 - dx_local
     end if  
     if (break_out) then
       print*,'break_out',break_out
       exit
     end if    
     d_qs   = d_qs / 2.d0
     norm     = sqrt(dot_product(d_qs,d_qs))
     dx_local = -d_qs/norm*spatial_resolution_local/2.
    !
     d_hits = (d_hits_m + d_hits_p) / 2.d0
     norm     = sqrt(dot_product(d_hits,d_hits))
     dx_local = d_hits/norm*spatial_resolution_local/2.
    !  dx_local = -d_qs*spatial_resolution_local/2.
     dx_b_calibration = dx_b_calibration + dx_local
    !  norm     = dsqrt(dot_product(dx_local,dx_local))
   !  if (dsqrt(norm) .lt. 2d-3) exit
   end do
   write(output_unit,*)
   write(output_unit,*) trim(output_line) , real(qs_0), sum_0
 end do   
 write(output_unit,*) 'dx_b_calibration**: ', real(dx_b_calibration),real(qs_0)
 print*
 print*
 print*
 write(output_unit,*) 'dx_b_calibration: ', real(dx_b_calibration)  
 write(output_unit,*) 'recommended x,y,z position for B-Field (m): ', real(b_field_position_in_lab(1:3)-dx_b_calibration)

 b_field_map = b_field_map_1amp * glad_current

end

subroutine store_det_pos(d,k,y)
 use vars
 implicit none
 double precision  y
 integer d,k, channel   ! d... detector, run-number ... coordinate
!
   channel = (y-detector_range(d,1,1))/detector_range(d,1,3)*nbr_channels_position_spectra + 1
   if (channel >0 .and. channel <= nbr_channels_position_spectra) then
     detector_position_spectra(d,k,channel) = detector_position_spectra(d,k,channel) + 1
   end if  
end


subroutine make_calib_histos
 use vars
 use calib
 double precision qs_event_by_event, ave(2), sigma(2), delta(2)
 integer  sum_hits,d,run, sum_hits_histo(nbr_detectors,nbr_runs)
!
 detector_position_spectra(:,nbr_runs+1:2*nbr_runs,:) = 0
 write_to_calib_histos = .true.
 call calculate_quadratic_deviations(qs_event_by_event,sum_hits)
 write_to_calib_histos = .false.
 call get_detector_position_spectra_parameter
 do d=1, nbr_detectors
   do run=1, max_dim_pos_spectra
     sum_hits_histo(d,run) = sum(detector_position_spectra(d,run,:))
   end do
 end do   
 do d=1, nbr_detectors
  print*,d, detector_name(d)
  do run=1, max_dim_pos_spectra/2
   if (10*sum_hits_histo(d,run)> maxval(sum_hits_histo(d,:)) ) then
     ave(1)   = detector_position_spectra_paramter(d,run,1)
     ave(2)   = detector_position_spectra_paramter(d,run+nbr_runs,1)
     sigma(1) = detector_position_spectra_paramter(d,run,2)
     sigma(2) = detector_position_spectra_paramter(d,run+nbr_runs,2)
     delta    = ave - track_ref_det_frame(run,d,1)
     print*,b_field_sweep(run), sum_hits_histo(d,run),ave(1), track_ref_det_frame(run,d,1),delta(1), sigma(1)
     print*,b_field_sweep(run), sum_hits_histo(d,run+nbr_runs),ave(2),track_ref_det_frame(run,d,1), delta(2), sigma(2)
   end if 
  end do  
  print*
 end do
end 

subroutine initialize_scatter_run_matrizes
 use vars
 use calib
 implicit none
!
 call get_derivatives
 call initialize_matrizes
 call initialize_y_corrections     
 print*,'initialize_scatter_run_matrizes done'

end 

subroutine calculate_quadratic_deviations(qs_event_by_event,sum_hits)
 use vars
 use calib
 implicit none
 logical all_done, skip_combination, skip, track_ok, new_combination, track_hit_pattern_f(nbr_detectors)
 integer run,hit, event, d, new_hit,det_pattern_id, pattern_id, d1, d2
 integer n_det_track,n_det_track_old
 integer nbr_hits_per_detector(nbr_detectors),track_hit_numbers(nbr_detectors)
 double precision qs_event_by_event, dpos
 double precision x_local(3), x_local_det(3), x_local_lab(3)
 double precision track_points_det_frame(nbr_detectors,max_nbr_hits_per_event,3)
 double precision track_points_lab_frame(nbr_detectors,max_nbr_hits_per_event,3) 
 double precision chi_parameter_out(6) ,track_parameter_out(6)
 integer counter_local(3,nbr_runs),sum_hits
 double precision avg_hits, sigma_hits
!
! print*,' entered subroutine calculate_quadratic_deviations'
 qs_event_by_event = 0.d0
 next_event        = 1
 counter_local     = 0
!
 do run=1,nbr_runs
   chi_parameter_out = 0.d0
   b_field_map = b_field_map_1amp * b_field_sweep(run)
   call single_track
   if (momentum_optimization) then
!  
     offset_ave_x = detector_track_interactions_det_frame(:,1)
     offset_ave_y = detector_track_interactions_det_frame(:,2)
   end if  
   track_ref_det_frame(run,:,:) = detector_track_interactions_det_frame(:,:)
   do event = 1,nbr_events_in_run(run)   
     !
     ! initialize event (sort in smaller arrays etc)
     !
     n_det_track_old   = 0
     chi_parameter_out(1:2) = 1.d30
     nbr_hits_per_detector  = 0
     current_event(run) = next_event(run)
     next_event(run)    = nbr_hits_in_event(run,event) + current_event(run)
     do hit = current_event(run),next_event(run)-1
       d                        = detector_id(run,hit) + 1
       nbr_hits_per_detector(d) = nbr_hits_per_detector(d) + 1
       x_local(1)               = x_pos(run,hit)
       x_local(2)               = y_pos(run,hit)
       x_local(3)               = 0.d0
       x_local_det = x_local
       call get_lab_position(d,x_local,x_local_lab ) 
       new_hit = nbr_hits_per_detector(d)
       track_points_det_frame(d,new_hit,:) = x_local_det
       track_points_lab_frame(d,new_hit,:) = x_local_lab
     end do  
     !
     ! event is initialized, do a few calculations
     !
     track_hit_numbers  = 0
     all_done           = .false.
     do
       !
       !  get hit-pattern of current track
       !
       do d=1,nbr_detectors
         track_hit_pattern(d) = (track_hit_numbers(d) > 0)        
       end do
       ! 
       !  do some checks on the track
       !
       !  request certain detector combinations
       !
       pattern_id = det_pattern_id(nbr_detectors,track_hit_pattern)
       track_ok   = trigger_matrix(pattern_id) 
       !
       !  do something with the track (e.g. call tracker)
       !   
       n_det_track = 0
       if (track_ok) then
         counter_local(3,run) = counter_local(3,run) +1
         chi2_single = 0.d0
         if (momentum_optimization) then
           do d=1,nbr_detectors
             if (track_hit_pattern(d).and.det_consider(d,1)) n_det_track = n_det_track + 1
           end do
           call get_single_track_parameter_var5
         else
           do d=1,nbr_detectors
             if (track_hit_pattern(d) ) then
!               n_det_track = n_det_track + 1
               dpos = detector_track_interactions_det_frame(d,1)-track_points_det_frame(d,track_hit_numbers(d),1)
               if (det_step_function(d,1) ) then
                 if (dabs(dpos)> 1.d0*sigma_track(d,1)) then
                   n_det_track = 0
                   exit
                 end if
               else
                 if (det_consider(d,1)) then
                   n_det_track = n_det_track + 1
                   chi2_single(1) = chi2_single(1) + dpos**2/sigma_track2(d,1)
                 end if  
               end if
             end if  
           end do
         end if  
         new_combination = (n_det_track > n_det_track_old)
         new_combination = new_combination .or. ((n_det_track == n_det_track_old) .and. chi2_single(1) < chi_parameter_out(1))
         if (new_combination) then
           chi_parameter_out(1:2)   = chi2_single   
           n_det_track_old          = n_det_track
           track_hit_pattern_f      = track_hit_pattern
!
           do d=1,nbr_detectors
             track_det_frame(d,:)   = track_points_det_frame(d,track_hit_numbers(d),:)
           end do  
!
           if (momentum_optimization) then
             track_parameter_out(1:3) = x_track1(1:3)                            ! starting position in lab frame (m)
             track_parameter_out(4:6) = x_track1(4:6)                           ! momentum 
           end if  
         end if  
       end if  
       !
       !  get next track candidate
       !
       do d1=1,nbr_detectors
         skip_combination = .false.
         if (track_hit_numbers(d1) < nbr_hits_per_detector(d1)) then
           if (track_hit_numbers(d1) == 0) then
             do d2=d1+1,nbr_detectors
               skip = (trigger_always_together(d1,d2).and.track_hit_numbers(d2)==0)
               skip = (skip.or.(trigger_never_together(d1,d2).and.track_hit_numbers(d2)>0))
               skip_combination = skip_combination .or. skip
               if (skip_combination) exit
             end do 
           end if
           if (skip_combination) then 
             all_done = (d1 == nbr_detectors)
           else  
             track_hit_numbers(d1) = track_hit_numbers(d1) + 1  
             exit
           end if  
         else
           track_hit_numbers(d1) = 0
           all_done = (d1 == nbr_detectors)
         end if
       end do
       if (all_done) exit
     end do
     if (chi_parameter_out(1) < 1.d30) then                            ! some valid combination found
       counter_local(1,run)   = counter_local(1,run) + 1                ! some good combination found  
       if (momentum_optimization) then
         chi_parameter_out(1) = sum((track_parameter_out(4:6)-x_start(4:6))**2)
 !        if (constrain_target_x) then
 !          chi_parameter_out(1) = chi_parameter_out(1) + (track_parameter_out(1)-x_start(1))**2
 !        end if
 !        if (constrain_target_y) then
 !          chi_parameter_out(1) = chi_parameter_out(1) + (track_parameter_out(2)-x_start(2))**2
 !        end if
       chi_parameter_out(1) = dsqrt(chi_parameter_out(1))
  !     print*,chi_parameter_out(1)
  !     call get_incoming_momentum(x_track1(4:6))

       end if

       if (chi_parameter_out(1) < chi_treshold) then

         if (write_to_calib_histos) then
           do d=1, nbr_detectors
             if (track_hit_pattern_f(d)) then
               call store_det_pos(d,nbr_runs+run,track_det_frame(d,1))
             end if  
           end do
         end if
         chi_parameter_out(3) = chi_parameter_out(3) + chi_parameter_out(1)
         chi_parameter_out(4) = chi_parameter_out(4) + chi_parameter_out(2)
         counter_local(2,run) = counter_local(2,run) + 1    
       end if  
       if (counter_local(1,run) > max_nbr_good_events) exit
     end if
   end do    
   if (counter_local(2,run) == 0) then
     chi_parameter_out(3:4) = 1.d37
 !    print*,'counter_local(2,run) == 0', run
   else  
     chi_parameter_out(3:4) = chi_parameter_out(3:4) / dble(counter_local(2,run))
   end if  
   qs_event_by_event = qs_event_by_event + chi_parameter_out(3)
 end do 

 avg_hits = dble(sum(counter_local(2,:))) / dble(nbr_runs)
 sigma_hits = 0.d0
 do run = 1,nbr_runs
   sigma_hits = sigma_hits + (dble(counter_local(2,run)) - avg_hits)**2  
 end do 
 sigma_hits   = dsqrt(sigma_hits/dble(nbr_runs) )
 qs_event_by_event = qs_event_by_event/dble(nbr_runs) 
! sum_hits          = sum(counter_local(2,:)) - sigma_hits
 sum_hits          = sum(counter_local(2,:)) 
 sum_hits          = sum_hits - 0.5*maxval(counter_local(2,:))   ! count highest value only 0.5
 sum_hits          = sum_hits + minval(counter_local(2,:))       ! count lowest value twice
! sum_hits          = minval(counter_local(2,:)) 
 b_field_map = b_field_map_1amp * glad_current
!print*,'      sum counter ', sum(counter_local(1,:)), sum(counter_local(2,:))
!print*,'          counter 1', counter_local(1,:)
!print*,'          counter 2', counter_local(2,:)
!print*,'          counter 3', counter_local(3,:)
!print*,'qs_event_by_event ', qs_event_by_event
 if (max_nbr_good_events == max_nbr_events_run+1) then 
   max_nbr_good_events = minval(counter_local(1,:))
   print*,'counter 1',counter_local(1,:)
 end if  
end 

subroutine get_incoming_momentum(p3_lab)
 use vars
 use calib
 implicit none
 integer counter_local
 double precision p_i                   ! initial z-momentum in CM-system. since 2 particles, the sum will be zero, hence only one is sufficient
 double precision p_f(3)                ! final momenta in CM-system. since 2 particles, the sum will be zero, hence only one is sufficient
 double precision e1, e2, e3, e4            ! total energies of particles 1 (incoming ion), 2 (resting target atom), 3 (outgoing ion), 4 (recoil target nucleus) in the CM-frame
 double precision e3_lab                    ! total energy of tracked particle in lab frame
 double precision p3_lab(3)              ! momentum of tracked particle in lab-frame
 double precision b_h, b_l, b,g, delta_e
 
!
 
 call get_velocity(x_reference(4:6))
 b_h      = v(3)/c
 b_l      = 0.d0
 counter_local  = 0
!
 p_f(1:2) = p3_lab(1:2)*m_b
 e3_lab = dsqrt(dot_product(p3_lab,p3_lab) * c**2 + (m_b*amu)**2 * c**4  )
! 
 do
   counter_local = counter_local + 1  
   b      = (b_h+b_l) / 2.d0
   g      = 1.d0/dsqrt(1.d0-b*b)
   p_i    = g * b * m_t*amu * c
   p_f(3) = g*p3_lab(3)-g*b*e3_lab/c
   e1  = dsqrt(p_i**2 * c**2 + (m_b*amu)**2 * c**4  )
   e2  = dsqrt(p_i**2 * c**2 + (m_t*amu)**2 * c**4  )
   e3  = dsqrt(dot_product(p_f,p_f) * c**2 + (m_b*amu)**2 * c**4  )
   e4  = dsqrt(dot_product(p_f,p_f) * c**2 + (m_t*amu)**2 * c**4  )
   delta_e = e3+e4-e1-e2
   if (dabs(delta_e/e3) < 1d-5 .or. counter_local > 100) exit
   if (delta_e > 0.d0) then
     b_l = b
   else
     b_h = b
   end if
 end do  
 p_incoming    = 0.d0
 if (counter_local < 100) p_incoming(3) = g*p_i+g*b*e1/c
end

subroutine read_sweep_events
 use vars
 use calib
 implicit none
 integer :: unit_read1=8
 integer :: unit_read2=9
 integer :: eastat, run, nbr_hits, nbr_hits_run, hit
 character*256 event_file
!
 nbr_events_in_run = 0
 current_event     = 0
 nbr_hits_in_event = 0
 next_event        = 0
 detector_position_spectra = 0
 open (unit=unit_read1, file=sweep_calibration_file , status='old')
   do run = 1, nbr_runs
     read(unit_read1,*,iostat=eastat) b_field_sweep(run),event_file
     
     open (unit=unit_read2, file=event_file , status='old')
       read(unit_read2,*,iostat=eastat) nbr_hits
       nbr_hits_run = 0 
       do while (eastat==0 .and. nbr_hits_run+nbr_hits <= max_nbr_hits_per_run .and. nbr_events_in_run(run)<max_nbr_events_run) 
         nbr_events_in_run(run)                   = nbr_events_in_run(run) + 1
         nbr_hits_in_event(run,nbr_events_in_run) = nbr_hits
         do hit =1,nbr_hits
           nbr_hits_run = nbr_hits_run + 1
           read(unit_read2,*) detector_id(run,nbr_hits_run),x_pos(run,nbr_hits_run),y_pos(run,nbr_hits_run)
           event_id(run,nbr_hits_run) = nbr_events_in_run(run)
           call store_det_pos(detector_id(run,nbr_hits_run)+1,run,x_pos(run,nbr_hits_run))
         end do
         read(unit_read2,*,iostat=eastat) nbr_hits
       end do
     close(unit_read2)
     print*,b_field_sweep(run),trim(event_file),'  ', nbr_hits_run, nbr_events_in_run(run)
   end do
 close(unit_read1)
!
 max_nbr_hits_per_event = maxval(nbr_hits_in_event)
end

subroutine initialize_sweep_calibration
 use vars
 use calib
 implicit none
 integer unit_read, nbr_detectors_optimize, d,i, nbr_detector_pairs
 integer :: eastat, pos_and, d1, d2,dimen
 real dummy
 character (LEN=length_detector_name) :: detector_name_local
 character (LEN=length_ch_trigger_logic)  :: ch_lock
!
 unit_read = 8
 nbr_runs  = 0
! 
 allocate ( optimize_detector(nbr_detectors) )
 allocate ( locked_pair(nbr_detectors,nbr_detectors,nbr_dimensions_optimize) )
 optimize_detector            = .false.
 locked_pair                  = .false.
!
 open (unit=unit_read, file=calib_file , status='old')
   read(unit_read,*) calibration_event_by_event
   read(unit_read,*) sweep_data
   read(unit_read,*) sweep_calibration_file 
   if (calibration_event_by_event) then
     read(unit_read,*) max_nbr_events_run
     read(unit_read,*) max_nbr_hits_per_run
     read(unit_read,*) nbr_detectors_optimize
!
     do i=1, nbr_detectors_optimize
       read(unit_read,*) detector_name_local
       do d=1,nbr_detectors
         if (trim(detector_name(d)) == trim(detector_name_local) ) optimize_detector(d) = .true.
       end do
     end do
!
     do dimen=1,nbr_dimensions_optimize
       read(unit_read,*) nbr_detector_pairs
       do i=1, nbr_detector_pairs
         read(unit_read,*) ch_lock
         pos_and = index(ch_lock, '.AND.')
         d1 = 0
         d2 = 0
         read(ch_lock(1:pos_and-1),*) detector_name_local
         do d=1,nbr_detectors
           if (trim(detector_name(d)) == trim(detector_name_local) ) then
             d1 = d
             exit
           end if  
         end do
         if (d1 == 0) then
           print*,'detector not recognized ',detector_name_local
           stop
         end if
         read(ch_lock(pos_and+5:length_ch_trigger_logic),*) detector_name_local
         do d=1,nbr_detectors
           if (trim(detector_name(d)) == trim(detector_name_local) ) then
             d2 = d
             exit
           end if  
         end do
         if (d2 == 0) then
           print*,'detector not recognized ',detector_name_local
           stop
         end if
         if (d1 /= d2) then  ! found a new, valid pair
           locked_pair(d1,d2,dimen) = .true.
           locked_pair(d2,d1,dimen) = .true.
           !
           !  make the input transitiv
           !
           do d=1,nbr_detectors
             if (d/=d1) then
               if ( locked_pair(d1,d,dimen) ) then
                 locked_pair(d,d2,dimen) = .true.
                 locked_pair(d2,d,dimen) = .true.
               end if
             end if
           end do
           do d=1,nbr_detectors
             if (d/=d2) then
               if ( locked_pair(d2,d,dimen) ) then
                 locked_pair(d,d1,dimen) = .true.
                 locked_pair(d1,d,dimen) = .true.
               end if
             end if
           end do
         end if
       end do  
     end do
   end if  
!
 close(unit_read)
!
 print*
!
 do d=1,nbr_detectors
    if (optimize_detector(d) ) then
      print*,detector_name(d), ' is considered for position-optimization'
    end if
 end do
 print*
 do dimen=1,nbr_dimensions_optimize
   do d=1,nbr_detectors
     if (any(locked_pair(d,:,dimen)) ) then
       print*,detector_name(d), ' is locked with ', locked_pair(d,:,dimen),' in dimension ',dimen
     end if
   end do     
 end do
 print*
!
 if ( calibration_event_by_event ) then
   print*,'will work with event-by-event data'
   open (unit=unit_read, file=sweep_calibration_file , status='old')
     read(unit_read,*,iostat=eastat) dummy
     do while (eastat==0) 
       nbr_runs = nbr_runs + 1
       read(unit_read,*,iostat=eastat) dummy
     end do
   close(unit_read)
   print*,'Number of magnetic field runs:', nbr_runs
   allocate( nbr_events_in_run(nbr_runs) )
   allocate( current_event(nbr_runs) )
   allocate( next_event(nbr_runs) )
   allocate( b_field_sweep(nbr_runs) )
   allocate( nbr_hits_in_event(nbr_runs,max_nbr_events_run) )
   allocate( detector_id(nbr_runs,max_nbr_hits_per_run) )
   allocate( event_id(nbr_runs,max_nbr_hits_per_run) )
   allocate( x_pos(nbr_runs,max_nbr_hits_per_run) )
   allocate( y_pos(nbr_runs,max_nbr_hits_per_run) )
   allocate( track_ref_det_frame(nbr_runs,nbr_detectors,3) )
   allocate( detector_track_interactions_lab_frame_m(nbr_detectors,3) )
   allocate( detector_track_interactions_lab_frame_p(nbr_detectors,3) )
   allocate( detector_track_interactions_det_frame_m(nbr_detectors,3) )
   allocate( detector_track_interactions_det_frame_p(nbr_detectors,3) )
   allocate( sum_xiyi_p(2*nbr_detectors) )
   allocate( sum_xixi_p(2*nbr_detectors) )
   allocate( sum_xiyi_m(2*nbr_detectors) )
   allocate( sum_xixi_m(2*nbr_detectors) )
   allocate( detector_track_interactions_det_frame_ref_m(nbr_detectors,3) )
   allocate( detector_track_interactions_det_frame_ref_p(nbr_detectors,3) )
   allocate( derivative_variable_p(2*nbr_detectors,nbr_variables) )
   allocate( derivative_variable_m(2*nbr_detectors,nbr_variables) )
   allocate( derivative_variable_ref(2*nbr_detectors,nbr_variables) )
   allocate( coefficients_y_correction_ref(nbr_detectors,nbr_functions_y_correction) )
   allocate( coefficients_y_correction_m(nbr_detectors,nbr_functions_y_correction) )
   allocate( coefficients_y_correction_p(nbr_detectors,nbr_functions_y_correction) )
   

   track_ref_det_frame = 0.d0
   detector_track_interactions_lab_frame_p = 0.d0
   detector_track_interactions_lab_frame_m = 0.d0
!
   max_dim_pos_spectra = 2*nbr_runs
   deallocate( detector_position_spectra )
   deallocate( detector_position_spectra_paramter )
   allocate( detector_position_spectra(nbr_detectors,max_dim_pos_spectra,nbr_channels_position_spectra) )
   allocate( detector_position_spectra_paramter(nbr_detectors,max_dim_pos_spectra,nbr_paramters_position_spectra) )
   detector_position_spectra = 0
   detector_position_spectra_paramter = 0.d0
!   
 else  
   print*,'will work with average interception points'
 end if
!
end


subroutine make_sweep_incoming_beam_calibration
 use vars
 implicit none
 integer unit_read,i,eastat, d
 integer, parameter :: max_nbr_points = 1000
 double precision local_current, x_pos
 double precision theta_in(2),theta_in_0(2),dtheta,dtheta_in(2)
 double precision x_track(max_nbr_points), c_track(max_nbr_points)
 integer          d_track(max_nbr_points), nbr_points
 double precision qs, qs_old, qs_0, d_qs(2), norm, vec3(3)
!
 write(output_unit,*) 'sweep incoming beam axis calibration is called'
 x_start = x_reference
!
!
 dx_b_calibration      = 0.d0
 theta_in              = -4.d-4
!
 unit_read = 26
 nbr_points = 0
!
 open (unit=unit_read, file=sweep_calibration_file)
   read(unit_read,*,iostat=eastat) d, local_current, x_pos
   do while (eastat==0)
     nbr_points = nbr_points + 1
     x_track(nbr_points) = x_pos
     c_track(nbr_points) = local_current
     d_track(nbr_points) = d
     read(unit_read,*,iostat=eastat) d, local_current, x_pos
   end do
 close(unit_read)
 x_track = x_track * 1.d-2
! 
 qs_0   = 1.d8
 dtheta = 1.d-4
 do  
  qs_old   = qs_0
  qs_0     = qs(max_nbr_points,nbr_points,d_track,x_track,c_track)
  if (qs_old < qs_0) then
    theta_in  = theta_in_0
    exit
  end if  
  write(output_unit,*) 'theta_in**: ', real(theta_in),real(dsqrt(qs_0 ))*100.
  theta_in_0 = theta_in
  do i=1,2
    theta_in    = theta_in_0
    theta_in(i) = dtheta + theta_in_0(i)
    call rotate_vector_x(x_reference(4:6),theta_in(1),vec3)
    call rotate_vector_y(vec3,theta_in(2),x_start(4:6))
    d_qs(i)             = qs(max_nbr_points,nbr_points,d_track,x_track,c_track) - qs_0
  end do
  norm      = sqrt(dot_product(d_qs,d_qs))
  dtheta_in = -d_qs/norm*dtheta/2.
  theta_in  = theta_in_0 + dtheta_in
 end do 
! 
 qs_0   = 1.d8
 dtheta = 1.d-5
 do  
  qs_old   = qs_0
  qs_0     = qs(max_nbr_points,nbr_points,d_track,x_track,c_track)
  if (qs_old < qs_0) then
    theta_in  = theta_in_0
    exit
  end if  
  write(output_unit,*) 'theta_in* : ', real(theta_in),real(dsqrt(qs_0 ))*100.,real(dsqrt(qs_old ))*100.
  theta_in_0 = theta_in
  do i=1,2
    theta_in    = theta_in_0
    theta_in(i) = dtheta + theta_in_0(i)
    call rotate_vector_x(x_reference(4:6),theta_in(1),vec3)
    call rotate_vector_y(vec3,theta_in(2),x_start(4:6))
    d_qs(i)             = qs(max_nbr_points,nbr_points,d_track,x_track,c_track) - qs_0
  end do
  norm      = sqrt(dot_product(d_qs,d_qs))
  dtheta_in = -d_qs/norm*dtheta/4.
  theta_in  = theta_in_0 + dtheta_in
  write(output_unit,*) real(d_qs),real(qs_0), real(dtheta_in)
 end do 
! 
 qs_0   = 1.d8
 dtheta = 1.d-6
 do  
  qs_old   = qs_0
  qs_0     = qs(max_nbr_points,nbr_points,d_track,x_track,c_track)
  if (qs_old < qs_0) then
    write(output_unit,*) 'theta_in  : ', real(theta_in),real(dsqrt(qs_0 ))*100.,real(dsqrt(qs_old ))*100.
    theta_in  = theta_in_0
    exit
  end if  
  write(output_unit,*) 'theta_in  : ', real(theta_in),real(dsqrt(qs_0 ))*100.,real(dsqrt(qs_old ))*100.
  theta_in_0 = theta_in
  do i=1,2
    theta_in    = theta_in_0
    theta_in(i) = dtheta + theta_in_0(i)
    call rotate_vector_x(x_reference(4:6),theta_in(1),vec3)
    call rotate_vector_y(vec3,theta_in(2),x_start(4:6))
    d_qs(i)             = qs(max_nbr_points,nbr_points,d_track,x_track,c_track) - qs_0
  end do
  norm      = sqrt(dot_product(d_qs,d_qs))
  dtheta_in = -d_qs/norm*dtheta/4.
  theta_in  = theta_in_0 + dtheta_in
 end do 
 write(output_unit,*) theta_in_0
 stop
 
!   
 write(output_unit,*) 'dx_b_calibration: ', real(dx_b_calibration)  
 write(output_unit,*) 'recommended position for B-Field: ', real(b_field_position_in_lab(1:3)-dx_b_calibration)

 b_field_map = b_field_map_1amp * glad_current
end



subroutine make_sweep_b_field_position_calibration
 use vars
 implicit none
 integer unit_read,i,eastat, d
 integer, parameter :: max_nbr_points = 1000
 double precision local_current, x_pos
 double precision x_track(max_nbr_points), c_track(max_nbr_points)
 integer          d_track(max_nbr_points), nbr_points
 double precision qs, qs_old, qs_0, d_qs(3), dx_local(3), dx_b_0(3), norm
!
 write(output_unit,*) 'sweep B-field position calibration is called'
 x_start = x_reference
!
!
 dx_b_calibration      = 0.d0
!
 unit_read = 25
 nbr_points = 0
!
 open (unit=unit_read, file=sweep_calibration_file)
   read(unit_read,*,iostat=eastat) d, local_current, x_pos
   do while (eastat==0)
     nbr_points = nbr_points + 1
     x_track(nbr_points) = x_pos
     c_track(nbr_points) = local_current
     d_track(nbr_points) = d
     read(unit_read,*,iostat=eastat) d, local_current, x_pos
   end do
 close(unit_read)
 x_track = x_track * 1.d-2
! 
 qs_0 = 1.d8
 do  
  qs_old   = qs_0
  qs_0     = qs(max_nbr_points,nbr_points,d_track,x_track,c_track)
  if (qs_old < qs_0) exit
  write(output_unit,*) 'dx_b_calibration**: ', real(dx_b_calibration),real(dsqrt(qs_0 ))*100.
!  write(output_unit,*) 'Square-root of Quadratic sum of all deviations (cm):',real(dsqrt(qs_0 ))*100.,qs_0,norm
  dx_b_0   = dx_b_calibration
  do i=1,3
    dx_b_calibration    = dx_b_0
    dx_b_calibration(i) = spatial_resolution + dx_b_0(i)
    d_qs(i)             = qs(max_nbr_points,nbr_points,d_track,x_track,c_track) - qs_0
  end do
  norm     = sqrt(dot_product(d_qs,d_qs))
  dx_local = -d_qs/norm*spatial_resolution/2.
  dx_b_calibration = dx_b_0 + dx_local
!  norm     = dsqrt(dot_product(dx_local,dx_local))
!  if (dsqrt(norm) .lt. 2d-3) exit
 end do 
 qs_0 = 1.d8
 do  
  qs_old   = qs_0
  qs_0     = qs(max_nbr_points,nbr_points,d_track,x_track,c_track)
  if (qs_old < qs_0) exit
  write(output_unit,*) 'dx_b_calibration* : ', real(dx_b_calibration),real(dsqrt(qs_0 ))*100.
!  write(output_unit,*) 'Square-root of Quadratic sum of all deviations (cm):',real(dsqrt(qs_0 ))*100.,qs_0,norm
  dx_b_0   = dx_b_calibration
  do i=1,3
    dx_b_calibration    = dx_b_0
    dx_b_calibration(i) = spatial_resolution/2. + dx_b_0(i)
    d_qs(i)             = qs(max_nbr_points,nbr_points,d_track,x_track,c_track) - qs_0
  end do
  norm     = sqrt(dot_product(d_qs,d_qs))
  dx_local = -d_qs/norm*spatial_resolution/4.
  dx_b_calibration = dx_b_0 + dx_local
!  norm     = dsqrt(dot_product(dx_local,dx_local))
!  if (dsqrt(norm) .lt. 2d-4) exit
 end do 
 qs_0 = 1.d8
 do  
  qs_old   = qs_0
  qs_0     = qs(max_nbr_points,nbr_points,d_track,x_track,c_track)
  if (qs_old < qs_0) exit
  write(output_unit,*) 'dx_b_calibration  : ', real(dx_b_calibration),real(dsqrt(qs_0 ))*100.
!  write(output_unit,*) 'Square-root of Quadratic sum of all deviations (cm):',real(dsqrt(qs_0 ))*100.,qs_0,norm
  dx_b_0   = dx_b_calibration
  do i=1,3
    dx_b_calibration    = dx_b_0
    dx_b_calibration(i) = spatial_resolution/5. + dx_b_0(i)
    d_qs(i)             = qs(max_nbr_points,nbr_points,d_track,x_track,c_track) - qs_0
  end do
  norm     = sqrt(dot_product(d_qs,d_qs))
  dx_local = -d_qs/norm*spatial_resolution/10.
  dx_b_calibration = dx_b_0 + dx_local
!  norm     = dsqrt(dot_product(dx_local,dx_local))
!  if (dsqrt(norm) .lt. 2d-4) exit
 end do 
!   
 write(output_unit,*) 'dx_b_calibration: ', real(dx_b_calibration)  
 write(output_unit,*) 'recommended x,y,z position for B-Field (m): ', real(b_field_position_in_lab(1:3)-dx_b_calibration)

 b_field_map = b_field_map_1amp * glad_current
end

double precision function qs(max_nbr_points,nbr_points,d_track,x_track,c_track)
 use vars
 implicit none
 integer i,max_nbr_points,nbr_points,d
 double precision x_track(max_nbr_points), c_track(max_nbr_points)
 integer          d_track(max_nbr_points)
!
 qs  = 0.d0
!
 do i = 1, nbr_points
   d = d_track(i)
   b_field_map = b_field_map_1amp * c_track(i)
   call single_track
   qs =  qs + (x_track(i)-detector_track_interactions_det_frame(d,1))**2     
 end do    
end 

subroutine make_sweep_detector_position_calibration
 use vars
 implicit none
 integer unit_read,eastat, d
 logical det_ok(nbr_detectors),ok
 double precision derivative_pos(3), d_det_pos(nbr_detectors,4)
 double precision local_current, x_pos
 double precision detector_position_in_ref(nbr_detectors,4)
 double precision d_track_interaction
 double precision sdax(nbr_detectors),sdaz(nbr_detectors)
 double precision sax2(nbr_detectors),saz2(nbr_detectors)
 double precision saxaz(nbr_detectors), new_positions(nbr_detectors,2)
 integer,parameter ::  n_local=2
 double precision m_local(n_local,n_local),v1_local(n_local),v2_local(n_local)
 double precision mi_local(n_local,n_local), quadratic_sum(2)
!
 write(output_unit,*) 'sweep detector position calibration is called, get derivatives'
 detector_position_in_ref = detector_position_in_lab(1:nbr_detectors,:)
 x_start = x_reference
!
 det_ok = .false.
 derivative_pos = 0.d0
 quadratic_sum  = 0.d0
 sdax  = 0.d0
 sdaz  = 0.d0
 sax2  = 0.d0
 saz2  = 0.d0
 saxaz = 0.d0
!  
 unit_read = 25
!
 b_field_map_1amp = b_field_map_1amp *(1.d0-0.d-3)
!
!  theta_x = 1.1d-3
!  x_start    = x_reference
!  x_start(4) = x_reference(4)*dcos(theta_x) + x_reference(6)*dsin(theta_x)
!  x_start(6) = x_reference(6)*dcos(theta_x) - x_reference(4)*dsin(theta_x)
!
!  theta_y = 0.d-3
!  x_start    = x_reference
!  x_start(5) = x_reference(5)*dcos(theta_y) + x_reference(6)*dsin(theta_y)
!  x_start(6) = x_reference(6)*dcos(theta_y) - x_reference(5)*dsin(theta_y)
!
 open (unit=unit_read, file=sweep_calibration_file)
   read(unit_read,*,iostat=eastat) d, local_current, x_pos
   do while (eastat==0)
     x_pos = x_pos/1.d2
     det_ok(d) = .true.
     b_field_map = b_field_map_1amp * (local_current+0.d0)
     !
     ! vary x-position
     ! 
     d_det_pos(d,1) = spatial_resolution*10.d0
     detector_position_in_lab(1:nbr_detectors,:) = detector_position_in_ref + d_det_pos
     call single_track
     d_track_interaction = detector_track_interactions_det_frame(d,1)
     detector_position_in_lab(1:nbr_detectors,:) = detector_position_in_ref - d_det_pos
     call single_track
     d_track_interaction = d_track_interaction - detector_track_interactions_det_frame(d,1)
     derivative_pos(1) = d_track_interaction / 2.d0/d_det_pos(d,1)
     !
     ! vary z-position
     ! 
     d_det_pos      = 0.d0
     d_det_pos(d,3) = spatial_resolution*10.d0
     detector_position_in_lab(1:nbr_detectors,:) = detector_position_in_ref + d_det_pos
     call single_track
     d_track_interaction = detector_track_interactions_det_frame(d,1)
     detector_position_in_lab(1:nbr_detectors,:) = detector_position_in_ref - d_det_pos
     call single_track
     d_track_interaction = d_track_interaction - detector_track_interactions_det_frame(d,1)
     derivative_pos(3) = d_track_interaction / 2.d0/d_det_pos(d,3)
     !
     detector_position_in_lab(1:nbr_detectors,:) = detector_position_in_ref
     call single_track
     write(output_unit,*) 'Detector, Current, dx (cm)', detector_name(d), real(local_current),&
            real((x_pos-detector_track_interactions_det_frame(d,1))*100)
     quadratic_sum(1) =  quadratic_sum(1)+ (x_pos-detector_track_interactions_det_frame(d,1))**2     
!     write(output_unit,*) 'Detector, Current, dx (cm)', detector_name(d), real(local_current),&
!            real((detector_track_interactions_det_frame(d,1))*100)
     !
     sdax(d)  = sdax(d) + (x_pos-detector_track_interactions_det_frame(d,1))*derivative_pos(1)
     sdaz(d)  = sdaz(d) + (x_pos-detector_track_interactions_det_frame(d,1))*derivative_pos(3)
     sax2(d)  = sax2(d) + derivative_pos(1)*derivative_pos(1)
     saz2(d)  = saz2(d) + derivative_pos(3)*derivative_pos(3)
     saxaz(d) = saxaz(d)+ derivative_pos(1)*derivative_pos(3)
     !
     read(unit_read,*,iostat=eastat) d, local_current, x_pos
   end do
 close(unit_read)
 b_field_map = b_field_map_1amp * glad_current
 detector_position_in_lab(1:nbr_detectors,:) =  detector_position_in_ref
 new_positions = 0.d0
 do d=1,nbr_detectors
  write(output_unit,*) detector_name(d)
  if (det_ok(d)) then
   m_local(1,1) = sax2(d)
   m_local(2,1) = saxaz(d)
   m_local(1,2) = saxaz(d)
   m_local(2,2) = saz2(d)
   v1_local(1)  = sdax(d)
   v1_local(2)  = sdaz(d)
   call invert_matrix(n_local,m_local, mi_local,ok)
   v2_local = matmul(mi_local,v1_local)
   quadratic_sum(2) = quadratic_sum(2) + dot_product(v2_local,v2_local)
   write(output_unit,*) 'x,z shift of detector (cm): ',real(v2_local)*100 
!   write(output_unit,*) 'new x,z positions (m)     : ',real(detector_position_in_ref(d,1)+v2_local(1)) , &
!                                         real(detector_position_in_ref(d,3)+v2_local(2))
   new_positions(d,1) =    detector_position_in_ref(d,1)+v2_local(1)                                 
   new_positions(d,2) =    detector_position_in_ref(d,3)+v2_local(2)     
   detector_position_in_lab(d,1) =  detector_position_in_ref(d,1)+v2_local(1)                              
   detector_position_in_lab(d,3) =  detector_position_in_ref(d,3)+v2_local(2)                              
  else
   write(output_unit,*) ' no data found for this detector' 
  end if 
 end do  
 print*
 do d=1,nbr_detectors
  if (det_ok(d)) then
    write(output_unit,*) detector_name(d),'new x,z positions (m) : ',new_positions(d,:)
  end if
 end do   
 print*
 write(output_unit,*) 'Square-root of Quadratic sum of all deviations (cm):',real(dsqrt(quadratic_sum ))*100.
end

subroutine write_new_geometry_file
 use vars
 use calib
 implicit none
 integer unit_write, d,j, run
 character*256 geometry_outpout_file_local
 character(LEN=5) :: ch5
!
 unit_write = 77
 do run=0,nbr_runs
  geometry_outpout_file_local = trim(geometry_outpout_file)
  if (run>0) then
    j = int(abs( b_field_sweep(run) ))
    write(ch5,'(i5.5)') j
    geometry_outpout_file_local=ch5 //'_'//trim(geometry_outpout_file) 
  end if  
  
  open (unit=unit_write, file=geometry_outpout_file_local)
   write(unit_write,*) master_borders(1,1)  , '! master border xl'
   write(unit_write,*) master_borders(1,2)  , '! master border xr'  
   write(unit_write,*) master_borders(2,1)  , '! master border yl'  
   write(unit_write,*) master_borders(2,2)  , '! master border yr'   
   write(unit_write,*) master_borders(3,1)  , '! master border zl'  
   write(unit_write,*) master_borders(3,2)  , '! master border zr'   
   write(unit_write,*) '''',trim(magnetic_field_file),''''  , '  ! magnetic_field_file'
   write(unit_write,*) b_field_position_in_lab(1)  , '! b_field_position_in_lab x'
   write(unit_write,*) b_field_position_in_lab(2)  , '! b_field_position_in_lab y'  
   write(unit_write,*) b_field_position_in_lab(3)  , '! b_field_position_in_lab z'  
   write(unit_write,*) b_field_position_in_lab(4)*180.d0/datan(1.d0)/4.d0    , '! b_field_position_in_lab angle'
!
   if (run==0) then
     write(unit_write,*) glad_current                , '! glad current'
   else
     write(unit_write,*) b_field_sweep(run)          , '! glad current'
   end if  
   write(unit_write,*) b_field_borders(1,1)  , '! b-field border xl'  
   write(unit_write,*) b_field_borders(1,2)  , '! b-field border xr'   
   write(unit_write,*) b_field_borders(2,1)  , '! b-field border yl'   
   write(unit_write,*) b_field_borders(2,2)  , '! b-field border yr'   
   write(unit_write,*) b_field_borders(3,1)  , '! b-field border zl'   
   write(unit_write,*) b_field_borders(3,2)  , '! b-field border zr'   
!
! detectors
!
   write(unit_write,*) nbr_detectors
!
   do d=1,nbr_detectors
     write(unit_write,*) '''',trim(detector_name(d)),''''   , '  ! detector name, max 10 letters, no dots, no spaces' 
     do j=1,3
       write(unit_write,*) detector_position_in_lab(d,j)  , '  ! detector position x,y,z' 
     end do
     write(unit_write,*) detector_position_in_lab(d,4)*180.d0/datan(1.d0)/4.d0   , '  ! detector position angle'
     do j=1,3
       write(unit_write,*) detector_range(d,j,1:2),  '  ! detector range x,y,z  / low-high' 
     end do
     write(unit_write,*) stopping_power(d)  , '  ! stopping power,  dE/dx (MeV / mm) '
     do j=1,3
       if (det_step_function(d,j)) then
         write(unit_write,*) -sigma_track(d,j) ,  '  ! detector uncertainty x,y,z  , > 0 .. Gauss , < 0 step function , 0 ignored' 
       else
         write(unit_write,*) sigma_track(d,j) ,  '  ! detector uncertainty x,y,z  , > 0 .. Gauss , < 0 step function , 0 ignored'   
       end if  
     end do
   end do
   write(unit_write,*) '''',trim(ch_trigger_logic),''''
  close(unit_write)
 end do
end
