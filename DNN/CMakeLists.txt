# Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual")Z

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/DNN
${R3BROOT_SOURCE_DIR}/DNN/InputClass
${R3BROOT_SOURCE_DIR}/DNN/ASCIIGen
${R3BROOT_SOURCE_DIR}/DNN/Masses
${R3BROOT_SOURCE_DIR}/DNN/FairTasks
${R3BROOT_SOURCE_DIR}/DNN/Digitizer
${R3BROOT_SOURCE_DIR}/DNN/Traditional
${R3BROOT_SOURCE_DIR}/DNN/Analysis
${R3BROOT_SOURCE_DIR}/DNN/Plots
${R3BROOT_SOURCE_DIR}/DNN/MT
${R3BROOT_SOURCE_DIR}/DNN/Objects
${R3BROOT_SOURCE_DIR}/DNN/TextFiles
${R3BROOT_SOURCE_DIR}/DNN/Reconstruction
${R3BROOT_SOURCE_DIR}/DNN/Translator
${R3BROOT_SOURCE_DIR}/DNN/MonteCarlo
${R3BROOT_SOURCE_DIR}/DNN/MonteCarlo/CADNeuLAND
${R3BROOT_SOURCE_DIR}/DNN/MonteCarlo/CADVETO
${R3BROOT_SOURCE_DIR}/DNN/MonteCarlo/CADNEBULA
${R3BROOT_SOURCE_DIR}/DNN/MonteCarlo/CADNEBULAVETO
${R3BROOT_SOURCE_DIR}/DNN/Experiment
${R3BROOT_SOURCE_DIR}/DNN/Experiment/Template
${R3BROOT_SOURCE_DIR}/DNN/Experiment/SAMURAI_7LiPN
${R3BROOT_SOURCE_DIR}/DNN/Experiment/TetraNeutron
${R3BROOT_SOURCE_DIR}/DNN/Presentation
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
InputClass/R3BInputClass.cxx
ASCIIGen/R3BASCIIFileGenerator.cxx
MonteCarlo/R3BCADDetector.cxx 
MonteCarlo/CADNeuLAND/R3BCADNeulandPoint.cxx 
MonteCarlo/CADNeuLAND/R3BCADNeuland.cxx
MonteCarlo/CADNeuLAND/R3BCADNeulandGeoPar.cxx
MonteCarlo/CADNeuLAND/R3BCADNeulandContFact.cxx
MonteCarlo/CADNeuLAND/R3BCADNeulandNeutron2DPar.cxx
MonteCarlo/CADVETO/R3BCADVETOPoint.cxx 
MonteCarlo/CADVETO/R3BCADVETO.cxx
MonteCarlo/CADVETO/R3BCADVETOGeoPar.cxx
MonteCarlo/CADVETO/R3BCADVETOContFact.cxx
MonteCarlo/CADVETO/R3BCADVETONeutron2DPar.cxx
MonteCarlo/CADNEBULA/R3BCADNEBULAPoint.cxx 
MonteCarlo/CADNEBULA/R3BCADNEBULA.cxx
MonteCarlo/CADNEBULA/R3BCADNEBULAGeoPar.cxx
MonteCarlo/CADNEBULA/R3BCADNEBULAContFact.cxx
MonteCarlo/CADNEBULA/R3BCADNEBULANeutron2DPar.cxx
MonteCarlo/CADNEBULAVETO/R3BCADNEBULAVETOPoint.cxx 
MonteCarlo/CADNEBULAVETO/R3BCADNEBULAVETO.cxx
MonteCarlo/CADNEBULAVETO/R3BCADNEBULAVETOGeoPar.cxx
MonteCarlo/CADNEBULAVETO/R3BCADNEBULAVETOContFact.cxx
MonteCarlo/CADNEBULAVETO/R3BCADNEBULAVETONeutron2DPar.cxx
Masses/Nucleus.cxx
Masses/Nuclei.cxx
Objects/ObjDouble.cxx
Objects/ObjInteger.cxx
Objects/ObjBoolian.cxx
Translator/R3BNeuLANDTranslator.cxx
FairTasks/R3BSignal.cxx
FairTasks/R3BSignalCluster.cxx
FairTasks/R3BSignalCluster_NoVect.cxx
FairTasks/R3BSimpleCluster.cxx
FairTasks/R3BMerger.cxx
FairTasks/R3BHandShakeClusterFinder.cxx
FairTasks/R3BHandShakeClusterFinder_Jan.cxx
FairTasks/R3BClusterTranslator.cxx
FairTasks/R3B_TradMed_NeutronTracker.cxx
FairTasks/R3BRecoTranslator.cxx
FairTasks/R3BMultReader.cxx
FairTasks/R3BBetaReconstructor.cxx
FairTasks/R3BSingleReconstructor.cxx
Traditional/R3BTradMethClass.cxx
Digitizer/R3BNeulandDigi.cxx
Digitizer/R3BNeulandPixel.cxx
Digitizer/DigitizingEngine.cxx
Digitizer/DigitizingTacQuila.cxx
Digitizer/R3BNeulandCADDigitizer.cxx
Digitizer/R3BHitConvertor.cxx
Digitizer/R3BNeulandCADHit.cxx
Digitizer/R3BNeulandCADCluster.cxx
Digitizer/R3BNeulandCADNeutron.cxx
Digitizer/R3BNeulandCADClusterFinder.cxx
Digitizer/R3BNeulandCADNeutronClusters.cxx
Digitizer/R3BNeutronConvertor.cxx
Digitizer/R3BNeulandCADNeutronReconstructionStatistics.cxx
Analysis/R3BMultMatrix.cxx
Analysis/R3BPhysList.cxx
Analysis/R3BDetectionRate.cxx
Analysis/R3BErelSpectrum.cxx
Plots/R3B_DNN_Plots.cxx
MT/HistMerger.cxx
TextFiles/NormalizationClass.cxx
TextFiles/R3BTextFileGenerator.cxx
Reconstruction/ScoringClass.cxx
Reconstruction/AllScorers.cxx
Reconstruction/R3BScoreCollector.cxx
Reconstruction/R3BNEBULAScoreCollector.cxx
Reconstruction/R3BScoreApplier.cxx
Reconstruction/R3BDNNScoreApplier.cxx
Experiment/Template/R3BExpTranslator_Template.cxx
Experiment/SAMURAI_7LiPN/R3BExpTranslator_7LiPN_250MeV.cxx
Experiment/SAMURAI_7LiPN/ExValue.cxx 
Experiment/SAMURAI_7LiPN/Excitation_Energy_7LiPN.cxx
Experiment/SAMURAI_7LiPN/CrossSections_7LiPN.cxx
Experiment/TetraNeutron/MassValue.cxx 
Experiment/TetraNeutron/InvMass_4n.cxx
Presentation/DNNDataPoint.cxx
Presentation/DNNDataGraph.cxx
Presentation/DNNDataHist.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF DNNLinkDef.h)
Set(LIBRARY_NAME DNN)
set(DEPENDENCIES Base FairTools Physics)

GENERATE_LIBRARY()

