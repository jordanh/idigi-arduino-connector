#  Copyright (C) 2011 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2  as published by
#  the Free Software Foundation.
#
# type 'make help' for more information

include $(IDIGI_RULES)

CFLAGS += $(DFLAGS) -Wall -Werror -Wextra -I../../include

OBJS = main.o firmware.o network.o os.o config.o data_service.o rci.o
LIBS = -lidigi -lc

# Name of executable to build
SAMPLE=linux_sample

all: $(SAMPLE)

$(OBJS): *.c *.h

$(SAMPLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L$(LIB) $(LIBS) -o $(SAMPLE)

.PHONY: clean
clean:
	-rm -f $(OBJS)

.PHONY: help
help:
	@echo "Makefile used to build the linux sample"
	@echo "Options:"
	@echo "    IDIGI_RULES     = Location and  name for toolset"
	@echo "    DEBUG         = true or false for debugging"
	@echo "    LIB           = path of idigi library"
	@echo "    LITTLE_ENDIAN = true or false for little endian"
	@echo "NOTE:"
	@echo "    The idigi library path must be added to the LD_LIBRARY_PATH"
	@echo "    environmental variable when running the application"

