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

IDIGI_DIR=../../../private
PUBLIC_HEADER_DIR=../../../public/include
PLATFORM_DIR=../../../public/run/platforms/template
SCRIPTS_DIR= ../../scripts

vpath $(IDIGI_DIR)/%.c
vpath $(PLATFORM_DIR)/%.c

# CFLAGS += $(DFLAGS) -Wall -Werror -Wextra -pedantic -std=c99 -DIDIGI_VERSION=0x1020000UL 
CFLAGS += $(DFLAGS) -Wall -Wextra -pedantic -std=c99 -DIDIGI_VERSION=0x1020000UL 
CFLAGS += -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -I$(PUBLIC_HEADER_DIR) -I$(PLATFORM_DIR)  -g -Os

OBJS = main.o $(IDIGI_DIR)/idigi_api.o $(PLATFORM_DIR)/os.o $(PLATFORM_DIR)/network.o $(PLATFORM_DIR)/config.o
LIBS = -lc -lz -lpthread

SAMPLE = idigi
all: $(SAMPLE)

CFLAGS += -Wl,-Map,$(SAMPLE).map

main.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h
ds_callback: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h
firmware: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h
$(IDIGI_DIR)/idigi_api.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h $(PUBLIC_HEADER_DIR)/idigi_config.h $(IDIGI_DIR)/idigi_def.h $(IDIGI_DIR)/bele.h

$(OBJS): *.c $(IDIGI_DIR)/*.c $(PLATFORM_DIR)/*.c

$(SAMPLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(SAMPLE)
	$(AT)python $(SCRIPTS_DIR)/iikmapsum.py $(SAMPLE).map | tee $(basename $@).stats 

.PHONY: clean
clean:
	-rm -f $(OBJS)


