void R3BNeulandCADDigitizer::EliminateParticle(TString const Particle)
{
    // Eliminates the contributions in the digitizer from NeuLAND points
    // that correspond to this particle type.
    
    // Only use particles that we know:
    if (Particle=="proton") {ForBiddenTracks.push_back(2212);           ForBiddenParticles.push_back(Particle);}
    if (Particle=="neutron") {ForBiddenTracks.push_back(2112);          ForBiddenParticles.push_back(Particle);}
    if (Particle=="electron") {ForBiddenTracks.push_back(11);           ForBiddenParticles.push_back(Particle);}
    if (Particle=="positron") {ForBiddenTracks.push_back(-11);          ForBiddenParticles.push_back(Particle);}
    if (Particle=="gamma") {ForBiddenTracks.push_back(22);              ForBiddenParticles.push_back(Particle);}
    if (Particle=="neutrinos") {ForBiddenTracks.push_back(12);          ForBiddenParticles.push_back(Particle);
                                ForBiddenTracks.push_back(-12); 
                                ForBiddenTracks.push_back(14); 
                                ForBiddenTracks.push_back(-14); 
                                ForBiddenTracks.push_back(16);
                                ForBiddenTracks.push_back(-16);}
    if (Particle=="pions") {ForBiddenTracks.push_back(211);             ForBiddenParticles.push_back(Particle);
                            ForBiddenTracks.push_back(-211);
                            ForBiddenTracks.push_back(111);}
    if (Particle=="muons") {ForBiddenTracks.push_back(13);              ForBiddenParticles.push_back(Particle);
                            ForBiddenTracks.push_back(-13);}
    if (Particle=="deuteron") {ForBiddenTracks.push_back(1000010020);   ForBiddenParticles.push_back(Particle);}
    if (Particle=="triton") {ForBiddenTracks.push_back(1000010030);     ForBiddenParticles.push_back(Particle);}
    if (Particle=="alpha") {ForBiddenTracks.push_back(1000020040);      ForBiddenParticles.push_back(Particle);}
    if (Particle=="C-ions") {ForBiddenTracks.push_back(1000060100);     ForBiddenParticles.push_back(Particle);
                             ForBiddenTracks.push_back(1000060110);
                             ForBiddenTracks.push_back(1000060120);
                             ForBiddenTracks.push_back(1000060130);
                             ForBiddenTracks.push_back(1000060140);
                             ForBiddenTracks.push_back(1000060150);}
    if (Particle=="Fragments") {ForBiddenTracks.push_back(0);           ForBiddenParticles.push_back(Particle);}
    
    // Done.
}

