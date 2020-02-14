How to analyze the data from the 7Li(p,n)7Be experiment.

1) Train your network with the Train.sh script.
2) Validate your network with the Validate.sh script
3) Process the experimental data twice. Use the 
   Experiment.sh script. for this. Do it once
   for the actual data and then rename the output 
   folder DNN_Experiment_Signal.
   Then, the second time for empty target runs.
   Rename this folder DNN_Experiment_Background afterwards.
4) For both experimental runs and the validation run,
   run the R3BRoot macro GetExcitationEnergies.cpp
5) Open ProcessHists.cpp and adapt the two boolians
   at the beginning and the outputpath in the code
   to your needs. Than, run the code ONCE for
   the experimental data and ONCE for the validation
   run with
   make clean
   make
   ./ProcessHists
6) Compute the cross sections with the macro Compute_CrossSections.cpp
   (this needs the proper input-file to work).
   As the macro is written, it will do the gauching
   to the validation run and the exp. cross section extraction in
   a single run. However, this will NOT give you the 
   physics list errors. NOTE: Manually set the MC_Epsilon
   to the 1n-->1n neutron separation matrix element of perfect reconstruction.
7) To estimate the physics list errors, do steps 1 & 3 for both
   QGSP_INCLXX_HP & QGSP_BERT_HP. For each of these two options,
   validate twice (once for each list). Then, use the macro
   Compute_CrossSections.cpp (only the first part) to compute
   the gauching efficiencies. This way, one gets 4 sets of
   efficiencies. Compute mean & deviations. Store this 
   in a separate file. The deviations are your physics list errors
   and the means are the efficiencies to use for the exp. cross sections.
8) To estimate the statistical errors, repeat steps 1,2,3 twice
   for the same physics list & compare efficiencies like in step 7.
   Quadratically add the differences to the computed errors. This 
   is the full statistical error: sqrt(n) (the output errors)
   and the difference (different DNN training).
9) Write your custom efficiency file (examples are in 
   ./Efficiencies/Ready_Use_Files directory.
10) Use the second part of Compute_CrossSections.cpp to compute
    the exp. cross sections. Be sure to call 
    CrossExp->UseCustomEfficiencies("Yourfile.txt");
    to ensure that your own efficiencies were used properly.
11) Do the FOLD-calculation. This is easy for the steps
    wsaw, normod, fold, dwhi_surf & display. Each time, 
    just run the ./run.sh script inside.
12) Do the MDA. Open MDA.cpp and adapt the global
    variables to your needs to select the proper file
    and method. Then simply run it by:
    make clean
    make 
    ./MDA
13) Done!
