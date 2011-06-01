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

include $(IDK_RULES)

ifeq ($(LITTLE_ENDIAN),true)
CFLAGS += -D_LITTLE_ENDIAN
endif

ifeq ($(DEBUG),true)
CFLAGS += -DDEBUG -g
else
CFLAGS += -DNDEBUG
endif

CFLAGS += -I../public/include -Wall

#SRCS := $(wildcard *.c)
#OBJS := $(patsubst %.c,%.o,$(SRCS))

OBJS := idk_api.o
IDK_LIB = $(LIB)/libidk.so

$(OBJS): *.c *.h

#.PHONY: all
all: $(IDK_LIB)

$(IDK_LIB): $(OBJS)
	$(LD) $(LDFLAGS) $@

.PHONY: clean
clean:
	-rm -f $(IDK_LIB) $(OBJS)

.PHONY: help
help:
	@echo "Syntax:   make IDK_RULES=[rules.in] LIB=[directory of libidk.so] [Options]"
	@echo "Where:
	@echo "    IDK_RULES = full path to your rules.in"
	@echo "    LIB = directory to store libidk.so"
	@echo "Options:"
	@echo "    DEBUG = true or false for debugging"
	@echo "    LITTLE_ENDIAN = true or false for little endian"

