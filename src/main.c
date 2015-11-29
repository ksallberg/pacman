#include<stdio.h>
#include<unistd.h>

const int width       = 19;
const int height      = 19;
const int ghost_count = 4;

#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_BLUE   "\x1b[34m"
#define ANSI_COLOR_CYAN   "\x1b[36m"
#define ANSI_COLOR_YELLOW "\x1b[0;93m"
#define ANSI_COLOR_RESET  "\x1b[0m"

/* char *m[] = {"         █         ", */
/*              " ███ ███ █ ███ ███ ", */
/*              "                   ", */
/*              " ███ █ █████ █ ███ ", */
/*              "     █   █   █     ", */
/*              "████ ███ █ ███ ████", */
/*              "████ █       █ ████", */
/*              "████ █ ██ ██ █ ████", */
/*              "       █   █       ", */
/*              "████ █ █████ █ ████", */
/*              "████ █       █ ████", */
/*              "████ █ █████ █ ████", */
/*              "         █         ", */
/*              " ███ ███ █ ███ ███ ", */
/*              "   █           █   ", */
/*              "██ █ █ █████ █ █ ██", */
/*              "     █   █   █     ", */
/*              " ███████ █ ███████ ", */
/*              "                   "}; */

char *m[] = {"         |         ",
             " ||| ||| | ||| ||| ",
             "                   ",
             " ||| | ||||| | ||| ",
             "     |   |   |     ",
             "|||| ||| | ||| ||||",
             "|||| |       | ||||",
             "|||| | || || | ||||",
             "       |   |       ",
             "|||| | ||||| | ||||",
             "|||| |       | ||||",
             "|||| | ||||| | ||||",
             "         |         ",
             " ||| ||| | ||| ||| ",
             "   |           |   ",
             "|| | | ||||| | | ||",
             "     |   |   |     ",
             " ||||||| | ||||||| ",
             "                   "};

struct Monster {
  char* color;
  int x;
  int y;
  int state; // 1 = hunting, 2 = fleeing, 3 = dead, 4 = pacman
};

struct Monster pacman;
struct Monster ghosts[4];

void clear_scene() {
  printf("\e[1;1H\e[2J");
}

void test() {

  int i = 0;
  for(; i < 19; i ++) {
    printf("%s\n", m[i]);
  }
}

void draw_scene() {
  int i, j, x = 0;
  int ghost_printed = 0;

  for(i = 0; i < height; i ++) {
    for(j = 0; j < width; j ++) {
      if(j == pacman.x && i == pacman.y) {
        printf(ANSI_COLOR_YELLOW "c" ANSI_COLOR_RESET);
      } else {
        for(x = 0; x < ghost_count; x++) {
          if(j == ghosts[x].x && i == ghosts[x].y) {
            printf("%s", ghosts[x].color);
            printf("m" ANSI_COLOR_RESET);
            ghost_printed = 1;
            /* Print only one ghost, if several
             * ghosts are located in the same cell,
             * then print the first and skip the rest.
             */
            break;
          }
        }
        if(ghost_printed == 0) {
          char str = m[i][j];
          printf("%c", str);
        }
        ghost_printed = 0;
      }
    }
    printf("\n");
  }

  if(pacman.x == height - 1) {
    pacman.x = pacman.y = 0;
  } else {
    pacman.x ++;
    pacman.y ++;
  }
}

int main() {

  int round = 0;

  pacman.x = 0;
  pacman.y = 0;
  pacman.state = 4;

  ghosts[0].x = 10;
  ghosts[0].y = 10;
  ghosts[0].state = 1;
  ghosts[0].color = ANSI_COLOR_RED;

  ghosts[1].x = 4;
  ghosts[1].y = 5;
  ghosts[1].state = 1;
  ghosts[1].color = ANSI_COLOR_CYAN;

  ghosts[2].x = 7;
  ghosts[2].y = 6;
  ghosts[2].state = 1;
  ghosts[2].color = ANSI_COLOR_GREEN;

  ghosts[3].x = 10;
  ghosts[3].y = 4;
  ghosts[3].state = 1;
  ghosts[3].color = ANSI_COLOR_BLUE;

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
