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
CC = gcc


IDIGICONNECTOR_DIR=../../..
IDIGI_DIR=$(IDIGICONNECTOR_DIR)/private
PUBLIC_HEADER_DIR=$(IDIGICONNECTOR_DIR)/public/include
PLATFORM_DIR=$(IDIGICONNECTOR_DIR)/public/run/platforms/linux
SCRIPTS_DIR=$(IDIGICONNECTOR_DIR)/dvt/scripts

vpath $(IDIGI_DIR)/%.c
vpath $(PLATFORM_DIR)/%.c

CFLAGS += $(DFLAGS) -Wall -Werror -Wextra -Wno-missing-field-initializers -pedantic -std=c99 -DIDIGI_VERSION=0x1020000UL 
CFLAGS += -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -I. -I$(PUBLIC_HEADER_DIR) -I$(PLATFORM_DIR) -g -Os

OBJS = application.o config.o $(IDIGI_DIR)/idigi_api.o $(PLATFORM_DIR)/os.o $(PLATFORM_DIR)/debug.o $(PLATFORM_DIR)/main.o
LIBS = -lc -lz -lpthread

ifeq ($(SSL), true)
OBJS += $(PLATFORM_DIR)/network_ssl.o
LIBS += -lssl
else
OBJS += $(PLATFORM_DIR)/network.o
endif

SAMPLE = idigi
all: $(SAMPLE)

CFLAGS += -Wl,-Map,$(SAMPLE).map

application.o:
$(IDIGI_DIR)/idigi_api.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h ./idigi_config.h \
                        $(IDIGI_DIR)/chk_config.h $(IDIGI_DIR)/debug.h $(IDIGI_DIR)/ei_packet.h \
                        $(IDIGI_DIR)/idigi_cc.h $(IDIGI_DIR)/idigi_ds.h $(IDIGI_DIR)/idigi_fw.h \
                        $(IDIGI_DIR)/idigi_msg.h $(IDIGI_DIR)/layer.h $(IDIGI_DIR)/network_intf.h \
                        $(IDIGI_DIR)/options.h $(IDIGI_DIR)/os_intf.h $(IDIGI_DIR)/bele.h \
                        $(IDIGI_DIR)/idigi_def.h

$(OBJS): *.c $(IDIGI_DIR)/*.c $(PLATFORM_DIR)/*.c Makefile idigi_config.h

$(SAMPLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(SAMPLE)

.PHONY: clean
clean:
	-rm -f $(OBJS)
	-rm -f $(SAMPLE).map 
	-rm -f $(SAMPLE).stats
	-rm -f $(SAMPLE)

