#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define row 25
#define col 80
#define SPEED 100000
#define S_SPEED 3000000
#define LIVE '*'
#define DEAD ' '

int count_live_neighbour_cell(int **a, int r, int c);
int **dyn_array(int **d);
void output(int **a, int r, int c);
void copy_array(int **arr1, int **arr2);
void new_gen(int *neib, int **a, int **b, int *);
void read_from_file(int **intro_m, int starter);
void display_intro();
int get_x(int i);
int get_y(int j);
int count_live_neighbour_cell(int **a, int i, int j);
int choose_starter(int *, int *);
void erase_field();
void clear_leaks(int **);
void change_speed(int *delay, int *flag, int *speedaoc);

int main() {
  int neighbour_live_cell, k = 1, delay = SPEED, flag = 1, starter = 0,
                           speedaoc = 21;
  int **a = NULL;
  a = dyn_array(a);
  int **b = NULL;
  b = dyn_array(b);

  erase_field();
  display_intro();
  usleep(S_SPEED);
  erase_field();

  choose_starter(&starter, &flag);
  erase_field();

  initscr();
  nodelay(stdscr, TRUE);

  read_from_file(a, starter);
  output(a, row, col);
  usleep(S_SPEED / 3);
  clear();

  while (flag) {
    change_speed(&delay, &flag, &speedaoc);

    // get new generation
    new_gen(&neighbour_live_cell, a, b, &flag);

    // print next generation
    printw("\nGeneration n%d   Current speed: %d\n", k, speedaoc);
    output(b, row, col);

    // copy b into a
    copy_array(b, a);
    k++;
    usleep(delay);
    clear();
  }
  nodelay(stdscr, FALSE);
  refresh();
  endwin();
  clear_leaks(a);
  clear_leaks(b);
  printf("Game over!\nThanks for playing!\n");
  return 0;
}

void output(int **a, int r, int c) {
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  attron(COLOR_PAIR(1));
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      if (!a[i][j]) {
        printw("%c", DEAD);
      } else {
        printw("%c", LIVE);
      }
    }
    printw("\n");
  }
  attroff(COLOR_PAIR(1));
  refresh();
}

void copy_array(int **arr1, int **arr2) {
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      arr2[i][j] = arr1[i][j];
    }
  }
}

int **dyn_array(int **d) {
  d = malloc(row * sizeof(int *));
  for (int i = 0; i < row; i++)
    d[i] = malloc(col * sizeof(int));
  return d;
}

void new_gen(int *neib, int **a, int **b, int *flag) {
  int count = 0;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      *neib = count_live_neighbour_cell(a, i, j);
      if (a[i][j] == 1 && (*neib == 2 || *neib == 3)) {
        b[i][j] = 1;
        count++;
      } else if (a[i][j] == 0 && *neib == 3) {
        b[i][j] = 1;
        count++;
      } else {
        b[i][j] = 0;
      }
    }
  }
  if (!count)
    *flag = 0;
}

int get_x(int i) { return (col + i) % col; }
int get_y(int j) { return (row + j) % row; }

int count_live_neighbour_cell(int **a, int i, int j) {
  int count = 0;
  count += a[get_y(i - 1)][get_x(j - 1)];
  count += a[get_y(i - 1)][get_x(j)];
  count += a[get_y(i - 1)][get_x(j + 1)];
  count += a[get_y(i)][get_x(j - 1)];
  count += a[get_y(i)][get_x(j + 1)];
  count += a[get_y(i + 1)][get_x(j + 1)];
  count += a[get_y(i + 1)][get_x(j)];
  count += a[get_y(i + 1)][get_x(j - 1)];
  return count;
}

void read_from_file(int **matrix, int starter) {
  FILE *fp;
  if (starter == 1)
    fp = fopen("glider.txt", "r");
  else if (starter == 2)
    fp = fopen("glider_gun.txt", "r");
  else if (starter == 3)
    fp = fopen("methuselah.txt", "r");
  else if (starter == 4)
    fp = fopen("supernova.txt", "r");
  else if (starter == 5)
    fp = fopen("spaceship.txt", "r");
  else if (starter == 6)
    fp = fopen("school21.txt", "r");
  else
    fp = fopen("rand_life.txt", "r");
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      fscanf(fp, "%d", &matrix[i][j]);
    }
  }
  fclose(fp);
}

void display_intro() {
  FILE *fp;
  fp = fopen("intro.txt", "r");
  char matrix[row][col];
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      fscanf(fp, "%c", &matrix[i][j]);
    }
  }
  fclose(fp);
  printf("\033[1;32m");
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      printf("%c", matrix[i][j]);
    }
  }
}

int choose_starter(int *starter, int *flag) {
  printf("Press + and - to control speed\nPress q to quit the game\nChoose "
         "starter position and press Enter:\n\n1. Glider\n2. Glider "
         "gun\n3. Methuselah\n4. Supernova\n5. Spaceship\n6. "
         "School 21\n");
  printf("\033[1;37m");
  if (!scanf("%d", starter))
    *flag = 0;
  return *starter;
}

void erase_field() { printf("\e[1;1H\e[2J"); }

void clear_leaks(int **matrix) {
  for (int i = 0; i < row; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

void change_speed(int *delay, int *flag, int *speedaoc) {
  char control = getch();
  if (control == '-' && *delay < 300000) {
    *delay += 10000;
    *speedaoc -= 1;
  }
  if (control == '+' && *delay > 10000) {
    *delay -= 10000;
    *speedaoc += 1;
  }
  if (control == 'q')
    *flag = 0;
}

// void read_from_input(int **a) {
//     for (int i = 0; i < row; i++) {
//         for (int j = 0; j < col; j++) {
//             scanf("%d", &a[i][j]);
//         }
//     }
// }
