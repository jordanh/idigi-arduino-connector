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
#
include $(IDIGI_RULES)

CFLAGS += $(DFLAGS) -Wall -Werror -Wextra -std=c99 -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -I../../include

OBJS = main.o firmware.o network.o os.o config.o data_service.o
LIBS = -lidigi -lc

ifneq ($(DATA_SERVICE), false)
DFLAGS += -DIDIGI_DATA_SERVICE
endif

ifneq ($(COMPRESSION), false)
CFLAGS += -DCOMPRESSION
LIBS += -lz
endif


# Name of executable to build
SAMPLE=linux_sample

all: $(SAMPLE)

$(OBJS): *.c *.h

$(SAMPLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L$(LIB) $(LIBS) -o $(SAMPLE)

.PHONY: clean
clean:
	-rm -f $(OBJS)

.PHONY: help
help:
	@echo "Makefile used to build the linux sample"
	@echo "Options:"
	@echo "    IDIGI_RULES     = Location and  name for toolset"
	@echo "    DEBUG         = true or false for debugging"
	@echo "    LIB           = path of idigi library"
	@echo "    LITTLE_ENDIAN = true or false for little endian"
	@echo "    DATA_SERVICE  = true or false for data service"
	@echo "    COMPRESSION   = true or false for compression (zlib) on data service
	@echo "NOTE:"
	@echo "    The idigi library path must be added to the LD_LIBRARY_PATH"
	@echo "    environmental variable when running the application"
	@echo "    if the idigi library was built as a shared library"

