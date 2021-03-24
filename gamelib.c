#include <stdio.h>
#include <stdlib.h>
#include "gamelib.h"

struct Giocatore* giocatori;
unsigned short quest_da_finire;
struct Stanza* stanza_inizio;
struct Stanza* lista_stanze;



int imposta_gioco(){
  printf("imposta\n");
  
  return 1;
}

int gioca(){
  printf("gioca\n");
  return 1;
}

int deallocate_memory(){
  printf("dealloca\n");
  return 1;
}

void printHello(){
  printf("ciaone\n");
}
