function ComputeOPT
% Computes optical potential parameters for fold based on Muhsins paper.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%% Definition of Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Definition of constants in physics (SI-units):
eps0 = 8.854187817e-12;           % Dielectric permeability of vacuum
qe = 1.60217662e-19;              % Elementary charge constant.

% Definition of projectile and target (assume n->p transition).
A_target = 90;
Z_target = 40;
A_project = 3;
Z_project = 2;
Ebeam_LAB = 420.0;
DEbeam_LAB = 0.252;
Eeject_LAB = 402.62;
DEeject_LAB = 0.252;

% Definition of paper paparemeters:
rc_coeff = 1.24; % [fm]
rc_off = 0.12; % [fm]
V0 = 118.3; % [MeV].
Ve = -0.13; % [dim. less]
r0_coeff = 1.30; % [fm]
r0_off = -0.48; % [fm]
ar0 = 0.82; % [fm]
Wv0 = 38.5; % [MeV]
Wve0 = 156.1; % [MeV]
Wvew = 52.4; % [MeV]
Ws0 = 35.0; % [MeV]
Wst = 34.2; % [MeV]
Wse0 = 30.8; % [MeV]
Wsew = 106.4; % [MeV]
rw_coeff = 1.31; % [fm]
rw_off = -0.13; % [fm]
aw0 = 0.84; % [fm]

% Definition of uncertainties in parameters:
D_rc_coeff = 0.0; % [fm]
D_rc_off = 0.0; % [fm]
D_V0 = 1.3; % [MeV]
D_Ve = 0.01; % [dim. less]
D_r0_coeff = 0.01; % [fm]
D_r0_off = 0.05; % [fm]
D_ar0 = 0.01; % [fm]
D_Wv0 = 3.9; % [MeV]
D_Wve0 = 11.9; % [MeV]
D_Wvew = 8.2; % [MeV]
D_Ws0 = 1.1; % [MeV]
D_Wst = 8.0; % [MeV]
D_Wse0 = 4.7; % [MeV]
D_Wsew = 8.0; % [MeV]
D_rw_coeff = 0.02; % [fm]
D_rw_off = 0.10; % [fm]
D_aw0 = 0.01; % [fm]

