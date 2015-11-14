#include<stdio.h>
#include<unistd.h>

int pacx = 0;
int pacy = 0;

int width = 50;
int height = 25;

#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_CYAN   "\x1b[36m"
#define ANSI_COLOR_YELLOW "\x1b[0;93m"
#define ANSI_COLOR_RESET  "\x1b[0m"

void clear_scene() {
  printf("\e[1;1H\e[2J");
}

void test() {
  printf("╔══════════════╗        ╔══╗\n");
  printf("║              ╠════════╣  ║\n");
  printf("╚══════════════╝        ╚══╝\n");
}

void draw_scene() {
  int i, j = 0;

  for(i = 0; i < height; i ++) {
    for(j = 0; j < width; j ++) {
      if(j == pacx && i == pacy) {
        printf(ANSI_COLOR_YELLOW "o" ANSI_COLOR_RESET);
      } else {
        printf(ANSI_COLOR_CYAN "\u2588" ANSI_COLOR_RESET);
      }
    }
    printf("\n");
  }

  if(pacx == height - 1) {
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
    test();
  }

  return 0;
}
