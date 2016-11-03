/*
Copyright (c) 2014-2016, Kristian Sällberg
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "queue.h"

const int width           = 19;
const int height          = 19;
const int ghost_count     = 4;
const int ghost_flee_time = 20;

#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_PINK   "\x1b[35m"
#define ANSI_COLOR_CYAN   "\x1b[36m"
#define ANSI_COLOR_YELLOW "\x1b[0;93m"
#define ANSI_COLOR_RESET  "\x1b[0m"
#define MIN(a, b) ((a > b) ? b : a)

int run;
extern int run;

int points;
extern int points;

int game_state;
extern int game_state;

int round_at_apple;
extern int round_at_apple;

int cur_round;
extern int cur_round;

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
             "         |         "};

int food[19][19];

struct Monster {
  char* color;
  int x;
  int y;
  int state; // 1 = hunting, 2 = fleeing, 3 = dead, 4 = pacman
  int dir;   // 0 = down,    1 = up,      2 = left, 3 = right
};

struct Pos {
  int x;
  int y;
};

struct Pos pos;

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

struct Pos *next_stink(Queue *q, int x, int y) {
  struct Pos *p = NULL;
  int i = 0;

  while(1) {
    p = get_at(q, i);
    i++;

    if(p == NULL) {
      return NULL;
    }

    if(p->x == x && p->y == y) {
      break;
    }
  }

  i ++;

  while(p->x == x && p->y == y) {
    p = get_at(q, i);
    i ++;
    if(p == NULL) {
      return NULL;
    }
  }

  return p;
}

void move_ghosts(Queue *q) {
  int i = 0;
  int new_x = 0;
  int new_y = 0;
  srand(time(NULL));

  struct Pos *stink_pos;

  for(i = 0; i < ghost_count; i++) {

    new_x = ghosts[i].x;
    new_y = ghosts[i].y;

    stink_pos = next_stink(q, new_x, new_y);

    if(stink_pos == NULL || game_state == 2) {
      switch(ghosts[i].dir) {
      case 0:
        new_y = ghosts[i].y + 1;
        break;
      case 1:
        new_y = ghosts[i].y - 1;
        break;
      case 2:
        new_x = ghosts[i].x - 1;
        break;
      case 3:
        new_x = ghosts[i].x + 1;
        break;
      }

      if(new_x > width - 1) {
        new_x = 0;
      }

      if(new_x < 0) {
        new_x = width - 1;
      }

      if(new_y < 0) {
        new_y = height - 1;
      }

      if(new_y > height - 1) {
        new_y = 0;
      }

      // Ghost is stuck
      if(ghosts[i].x == new_x && ghosts[i].y == new_y) {

        ghosts[i].dir = rand() % 4;

      } else if(m[new_y][new_x] == ' ') {

        ghosts[i].x = new_x;
        ghosts[i].y = new_y;

      } else {

        ghosts[i].dir = rand() % 4;
      }

    } else {

      ghosts[i].x = stink_pos->x;
      ghosts[i].y = stink_pos->y;
    }
  }
}

void draw_scene(Queue *q) {
  int i, j, x = 0;
  int ghost_printed = 0;

  for(i = 0; i < height; i ++) {
    for(j = 0; j < width; j ++) {
      if(j == pacman.x && i == pacman.y) {
        printf(ANSI_COLOR_YELLOW "ᗧ" ANSI_COLOR_RESET);
      } else {
        for(x = 0; x < ghost_count; x++) {
          if(j == ghosts[x].x && i == ghosts[x].y) {
            if(game_state == 2 &&
               ghosts[x].state == 1) {
              printf("%s", ANSI_COLOR_CYAN);
              printf("ᗣ" ANSI_COLOR_RESET);
              ghost_printed = 1;
            } else if(game_state == 1 &&
                      ghosts[x].state == 1) {
              printf("%s", ghosts[x].color);
              printf("ᗣ" ANSI_COLOR_RESET);
              ghost_printed = 1;
            }
            /* Print only one ghost, if several
             * ghosts are located in the same cell,
             * then print the first and skip the rest.
             */
            break;
          }
        }
        if(ghost_printed == 0) {
          if (food[j][i] == 1) {
            printf(ANSI_COLOR_YELLOW "." ANSI_COLOR_RESET);
          } else if (food[j][i] == 2) {
            printf(ANSI_COLOR_YELLOW "•" ANSI_COLOR_RESET);
          } else {
            char str = m[i][j];
            printf("%c", str);
          }
        }
        ghost_printed = 0;
      }
    }
    printf("\n\r");
  }

  move_ghosts(q);
  printf("Score: %d\n", points);
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

    if(new_x > width - 1) {
      new_x = 0;
    }

    if(new_x < 0) {
      new_x = width - 1;
    }

    if(new_y < 0) {
      new_y = height - 1;
    }

    if(new_y > height - 1) {
      new_y = 0;
    }

    if(m[new_y][new_x] == ' ') {
      pacman.x = new_x;
      pacman.y = new_y;
    }

    if(food[pacman.x][pacman.y] == 1) {
      points ++;
      food[pacman.x][pacman.y] = 0;
    }

    if(food[pacman.x][pacman.y] == 2) {
      food[pacman.x][pacman.y] = 0;
      game_state = 2;
      round_at_apple = cur_round;
    }
  }

  return NULL;
}

