#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

struct Giocatore* giocatori;
unsigned short quest_da_finire;
struct Stanza* stanza_inizio;
struct Stanza* lista_stanze;


void menu_imposta(){
  printf("Quanti giocatori partecipano? ");
}

int imposta_gioco(){
  printf("imposta\n");
  int n_players = 0;
  menu_imposta();
  while (get_int(&n_players) != 0 || n_players < 4 || n_players > 10){
    printf("Input non consentito, inserisci un numero compreso tra 4 e 10\n");
  }
  // pointer to struct * 4 + enum + char + enum * 2 --------NOt NOT NOt
  // pointer + enum*2 == 16
  giocatori = (struct Giocatore*) malloc(n_players*(8 + sizeof(int)*2));

  // pseudo-random colors shuffle
  time_t t;
  srand((unsigned) time(&t));
  char r_int1 = 0;
  char r_int2 = 0;
  char nums[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  for (int i = 0; i < 35; i++){
    r_int1 = rand()%10;
    while ((r_int2 = rand()%10) == r_int1);
    // swap array elements
    nums[r_int1] = nums[r_int1] ^ nums[r_int2];
    nums[r_int2] = nums[r_int1] ^ nums[r_int2];
    nums[r_int1] = nums[r_int1] ^ nums[r_int2];
  }
  /*
  for (int i = 0; i < 10; i++){
    printf("%d ", nums[i]);
  }
  */
  printf("Giocatori impostati: %d\n", n_players);
  return 0;
}

int gioca(){
  printf("gioca\n");
  return 0;
}

int deallocate_memory(){
  printf("dealloca\n");
  return 0;
}
