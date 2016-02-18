.PHONY: clean install uninstall all

PREFIX ?= /usr/local
BINS    = emb_semver
RM      = rm -f
SRC     = $(wildcard src/*.c)
DEPS    = $(wildcard deps/*/*.c)
OBJS    = $(DEPS:.c=.o)

CFLAGS = -std=c99 -Ideps -O3
CFLAGS += -Wall -Werror -Wno-unused-function -Wno-unused-value -Wno-unused-variable

all: $(BINS)

$(BINS): $(SRC) $(OBJS)
	$(CC) $(CFLAGS) -o $@ src/$(@:.exe=).c $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS)

install: $(BINS)
	cp -f $(BINS) $(PREFIX)/bin/$(BINS)

uninstall:
	rm -f $(PREFIX)/bin/mon

clean:
	$(foreach c, $(BINS), $(RM) $(c);)
	$(RM) $(OBJS)
