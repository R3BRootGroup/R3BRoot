#include "R3BPrimaryGenerator.h"
#include "TSystem.h"
#include "TString.h"
#include "TRandom.h"

R3BPrimaryGenerator::R3BPrimaryGenerator() : FairPrimaryGenerator()
{
}

R3BPrimaryGenerator::R3BPrimaryGenerator(const char* name, const char* title)
  : FairPrimaryGenerator( name, title )
{
}

R3BPrimaryGenerator::~R3BPrimaryGenerator() {
}

void R3BPrimaryGenerator::AddTrack(Int_t pdgid, Double_t px, Double_t py,
				   Double_t pz, Double_t vx, Double_t vy,
				   Double_t vz, Int_t parent,Bool_t wanttracking,Double_t e) {
  static Int_t iprim=0;
  Double_t mass; 
  //Int_t warn=0;
  iprim++;
  cout<<" primary# "<<iprim<<endl;
  //Int_t Z,A,pdg1;
  //Bool_t flag;

    // ---> Add event vertex to track vertex
    vx += fVertex.X();
    vy += fVertex.Y();
    vz += fVertex.Z();

    // ---> Convert K0 and AntiK0 into K0s and K0L
    if ( pdgid == 311 || pdgid == -311 ) {
	Double_t test = gRandom->Uniform(0.,1.);
	if (test >= 0.5) pdgid = 310;    // K0S
	else             pdgid = 130;    // K0L
    }

    // ---> Check whether particle type is in PDG Database
    TDatabasePDG* pdgBase = TDatabasePDG::Instance();
    if ( ! pdgBase ) Fatal("R3BPrimaryGenerator",
			   "No TDatabasePDG instantiated");
    TParticlePDG* pdgPart = pdgBase->GetParticle(pdgid);

     if(pdgid==1114||pdgid==2114||pdgid==2214||pdgid==2224||pdgid==0){
      cout<<"delta found!"<<endl;
      return;}

    if ( ! pdgPart ) {
       cout<<"pdg that is not found....."<<pdgid<<endl;
       
    if(pdgid>1000000000&&pdgid<1000010000){
      cout<<"problematic cluster found!"<<endl;
      return;}
    
	this->ExtendIon2(pdgid);
	//if(flag){return;}

	if( e<0){
	    cerr << "\033[5m\033[31m -E R3BPrimaryGenerator: PDG code " << pdgid << " not found in database.\033[0m " << endl;
	    cerr << "\033[5m\033[31m -E R3BPrimaryGenerator: Discarding particle \033[0m " << endl;
	    cerr << "\033[5m\033[31m -E R3BPrimaryGenerator: now MC Index is corrupted \033[0m " << endl;
	    //return;

	}else{
	    cout << "\033[5m\033[31m -W R3BPrimaryGenerator: PDG code " << pdgid << " not found in database. This warning can be savely ignored.\033[0m " << endl;
	}
    }

    // ---> Get mass and calculate energy of particle
    if(e<0){
	mass = pdgBase->GetParticle(pdgid)->Mass();

	e = TMath::Sqrt( px*px + py*py + pz*pz + mass*mass );
    }// else, use the value of e given to the function

    // ---> Set all other parameters required by PushTrack
    Int_t    doTracking =  0;   // Go to tracking
    if(fdoTracking && wanttracking) doTracking = 1 ;
    Int_t    dummyparent     = -1;   // Primary particle (now the value is -1 by default)
    Double_t tof        =  0.;  // Time of flight
    Double_t polx       =  0.;  // Polarisation
    Double_t poly       =  0.;
    Double_t polz       =  0.;
    Int_t    ntr        =  0;   // Track number; to be filled by the stack
    Double_t weight     =  1.;  // Weight
    Int_t    status     =  0;   // Generation status

    if( parent!=-1) parent+=fMCIndexOffset;// correct for tracks which are in list before generator is called
    // Add track to stack
    fStack->PushTrack(doTracking, dummyparent, pdgid, px, py, pz, e, vx, vy, vz,
		      tof, polx, poly, polz, kPPrimary, ntr, weight, status, parent);
    fNTracks++;

}


