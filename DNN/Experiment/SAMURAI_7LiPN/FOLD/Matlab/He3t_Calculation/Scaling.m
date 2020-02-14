function Scaling
% Process Fortran output:
A = GetData;
B = GetDataSurf;
C = GetDataSO;

[Zero Index] = min(abs(A-0.5));
n = max(size(A));

Radius = 6.36718;
mesh = Radius/Index;

x = zeros(10,1);

for k = 1:1:n
x(k) = (k-1)*mesh+1e-3;
end

% Process analythical output:
R = Radius;
a = 0.84;
f = @(x) 1./(1+exp((x-R)./a)); % Wood-Saxon potential (with minus-sign!)
g = @(x) (4*a).*(f(x-0.001)-f(x+0.001))./0.002; % Wood-saxon derivative.
h = @(x) (2./x).*(f(x-0.001)-f(x+0.001))./0.002;
y = f(x); 
z = g(x);
so = h(x);

% Make pictures:
close all
figure
hold on
plot(x,-A,'b-','LineWidth',2);
plot(x,-B,'g-','LineWidth',2);
plot(x,-C,'m-','LineWidth',2);
plot(x,-y,'k-','LineWidth',2);
plot(x,-z,'r-','LineWidth',2);
plot(x,-so,'c-','LineWidth',2);

end