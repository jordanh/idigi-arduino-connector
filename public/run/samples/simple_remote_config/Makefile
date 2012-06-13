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
OD = objdump

IDIGI_DIR=../../../../private
PUBLIC_HEADER_DIR=../../../include
PLATFORM_DIR=../../platforms/linux

vpath $(IDIGI_DIR)/%.c
vpath $(PLATFORM_DIR)/%.c

CFLAGS += -Winit-self -Wbad-function-cast -Wpointer-arith -Wlogical-op -Wmissing-parameter-type -Wstrict-prototypes -Wformat-security 
CFLAGS += -Wformat-y2k -Wold-style-definition -Wcast-align -Wformat-nonliteral -Wpadded -Wredundant-decls -Wvariadic-macros
CFLAGS += -Wno-error=padded -Wno-error=format-nonliteral -Wno-unused-function

CFLAGS += $(DFLAGS) -MD -Wall -Werror -Wextra -Wno-missing-field-initializers -pedantic -std=c99 -DIDIGI_VERSION=0x1020000UL 
CFLAGS += -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -I. -I$(PUBLIC_HEADER_DIR) -I$(PLATFORM_DIR) -I. -g
#CFLAGS += -DIDIGI_RCI_MAXIMUM_CONTENT_LENGTH=256

ifeq ($(RCI_DEBUG),true)
CFLAGS += -DRCI_DEBUG -g -O0
else
CFLAGS += -Os
endif

LIBS = -lc -lz -lpthread

SAMPLE = idigi
.PHONY:all
all: $(SAMPLE)

LDFLAGS += -Wl,-Map=$(SAMPLE).map,--sort-common

SRCS = system.c remote_config_cb.c application.c firmware.c \
       $(PLATFORM_DIR)/main.c $(PLATFORM_DIR)/os.c $(PLATFORM_DIR)/config.c \
       $(PLATFORM_DIR)/network.c $(PLATFORM_DIR)/debug.c $(IDIGI_DIR)/idigi_api.c

# removed for testing - ASK
# $(IDIGI_DIR)/idigi_api.c $(PLATFORM_DIR)/network.c

OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

-include $(DEPS)

$(SAMPLE): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@
ifeq ($(RCI_EXTRA),true)
	$(OD) -dS $@ >$@.dis
	awk -f size.awk idigi.map
endif
    
.PHONY: clean
clean:
	-rm -f $(SAMPLE) $(OBJS) $(DEPS)


