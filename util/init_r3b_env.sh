#! bash

if [[ -z "${SIMPATH}" ]]; then
    echo "ERROR: SIMPATH is not set!"
else
    mkdir -p ~/miniconda3
    wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda3/miniconda.sh
    bash ~/miniconda3/miniconda.sh -b -u -p ~/miniconda3
    rm ~/miniconda3/miniconda.sh
    source ~/miniconda3/bin/activate
    conda init --all
    echo "
    name: r3bdev
    channels:
      - conda-forge
    dependencies:
      - python==$(${SIMPATH}/bin/root-config --python-version)
      - numpy
      - matplotlib
      - seaborn
      - pandas
      - scikit-learn
      - scipy
      - jupyterlab
      - jupyterlab_code_formatter
      - black
      - jupyterlab_vim
      - jupyterlab-lsp
      - conan
    " > /tmp/r3bdev.yml
    conda env create -f /tmp/r3bdev.yml
fi
