// This header contains the functions to actually decide 
// which clusters are the primary ones. It contains
// the sorting by R-value, the IsElastic-test, etc.

// Auxillary sorting function used in the std::vector-sorting by R-value.
bool AuxSortClustersBeta(R3BSignalCluster *c1, R3BSignalCluster *c2)
{
  TVector3 pos1;
  TVector3 pos2;
  c1->StartPosition(pos1);
  c2->StartPosition(pos2);
  
  // NOTE: These start positions are in global coordinates.
  Double_t t0 = TMath::Sqrt((fTarget_Xpos_global - fBeam_Xpos_global)*(fTarget_Xpos_global - fBeam_Xpos_global) + (fTarget_Ypos_global - fBeam_Ypos_global)*(fTarget_Ypos_global - fBeam_Ypos_global) + (fTarget_Zpos_global - fBeam_Zpos_global)*(fTarget_Zpos_global - fBeam_Zpos_global));
  t0 = t0/(gBeamBeta*c);
  
  Double_t Travel_Dist1 = TMath::Sqrt((pos1.X() - fTarget_Xpos_global)*(pos1.X() - fTarget_Xpos_global) + (pos1.Y() - fTarget_Ypos_global)*(pos1.Y() - fTarget_Ypos_global) + (pos1.Z() - fTarget_Zpos_global)*(pos1.Z() - fTarget_Zpos_global));
  Double_t time1 = c1->GetStartT() - t0;
  Double_t beta1 = Travel_Dist1/(time1*c);
  
  Double_t Travel_Dist2 = TMath::Sqrt((pos2.X() - fTarget_Xpos_global)*(pos2.X() - fTarget_Xpos_global) + (pos2.Y() - fTarget_Ypos_global)*(pos2.Y() - fTarget_Ypos_global) + (pos2.Z() - fTarget_Zpos_global)*(pos2.Z() - fTarget_Zpos_global));
  Double_t time2 = c2->GetStartT() - t0;
  Double_t beta2 = Travel_Dist2/(time2*c);
  
  // Now continue with the Original code:
  
  Double_t val1 = TMath::Abs(beta1-gBeamBeta)/c1->GetEnergy();
  Double_t val2 = TMath::Abs(beta2-gBeamBeta)/c2->GetEnergy();
  if(val1 < val2) {
    return true;
  } else {
    return false;
  }
}


// The actual reconstruction-function:
Int_t R3B_TradMed_NeutronTracker::AdvancedMethod()
{
    // Loop over clusters
    fNofClusters = fArrayCluster->GetEntries();
    R3BSignalCluster *cluster;
    
    for(Int_t ic = 0; ic < fNofClusters; ic++) 
    {
        // Get pointer to the cluster
        cluster = (R3BSignalCluster*) fArrayCluster->At(ic);
        
        // Process starting from this cluster
        NextIteration(ic, cluster);
    }

    // Sort clusters
    SortClustersBeta();

    // Declare trackers:
    Int_t nOutput = 0;
    Bool_t* mapUsed = new Bool_t[nBarsTotal];
    for(Int_t i = 0; i < nBarsTotal; i++) 
    {
        mapUsed[i] = kFALSE;
    }
    
    // Assign primaries:
    for(Int_t i = 0; i < nNeut; i++) 
    {
        for(Int_t ic = 0; ic < fNofClusters1; ic++) 
        {
            cluster = fVectorCluster.at(ic);

            // Skip found clusters
            if(mapUsed[ic]) 
            {
                continue;
            }

            // Check energy of the cluster
            if(cluster->GetEnergy() < 2.5 && ic > 0) 
            {
                continue;
            }

            // Check if in the beta-window
            TVector3 pos;
            cluster->StartPosition(pos);
      
            // NOTE: This start position is in global coordinates:
            Double_t Travel_Dist = TMath::Sqrt((pos.X() - fTarget_Xpos)*(pos.X() - fTarget_Xpos) + (pos.Y() - fTarget_Ypos)*(pos.Y() - fTarget_Ypos) + (pos.Z() - fTarget_Zpos)*(pos.Z() - fTarget_Zpos));
            Double_t t0 = TMath::Sqrt((fTarget_Xpos - fBeam_Xpos)*(fTarget_Xpos - fBeam_Xpos) + (fTarget_Ypos - fBeam_Ypos)*(fTarget_Ypos - fBeam_Ypos) + (fTarget_Zpos - fBeam_Zpos)*(fTarget_Zpos - fBeam_Zpos));
            t0 = t0/(gBeamBeta*c);
            Double_t time = cluster->GetStartT() - t0;
            Double_t beta = Travel_Dist/(time*c);
      
            // NOTE: Now we have overall beta (coordinate-invariant) and we can test it:
            //if(TMath::Abs(beta-beamBeta) > (0.05*600./beamEnergy) && ic > 0) 
            //{
            //    continue;
            //}
     
            // Create neutron track
            mapUsed[ic] = kTRUE;
            new ((*fNeutHits)[fNofTracks]) R3BSignal(pos.X(),pos.Y(),pos.Z(),cluster->GetStartT());
            nOutput += 1;
            fNofTracks += 1;
            break;
        }
    }
  
    // Fill the true multiplicity:
    // cout << " nNeut = " << nNeut << " | fNofClusters = " << fNofClusters << " | fNofClusters1 = " << fNofClusters1 << " | fNofTracks = " << fNofTracks << "\n";
    FoundMultiplicity->Fill((Int_t) fNofTracks);

    // Finish up:
    delete mapUsed;
    return nOutput;
}

