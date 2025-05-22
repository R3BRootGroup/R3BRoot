ARG BUILD_REPO=r3bdev:fedora-latest

FROM yanzhaowang/${BUILD_REPO}

ARG R3B_BRNAME="edwin_dev"
ARG THREAD_NUM=4

RUN yes | dnf upgrade --refresh &&\
        pip install conan --upgrade &&\
        mkdir -p /opt && cd /opt &&\
        git clone --depth 1 --branch ${R3B_BRNAME} https://github.com/YanzhaoW/R3BRoot.git &&\
        cd R3BRoot && git submodule update --init && cmake --preset default -GNinja -DWITH_DOX=NO &&\
        cmake --build --preset default -- -j${THREAD_NUM} &&\
        source build/config.sh && root -l -q neuland/geobase/create_neuland_geo.C

ENV CONFIG_FILE="/opt/R3BRoot/build/config.sh"
ENTRYPOINT ["/bin/bash", "--rcfile", "/opt/R3BRoot/build/config.sh"]
