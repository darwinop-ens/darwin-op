###############################################################
#
# Purpose: Makefile for "demo"
# Author.: robotis
# Version: 0.1
# License: GPL
#
###############################################################

TARGETS=$(shell find . -mindepth 1 -maxdepth 1 -type d | sed s/.*\\.svn.*//)
all_TARGETS=$(addprefix all_,$(TARGETS))
clean_TARGETS=$(addprefix clean_,$(TARGETS))
libclean_TARGETS=$(addprefix libclean_,$(TARGETS))
tgz_TARGETS=$(addprefix tgz_,$(TARGETS))

.PHONY: force

all: $(all_TARGETS)

$(all_TARGETS): force
	make -C $(patsubst all_%,%,$@) all

clean: $(clean_TARGETS)

$(clean_TARGETS): force
	make -C $(patsubst clean_%,%,$@) clean

libclean: $(libclean_TARGETS)

$(libclean_TARGETS): force
	make -C $(patsubst libclean_%,%,$@) libclean

distclean: clean libclean

tgz: clean $(tgz_TARGETS)

$(tgz_TARGETS): force
	make -C $(patsubst tgz_%,%,$@) tgz

