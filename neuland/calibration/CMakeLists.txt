set(LIBRARY_NAME R3BNeulandCalibration)
set(LINKDEF NeulandCalibrationLinkDef.h)

set(DEPENDENCIES R3Bbase R3BData R3BTCal)

set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${R3BROOT_SOURCE_DIR}/neuland/calibration
    ${R3BROOT_SOURCE_DIR}/neuland/shared
    ${R3BROOT_SOURCE_DIR}/r3bdata/losData
    ${R3BROOT_SOURCE_DIR}/tcal)
include_directories(${INCLUDE_DIRECTORIES})

set(SRCS
    R3BNeulandMappedHist.cxx
    R3BNeulandMapped2Cal.cxx
    R3BNeulandMapped2CalPar.cxx
    R3BNeulandTacquilaMapped2Cal.cxx
    R3BNeulandTacquilaMapped2CalPar.cxx
    R3BNeulandTacquilaMapped2QCalPar.cxx
    R3BNeulandHitHist.cxx
    # R3BNeulandCalTest.cxx
    R3BNeulandHitPar.cxx
    R3BNeulandCal2HitPar.cxx
    R3BLandCosmic1LSQR.cxx
    R3BLandCosmic1Util.cxx
    R3BNeulandParFact.cxx
    R3BNeulandCal2Hit.cxx
    R3BNeulandHitModulePar.cxx
    R3BNeulandQCalPar.cxx
    R3BNeulandHitFiller.cxx
    R3BNeulandQCalFiller.cxx)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

generate_library()
