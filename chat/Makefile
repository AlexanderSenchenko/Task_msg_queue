NCURSES_CFLAGS := -D_GNU_SOURSE -I/usr/include/ncursesw
NCURSES_LIBS := -lncursesw -ltinfo

all:
	gcc server.c list.c -o server -g
	gcc init_chat.c -c -g $(NCURSES_CFLAGS)
	gcc client.c -c -g $(NCURSES_CFLAGS)
	gcc client.o init_chat.o -o client $(NCURSES_LIBS)
