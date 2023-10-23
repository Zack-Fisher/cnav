#include <termios.h>
#include <unistd.h>

static struct termios oldt, newt;

void term_setup() {
  tcgetattr(STDIN_FILENO, &oldt);
  // disable character ECHO by default, we want to be able to control which
  // characters get mirrored to the screen. also disable buffering.
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  newt.c_cc[VMIN] = 1;
  newt.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void term_restore() { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); }
