# ***************************************************************************
# Copyright (c) 2011, 2012 Digi International Inc.,
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

IDIGI_DIR=../../../../private
PUBLIC_HEADER_DIR=../../../include

vpath $(IDIGI_DIR)/%.c

CFLAGS += -Winit-self -Wbad-function-cast -Wpointer-arith -Wlogical-op -Wmissing-parameter-type -Wstrict-prototypes -Wformat-security 
CFLAGS += -Wformat-y2k -Wold-style-definition -Wcast-align -Wformat-nonliteral -Wpadded -Wredundant-decls -Wvariadic-macros
# CFLAGS += -Wjump-misses-init -Wtrampolines 
CFLAGS += -Wno-error=padded -Wno-error=format-nonliteral

CFLAGS += $(DFLAGS) -Wall -Werror -Wextra -pedantic -std=c99 -DIDIGI_VERSION=0x1020000UL 
CFLAGS += -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -I. -I$(PUBLIC_HEADER_DIR) -Os

OBJS = main.o debug.o $(IDIGI_DIR)/idigi_api.o
LIBS = -lc -lz

SAMPLE = idigi
all: $(SAMPLE)

LDFLAGS += $(DFLAGS) -Wl,-Map,$(SAMPLE).map

main.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h
$(IDIGI_DIR)/idigi_api.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h ./idigi_config.h \
                        $(IDIGI_DIR)/chk_config.h $(IDIGI_DIR)/debug.h $(IDIGI_DIR)/ei_packet.h \
                        $(IDIGI_DIR)/idigi_cc.h $(IDIGI_DIR)/idigi_ds.h $(IDIGI_DIR)/idigi_fw.h \
                        $(IDIGI_DIR)/idigi_msg.h $(IDIGI_DIR)/layer.h $(IDIGI_DIR)/network_intf.h \
                        $(IDIGI_DIR)/options.h $(IDIGI_DIR)/os_intf.h $(IDIGI_DIR)/bele.h \
                        $(IDIGI_DIR)/idigi_def.h $(IDIGI_DIR)/rci_service.h $(IDIGI_DIR)/rci_parser.h

$(OBJS): *.c $(IDIGI_DIR)/*.c

$(SAMPLE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(SAMPLE)

.PHONY: clean
clean:
	-rm -f $(OBJS)


