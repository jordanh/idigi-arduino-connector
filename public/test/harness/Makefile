include $(IDIGI_RULES)

ifeq ($(LITTLE_ENDIAN),true)
CFLAGS += -D_LITTLE_ENDIAN
endif

ifeq ($(DEBUG),true)
CFLAGS += -DDEBUG -g
else
CFLAGS += -DNDEBUG -O2
endif

SRC=src/config.c src/data_service.c src/firmware.c src/main.c src/network.c src/os.c src/rci.c src/inih/ini.c 
OBJS=${SRC:.c=.o}
EXEC=iik_test

CFLAGS += -I../../include -I./include -L../../../ -lidigi

all: $(EXEC)

$(OBJS): $(SRC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(OBJS) $(EXEC)
