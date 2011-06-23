#
#  Copyright (C) 2011 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2  as published by
#  the Free Software Foundation.
#
#  Type 'make help' for more information.

include $(IDIGI_RULES)

LIB_SRC_DIR=./private
SAMPLE_DIR=./public/sample
PUBLIC_HDR_DIR=./public/include
OBJDIR=$(LIB_SRC_DIR)

ifeq ($(LITTLE_ENDIAN),true)
CFLAGS += -D_LITTLE_ENDIAN
endif

ifneq ($(FACILITY_FW), false)
CFLAGS += -D_FIRMWARE_FACILITY
endif

ifneq ($(DATA_SERVICE), false)
CFLAGS += -D_DATA_SERVICE
endif

ifeq ($(FACILITY_RCI), true)
CFLAGS += -D_RCI_FACILITY
endif

ifeq ($(DEBUG),true)
CFLAGS += -DDEBUG -g
else
CFLAGS += -DNDEBUG -O2
endif

CFLAGS += -I$(PUBLIC_HDR_DIR) -I./private -Wall -Werror -Wextra

vpath $(LIB_SRC_DIR)/%.c
vpath $(LIB_SRC_DIR)/%.h

LIB =$(LIBDIR)/libidigi.so

all: $(LIB)

OBJS = $(OBJDIR)/idigi_api.o

$(OBJS): $(LIB_SRC_DIR)/*.c $(LIB_SRC_DIR)/*.h $(PUBLIC_HDR_DIR)/*.h

$(LIB): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

MAKE= make IDIGI_RULES="../../../$(IDIGI_RULES)" DEBUG="$(DEBUG)" LITTLE_ENDIAN="$(LITTLE_ENDIAN)" LIB="../../../$(LIBDIR)"

linux:
	echo "building" $(SAMPLE_DIR)/linux;\
	TARGETDIR=`pwd` ; cd $(SAMPLE_DIR)/linux;\
	$(MAKE) -f Makefile all;\
	cd $${TARGETDIR};

.PHONY: clean
clean:
	-rm -f $(OBJS) $(LIB)
	TARGETDIR=`pwd` ; cd $(SAMPLE_DIR)/linux;\
	$(MAKE) -f Makefile clean;\
	cd $${TARGETDIR};\

.PHONY: help
help:
	@echo "This makefile only supports the GNU toolchain"
	@echo "Options:"
	@echo "    IDIGI_RULES   = Location and name of toolset"
	@echo "    LIBDIR        = Location of library"
	@echo "    DEBUG         = true or false for debugging"
	@echo "    LITTLE_ENDIAN = true or false for little endian"
	@echo "    FACILITY_FW   = true or false for firmware upgrade capability"
	@echo "    DATA_SERVICE  = true or false for data service capability"
	@echo "    FACILITY_RCI  = true or false for RCI capability"
	@echo "Targets:"
	@echo "    all           - Build idigi library"
	@echo "    linux         - Build linux sample"
	@echo "    clean         - Delete all object files"
	@echo "IDIGI_RULES contain the following:"
	@echo "    CC            - Pointer to compiler"
	@echo "    LD            - Pointer to linker"
	@echo "    CFLAGS        - Options to the compiler"
	@echo "    LDFLAGS       - Options to the linker"
	@echo "NOTES:"
	@echo "    If you only wish to build a sample you can type make in the"
	@echo "    sample subdirectory once the idigi library is built."
