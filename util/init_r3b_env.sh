#! bash

if [[ -z "${SIMPATH}" ]]; then
    echo "ERROR: SIMPATH is not set!"
else
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
