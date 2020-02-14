function Extrapolate

% Extrapolates optical model parameters to A of your choice.
A = [12 58 90].';
A_me1 = 116;
A_me2 = 122;

% Parameters:
VR = [19.73 35.16 31.20].';
rR = [1.592 1.320 1.363].';
aR = [0.705 0.840 0.818].';
VI = [37.76 44.43 42.06].';
rI = [0.989 1.021 1.044].';
aI = [0.868 1.018 1.055].';

% Define x and y:
x = A;
y = aI;

% Define means:
n = max(size(x));
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
x_ev = 0:1:150;
y_ev = f(x_ev);

% add it to the vector:
x(n+1) = A_me1; x(n+2) = A_me2;
y(n+1) = y_me1; y(n+2) = y_me2;

% Make a plot:
close all;
figure;
hold on;
plot(x_ev,y_ev,'r-','LineWidth',2);
plot(x,y,'b*','LineWidth',4);

disp('Parameterw of interest = ');
Md = [A_me1 y_me1;A_me2 y_me2];
disp(Md);

% Done:
end