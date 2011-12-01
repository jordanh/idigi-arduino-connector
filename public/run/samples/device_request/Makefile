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
CC = gcc

IDIGI_DIR=../../../../private
PUBLIC_HEADER_DIR=../../../include
PLATFORM_DIR=../../platforms/linux
SCRIPTS_DIR= ../../../../scripts

vpath $(IDIGI_DIR)/%.c
vpath $(PLATFORM_DIR)/%.c

CFLAGS += $(DFLAGS) -Wall -Werror -Wextra -pedantic -std=c99 -DIDIGI_VERSION=0x1010000UL 
CFLAGS += -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -I$(PUBLIC_HEADER_DIR) -I$(PLATFORM_DIR)  -g -Os

OBJS = application.o device_request.o $(IDIGI_DIR)/idigi_api.o $(PLATFORM_DIR)/os.o $(PLATFORM_DIR)/network.o  $(PLATFORM_DIR)/config.o $(PLATFORM_DIR)/main.o 
LIBS = -lc -lz -lpthread

SAMPLE = idigi
all: $(SAMPLE)

CFLAGS += -Wl,-Map,$(SAMPLE).map

application.o:
device_request.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h
$(IDIGI_DIR)/idigi_api.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h $(PUBLIC_HEADER_DIR)/idigi_config.h \
                        $(IDIGI_DIR)/chk_config.h $(IDIGI_DIR)/debug.h $(IDIGI_DIR)/ei_packet.h \
                        $(IDIGI_DIR)/idigi_cc.h $(IDIGI_DIR)/idigi_ds.h $(IDIGI_DIR)/idigi_fw.h \
                        $(IDIGI_DIR)/idigi_msg.h $(IDIGI_DIR)/layer.h $(IDIGI_DIR)/network_intf.h \
                        $(IDIGI_DIR)/options.h $(IDIGI_DIR)/os_intf.h $(IDIGI_DIR)/bele.h \
                        $(IDIGI_DIR)/idigi_def.h
$(PLATFORM_DIR)/main.o: $(PUBLIC_HEADER_DIR)/idigi_api.h $(PUBLIC_HEADER_DIR)/idigi_types.h $(PLATFORM_DIR)/platform.h

$(OBJS): *.c $(IDIGI_DIR)/*.c $(PLATFORM_DIR)/*.c

$(SAMPLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(SAMPLE) 
	$(AT)python $(SCRIPTS_DIR)/iikmapsum.py $(SAMPLE).map | tee $(basename $@).stats

.PHONY: clean
clean:
	-rm -f $(OBJS)


