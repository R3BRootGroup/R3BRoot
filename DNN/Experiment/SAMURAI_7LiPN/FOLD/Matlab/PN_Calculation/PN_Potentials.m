function PN_Potentials(Ebeam)
  % Value that we want:
  Aours = 7;
  Ebeam = 252.7;
  
  % Load parameters:
  [A,rV,aV,v1,v2,v3,v4,w1,w2,rD,aD,d1,d2,d3,rSO,aSO,vSO1,vSO2,wSO1,wSO2,rC,Enf] = Neutron_Parameters;
  % [A,rV,aV,v1,v2,v3,v4,w1,w2,rD,aD,d1,d2,d3,rSO,aSO,vSO1,vSO2,wSO1,wSO2,rC,Enf] = Proton_Parameters;
  
  % Compute potentials:
  rR = rV;
  aR = aV;
  VR = v1.*(1 - v2.*(Ebeam - Enf) + v3.*(Ebeam-Enf).^2 - v4.*(Ebeam-Enf).^3);
  rI = rV;
  aI = rV;
  VI = (w1.*(Ebeam-Enf).^2)./((Ebeam-Enf).^2 + w2.^2);
  Ws = exp(-d2.*(Ebeam-Enf)).*(d1.*(Ebeam-Enf).^2)./((Ebeam-Enf).^2 + d3.^2);
  rC = rC;
  
  % Next, compute linear regression lines:
  x = A.^(1/3);
  
  % Select our case:
  y = rR;
  % y = aR;
  % y = VR;
  % y = rI;
  % y = aI;
  % y = VI;
  % y = Ws;
  % y = rC;
  
  % Compute linear regression:
 n = max(size(x));
 xmu = sum(x)/n;
 ymu = sum(y)/n;
 xydev = sum((x - xmu).*(y - ymu))/n;
 x2dev = sum((x - xmu).*(x - xmu))/n;
 slope = xydev/x2dev;
 offset = ymu - slope*xmu;
 
 % make a picture:
 xmax = max(x);
 xplot = [0:0.01:1.1*xmax];
 yplot = slope.*xplot + offset;
 
 close all;
 figure;
 plot(xplot,yplot, 'r-', 'LineWidth', 3.0);
 hold on;
 plot(x,y, 'b*', 'LineWidth', 5.0);
 
 % compute our values:
 xours = Aours.^(1/3);
 yours = slope*xours + offset;
 plot(xours,yours, 'g*', 'LineWidth', 10.0);
 
 disp('A of interest = ');
 disp(Aours);
 disp('Computed parameter = ');
 disp(yours);
 
 % Done.
 end
 
 