Bool_t R3BPrimaryGenerator::ExtendIon(Int_t pdgid){

    TDatabasePDG* db=TDatabasePDG::Instance();
    static Int_t extend;
    Int_t a,z,isstable,ndecay,ndau;
    Int_t decay,dummy,pdgid2,w;
    char c[512];
    TMCParticleType partType = kPTIon;
    Double_t amu=0.931494028;
    Double_t massGeV;

    char* pnames[]={(char*)"H",(char*)"He",(char*)"Li"};
    Double_t mass=amu;
    Int_t isospin=2;
    Int_t isospin3=2;
    Int_t spin=2;
    Int_t parity=+1;
    Double_t lifetime=1e+10;
    Double_t width=1e-10;
    Bool_t stable=kTRUE;

    Int_t mode[6][3]={{0}};
    Float_t bratio[6]={0.0};

    if(extend==0){

       cout<<"-I- R3BPrimaryGenerator: PDG update instantiated!"<<endl;

       TString WorkDir = gSystem->Getenv("VMCWORKDIR");
       TString fFilename  = WorkDir + "/input/chimera/plist.in";

       FILE* in=fopen(fFilename.Data(),"r");
       if(fgets(c,200,in)) {;}
	//cout<<"line0: "<<c<<endl;

	while((c[0]=getc(in))!=EOF){
	    if(c[0]!='#'){
		ungetc(c[0],in);
		if(fscanf(in,"%d",&a)){;}
	if(fscanf(in,"%d",&z)){;}
		if(fscanf(in,"%lf",&mass)){;}
		if(fscanf(in,"%d",&isospin)){;}
		if(fscanf(in,"%d",&isospin3)){;}
		if(fscanf(in,"%d",&spin)){;}
		if(fscanf(in,"%d",&parity)){;}
		if(fscanf(in,"%lf",&lifetime)){;}
		if(fscanf(in,"%lf",&width)){;}
		if(fscanf(in,"%d",&isstable)){;}
		if(fscanf(in,"%d",&ndecay)){;}
		if(fgets(c,200,in)){;}

		cout<< "-I- R3BPrimaryGenerator adding new Particle Definition: "<< endl <<
		    "    A= "<<a<<" Z= "<<z<<" lifetime= "<<lifetime<<" width= "<<width<<" ndecays= "<<ndecay<<endl;

		if(isstable==0){stable=kFALSE;}
		else
		{stable=kTRUE;}

		pdgid2=1000000000+z*10000+a*10;
		if(pdgid==pdgid2)w=1;
		massGeV=mass*amu;

		//root PDG update
		db->AddParticle(Form("%s%d",pnames[z-1],a),Form("%s%d",pnames[z-1],a),massGeV,stable,width,z,"ion",pdgid2);



		//VMC PDG update
		gMC->DefineParticle(pdgid2,Form("%s%d",pnames[z-1],a), partType,massGeV
			    ,z, lifetime,"nucleus", width,spin, parity, 0,
				    isospin3, isospin, 0, 0, 1, stable);

		//cout<<"particle(ion) defined!"<<endl;

		if(fgets(c,200,in)){;}

		if(ndecay){

		    decay=0;
		    while(((c[0]=getc(in))!=EOF) && (decay<ndecay)){
			if(c[0]!='#'){
			    ungetc(c[0],in);
			    if(fscanf(in,"%d",&dummy)){;}
			    if(fscanf(in,"%f",&bratio[decay])){;}
			    if(fscanf(in,"%d",&ndau)){;}
			    cout<<"decay# "<<decay<<"number of daughters "<<ndau<<endl;
			    for(Int_t idau=0;idau<ndau;idau++){
				if(fscanf(in,"%d",&mode[decay][idau])){;}
				cout<<"daughter= "<<mode[decay][idau]<<endl;
			    }

			    //root PDG decay update
			    TParticlePDG* P=db->GetParticle(pdgid2);
			    for(decay=0;decay<ndecay;decay++){
				P->AddDecayChannel(111,bratio[decay],ndau,mode[decay]);
			    }
			    //VMC decay update
			    gMC->SetDecayMode(pdgid2,bratio,mode);
			    cout<<"decay mode set!"<<endl;
			    cout<<"decay# "<<decay<<" bratio= "<<bratio[decay]<<endl;
			    decay++;

			}
			if(fgets(c,200,in)){;}
		    }
		}

	    }
	    if(fgets(c,200,in)){;}
	}
	fclose(in);
	if(w==0){
	    cout<<"-W- R3BPrimaryGenerator: PDG base needs updating!!! Particle "<<pdgid<<" not in base"<<endl;
	    extend++;
	    return kTRUE;
	}
	else
	{
	    cout<<"-W- R3BPrimaryGenerator PDG updating completed!"<<endl;
	    extend++;
	    WorkDir = gSystem->Getenv("VMCWORKDIR");
	    TString wFilename  = WorkDir + "/input/chimera/ionlist.in";

	    WriteTable(wFilename.Data());
	    return kFALSE;}

    }else
    {
	cout<<"-W- R3BPrimaryGenerator PDG base needs updating!!! Particle "<<pdgid<<" not in base"<<endl;
	extend++;
	return kTRUE;

    }

}

