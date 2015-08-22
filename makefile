PROG := ellite
SRCS := ellite.c gpio7seg.c
OBJS := $(SRCS:%.c=%.o)
CFLAGS := -O2 -I.
LIBS := -lpthread -lwiringPi

CC := gcc

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(LIBS) -c $<

%.o: %.h

.PHONY: clean
clean:
	rm -f $(OBJS) $(PROG)