// Sorting-function based on R-value:
void R3B_TradMed_NeutronTracker::SortClustersBeta()
{
  if(0 == fNofClusters) {
    return;
  }

  R3BSignalCluster *cluster;
  for(Int_t ic = 1; ic < fNofClusters; ic++) {
    cluster = (R3BSignalCluster*) fArrayCluster->At(ic);
    if(cluster->IsMarked()) {
      continue;
    }
    fVectorCluster.push_back(cluster);
  }// Clusters

  fNofClusters1 = fVectorCluster.size();
  if(fNofClusters1 > 1) {
    std::sort(fVectorCluster.begin(), fVectorCluster.end(), AuxSortClustersBeta);
  }

  cluster = (R3BSignalCluster*) fArrayCluster->At(0);
  fVectorCluster.insert(fVectorCluster.begin(), cluster);
  fNofClusters1 = fVectorCluster.size();
}

// iteration in the elastic-test:
void R3B_TradMed_NeutronTracker::NextIteration(Int_t curIndex, R3BSignalCluster *curClus)
{
  if(curClus->GetSize() < 2) {
    return;
  }

  // Loop over clusters
  R3BSignalCluster *cluster;
  for(Int_t ic = (curIndex+1); ic < fNofClusters; ic++) {
    cluster = (R3BSignalCluster*) fArrayCluster->At(ic);
    // Check if elastic scattering
    if(IsElastic(curClus, cluster)) {
      // Elastic, so mark the cluster
      cluster->Mark();
      // Stop the loop
      break;
    }
  }// clusters
}

