#include<stdio.h>
#include<unistd.h>

int pacx = 0;
int pacy = 0;

#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_CYAN   "\x1b[36m"
#define ANSI_COLOR_YELLOW "\x1b[0;93m"
#define ANSI_COLOR_RESET  "\x1b[0m"

void clear_scene() {
  printf("\e[1;1H\e[2J");
}

void draw_scene() {
  int i, j = 0;

  for(i = 0; i < 10; i ++) {
    for(j = 0; j < 20; j ++) {
      if(j == pacx && i == pacy) {
        printf(ANSI_COLOR_YELLOW "o" ANSI_COLOR_RESET);
      } else {
        printf(ANSI_COLOR_CYAN   "x" ANSI_COLOR_RESET);
      }
    }
    printf("\n");
  }

  if(pacx == 9) {
    pacx = pacy = 0;
  } else {
    pacx ++;
    pacy ++;
  }
}

int main() {

  int round = 0;

  for(;;) {

    if(++round >= 1000) {
      break;
    }
    usleep(100000);

    clear_scene();
    draw_scene();
  }

  return 0;
}
