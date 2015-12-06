#include<stdio.h>
#include<unistd.h>
#include<ncurses.h>
#include<pthread.h>

const int width       = 19;
const int height      = 19;
const int ghost_count = 4;

#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_BLUE   "\x1b[34m"
#define ANSI_COLOR_CYAN   "\x1b[36m"
#define ANSI_COLOR_YELLOW "\x1b[0;93m"
#define ANSI_COLOR_RESET  "\x1b[0m"

int run;
extern int run;

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

void move_ghosts() {
  int i = 0;
  for(i = 0; i < ghost_count; i++) {
    ghosts[i].x += 1;
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
    printf("\n\r");
  }

  move_ghosts();
}

/* Function that will be executed in the thread. */
void *keyboard_runner(void *void_ptr) {

  int ch, new_x, new_y;

  while(run!=0) {
    new_x = pacman.x;
    new_y = pacman.y;
    ch = getch(); // Read keyboard
    switch(ch) {
      case 258:
        new_y = pacman.y + 1;
        break;
      case 259:
        new_y = pacman.y - 1;
        break;
      case 260:
        new_x = pacman.x - 1;
        break;
      case 261:
        new_x = pacman.x + 1;
        break;
      case 113:
        run = 0;
        break;
      default:
        run = ch;
    }

    if(m[new_y][new_x] == ' ') {
      pacman.x = new_x;
      pacman.y = new_y;
    }
  }

  return NULL;
}

int main() {

  int round = 0;
  pthread_t keyb_thread;

  run = 1;

  // Curses init
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  // Fork off a thread of execution for reading the
  // player's keyboard controls
  if(pthread_create(&keyb_thread, NULL, keyboard_runner, NULL)) {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }

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

  while(run!=0) {
    if(++round >= 1000) {
      break;
    }
    usleep(100000);
    clear_scene();
    draw_scene();
  }

  // Join thread / wait for it to close.
  if(pthread_join(keyb_thread, NULL)) {
    fprintf(stderr, "Error joining thread\n");
    return 2;
  }

  // Curses destroy
  refresh();
  endwin();

  return 0;
}
