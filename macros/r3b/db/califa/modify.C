Int_t  modify(string text)
{
  Int_t seqno;
  string type, file;
  stringstream infoline;

  infoline<<text;

  infoline >> type >> seqno >> file;
  
  infoline.clear();

  vector<string> data;
  string datasegments, line;

  ifstream infile(file.c_str());
  while (getline(infile,line)) {
   stringstream dataline;
   dataline<<line;
   while (getline(dataline,datasegments,'\t')) {
    if(datasegments=="null")
     data.push_back("");
    else
     data.push_back(datasegments);
   }
   dataline.clear();
  }

  // Generate a unique RunID
  FairRunIdGenerator runID;
  UInt_t runId =  runID.generateId();
   
  // Create the Runtime Database ( parameter manager class )
  FairRuntimeDb* db = FairRuntimeDb::instance();
  // Create in memory the relevant container
  //R3BCaloGeometryPar* par = (R3BCaloGeometryPar*)(db->getContainer("CaloGeometryPar"));

  // Set the SQL based IO as second input
  FairParTSQLIo* input_db = new FairParTSQLIo();  
  input_db->SetVerbosity(1);

  // Set Global SeqNo ( Replication Global Index Base )
  //inp2->SetGlobalSeqNoIn();

  // Shutdown Mode ( True, False )
  input_db->SetShutdown(kTRUE);


  // Open first input
  input_db->open();
  db->setFirstInput(input_db);

  // Set the output=input
  db->setOutput(input_db); 

  cout << "-I- CALIFA GEO PRIMING:  Opening database + Meta Data done via TSQLIO .. " << endl;

  switch(atoi(type.c_str()))
  {
   case 1:
  	// Store the addon board
	R3BCaloAddonBoardPar add;
  	add.SetId(data[0]);
  	add.SetOrigin(data[1]);
  	add.SetLocation(data[2]);
  	add.SetObservation(data[3]);
	add.store(runId);
	add.update(seqno);
	break;
   case 2:
  	// Store the amplifier
  	R3BCaloAmplifierPar amp;
  	amp.SetId(data[0]);
  	amp.SetSerialNumber(data[1]);
  	amp.SetOrigin(data[2]);
  	amp.SetLocation(data[3]);
  	amp.SetObservation(data[4]);
	amp.store(runId);
	amp.update(seqno);
	break;
   case 3:
	// Store the APD
 	R3BCaloAPDPar apd;
  	apd.SetId(atoi(data[0].c_str()));
  	apd.SetIdHamamatsu(atoi(data[1].c_str()));
  	apd.SetVrHamamatsu(atoi(data[2].c_str()));
  	apd.SetVBreakdownHamamatsu(atoi(data[3].c_str()));
  	apd.SetDarkCurrentHamamatsu(atof(data[4].c_str()));
  	apd.SetOrigin(data[5]);
  	apd.SetLocation(data[6]);
  	apd.SetObservation(data[7]);
  	apd.store(runId);
	apd.update(seqno);
	break;
   case 4:
	// Store the APD Lab
  	R3BCaloAPDLabPar apdLab;
  	apdLab.SetId(atoi(data[0].c_str()));
  	apdLab.SetVrG40(atof(data[1].c_str()));
  	apdLab.SetVrG50(atof(data[2].c_str()));
 	apdLab.SetVrG60(atof(data[3].c_str()));
  	apdLab.SetThreshold(atof(data[4].c_str()));
  	apdLab.SetThresholdGain(atof(data[5].c_str()));
  	apdLab.SetSlopeTemperature(data[6]);
  	apdLab.SetDarkCurrent(atof(data[7].c_str()));
  	apdLab.SetDarkCurrentGain(atof(data[8].c_str()));
  	apdLab.SetDarkCurrentTemp(atof(data[9].c_str()));
  	apdLab.SetResolutionLED(atof(data[10].c_str()));
  	apdLab.SetResolutionLEDTemp(atof(data[11].c_str()));
  	apdLab.SetResolutionFe(atof(data[12].c_str()));
  	apdLab.SetResolutionFeGain(atof(data[13].c_str()));
 	apdLab.SetChannelFe(atof(data[14].c_str()));
  	apdLab.SetChannelFeGain(atof(data[15].c_str()));
  	apdLab.SetChannelFeTemp(atof(data[16].c_str()));
  	apdLab.SetLEDVoltage(atoi(data[17].c_str()));
  	apdLab.SetPreamplifier(data[18]);
  	apdLab.SetAmplifier(data[19]);
  	apdLab.SetMCAScale(atoi(data[20].c_str()));
  	apdLab.SetTimeMeasurement(atoi(data[21].c_str()));
  	apdLab.SetShapingTimeMeasurement(atoi(data[22].c_str()));
  	apdLab.SetDateMeasurement(data[23]);
	apdLab.SetObservation(data[24]);
	apdLab.store(runId);
	break;
   case 5:
	// Store the crystal
  	R3BCaloCrystalPar crystal;
  	crystal.SetId(atoi(data[0].c_str()));
  	crystal.SetIdAmcrys(atoi(data[1].c_str()));
  	crystal.SetEnergySource(data[2]);
        crystal.SetAbsoluteLO(atof(data[3].c_str()));
        crystal.SetLO(atof(data[4].c_str()));
	crystal.SetEnergyResolution(atof(data[5].c_str()));
        crystal.SetOrigin(data[6]);
  	crystal.SetLocation(data[7]);
	crystal.SetObservation(data[8]);
  	crystal.store(runId);
	crystal.update(seqno);
	break;
   case 6:
	// Store the crystal lab
  	R3BCaloCrystalLabPar crystalLab;
  	crystalLab.SetId(atoi(data[0].c_str()));
  	crystalLab.SetResolution(atof(data[1].c_str()));
  	crystalLab.SetChannelCo1(atof(data[2].c_str()));
  	crystalLab.SetChannelCo1Gain(atof(data[3].c_str()));
  	crystalLab.SetChannelCo1Temp(atof(data[4].c_str()));
  	crystalLab.SetResolutionCo1(atof(data[5].c_str()));
  	crystalLab.SetResolutionCo1Gain(atof(data[6].c_str()));
  	crystalLab.SetResolutionCo1Temp(atof(data[7].c_str()));
  	crystalLab.SetChannelCo2(atof(data[8].c_str()));
  	crystalLab.SetChannelCo2Gain(atof(data[9].c_str()));
  	crystalLab.SetChannelCo2Temp(atof(data[10].c_str()));
  	crystalLab.SetResolutionCo2(atof(data[11].c_str()));
	crystalLab.SetResolutionCo2Gain(atof(data[12].c_str()));
  	crystalLab.SetResolutionCo2Temp(atof(data[13].c_str()));
  	crystalLab.SetChannelCs(atof(data[14].c_str()));
  	crystalLab.SetChannelCsGain(atof(data[15].c_str()));
  	crystalLab.SetChannelCsTemp(atof(data[16].c_str()));
  	crystalLab.SetResolutionCs(atof(data[17].c_str()));
  	crystalLab.SetResolutionCsGain(atof(data[18].c_str()));
  	crystalLab.SetResolutionCsTemp(atof(data[19].c_str()));
  	crystalLab.SetChannelBs1(atof(data[20].c_str()));
  	crystalLab.SetChannelBs1Gain(atof(data[21].c_str()));
	crystalLab.SetChannelBs1Temp(atof(data[22].c_str()));
  	crystalLab.SetResolutionBs1(atof(data[23].c_str()));
  	crystalLab.SetResolutionBs1Gain(atof(data[24].c_str()));
  	crystalLab.SetResolutionBs1Temp(atof(data[25].c_str()));
  	crystalLab.SetChannelBs2(atof(data[26].c_str()));
  	crystalLab.SetChannelBs2Gain(atof(data[27].c_str()));
  	crystalLab.SetChannelBs2Temp(atof(data[28].c_str()));
  	crystalLab.SetResolutionBs2(atof(data[29].c_str()));
  	crystalLab.SetResolutionBs2Gain(atof(data[30].c_str()));
  	crystalLab.SetResolutionBs2Temp(atof(data[31].c_str()));
  	crystalLab.SetPreamplifier(data[32]);
  	crystalLab.SetAmplifier(data[33]);
  	crystalLab.SetMCAScale(atoi(data[34].c_str()));
  	crystalLab.SetTimeMeasurement(atoi(data[35].c_str()));
  	crystalLab.SetShapingTimeMeasurement(atoi(data[36].c_str()));
  	crystalLab.SetDateMeasurement(data[37]);
	crystalLab.SetObservation(data[38]);
  	crystalLab.store(runId);
	break;
   case 7:
        // Store the detection unit
	R3BCaloDetectionUnitPar detun;
	detun.SetId(atoi(data[0].c_str()));
	detun.SetTypeId(atoi(data[1].c_str()));
	detun.SetCrystal(atoi(data[2].c_str()));
	detun.SetAPD(atoi(data[3].c_str()));
	detun.SetOrigin(data[4]);
	detun.SetLocation(data[5]);
	detun.SetPosition(atoi(data[6].c_str()));
	detun.SetObservation(data[7]);
	detun.store(runId);
	detun.update(seqno);
	break;
   case 8:
        // Store the detector
	R3BCaloDetectorPar det;
	det.SetId(data[0]);
	det.SetOrigin(data[1]);
	det.SetLocation(data[2]);
	det.SetObservation(data[3]);
	det.store(runId);
	det.update(seqno);
	break;
   case 9:
	// Store the FEBEX
  	R3BCaloFEBEXPar febex;
  	febex.SetId(data[0]);
	febex.SetPreamplifier(data[1]);
  	febex.SetOrigin(data[2]);
  	febex.SetLocation(data[3]);
  	febex.SetObservation(data[4]);
  	febex.store(runId);
	febex.update(seqno);
	break;
   case 10:
        // Store the preamplifier
	R3BCaloPreamplifierPar preamp;
  	preamp.SetId(data[0]);
  	preamp.SetSerialNumber(data[1]);
  	preamp.SetRange(data[2]);
  	preamp.SetOrigin(data[3]);
  	preamp.SetLocation(data[4]);
  	preamp.SetObservation(data[5]);
  	preamp.store(runId);
	preamp.update(seqno);
	break;
   default:
	cout << "Type not recognized" << endl;
	break;
  }
  // Write just added container
  db->addRun(runId,-1);
  //db->writeContainer(par, db->getCurrentRun());


  // FairRuntimeDB deletion
  if (db) delete db;
  return 0;
}
