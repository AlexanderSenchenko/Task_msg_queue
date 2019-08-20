NCURSES_CFLAGS := -D_GNU_SOURSE -I/usr/include/ncursesw
NCURSES_LIBS := -lncursesw -ltinfo
PTHREAD := -lpthread

all:
	gcc list.c -c -g
	gcc client_manager.c -c -g
	gcc server.c -c -g
	gcc server.o list.o client_manager.o -o server
	gcc act.c -c -g $(NCURSES_CFLAGS)
	gcc init_chat.c -c -g $(NCURSES_CFLAGS)
	gcc client.c -c -g $(NCURSES_CFLAGS)
	gcc client.o init_chat.o list.o act.o -o client $(NCURSES_LIBS) $(PTHREAD)