// The actual elastic-test:
Bool_t R3B_TradMed_NeutronTracker::IsElastic(R3BSignalCluster *c1, R3BSignalCluster *c2)
{
    // Set global target coordinates:
  Double_t x0=fTarget_Xpos;
  Double_t y0=fTarget_Ypos;
  Double_t z0=fTarget_Zpos;
  
  // NOTE: t must be interaction: The beta of the beam is stored in gBeamBeta.
  // vacuum speed of light is stored in c.
  Double_t t0 = TMath::Sqrt((fTarget_Xpos - fBeam_Xpos)*(fTarget_Xpos - fBeam_Xpos) + (fTarget_Ypos - fBeam_Ypos)*(fTarget_Ypos - fBeam_Ypos) + (fTarget_Zpos - fBeam_Zpos)*(fTarget_Zpos - fBeam_Zpos));
  t0 = t0/(gBeamBeta*c);

//  Double_t v1xmin,v1ymin,v1zmin,v1xmax,v1ymax,v1zmax;
  Double_t v4xmin,v4ymin,v4zmin;//,v4xmax,v4ymax,v4zmax;
  Double_t v3xmin,v3ymin,v3zmin;//,v3xmax,v3ymax,v3zmax;
//  Double_t v6xmin,v6ymin,v6zmin,v6xmax,v6ymax,v6zmax;

  //    search for scattering
  //    elastic scattering of particle 1 (neutron) on a particle 2 (proton) at rest.
  //    Outgoing particles are 3 (scattered neutron) and 4 (recoil proton).
  //    angle of particles after scattering is theta3 and theta4
  //    K is kinetic energy, p is momentum

  TVector3 pos1;
  TVector3 pos2;

  c1->StartPosition(pos1);

  //    incoming particle
  // vector from previous interaction to present interaction      
  // NOTE: We compute differences between global coordinates ==> Correct!
  Double_t v1x=pos1.X()-x0;
  Double_t v1y=pos1.Y()-y0;
  Double_t v1z=pos1.Z()-z0;
  // time difference and distance between previous interaction to present interaction
  Double_t dt=c1->GetStartT()-t0;	 
  Double_t dr=sqrt(v1x*v1x + v1y*v1y + v1z*v1z);

  Double_t beta1=dr/(dt*c);
  Double_t beta1max=(dr+dio)/dt/c;
  Double_t beta1min=(dr-dio)/dt/c;
  if (beta1max>0.99) beta1max=0.99;
  if (beta1min<0.) beta1min=0.;
  
  Double_t gamma1=1./sqrt(1.-beta1*beta1);  
  Double_t gamma1min=1./sqrt(1.-beta1min*beta1min);  
  Double_t gamma1max=1./sqrt(1.-beta1max*beta1max);  
  Double_t p1=beta1*gamma1*1.*amu;
  Double_t p1min=beta1min*gamma1min*1.*amu;
  Double_t p1max=beta1max*gamma1max*1.*amu;
  Double_t En1=sqrt(p1*p1+amu*amu);
  Double_t En1min=sqrt(p1min*p1min+amu*amu);
  Double_t En1max=sqrt(p1max*p1max+amu*amu);
  Double_t K1=En1-amu;	    
  Double_t K1min=En1min-amu;	    
  Double_t K1max=En1max-amu;	    
  
  // particle 4 is proton
  c1->StopPosition(pos2);
  // NOTE: We compute differences between global coordinates ==> Correct!
  Double_t v4x=(pos2-pos1).X();
  Double_t v4y=(pos2-pos1).Y();
  Double_t v4z=(pos2-pos1).Z();
  //vector perpendicular to scattering plane   
  Double_t v5x=v1y*v4z-v1z*v4y;
  Double_t v5y=v1z*v4x-v1x*v4z;
  Double_t v5z=v1x*v4y-v1y*v4x;
  
  Double_t tempAngle;
  dt=c1->GetStopT()-c1->GetStartT();
  dr=sqrt(v4x*v4x + v4y*v4y + v4z*v4z);
      	    
  Double_t theta4Measured=acos((v1x*v4x + v1y*v4y + v1z*v4z )
			      /sqrt(v1x*v1x + v1y*v1y + v1z*v1z)/sqrt(v4x*v4x + v4y*v4y + v4z*v4z));		
  Double_t theta4Measuredmin=theta4Measured;
  Double_t theta4Measuredmax=theta4Measured;
  if((v4x*v4x + v4y*v4y + v4z*v4z)==0.){
    theta4Measured=1.55;
    theta4Measuredmin=0.;
    theta4Measuredmax=1.55;
  }
  for(Int_t k=1;k<3;k++){
    if (k==1){
      v4xmin=v4x-dio;
    } 
    else{
      v4xmin=v4x+dio;
    }
    for(Int_t l=1;l<3;l++){
      if(l==1){
	v4ymin=v4y-dio;
      }
      else{
	v4ymin=v4y+dio;
      }
      for(Int_t m=1;m<3;m++){
	if(m==1){
	  v4zmin=v4z-dio;
	}
	else{
	  v4zmin=v4z+dio;
	}
	tempAngle=acos((v1x*v4xmin + v1y*v4ymin + v1z*v4zmin) 
		       /sqrt(v1x*v1x + v1y*v1y + v1z*v1z)
		       /sqrt(v4xmin*v4xmin + v4ymin*v4ymin + v4zmin*v4zmin));
	if (tempAngle>theta4Measuredmax) theta4Measuredmax=tempAngle;		     
	if (tempAngle<theta4Measuredmin) theta4Measuredmin=tempAngle;		     
      }
    }
  }
  if (theta4Measuredmax>1.55) theta4Measuredmax=1.55;
  
  // calculate velocity of neutron after scattering
  TVector3 pos3;
	 
  // Vector of scattered neutron    
  // NOTE: We compute differences between global coordinates ==> Correct!
  c2->StartPosition(pos3);
  Double_t v3x=(pos3-pos1).X();
  Double_t v3y=(pos3-pos1).Y();
  Double_t v3z=(pos3-pos1).Z();
  
  Double_t v6x=v1y*v3z-v1z*v3y;
  Double_t v6y=v1z*v3x-v1x*v3z;
  Double_t v6z=v1x*v3y-v1y*v3x;
  
  Double_t theta56=acos((v5x*v6x + v5y*v6y + v5z*v6z )
		       /sqrt(v5x*v5x + v5y*v5y + v5z*v5z)/sqrt(v6x*v6x + v6y*v6y + v6z*v6z));		     
  if((v5x*v5x + v5y*v5y + v5z*v5z)==0. || (v6x*v6x + v6y*v6y + v6z*v6z)==0.){
    theta56=3.14;
  }
  
  Double_t theta3=acos((v1x*v3x + v1y*v3y + v1z*v3z )
		      /sqrt(v1x*v1x + v1y*v1y + v1z*v1z)/sqrt(v3x*v3x + v3y*v3y + v3z*v3z));		     
  Double_t theta3min=theta3;
  Double_t theta3max=theta3;
  
  for(Int_t k=1;k<3;k++){
    if (k==1){
      v3xmin=v3x-dio;
    } 
    else{
      v3xmin=v3x+dio;
    }
    for(Int_t l=1;l<3;l++){
      if(l==1){
	v3ymin=v3y-dio;
      }
      else{
	v3ymin=v3y+dio;
      }
      for(Int_t m=1;m<3;m++){
	if(m==1){
	  v3zmin=v3z-dio;
	}
	else{
	  v3zmin=v3z+dio;
	}
	tempAngle=acos((v1x*v3xmin + v1y*v3ymin + v1z*v3zmin) 
		       /sqrt(v1x*v1x + v1y*v1y + v1z*v1z)
		       /sqrt(v3xmin*v3xmin + v3ymin*v3ymin + v3zmin*v3zmin));
	if (tempAngle>theta3max) theta3max=tempAngle;		     
	if (tempAngle<theta3min) theta3min=tempAngle;		     
      }
      
    }
  }
  
  if (theta3max>1.55) theta3max=1.55;
  
  dt=c2->GetStartT()-c1->GetStartT();
  dr=sqrt(v3x*v3x + v3y*v3y + v3z*v3z);

  Double_t beta3=dr/dt/c;
  Double_t beta3max=(dr+dio)/dt/c;
  Double_t beta3min=(dr-dio)/dt/c;
  
  if (beta3max>0.99) beta3max=0.99;
  if (beta3min<0.) beta3min=0.;
  
  Double_t gamma3=1./sqrt(1.-beta3*beta3);  
  Double_t gamma3min=1./sqrt(1.-beta3min*beta3min);  
  Double_t gamma3max=1./sqrt(1.-beta3max*beta3max);  
  Double_t p3=beta3*gamma3*1.*amu;
  Double_t p3min=beta3min*gamma3min*1.*amu;
  Double_t p3max=beta3max*gamma3max*1.*amu;
  if (p3>p1) p3=p1;
//  Double_t En3=sqrt(p3*p3+amu*amu);
//  Double_t En3min=sqrt(p3min*p3min+amu*amu);
//  Double_t En3max=sqrt(p3max*p3max+amu*amu);
//  Double_t K3=En3-amu;
//  Double_t K3min=En3min-amu;	    
//  Double_t K3max=En3max-amu;	    
  
  Double_t Ma,Mb,Mc,Md,Ka,Thc,Ei,Pa,AA,BB,a,b,cc,Pc1,Pc2;
  Double_t Pd1,Thd1,Ec1,Ed1,Kc1,Kd1,Qsqr1;
//  Double_t Pd2,Thd2,Ec2,Ed2,Kc2,Kd2,Qsqr2;
  Ma = 1.0087*amu;
  Mb = 1.0073*amu;
  Mc = Ma;
  Md = Mb;
  // calculate momentum of scattered proton and neutron
  Ka = K1;
  Thc = theta3;
  Ei=Ma +Mb +Ka;
  Pa = sqrt( Ka * Ka +2. * Ka * Ma );
  AA = Ei * Ei - Md * Md +Mc * Mc - Pa * Pa;
  BB = AA * AA - 4. * Ei * Ei * Mc * Mc;	
  a = 4. * Pa * Pa * cos(Thc) * cos(Thc) - 4. * Ei * Ei;
  b = 4. * AA * Pa * cos(Thc);
  cc = BB;	
  Pc1 = -b/(2. * a)+sqrt( (b * b)/( 4. * a * a) - ( cc / a ) );
  if(Pc1<0.) Pc1=0.;
  Pc2 = -b/(2. * a)-sqrt( (b * b)/( 4. * a * a) - ( cc / a ) );	
  Pd1 = sqrt( Pc1 * Pc1 +Pa * Pa - 2. * Pc1 * Pa * cos(Thc) );
  Thd1 = acos( ( Pc1 * Pc1 - Pd1 * Pd1 - Pa * Pa) / ( -2. * Pd1 * Pa ) ) ;
  Ec1 = sqrt( Pc1 * Pc1 + Ma * Ma );
  Ed1 = sqrt( Pd1 * Pd1 + Mb * Mb );
  Kc1 = Ec1 - Ma;
  Kd1 = Ed1 - Mb;
  Qsqr1 = (- ( Ka - Kc1 ) * (Ka - Kc1 ) +  ( Pa * Pa + Pc1 * Pc1 - 2. * Pa * Pc1 * cos(Thc) ))/197.327/197.327;
  Double_t p3b=Pc1;
//  Double_t K3b=Kc1;
//  Double_t E3b=Ec1;	    
  Double_t theta4=Thd1;
  Double_t p4b=Pd1;
//  Double_t K4b=Kd1;
//  Double_t E4b=Ed1;
  
  Double_t p3bmin=p3b;
  Double_t p3bmax=p3b;
  Double_t p4bmin=p4b;
  Double_t p4bmax=p4b;
  Double_t theta4min=theta4;
  Double_t theta4max=theta4;
  
  // calculate minimum and maximum(within errors) momentum of scattered proton and neutron
  for (Int_t m=1;m<5;m++){
    if(m==1){
      Ka = K1min;
      Thc = theta3min;
    }
    if(m==2){
      Ka = K1min;
      Thc = theta3max;
    }
    if(m==3){
      Ka = K1max;
      Thc = theta3min;
    }
    if(m==4){
      Ka = K1max;
      Thc = theta3max;
    }
    
    Ei=Ma +Mb +Ka;
    Pa = sqrt( Ka * Ka +2. * Ka * Ma );
    AA = Ei * Ei - Md * Md +Mc * Mc - Pa * Pa;
    BB = AA * AA - 4. * Ei * Ei * Mc * Mc;	
    a = 4. * Pa * Pa * cos(Thc) * cos(Thc) - 4. * Ei * Ei;
    b = 4. * AA * Pa * cos(Thc);
    cc = BB;	
    Pc1 = -b/(2. * a)+sqrt( (b * b)/( 4. * a * a) - ( cc / a ) );
    if (((b * b)/( 4. * a * a) - ( cc / a ))<0.) Pc1=0.;
    if(Pc1<0.) Pc1=0.;
    Pc2 = -b/(2. * a)-sqrt( (b * b)/( 4. * a * a) - ( cc / a ) );	
    Pd1 = sqrt( Pc1 * Pc1 +Pa * Pa - 2. * Pc1 * Pa * cos(Thc) );
    Thd1 = acos( ( Pc1 * Pc1 - Pd1 * Pd1 - Pa * Pa) / ( -2. * Pd1 * Pa ) ) ;
    
    if(Pc1<p3bmin) p3bmin=Pc1;
    if(Pc1>p3bmax) p3bmax=Pc1;
    if(Pd1<p4bmin) p4bmin=Pd1;
    if(Pd1>p4bmax) p4bmax=Pd1;
    if(Thd1<theta4min) theta4min=Thd1;
    if(Thd1>theta4max) theta4max=Thd1;
  }
  
  Ec1 = sqrt( p3bmin * p3bmin + Ma * Ma );
  Kc1 = Ec1 - Ma;
  Ed1 = sqrt( p4bmin * p4bmin + Mb * Mb );
  Kd1 = Ed1 - Mb;
  Qsqr1 = (- ( Ka - Kc1 ) * (Ka - Kc1 ) +  ( Pa * Pa + Pc1 * Pc1 - 2. * Pa * Pc1 * cos(Thc) ))/197.327/197.327;
//  Double_t K3bmin=Kc1;
//  Double_t E3bmin=Ec1;	    
  Double_t K4bmin=Kd1;
//  Double_t E4bmin=Ed1;
  Ec1 = sqrt( p3bmax * p3bmax + Ma * Ma );
  Kc1 = Ec1 - Ma;
  Ed1 = sqrt( p4bmax * p4bmax + Mb * Mb );
  Kd1 = Ed1 - Mb;
  Qsqr1 = (- ( Ka - Kc1 ) * (Ka - Kc1 ) +  ( Pa * Pa + Pc1 * Pc1 - 2. * Pa * Pc1 * cos(Thc) ))/197.327/197.327;
  
//  Double_t K3bmax=Kc1;
//  Double_t E3bmax=Ec1;	    
  Double_t K4bmax=Kd1;
//  Double_t E4bmax=Ed1;
  
  
  // decide if Cluster comes from scattered neutron or another neutron is needed!
  Double_t protonEnergy=8.76839+4.13858*c1->GetEnergy()-0.00337368*c1->GetEnergy()*c1->GetEnergy();
  if(p3bmax>p3min && p3bmin<p3max && theta4max>theta4Measuredmin && theta4min<theta4Measuredmax &&
     K4bmax>protonEnergy && K4bmin<protonEnergy && theta56*180./3.14>120.) {
    return kTRUE;
  } else {
    return kFALSE;
  }
}
