CMAKE_VER ?= 3.23.1
CMAKE_LINK ?= https://github.com/Kitware/CMake/releases/download/v$(CMAKE_VER)/cmake-$(CMAKE_VER).tar.gz
CMAKE_TAR ?= cmake-$(CMAKE_VER).tar.gz
CMAKE_DIR ?= cmake-$(CMAKE_VER)
CMAKE_BUILD ?= cmake-build
CMAKE_INSTALL ?= $(abspath ../cmake)
MAKE_FLAGS ?= -j4

define MODULEFILE
#%Module1.0#####################################################################

proc ModulesHelp { } {

        puts stderr " "
        puts stderr "This module loads the CMake utility"
        puts stderr "\\nVersion $(CMAKE_VER)\\n"

}
module-whatis "Name: CMake"
module-whatis "Version: $(CMAKE_VER)"
module-whatis "Category: utility, developer support"
module-whatis "Keywords: System, Utility"
module-whatis "Description: CMake is an open-source, cross-platform family of tools designed to build, test and package software"
module-whatis "URL https://cmake.org/"

set     version             $(CMAKE_VER)

prepend-path    PATH                $(CMAKE_INSTALL)/bin
endef

define ADDPATHCMD
#!/bin/bash
export PATH=$(CMAKE_INSTALL)/bin:$${PATH}
endef


all: install

download: $(CMAKE_TAR)

extract: $(CMAKE_DIR)

build: $(CMAKE_BUILD)

install: $(CMAKE_INSTALL)

.PHONY: module
module: $(CMAKE_VER)

$(CMAKE_TAR):
	wget $(CMAKE_LINK)

$(CMAKE_DIR): $(CMAKE_TAR)
	tar -xzf $(CMAKE_TAR)

$(CMAKE_BUILD): $(CMAKE_DIR)
	mkdir -pv $(CMAKE_BUILD)
	cd $(CMAKE_BUILD) && cmake ../$(CMAKE_DIR) -DCMAKE_INSTALL_PREFIX=$(CMAKE_INSTALL)
	$(MAKE) -C $(CMAKE_BUILD) $(MAKE_FLAGS)

$(CMAKE_INSTALL): $(CMAKE_BUILD)
	$(MAKE) -C $(CMAKE_BUILD) install

export ADDPATHCMD
export MODULEFILE
$(CMAKE_VER):
	mkdir -pv $(CMAKE_INSTALL)/modulefiles
	echo "$$MODULEFILE" > $(CMAKE_INSTALL)/modulefiles/$(CMAKE_VER)
	echo "$$ADDPATHCMD" > $(CMAKE_INSTALL)/addToPath.sh

.PHONY: clean
clean:
	rm -f $(CMAKE_BUILD)
	
.PHONY: purge
purge:
	rm -rf ./$(CMAKE_BUILD) ./$(CMAKE_DIR) ../cmake $(CMAKE_VER) ./$(CMAKE_TAR)
