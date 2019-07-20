#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>

struct termios orig_termios;

void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
  if(tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(
    IXON // Disable ^S ^Q
    | ICRNL // Disable ^M,
    | BRKINT| INPCK | ISTRIP | CS8
  );
  raw.c_oflag = ~(
    OPOST // Disable output processing e.g \n => \r\n
  );
  raw.c_lflag &= ~(
    ECHO // Disable Echoing
    | ICANON // Disable canonical mode
    | ISIG // Disable signal ^C ^Z
    | IEXTEN // Disable ^V (and ^O in macOS)
    );
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw))
    die("tcsetattr");
} 

int main() {
  enableRawMode();

  char c = '\0';
  while (1) {
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) 
      die("read");
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == 'q') break;
  }
  return 0;
}
