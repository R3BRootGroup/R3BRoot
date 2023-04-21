#! /bin/bash

if [ -z "${GITHUB_WORKSPACE}" ];
then
    echo "Not in github CI/CD test!"
    export GITHUB_WORKSPACE=$( pwd )
    # exit 0
fi

config=${GITHUB_WORKSPACE}/build/config.sh
if [ -f "${config}" ]; 
then
    source ${config}
else
    echo "Failed to source ${config} in the build folder!"
    exit 1
fi

echo "Generating geo files for ctest"

root -l -q -x ${GITHUB_WORKSPACE}/music/geobase/create_music_geo.C
root -l -q -x ${GITHUB_WORKSPACE}/rpc/geobase/create_rpc_geo.C
root -l -q -x ${GITHUB_WORKSPACE}/alpide/geobase/create_target_area_2023_geo.C
root -l -q -x ${GITHUB_WORKSPACE}/mwpc/geobase/create_mwpc0_geo.C
root -l -q -x ${GITHUB_WORKSPACE}/mwpc/geobase/create_mwpc1and2_geo.C
root -l -q -x ${GITHUB_WORKSPACE}/twim/geobase/create_twin_geo.C

if [ -d "${GITHUB_WORKSPACE}/sofia" ];
then
    root -l -q -x ${GITHUB_WORKSPACE}/sofia/tofwall/geobase/create_softofw_geo.C
fi
