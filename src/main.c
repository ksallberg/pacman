#include<stdio.h>

int main() {

  printf("Hello pacworld!\n");
  int something;

  for(;;) {

    something = getchar();

    printf("You entered: ");
    putchar(something);

    if(something == 'q') {

      printf("Good bye!");
      break;
    }
  }

  return 0;
}
