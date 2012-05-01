#
#  Copyright (c) 2011-2012 Digi International Inc., All Rights Reserved
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

CFLAGS += $(DFLAGS) -MD -Wall -Werror -Wextra -pedantic -std=c99 -DIDIGI_VERSION=0x1010000UL 
CFLAGS += -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -I$(PUBLIC_HEADER_DIR) -I$(PLATFORM_DIR) -I. -g
CFLAGS += -DIDIGI_RCI_MAXIMUM_CONTENT_LENGTH=256

ifeq ($(RCI_DEBUG),true)
CFLAGS += -DRCI_DEBUG
endif

LIBS = -lc -lz -lpthread

SAMPLE = idigi
.PHONY:all
all: $(SAMPLE)

LDFLAGS += -Wl,-Map=$(SAMPLE).map,--sort-common

SRCS = gps_stats.c device_info.c \
       device_stats.c ethernet.c debug_info.c serial.c remote_config_cb.c remote_config.c application.c \
       main.c os.c config.c firmware.c \
       $(PLATFORM_DIR)/network.c

# removed for testing - ASK
# $(IDIGI_DIR)/idigi_api.c $(PLATFORM_DIR)/network.c

OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

-include $(DEPS)

$(SAMPLE): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@
ifeq ($(RCI_DEBUG),true)
	$(OD) -dS $@ >$@.dis
	awk -f size.awk idigi.map
endif
    
.PHONY: clean
clean:
	-rm -f $(SAMPLE) $(OBJS) $(DEPS)


