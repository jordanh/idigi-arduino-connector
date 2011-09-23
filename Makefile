#
#  Copyright (c) 1996-2011 Digi International Inc., All Rights Reserved
#
#  This software contains proprietary and confidential information of Digi
#  International Inc.  By accepting transfer of this copy, Recipient agrees
#  to retain this software in confidence, to prevent disclosure to others,
#  and to make no use of this software other than that for which it was
#  delivered.  This is an unpublished copyrighted work of Digi International
#  Inc.  Except as permitted by federal law, 17 USC 117, copying is strictly
#  prohibited.
#
#  Restricted Rights Legend
#
#  Use, duplication, or disclosure by the Government is subject to
#  restrictions set forth in sub-paragraph (c)(1)(ii) of The Rights in
#  Technical Data and Computer Software clause at DFARS 252.227-7031 or
#  subparagraphs (c)(1) and (2) of the Commercial Computer Software -
#  Restricted Rights at 48 CFR 52.227-19, as applicable.
#
#  Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
#
# =======================================================================
#
#  Type 'make help' for more information.

include $(IDIGI_RULES)

LIB_SRC_DIR=./private
SAMPLE_DIR=./public/sample
PUBLIC_HDR_DIR=./public/include
OBJDIR=$(LIB_SRC_DIR)

ifeq ($(LITTLE_ENDIAN),true)
DFLAGS += -D_LITTLE_ENDIAN
endif

ifneq ($(FACILITY_FW), false)
DFLAGS += -D_FIRMWARE_FACILITY
endif

ifneq ($(COMPRESSION), false)
DFLAGS += -DIDIGI_COMPRESSION_BUILTIN
endif

ifneq ($(DATA_SERVICE), false)
DFLAGS += -DIDIGI_DATA_SERVICE
endif

ifeq ($(DEBUG),true)
DFLAGS += -DDEBUG -g
else
DFLAGS += -DNDEBUG -O2
endif

CFLAGS += $(DFLAGS) -I$(PUBLIC_HDR_DIR) -I./private -Wall -Werror -Wextra -Wpointer-arith -std=c99

vpath $(LIB_SRC_DIR)/%.c
vpath $(LIB_SRC_DIR)/%.h

# Default LIBDIR is the currect directory
ifeq ($(LIBDIR),)
	LIBDIR = ./
endif

ifeq ($(LDFLAGS),)
LDFLAGS = -shared
endif

# By default build a static library
ifneq ($(SHARED_LIBRARY), true)
LIB =$(LIBDIR)/libidigi.a
else
LIB =$(LIBDIR)/libidigi.so
endif

all: $(LIB)

OBJS = $(OBJDIR)/idigi_api.o

$(OBJS): $(LIB_SRC_DIR)/*.c $(LIB_SRC_DIR)/*.h $(PUBLIC_HDR_DIR)/*.h

$(LIB): $(OBJS)

ifeq ($(SHARED_LIBRARY), true)
	$(LD) $(LDFLAGS) $^ -o $@
else
	$(AR) $(ARFLAGS) $@ $^
endif

MAKE= make IDIGI_RULES="../../../$(IDIGI_RULES)" DEBUG="$(DEBUG)" DFLAGS="$(DFLAGS)" LIB="../../../$(LIBDIR)" COMPRESSION="$(COMPRESSION)"

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
	@echo "    COMPRESSION   = true or false for compression (zlib) on data service
	@echo "    SHARED_LIBRARY = true or false for building shared library"
	@echo "Targets:"
	@echo "    all           - Build idigi library"
	@echo "    linux         - Build linux sample"
	@echo "    clean         - Delete all object files"
	@echo "IDIGI_RULES contain the following:"
	@echo "    CC            - Pointer to compiler"
	@echo "    LD            - Pointer to linker"
	@echo "    CFLAGS        - Options to the compiler"
	@echo "    LDFLAGS       - Options to the linker"
