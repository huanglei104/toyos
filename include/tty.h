void tty_init();
void tty_put_at_screen(char *str);
void tty_put_at_terminal(int termno, const char *str);
void tty_put_with_xy(int termno, const char *str, int x, int y);
void tty_scroll(int termno, int dir);
void tty_set_cursor(int termno, int x, int y);
void tty_scroll(int termno, int dir);
void tty_switch(int termno);
