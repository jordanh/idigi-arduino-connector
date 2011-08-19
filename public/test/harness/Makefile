include $(IDIGI_RULES)

CFLAGS += -std=c99 -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE

ifeq ($(LITTLE_ENDIAN),true)
CFLAGS += -D_LITTLE_ENDIAN
endif

ifeq ($(DEBUG),true)
CFLAGS += -DDEBUG -g
else
CFLAGS += -DNDEBUG -O2
endif

CFLAGS += -I../../include -I./include -lc

ifneq ($(COMPRESSION), false)
  CFLAGS += -lz
endif


ifeq ($(SHARED_LIBRARY), true)
  CFLAGS += -L$(LIBDIR) -lidigi
else
  EFLAGS += $(LIBDIR)/libidigi.a
endif

SRC=src/network.c src/data_service.c src/firmware.c src/main.c src/config.c src/os.c src/inih/ini.c 
OBJS=${SRC:.c=.o}
EXEC=iik_test

all: $(EXEC)

$(OBJS): $(SRC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ $(EFLAGS) -o $@

clean:
	$(RM) $(OBJS) $(EXEC)