int main() {
  pthread_t keyb_thread;
  int i, j, c = 0;
  Queue *x;
  struct Pos *p;

  x = q_create();
  cur_round = 0;

  pos.x = 0;
  pos.y = 0;

  // Initialize food
  for(i = 0; i < width; i ++) {
    for(j = 0; j < height; j ++) {
      food[i][j] = 0;
    }
  }

  food[0][2] = 1;
  food[0][4] = 1;

  for(i=1; i < 16; i ++) {
    food[4][i] = 1;
  }

  for(i=1; i < 16; i ++) {
    food[14][i] = 1;
  }

  // Initialize apples (game state changer)
  food[2][12] = 2;
  food[16][12] = 2;

  run = 1;
  points = 0;
  game_state = 1; // state 1 = ghosts chasing
                  // state 2 = ghosts fleeing

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
  ghosts[0].dir = 0;

  ghosts[1].x = 4;
  ghosts[1].y = 5;
  ghosts[1].state = 1;
  ghosts[1].color = ANSI_COLOR_CYAN;
  ghosts[1].dir = 0;

  ghosts[2].x = 7;
  ghosts[2].y = 6;
  ghosts[2].state = 1;
  ghosts[2].color = ANSI_COLOR_GREEN;
  ghosts[2].dir = 0;

  ghosts[3].x = 10;
  ghosts[3].y = 4;
  ghosts[3].state = 1;
  ghosts[3].color = ANSI_COLOR_PINK;
  ghosts[3].dir = 0;

  while(run!=0) {

    // Make ghosts dangerous again.
    if(round_at_apple == cur_round - ghost_flee_time) {
      game_state = 1;
    }

    usleep(180000);

    clear_scene();
    draw_scene(x);

    for(i=0; i < 4; i ++) {
      if(ghosts[i].x == pacman.x &&
         ghosts[i].y == pacman.y &&
         game_state == 1) {
        run = 0;
        break;
      }

      if(ghosts[i].x == pacman.x &&
         ghosts[i].y == pacman.y &&
         game_state == 2) {
        ghosts[i].state = 2;
      }

      // quit if all food is eaten
      if(points == 32) {
        run = 0;
        break;
      }
    }

    p = (struct Pos *) malloc(sizeof(struct Pos));
    p->x = pacman.x;
    p->y = pacman.y;

    q_shift(x, p);

    cur_round ++;
  }

  clear_scene();

  // Curses destroy
  refresh();
  endwin();

  printf("Score: %d\n", points);

  // Clean up
  for(i = 0; i < MIN(50, cur_round); i ++) {
    p = get_at(x, i);
    free(p);
  }

  return 0;
}
