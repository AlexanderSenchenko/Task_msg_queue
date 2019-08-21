#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <ncurses.h>
#include <stdlib.h>

#include "init_chat.h"

#define COL_LIST_USERS 18

#define ROW_WIN_MSG 3

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char*) &size);
	resizeterm(size.ws_row, size.ws_col);
}

int init_chat(WINDOW** win_text, WINDOW** win_user, WINDOW** win_msg)
{
	int max_y, max_x;
	int col_text;
	int row;

	initscr();
	signal(SIGWINCH, sig_winch);
	cbreak();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	refresh();

	getmaxyx(stdscr, max_y, max_x);

	col_text = max_x - COL_LIST_USERS - 1;

	row = max_y - ROW_WIN_MSG;

	if (init_win(win_text, row, col_text, 0, 0) == -1)
		return -1;

	if (init_win(win_user, row, COL_LIST_USERS, 0, col_text + 1) == -1)
		return -1;

	if (init_win(win_msg, ROW_WIN_MSG, max_x, row, 0) == -1)
		return -1;

	return 0;
}

int init_win(WINDOW** win, int row, int col, int starty, int startx)
{
	WINDOW* winbox;

	winbox = newwin(row, col, starty, startx);
	if (winbox == NULL)
		return -1;

	box(winbox, '|', '-');

	wrefresh(winbox);

	*win = derwin(winbox, row - 2, col - 2, 1, 1);

	delwin(winbox);

	return 0; 
}
