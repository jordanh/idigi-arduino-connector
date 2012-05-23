# ***************************************************************************
# Copyright (c) 2012 Digi International Inc.,
# All rights not expressly granted are reserved.
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
#
# ***************************************************************************
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)gcc
CXX     = $(CROSS_COMPILE)g++
STRIP   = $(CROSS_COMPILE)strip
AR      = $(CROSS_COMPILE)ar

IIK_DIR=../../..
IDIGI_DIR=$(IIK_DIR)/private
PUBLIC_HEADER_DIR=$(IIK_DIR)/public/include
PLATFORM_DIR=$(IIK_DIR)/public/run/platforms/linux
DVT_DIR=$(IIK_DIR)/dvt
SCRIPTS_DIR=$(DVT_DIR)/scripts

vpath $(IDIGI_DIR)/%.c

CFLAGS += $(DFLAGS) -Wall -Werror -Wextra -pedantic -std=c99 -DIDIGI_VERSION=0x1020000UL 
CFLAGS += -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -I. -I$(PUBLIC_HEADER_DIR) -I$(PLATFORM_DIR) -g -Os

OBJS = main.o $(IDIGI_DIR)/idigi_api.o
LIBS = -lc -lz 

SAMPLE = idigi
all: $(SAMPLE)

CFLAGS += -Wl,-Map,$(SAMPLE).map -DIDIGI_DATA_SERVICE -DIDIGI_FIRMWARE_SERVICE -D____IDIGI_COMPRESSION -D___IDIGI_DEBUG -D___USE_THE_DEFINES

main.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h Makefile
$(IDIGI_DIR)/idigi_api.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h $(PUBLIC_HEADER_DIR)/idigi_config.h \
                        $(IDIGI_DIR)/chk_config.h $(IDIGI_DIR)/debug.h $(IDIGI_DIR)/ei_packet.h \
                        $(IDIGI_DIR)/idigi_cc.h $(IDIGI_DIR)/idigi_ds.h $(IDIGI_DIR)/idigi_fw.h \
                        $(IDIGI_DIR)/idigi_msg.h $(IDIGI_DIR)/layer.h $(IDIGI_DIR)/network_intf.h \
                        $(IDIGI_DIR)/options.h $(IDIGI_DIR)/os_intf.h $(IDIGI_DIR)/bele.h \
                        $(IDIGI_DIR)/idigi_def.h Makefile

$(OBJS): *.c $(IDIGI_DIR)/*.c

$(SAMPLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(SAMPLE)
	$(AT)python $(SCRIPTS_DIR)/iikmapsum.py $(SAMPLE).map | tee $(basename $@).stats 

.PHONY: clean
clean:
	-rm -f $(OBJS)
	-rm -f $(SAMPLE).map 
	-rm -f $(SAMPLE).stats
	-rm -f $(SAMPLE)
	-rm -f $(DVT_DIR)/memory_usage.txt


