FROM yanzhaowang/r3bdev:fedora41

ENV FAIRROOTPATH="/root/FairRoot"
ENV UCESB_DIR="/root/ucesb"
ENV PEDEDIR="/root"
ENV THREAD_NUM=4
ENV R3B_BRNAME="neuland_new_calibration"

RUN yes | dnf upgrade --refresh &&\
        dnf install -y byacc && pip install conan --upgrade &&\
        mkdir -p $FAIRROOTPATH &&\
        cd /tmp &&\
        git clone --depth 1 --branch dev https://github.com/FairRootGroup/FairRoot.git &&\
        cd FairRoot &&\
        mkdir build && cd build && cmake -GNinja -DCMAKE_INSTALL_PREFIX=${FAIRROOTPATH} .. &&\
        ninja -j${THREAD_NUM} && ninja install &&\
        cd /root && git clone https://git.chalmers.se/expsubphys/ucesb.git && cd ucesb && make empty/empty &&\
        cd /root &&\
        git clone --depth 1 --branch ${R3B_BRNAME} https://github.com/YanzhaoW/R3BRoot.git &&\
        cd R3BRoot && git submodule update --init && cmake --preset default -GNinja -DWITH_DOX=NO &&\
        cmake --build --preset default -- -j${THREAD_NUM} &&\
        source build/config.sh && root -l -q neuland/geobase/create_neuland_geo.C &&\
        rm -rf /tmp/FairRoot

ENV CONFIG_FILE="/root/R3BRoot/build/config.sh"
ENTRYPOINT ["/bin/bash", "--rcfile", "/root/R3BRoot/build/config.sh"]
