include $(IDIGI_RULES)

ifeq ($(LITTLE_ENDIAN),true)
CFLAGS += -D_LITTLE_ENDIAN
endif

ifeq ($(DEBUG),true)
CFLAGS += -DDEBUG -g
else
CFLAGS += -DNDEBUG -O2
endif

SRC=main.c config.c firmware.c os.c network.c data_service.c inih/ini.c
OBJS=${SRC:.c=.o}
EXEC=iik_test

CFLAGS += -I../include -I../sample/linux -L../../ -lidigi

all: $(EXEC)

$(OBJS): $(SRC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(OBJS) $(EXEC)
