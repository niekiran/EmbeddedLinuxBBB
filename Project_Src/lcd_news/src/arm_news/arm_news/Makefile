CFLAGS+=-Wall -Werror 
LDFLAGS+=`curl-config --libs`
CC=arm-linux-gnueabihf-gcc
TARGETS=news
OBJS=jsmn.o

all: $(TARGETS) .gitignore

clean:
	$(RM) $(TARGETS)
	$(RM) $(OBJS)

news: $(OBJS) simple.o
	$(CC) -o $@ $^ $(LDFLAGS)


.PHONY: all clean
