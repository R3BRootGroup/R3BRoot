function Interpolate

% Input flags:
A1over3 = 1; % If this is 1, we will interpolate in A^1/3 instead of A.
Picture = 0; % Parameter number of the picture that you want.
ZrPaper = 0; % If this is 1, we only use the 3 nuclei from the Zr90-paper.
LargePb = 1; % If this is 1, we will use 35 MeV and 50 MeV of the Pb-wells.

% Interpolate optical model parameters to A of your choice.
% Includes some more data on this topic.
A = [12 28 58 90 208].';
A_me1 = 7;
A_me2 = 7;

% Parameters:
VR = [19.73 25.10 35.16 31.20 31.04].';
rR = [1.592 1.430 1.320 1.363 1.347].';
aR = [0.705 0.833 0.840 0.818 0.846].';
VI = [37.76 40.00 44.43 42.06 46.70].';
rI = [0.989 0.963 1.021 1.044 1.008].';
aI = [0.868 1.031 1.018 1.055 1.282].';

% Use only Zr90 paper:
if ZrPaper==1
A = [12 58 90].';
VR = [19.73 35.16 31.20].';
rR = [1.592 1.320 1.363].';
aR = [0.705 0.840 0.818].';
VI = [37.76 44.43 42.06].';
rI = [0.989 1.021 1.044].';
aI = [0.868 1.018 1.055].';
end

% Replace Pb208 with 35.0 & 50 which, according
% to Remco gives almost the same chi^2:
if (ZrPaper~=1)&&(LargePb==1)
VR(5) = 35.0;
VI(5) = 50.0;
end

% Now make a for-loop over all 6 parameters:
Md = zeros(2,7);
Md(1,1) = A_me1;
Md(2,1) = A_me2;
n = max(size(A));

if A1over3==1
A_me1 = A_me1^(1/3);
A_me2 = A_me2^(1/3);
end

% Do the loop:
for k=1:1:6

% Define x:
x = A;
x_ev = 0:1:250;

if A1over3==1
x = x.^(1/3);
x_ev = x_ev.^(1/3);
end

% Define y:
if k==1
y = VR;
elseif k==2
y = rR;
elseif k==3
y = aR;
elseif k==4
y = VI;
elseif k==5
y = rI;
elseif k==6
y = aI;
end

% Define means:
mux = sum(x)/n;
muy = sum(y)/n;
muxy = x.'*y/n;
mux2 = x.'*x/n;
mux2 = y.'*y/n;

% Define linear regression:
M = [x ones(n,1)];
MTM = M.'*M;
MTb = M.'*y;
C = inv(MTM)*MTb;
a = C(1);
b = C(2);

% Compute new point:
f = @(x) a.*x + b;
y_me1 = f(A_me1);
y_me2 = f(A_me2);
y_ev = f(x_ev);

% add it to the vector:
x(n+1) = A_me1; 
x(n+2) = A_me2;
y(n+1) = y_me1; 
y(n+2) = y_me2;

% Make a plot:
if k==Picture
close all;
figure;
hold on;
plot(x_ev,y_ev,'r-','LineWidth',2);
plot(x,y,'b*','LineWidth',4);
xlabel('Atomic mass number A');

if k==1
  ylabel('Optical potential parameter VR');
  saveas(gca,'./Interpolation_VR.png');
elseif k==2
  ylabel('Optical potential parameter rR');
  saveas(gcf,'./Interpolation_rR.png');
elseif k==3
  ylabel('Optical potential parameter aR');
  saveas(gcf,'./Interpolation_aR.png');
elseif k==4
  ylabel('Optical potential parameter VI');
  saveas(gcf,'./Interpolation_VI.png');
elseif k==5
  ylabel('Optical potential parameter rI');
  saveas(gcf,'./Interpolation_rI.png');
elseif k==6
  ylabel('Optical potential parameter aI');
  saveas(gcf,'./Interpolation_aI.png');
else
  disp('OEI');
end
end

% Secure output:
Md(1,k+1) = y_me1;
Md(2,k+1) = y_me2;

% End for-loop:
end

disp('Computed optical potentials [A VR rR aR VI rI aI] = ');
disp(Md);
disp('');
disp('Real well strength = ');
disp([Md(1,2)*(Md(1,3)^3) Md(2,2)*(Md(2,3)^3)]);
disp('');
disp('Imaginary well strength = ');
disp([Md(1,5)*(Md(1,6)^3) Md(2,5)*(Md(2,6)^3)]);
disp('');

% Done:
end