% Definition of correlation coefficients (sequence is as above:
A = [0     0     0     0     0     0     0     0     0     0     0     0     0     0     0     0     0;  
     0     0     0     0     0     0     0     0     0     0     0     0     0     0     0     0     0;  
     0     0     0     0     0     0     0     0     0     0     0     0     0     0     0     0     0;  
     0     0    -0.3   0     0     0     0     0     0     0     0     0     0     0     0     0     0;  
     0     0    -0.3  -0.4   0     0     0     0     0     0     0     0     0     0     0     0     0;  
     0     0    -0.1   0.4  -0.9   0     0     0     0     0     0     0     0     0     0     0     0;  
     0     0     0.5   0.4  -0.6   0.3   0     0     0     0     0     0     0     0     0     0     0;  
     0     0    -0.3   0.5  -0.1   0.1   0.2   0     0     0     0     0     0     0     0     0     0;  
     0     0    -0.2  -0.2   0.2  -0.1  -0.2   0.5   0     0     0     0     0     0     0     0     0;  
     0     0     0    -0.3   0.1  -0.2   0     0.4   0.8   0     0     0     0     0     0     0     0;  
     0     0    -0.1   0.1   0     0.1  -0.2  -0.1  -0.3  -0.6   0     0     0     0     0     0     0;  
     0     0     0.2   0     0.1  -0.2   0    -0.2  -0.2  -0.3   0     0     0     0     0     0     0;  
     0     0    -0.1  -0.2   0.2  -0.1  -0.4   0     0.2  -0.1  -0.1  -0.3   0     0     0     0     0;  
     0     0    -0.1  -0.4   0.2  -0.1  -0.5  -0.3   0.4   0.3  -0.1   0.2   0.1   0     0     0     0;  
     0     0     0    -0.2   0.1  -0.1   0.1   0.2   0.3   0.5  -0.2  -0.7   0.2  -0.2   0     0     0;  
     0     0     0.1   0.4  -0.3   0.2   0.2  -0.1  -0.3  -0.3   0.1   0.6  -0.4   0    -0.9   0     0;  
     0     0    -0.1  -0.5   0.1   0    -0.4  -0.3   0    -0.1   0     0     0.1   0.4   0    -0.3   0];
     
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%% Transform our matrix to a covariance matrix%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

A = A + A.';
A = A + eye(17);

A(:,1) = A(:,1)*D_rc_coeff;
A(:,2) = A(:,2)*D_rc_off; 
A(:,3) = A(:,3)*D_V0; 
A(:,4) = A(:,4)*D_Ve; 
A(:,5) = A(:,5)*D_r0_coeff; 
A(:,6) = A(:,6)*D_r0_off; 
A(:,7) = A(:,7)*D_ar0; 
A(:,8) = A(:,8)*D_Wv0; 
A(:,9) = A(:,9)*D_Wve0; 
A(:,10) = A(:,10)*D_Wvew; 
A(:,11) = A(:,11)*D_Ws0; 
A(:,12) = A(:,12)*D_Wst;
A(:,13) = A(:,13)*D_Wse0; 
A(:,14) = A(:,14)*D_Wsew; 
A(:,15) = A(:,15)*D_rw_coeff;
A(:,16) = A(:,16)*D_rw_off;
A(:,17) = A(:,17)*D_aw0; 

A(1,:) = A(1,:)*D_rc_coeff;
A(2,:) = A(2,:)*D_rc_off; 
A(3,:) = A(3,:)*D_V0; 
A(4,:) = A(4,:)*D_Ve; 
A(5,:) = A(5,:)*D_r0_coeff; 
A(6,:) = A(6,:)*D_r0_off; 
A(7,:) = A(7,:)*D_ar0; 
A(8,:) = A(8,:)*D_Wv0; 
A(9,:) = A(9,:)*D_Wve0; 
A(10,:) = A(10,:)*D_Wvew; 
A(11,:) = A(11,:)*D_Ws0; 
A(12,:) = A(12,:)*D_Wst;
A(13,:) = A(13,:)*D_Wse0; 
A(14,:) = A(14,:)*D_Wsew; 
A(15,:) = A(15,:)*D_rw_coeff;
A(16,:) = A(16,:)*D_rw_off;
A(17,:) = A(17,:)*D_aw0; 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%% Performing the computation %%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Coulomb radius:
Rc = rc_coeff*(A_target^(1/3)) + rc_off; % [coulomb radius in fm]
dRc_drc_coeff = A_target^(1/3);
dRc_drc_off = 1;
DRc = dRc_drc_coeff*dRc_drc_coeff*A(1,1) + dRc_drc_off*dRc_drc_off*A(2,2) + 2*dRc_drc_coeff*dRc_drc_off*A(1,2);
DRc = sqrt(DRc);                        % [Error in Coulomb radius in fm].
Rc_fold = Rc/(A_target^(1/3));          % [Coulomb radius FOLD-input]
DRc_fold = DRc/(A_target^(1/3));        % [Error in Coulomb radius FOLD-input]

% Coulomb excitation (Skip errors in physics constants, Z & A):
Ec_before = (6*Z_target*Z_project*qe*qe)/(5*Rc*1e-15); % [Joule].
dEc_before_dRc = Ec_before*(-1/Rc);
DEc_before = sqrt(dEc_before_dRc*dEc_before_dRc*DRc*DRc);
Ec_before = Ec_before/(4*pi*eps0);
DEc_before = DEc_before/(4*pi*eps0);
Ec_before = Ec_before/(qe*1e6); % [MeV].
DEc_before = DEc_before/(qe*1e6); % [MeV].
Ec_after = (6*(Z_target+1)*(Z_project-1)*qe*qe)/(5*Rc*1e-15);
dEc_after_dRc = Ec_after*(-1/Rc);
DEc_after = sqrt(dEc_after_dRc*dEc_after_dRc*DRc*DRc);
Ec_after = Ec_after/(4*pi*eps0);
DEc_after = DEc_after/(4*pi*eps0);
Ec_after = Ec_after/(qe*1e6); % [MeV]. 
DEc_after = DEc_after/(qe*1e6); % [MeV]. 

% Computation of the real volume Wood-Saxon:
RR = r0_coeff*(A_target^(1/3)) + r0_off;
dRR_dr0_coeff = (A_target^(1/3));
dRR_dr0_off = 1;
DRR = sqrt(dRR_dr0_coeff*dRR_dr0_coeff*A(5,5) + dRR_dr0_off*dRR_dr0_off*A(6,6) + 2*dRR_dr0_coeff*dRR_dr0_off*A(5,6));
RR_fold = RR/(A_target^(1/3));
DRR_fold = DRR/(A_target^(1/3));
aR = ar0;
DaR = sqrt(A(7,7));

VR_before = V0 + Ve*(Ebeam_LAB-Ec_before); % [MeV]
dVR_before_dV0 = 1;
dVR_before_dVe = Ebeam_LAB-Ec_before;
dVR_before_dEbeam_LAB = Ve;
dVR_before_dEc_before = -Ve;
DVR_before = sqrt(dVR_before_dV0*dVR_before_dV0*A(3,3) + dVR_before_dVe*dVR_before_dVe*A(4,4) + 2*dVR_before_dV0*dVR_before_dVe*A(3,4) + dVR_before_dEbeam_LAB*dVR_before_dEbeam_LAB*DEbeam_LAB*DEbeam_LAB + dVR_before_dEc_before*dVR_before_dEc_before*DEc_before*DEc_before);

VR_after = V0 + Ve*(Eeject_LAB-Ec_after); % [MeV]
dVR_after_dV0 = 1;
dVR_after_dVe = Eeject_LAB-Ec_after;
dVR_after_dEeject_LAB = Ve;
dVR_after_dEc_after = -Ve;
DVR_after = sqrt(dVR_after_dV0*dVR_after_dV0*A(3,3) + dVR_after_dVe*dVR_after_dVe*A(4,4) + 2*dVR_after_dVe*dVR_after_dV0*A(3,4) + dVR_after_dEeject_LAB*dVR_after_dEeject_LAB*DEeject_LAB*DEeject_LAB + dVR_after_dEc_after*dVR_after_dEc_after*DEc_after*DEc_after);

% Computation of the imaginary volume Wood-Saxon:
RI = rw_coeff*(A_target^(1/3)) + rw_off;
dRI_drw_coeff = A_target^(1/3);
dRI_drw_off = 1;
DRI = sqrt(dRI_drw_coeff*dRI_drw_coeff*A(15,15) + dRI_drw_off*dRI_drw_off*A(16,16) + 2*dRI_drw_coeff*dRI_drw_off*A(15,16));
RI_fold = RI/(A_target^(1/3));
DRI_fold = DRI/(A_target^(1/3));
aI = aw0;
DaI = sqrt(A(17,17));

VI_before = Wv0*(1/(1 + exp((Wve0 - (Ebeam_LAB - Ec_before))/Wvew)));
dVI_before_dWv0 = (1/(1 + exp((Wve0 - (Ebeam_LAB - Ec_before))/Wvew)));
dVI_before_dWve0 = (Wv0*(1/(1 + exp(((Wve0+0.1*sqrt(A(9,9))) - (Ebeam_LAB - Ec_before))/Wvew))) - Wv0*(1/(1 + exp(((Wve0-0.1*sqrt(A(9,9))) - (Ebeam_LAB - Ec_before))/Wvew))))/(0.2*sqrt(A(9,9)));
dVI_before_dEbeam_LAB = (Wv0*(1/(1 + exp((Wve0 - ((Ebeam_LAB+0.1*DEbeam_LAB) - Ec_before))/Wvew))) - Wv0*(1/(1 + exp((Wve0 - ((Ebeam_LAB-0.1*DEbeam_LAB) - Ec_before))/Wvew))))/(0.2*DEbeam_LAB);
DEc_before = DEc_before + 0.00001; % Avoid division by zero.
dVI_before_dEc_before = (Wv0*(1/(1 + exp((Wve0 - (Ebeam_LAB - (Ec_before+0.1*DEc_before)))/Wvew))) - Wv0*(1/(1 + exp((Wve0 - (Ebeam_LAB - (Ec_before-0.1*DEc_before)))/Wvew))))/(0.2*DEc_before);
DEc_before = DEc_before - 0.00001; % put the shift back.
dVI_before_dWvew = (Wv0*(1/(1 + exp((Wve0 - (Ebeam_LAB - Ec_before))/(Wvew+0.1*sqrt(A(10,10)))))) - Wv0*(1/(1 + exp((Wve0 - (Ebeam_LAB - Ec_before))/(Wvew-0.1*sqrt(A(10,10)))))))/(0.2*sqrt(A(10,10)));
DVI_before = 0;
DVI_before = DVI_before + dVI_before_dWv0*dVI_before_dWv0*A(8,8) + 2*dVI_before_dWv0*dVI_before_dWve0*A(8,9) + 2*dVI_before_dWv0*dVI_before_dWvew*A(8,10);
DVI_before = DVI_before + dVI_before_dWve0*dVI_before_dWve0*A(9,9) + 2*dVI_before_dWve0*dVI_before_dWvew*A(9,10);
DVI_before = DVI_before + dVI_before_dWvew*dVI_before_dWvew*A(10,10);
DVI_before = DVI_before + dVI_before_dEbeam_LAB*dVI_before_dEbeam_LAB*DEbeam_LAB*DEbeam_LAB + dVI_before_dEc_before*dVI_before_dEc_before*DEc_before*DEc_before;
DVI_before = sqrt(DVI_before);

VI_after = Wv0*(1/(1 + exp((Wve0 - (Eeject_LAB - Ec_after))/Wvew)));
dVI_after_dWv0 = (1/(1 + exp((Wve0 - (Eeject_LAB - Ec_after))/Wvew)));
dVI_after_dWve0 = (Wv0*(1/(1 + exp(((Wve0+0.1*sqrt(A(9,9))) - (Eeject_LAB - Ec_after))/Wvew))) - Wv0*(1/(1 + exp(((Wve0-0.1*sqrt(A(9,9))) - (Eeject_LAB - Ec_after))/Wvew))))/(0.2*sqrt(A(9,9)));
dVI_after_dEeject_LAB = (Wv0*(1/(1 + exp((Wve0 - ((Eeject_LAB+0.1*DEeject_LAB) - Ec_after))/Wvew))) - Wv0*(1/(1 + exp((Wve0 - ((Eeject_LAB-0.1*DEeject_LAB) - Ec_after))/Wvew))))/(0.2*DEeject_LAB);
DEc_after = DEc_after + 0.00001; % Avoid division by zero.
dVI_after_dEc_after = (Wv0*(1/(1 + exp((Wve0 - (Eeject_LAB - (Ec_after+0.1*DEc_after)))/Wvew))) - Wv0*(1/(1 + exp((Wve0 - (Eeject_LAB - (Ec_after-0.1*DEc_after)))/Wvew))))/(0.2*DEc_after);
DEc_after = DEc_after - 0.00001; % Put the shift back.
dVI_after_dWvew = (Wv0*(1/(1 + exp((Wve0 - (Eeject_LAB - Ec_after))/(Wvew+0.1*sqrt(A(10,10)))))) - Wv0*(1/(1 + exp((Wve0 - (Eeject_LAB - Ec_after))/(Wvew-0.1*sqrt(A(10,10)))))))/(0.2*sqrt(A(10,10)));
DVI_after = 0;
DVI_after = DVI_after + dVI_after_dWv0*dVI_after_dWv0*A(8,8) + 2*dVI_after_dWv0*dVI_after_dWve0*A(8,9) + 2*dVI_after_dWv0*dVI_after_dWvew*A(8,10);
DVI_after = DVI_after + dVI_after_dWve0*dVI_after_dWve0*A(9,9) + 2*dVI_after_dWve0*dVI_after_dWvew*A(9,10);
DVI_after = DVI_after + dVI_after_dWvew*dVI_after_dWvew*A(10,10);
DVI_after = DVI_after + dVI_after_dEeject_LAB*dVI_after_dEeject_LAB*DEeject_LAB*DEeject_LAB + dVI_after_dEc_after*dVI_after_dEc_after*DEc_after*DEc_after;
DVI_after = sqrt(DVI_after);

% Computation of the imaginary surface Wood-Saxon:
epsilon = (A_target - 2*Z_target)/(A_target);
Depsilon = 0;

VIsurf_before = (Ws0 + epsilon*Wst)/(1 + exp(((Ebeam_LAB - Ec_before) - Wse0)/Wsew));
dVIsurf_before_dWs0 = 1/(1 + exp(((Ebeam_LAB - Ec_before) - Wse0)/Wsew));
dVIsurf_before_dWst = epsilon/(1 + exp(((Ebeam_LAB - Ec_before) - Wse0)/Wsew));
dVIsurf_before_dWse0 = ((Ws0 + epsilon*Wst)/(1 + exp(((Ebeam_LAB - Ec_before) - (Wse0+0.1*sqrt(A(13,13))))/Wsew)) - (Ws0 + epsilon*Wst)/(1 + exp(((Ebeam_LAB - Ec_before) - (Wse0-0.1*sqrt(A(13,13))))/Wsew)))/(0.2*sqrt(A(13,13)));
dVIsurf_before_dWsew = ((Ws0 + epsilon*Wst)/(1 + exp(((Ebeam_LAB - Ec_before) - Wse0)/(Wsew+0.1*sqrt(A(14,14))))) - (Ws0 + epsilon*Wst)/(1 + exp(((Ebeam_LAB - Ec_before) - Wse0)/(Wsew-0.1*sqrt(A(14,14))))))/(0.2*sqrt(A(14,14)));
dVIsurf_before_depsilon = Wst/(1 + exp(((Ebeam_LAB - Ec_before) - Wse0)/Wsew));
dVIsurf_before_dEbeam_LAB = ((Ws0 + epsilon*Wst)/(1 + exp((((Ebeam_LAB+0.1*DEbeam_LAB) - Ec_before) - Wse0)/Wsew)) - (Ws0 + epsilon*Wst)/(1 + exp((((Ebeam_LAB-0.1*DEbeam_LAB) - Ec_before) - Wse0)/Wsew)))/(0.2*DEbeam_LAB);
DEc_before = DEc_before + 0.00001; % Avoid division by zero.
dVIsurf_before_dEc_before = ((Ws0 + epsilon*Wst)/(1 + exp(((Ebeam_LAB - (Ec_before+0.1*DEc_before)) - Wse0)/Wsew)) - (Ws0 + epsilon*Wst)/(1 + exp(((Ebeam_LAB - (Ec_before-0.1*DEc_before)) - Wse0)/Wsew)))/(0.2*DEc_before);
DEc_before = DEc_before - 0.00001; % put the shift back.
DVIsurf_before = 0;
DVIsurf_before = DVIsurf_before + dVIsurf_before_dWs0*dVIsurf_before_dWs0*A(11,11); + 2*dVIsurf_before_dWs0*dVIsurf_before_dWst*A(11,12) + 2*dVIsurf_before_dWs0*dVIsurf_before_dWse0*A(11,13) + 2*dVIsurf_before_dWs0*dVIsurf_before_dWsew*A(11,14);
DVIsurf_before = DVIsurf_before + dVIsurf_before_dWst*dVIsurf_before_dWst*A(12,12); + 2*dVIsurf_before_dWst*dVIsurf_before_dWse0*A(12,13) + 2*dVIsurf_before_dWst*dVIsurf_before_dWsew*A(12,14);
DVIsurf_before = DVIsurf_before + dVIsurf_before_dWse0*dVIsurf_before_dWse0*A(13,13) + 2*dVIsurf_before_dWse0*dVIsurf_before_dWsew*A(13,14);
DVIsurf_before = DVIsurf_before + dVIsurf_before_dWsew*dVIsurf_before_dWsew*A(14,14);
DVIsurf_before = DVIsurf_before + dVIsurf_before_depsilon*dVIsurf_before_depsilon*Depsilon*Depsilon;
DVIsurf_before = DVIsurf_before + dVIsurf_before_dEbeam_LAB*dVIsurf_before_dEbeam_LAB*DEbeam_LAB*DEbeam_LAB;
DVIsurf_before = DVIsurf_before + dVIsurf_before_dEc_before*dVIsurf_before_dEc_before*DEc_before*DEc_before;
DVIsurf_before = sqrt(DVIsurf_before);

VIsurf_after = (Ws0 - epsilon*Wst)/(1 + exp(((Eeject_LAB - Ec_after) - Wse0)/Wsew));
dVIsurf_after_dWs0 = 1/(1 + exp(((Eeject_LAB - Ec_after) - Wse0)/Wsew));
dVIsurf_after_dWst = epsilon/(1 + exp(((Eeject_LAB - Ec_after) - Wse0)/Wsew));
dVIsurf_after_dWse0 = ((Ws0 + epsilon*Wst)/(1 + exp(((Eeject_LAB - Ec_after) - (Wse0+0.1*sqrt(A(13,13))))/Wsew)) - (Ws0 + epsilon*Wst)/(1 + exp(((Eeject_LAB - Ec_after) - (Wse0-0.1*sqrt(A(13,13))))/Wsew)))/(0.2*sqrt(A(13,13)));
dVIsurf_after_dWsew = ((Ws0 + epsilon*Wst)/(1 + exp(((Eeject_LAB - Ec_after) - Wse0)/(Wsew+0.1*sqrt(A(14,14))))) - (Ws0 + epsilon*Wst)/(1 + exp(((Eeject_LAB - Ec_after) - Wse0)/(Wsew-0.1*sqrt(A(14,14))))))/(0.2*sqrt(A(14,14)));
dVIsurf_after_depsilon = Wst/(1 + exp(((Eeject_LAB - Ec_after) - Wse0)/Wsew));
dVIsurf_after_dEeject_LAB = ((Ws0 + epsilon*Wst)/(1 + exp((((Eeject_LAB+0.1*DEeject_LAB) - Ec_after) - Wse0)/Wsew)) - (Ws0 + epsilon*Wst)/(1 + exp((((Eeject_LAB-0.1*DEeject_LAB) - Ec_after) - Wse0)/Wsew)))/(0.2*DEeject_LAB);
DEc_after = DEc_after + 0.00001; % Avoid division by zero.
dVIsurf_after_dEc_after = ((Ws0 + epsilon*Wst)/(1 + exp(((Eeject_LAB - (Ec_after+0.1*DEc_after)) - Wse0)/Wsew)) - (Ws0 + epsilon*Wst)/(1 + exp(((Eeject_LAB - (Ec_after-0.1*DEc_after)) - Wse0)/Wsew)))/(0.2*DEc_after);
DEc_after = DEc_after - 0.00001; % put the shift back.
DVIsurf_after = 0;
DVIsurf_after = DVIsurf_after + dVIsurf_after_dWs0*dVIsurf_after_dWs0*A(11,11); + 2*dVIsurf_after_dWs0*dVIsurf_after_dWst*A(11,12) + 2*dVIsurf_after_dWs0*dVIsurf_after_dWse0*A(11,13) + 2*dVIsurf_after_dWs0*dVIsurf_after_dWsew*A(11,14);
DVIsurf_after = DVIsurf_after + dVIsurf_after_dWst*dVIsurf_after_dWst*A(12,12); + 2*dVIsurf_after_dWst*dVIsurf_after_dWse0*A(12,13) + 2*dVIsurf_after_dWst*dVIsurf_after_dWsew*A(12,14);
DVIsurf_after = DVIsurf_after + dVIsurf_after_dWse0*dVIsurf_after_dWse0*A(13,13) + 2*dVIsurf_after_dWse0*dVIsurf_after_dWsew*A(13,14);
DVIsurf_after = DVIsurf_after + dVIsurf_after_dWsew*dVIsurf_after_dWsew*A(14,14);
DVIsurf_after = DVIsurf_after + dVIsurf_after_depsilon*dVIsurf_after_depsilon*Depsilon*Depsilon;
DVIsurf_after = DVIsurf_after + dVIsurf_after_dEeject_LAB*dVIsurf_after_dEeject_LAB*DEeject_LAB*DEeject_LAB;
DVIsurf_after = DVIsurf_after + dVIsurf_after_dEc_after*dVIsurf_after_dEc_after*DEc_after*DEc_after;
DVIsurf_after = sqrt(DVIsurf_after);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%% Illustration part %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Function definitions (imaginary part):
R = RI;
a = aI;
f = @(x) -1./(1+exp((x-R)./a)); % Wood-Saxon potential (with minus-sign!)
g = @(x) (-4*a).*(f(x-0.001)-f(x+0.001))./0.002; % Wood-saxon derivative.
x = 0:0.01:15;
y = f(x);
z = g(x);
n = max(size(x));
Int_V = 0;
Int_S = 0;
mesh = x(2)-x(1);

for k = 1:1:n
Int_V = Int_V + f(x(k))*(x(k)^2)*4*pi*mesh;
Int_S = Int_S + g(x(k))*(x(k)^2)*4*pi*mesh;
end

Volume_Ratio = Int_S/Int_V;

close all
figure
hold on
plot(x,y,'b-','LineWidth',2)
plot(x,-z,'g-','LineWidth',2)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%% Output definition %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Display output:
fprintf('Coulomb radius as requested by FOLD [fm]: %d +/- %f \n', Rc_fold, DRc_fold);
fprintf('Coulomb energy correction Input Channel [MeV]: %d +/- %f \n', Ec_before, DEc_before);
fprintf('Coulomb energy correction Output Channel [MeV]: %d +/- %f \n', Ec_after, DEc_after);
disp('');
fprintf('Real opt. pot. Input Channel VR [MeV]: %d +/- %f \n', VR_before, DVR_before);
fprintf('Real opt. pot. Input Channel RR [fm]: %d +/- %f \n', RR_fold, DRR_fold);
fprintf('Real opt. pot. Input Channel aR [fm]: %d +/- %f \n', aR, DaR);
fprintf('Real opt. pot. Input Channel soR [?]: %d +/- %f \n', 0, 0);
disp('');
fprintf('Imaginary opt. pot. Input Channel VI [MeV]: %d +/- %f \n', VI_before, DVI_before);
fprintf('Imaginary opt. pot. Input Channel VIsurf [MeV]: %d +/- %f \n', VIsurf_before, DVIsurf_before);
fprintf('Imaginary opt. pot. Input Channel RI [fm]: %d +/- %f \n', RI_fold, DRI_fold);
fprintf('Imaginary opt. pot. Input Channel aI [fm]: %d +/- %f \n', aI, DaI);
fprintf('Imaginary opt. pot. Input Channel soI [?]: %d +/- %f \n', 0, 0);
disp('');
fprintf('Real opt. pot. Output Channel VR [MeV]: %d +/- %f \n', VR_after, DVR_after);
fprintf('Real opt. pot. Output Channel RR [fm]: %d +/- %f \n', RR_fold, DRR_fold);
fprintf('Real opt. pot. Output Channel aR [fm]: %d +/- %f \n', aR, DaR);
fprintf('Real opt. pot. Output Channel soR [?]: %d +/- %f \n', 0, 0);
disp('');
fprintf('Imaginary opt. pot. Output Channel VI [MeV]: %d +/- %f \n', VI_after, DVI_after);
fprintf('Imaginary opt. pot. Output Channel VIsurf [MeV]: %d +/- %f \n', VIsurf_after, DVIsurf_after);
fprintf('Imaginary opt. pot. Output Channel RI [fm]: %d +/- %f \n', RI_fold, DRI_fold);
fprintf('Imaginary opt. pot. Output Channel aI [fm]: %d +/- %f \n', aI, DaI);
fprintf('Imaginary opt. pot.Output Channel soI [?]: %d +/- %f \n', 0, 0);
disp('');
fprintf('Ratio of imaginary vol. and surf. function integrals : %d \n', Volume_Ratio);

end
