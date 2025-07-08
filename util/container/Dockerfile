ARG BUILD_REPO=r3bdev:fedora-latest

FROM yanzhaowang/${BUILD_REPO}

ARG R3B_BRNAME="edwin_dev"
ARG THREAD_NUM=4

RUN yes | dnf upgrade --refresh &&\
        pip install conan --upgrade &&\
        mkdir -p /opt && cd /opt &&\
        git clone --depth 1 --branch ${R3B_BRNAME} https://github.com/YanzhaoW/R3BRoot.git &&\
        cd R3BRoot && git submodule update --init && cmake --preset default -GNinja -DWITH_DOX=NO &&\
        export ROOT_CXX_STANDARD=$(${SIMPATH}/bin/root-config --cxxstandard)
        cmake --preset default . -DCMAKE_CXX_STANDARD=${ROOT_CXX_STANDARD}
        cmake --build --preset default -- -j${THREAD_NUM}

ENV CONFIG_FILE="/opt/R3BRoot/build/config.sh"
COPY --chmod=755 entrypoint.sh /opt/entrypoint.sh
ENTRYPOINT ["/opt/entrypoint.sh"]
