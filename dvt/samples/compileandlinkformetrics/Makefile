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