void R3BPrimaryGenerator::ExtendIon2(Int_t pdgid){
    Int_t pdg1,pdg2;
    Int_t Z,A;
    //Double_t lifetime;
    char* pname;
    Double_t amu=0.931494028;

   
    TDatabasePDG* db=TDatabasePDG::Instance();
    cout<<"pdgid= "<<pdgid<<endl;
    pdg1=pdgid-1000000000;
    cout<<"pdg1= "<<pdg1<<endl;
    Z=TMath::Nint(pdg1/10000.);
    pdg2=pdg1-Z*10000;
    cout<<"pdg2= "<<pdg2<<endl;
    A=TMath::Nint(pdg2/10.);
    cout<<"Z= "<<Z<<" A= "<<A<<endl;
    pname=Form("A%d_Z%d",A,Z);
    cout<<pname<<endl;
  
    db->AddParticle(Form("%s_l",pname),Form("%s_l",pname),A*amu,1,0.0,Z*3,"Ion",pdgid+1);
    gMC->DefineIon(Form("A%d_Z%d",A,Z),Z,A,Z,A*amu);

    
    cout<<" mass= "<<A*amu<<endl;
    TString name=gMC->ParticleName(pdgid);
    cout<<"name is "<<name<<endl;

}

void R3BPrimaryGenerator::WriteTable(const char* filename){
    FILE *file = fopen(filename,"w");
    if (file == 0) {
	Error("WritePDGTable","Could not open PDG particle file %s",filename);
    }

    fprintf(file,"#--------------------------------------------------------------------\n");
    fprintf(file,"#    i   NAME.............  KF AP   CLASS      Q        MASS     WIDTH  2*I+1 I3 2*S+1 FLVR TrkCod N(dec)\n");
    fprintf(file,"#--------------------------------------------------------------------\n");
    TDatabasePDG* db=TDatabasePDG::Instance();
    const THashList* plist;
    plist=db->ParticleList();
    Int_t nparts=plist->GetEntries();
    for(Int_t i=521;i<nparts;++i) {
	TParticlePDG *p = dynamic_cast<TParticlePDG*>(plist->At(i));
	if(!p) continue;

	Int_t ich=i+1;
	Int_t kf=p->PdgCode();
	fprintf(file,"%5i %-20s %- 6i ", ich, p->GetName(), kf);

	Int_t anti=p->AntiParticle() ? 1:0;
	if(kf<0) {
	    for(Int_t j=0;j<nparts;++j) {
		TParticlePDG *dummy = dynamic_cast<TParticlePDG*>(plist->At(j));
		if(dummy==p->AntiParticle()) {
		    anti=j+1;
		    break;
		}
	    }
	    fprintf(file,"%i 0\n",anti);
	    continue;
	}

	fprintf(file,"%i ",anti);
	fprintf(file,"%i ",100);
	fprintf(file,"%s ",p->ParticleClass());
	fprintf(file,"% i ",(Int_t)p->Charge());
	fprintf(file,"%.5le ",p->Mass());
	fprintf(file,"%.5le ",p->Width());
	fprintf(file,"%i ",(Int_t)p->Isospin());
	fprintf(file,"%i ",(Int_t)p->I3());
	fprintf(file,"%i ",(Int_t)p->Spin());
	fprintf(file,"%i ",-1);
	fprintf(file,"%i ",p->TrackingCode());
	Int_t nch=p->NDecayChannels();
	fprintf(file,"%i\n",nch);
	if(nch==0) {
	    continue;
	}
	fprintf(file,"#----------------------------------------------------------------------\n");
	fprintf(file,"#    decay  type(PY6)    BR     Nd         daughters(codes, then names)\n");
	fprintf(file,"#----------------------------------------------------------------------\n");
	for(Int_t j=0;j<nch;++j) {
	    TDecayChannel *dc=p->DecayChannel(j);

	    fprintf(file,"%9i   ",dc->Number()+1);
	    fprintf(file,"%3i   ",dc->MatrixElementCode());
	    fprintf(file,"%.5le  ",dc->BranchingRatio());
	    Int_t ndau=dc->NDaughters();
	    fprintf(file,"%3i       ",ndau);
	    for (int idau=0; idau<ndau; idau++) {
		fprintf(file,"%- 6i ",dc->DaughterPdgCode(idau));
	    }
	    for (int idau=0; idau<ndau; idau++) {
		TParticlePDG *dummy=db->GetParticle(dc->DaughterPdgCode(idau));
		if(dummy)
		    fprintf(file,"%-10s ",dummy->GetName());
		else
		    fprintf(file,"%-10s ","???");
	    }
	    fprintf(file,"\n");
	}
    }
    fclose(file);
}

ClassImp(R3BPrimaryGenerator)

