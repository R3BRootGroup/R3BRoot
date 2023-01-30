# this file is automatically symlinked to GNUmakefile when
# the configuration script ${CMAKE_SOURCE_DIR}/lustretar.sh exists.

$(info You are using the c3w makefile wrapper)

SRC_UTIL_MAKE=$(shell readlink -f GNUmakefile)
SRC_UTIL=$(shell dirname $(SRC_UTIL_MAKE) )

c3w: all
	@echo "Trying to create c3w tarball. "
	$(SRC_UTIL)/build_c3w.sh

include Makefile # include the cmake generated makefile

