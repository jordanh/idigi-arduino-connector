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
PLATFORM_DIR=../../platforms/linux

vpath $(IDIGI_DIR)/%.c
vpath $(PLATFORM_DIR)/%.c

CFLAGS += $(DFLAGS) -Wall -Werror -Wextra -pedantic -Wno-missing-field-initializers -std=c99 -DIDIGI_VERSION=0x1020000UL -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -I. -I$(PUBLIC_HEADER_DIR) -I$(PLATFORM_DIR)  -g

OBJS = application.o  put_request.o $(IDIGI_DIR)/idigi_api.o $(PLATFORM_DIR)/os.o $(PLATFORM_DIR)/config.o $(PLATFORM_DIR)/debug.o $(PLATFORM_DIR)/network.o $(PLATFORM_DIR)/main.o
LIBS = -lc -lz

SAMPLE = idigi
all: $(SAMPLE)

main.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h
firmware.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h
$(IDIGI_DIR)/idigi_api.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h ./idigi_config.h $(IDIGI_DIR)/idigi_def.h $(IDIGI_DIR)/bele.h

$(OBJS): *.c $(IDIGI_DIR)/*.c $(PLATFORM_DIR)/*.c

$(SAMPLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(SAMPLE) 

.PHONY: clean
clean:
	-rm -f $(OBJS)


