#
#  irl/Makefile
#
#  Copyright (C) 2010 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2  as published by
#  the Free Software Foundation.
# In rules.in:
# CC pointer to compiler
# LD pointer to linker
# $CLFAGS options to the compiler
# LDFLAGS options to the linker

# arguments
# LITTLE_ENDIAN={true|false}
# LIBRARY=path
# DEBUG={true|false}

# end of rules.in

include $(IRL_RULES)

ifeq ($(LITTLE_ENDIAN),true)
CFLAGS += -D_LITTLE_ENDIAN
endif

ifeq ($(DEBUG),true)
CFLAGS += -DDEBUG -g
else
CFLAGS += -DNDEBUG
endif

CFLAGS += -I../public/include -Wall

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))

.PHONY: all
all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(LD) $(LDFLAGS) $@

.PHONY: clean
clean:
	-rm -f $(LIBRARY) $(OBJS)

.PHONY: help
help:
	@echo "Syntax:"
	@echo "    IRL_RULES = The full path name for toolset"
	@echo "    LIBRARY = Location and name of library"
	@echo "    DEBUG = true or false for debugging"
	@echo "    LITTLE_ENDIAN = true or false for little endian"

