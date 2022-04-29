#!/bin/bash

FLEX_PATH=${HOME}/flex

wget https://github.com/westes/flex/releases/download/v2.6.4/flex-2.6.4.tar.gz

tar -xf flex-2.6.4.tar.gz

cd flex-2.6.4

./configure --prefix=${FLEX_PATH} && make && make install

mkdir -pv ${FLEX_PATH}/modulefiles

cat << EOF > ${FLEX_PATH}/modulefiles/flex
#%Module1.0#####################################################################

proc ModulesHelp { } {

        puts stderr " "
        puts stderr "This module loads the Tlang utility"
        puts stderr "\\nVersion 2.6.4\\n"

}
module-whatis "Name: Flex"
module-whatis "Version: 2.6.4"
module-whatis "Category: utility, developer support"
module-whatis "Keywords: Compiler, Utility"
module-whatis "Description: Tlang is an open-source compiler infrastructure"

set     version             2.6.4

prepend-path    PATH                ${FLEX_PATH}/bin
prepend-path    CPATH               ${FLEX_PATH}/include
prepend-path    LIBRARY_PATH        ${FLEX_PATH}/lib
prepend-path    LD_LIBRARY_PATH     ${FLEX_PATH}/lib
prepend-path    FLEX_ROOT           ${FLEX_PATH}
prepend-path    FLEX_LIB            ${FLEX_PATH}/lib
prepend-path    FLEX_INC            ${FLEX_PATH}/include
EOF
