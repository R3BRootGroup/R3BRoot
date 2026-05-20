#! /bin/bash

if [ "$#" -eq 0 ]
then 
    echo "INFO: No arguments supplied."
else
    source /opt/R3BRoot/build/config.sh
    if [ "$1" = "neuland" ]
    then
        neuland ${@:2}
    elif [ "$1" = "macro" ]
    then
        root -l -q ${@:2}
    elif [ "$1" = "init-conda" ]
    then
        source /opt/R3BRoot/util/init_r3b_env.sh
    elif [ "$1" = "jupyterlab" ]
    then
        source ~/miniconda3/bin/activate
        conda init --all
        conda activate r3bdev
        jupyter lab ${@:2}
    else
        echo "ERROR: Unrecognized arguments supplied. Please give arguments starting with \"neuland\", \"macro\", \"jupyterlab\" or \"init-conda\""
    fi
fi
