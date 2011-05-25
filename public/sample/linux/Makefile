#
#IDK Makefile
#
#  Copyright (C) 2011 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2  as published by
#  the Free Software Foundation.
# In rules.in:
# CC pointer to compiler
# LD pointer to linker
# CFLAGS options to the compiler
# LDFLAGS options to the linker
#
# NOTE: The library path must be added to the LD_LIBRARY_PATH environmental
# variable when running the application 

# arguments
# LITTLE_ENDIAN={true|false}
# LIBRARY=path
# DEBUG={true|false}

# end of rules.in

include $(IDK_RULES)

ifeq ($(LITTLE_ENDIAN),true)
CFLAGS += -D_LITTLE_ENDIAN
endif

ifeq ($(DEBUG),true)
CFLAGS += -DDEBUG -g
else
CFLAGS += -DNDEBUG -O2
endif

CFLAGS += -Wall -Werror -I../../include

LIBS=-lc

OBJS = main.o firmware.o network.o os.o
LIBS = -lidk -lc

$(OBJS): *.c *.h

all: sample

sample: $(OBJS)
	$(CC)  -L$(LIB) $(CFLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o sample

.PHONY: clean
clean:
	-rm -f $(OBJS)

.PHONY: help
help:
	@echo "Options:"
	@echo "    IDK_RULES = Location and  name for toolset"
	@echo "    DEBUG = true or false for debugging"
	@echo "    LIB = path of idigi library"
	@echo "    LITTLE_ENDIAN = true or false for little endian"

