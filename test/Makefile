
CC ?= gcc
EXEC = test
OBJS = test.o

#CFLAGS += -DDEBUG
CFLAGS +=


all: $(EXEC)

$(EXEC): $(OBJS) $(CONF_H)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	-rm -f $(EXEC) *.o
