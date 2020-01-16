#!/bin/bash
export WERROR=1
export BUILDONLY=1
exec $(dirname $0)/travis-generate-dockerfile.sh
