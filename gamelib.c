#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

struct Giocatore* giocatori;
unsigned short quest_da_finire;
struct Stanza* stanza_inizio;
struct Stanza* lista_stanze;

char room[4][17] = {"vuota", "quest_semplice", "quest_complicata", "botola"};
char state[4][13] = {"astronauta", "impostore", "assassino", "defenestrato"};
char colors[10][7] = {"red", "blue", "green", "yellow", "orange", "black", "white", "purple", "cyan", "brown"};

void menu_giocatori(){
  printf("Imposta il numero di giocatori [4-10]: ");
}

void menu_quest(){
  printf("Imposta il numero di quest [4-50]: ");
}

void menu_imposta(){
  printf("Cosa vuoi fare:\n");
  printf("1) Vedi informazioni giocatori\n");
  printf("2) Gioca\n");
}

void stampa_giocatori(int n){
  for (int player = 0; player < n; player++){
    printf("%s\t%s\tStanza iniziale -> %s\n", colors[(giocatori+player)->player_name],
              state[(giocatori+player)->player_state],
              room[(giocatori+player)->player_room->type]
              );
  }
}

int inizia_gioco(){
  return 0;
}


int imposta_gioco(){

  int n_players = 0;
  menu_giocatori();
  while (get_int(&n_players) != 0 || n_players < 4 || n_players > 10){
    printf("Input non consentito, inserisci un numero compreso tra 4 e 10\n");
    menu_giocatori();
  }
  // pointer to struct * 4 + enum + char + enum * 2 --------NOt NOT NOt
  // pointer + enum*2 == 16

  giocatori = (struct Giocatore*) malloc(n_players*(8 + sizeof(int)*2));
  stanza_inizio = (struct Stanza*) malloc(8*4 + sizeof(int) + sizeof(char));


  /* slower
  clock_t begin = clock();
  char nums[n_players];
  for (int i = 0; i < n_players; i++){
    nums[i] = i;
  }
  for (int i = 0; i < 35; i++){
    r_int1 = rand()%n_players;
    while ((r_int2 = rand()%n_players) == r_int1);
    // swap array elements
    nums[r_int1] = nums[r_int1] ^ nums[r_int2];
    nums[r_int2] = nums[r_int1] ^ nums[r_int2];
    nums[r_int1] = nums[r_int1] ^ nums[r_int2];
  }


  for (int player = 0; player < n_players; player++){
    (giocatori+player)->player_name = nums[player];
  }
  clock_t end = clock();
  printf("%f\n",(double)(end - begin) / CLOCKS_PER_SEC );
  */


  // inizializza name e state
  for (int player = 0; player < n_players; player++){
    (giocatori+player)->player_room = stanza_inizio;
    (giocatori+player)->player_state = astronauta;
    (giocatori+player)->player_name = player;
  }

  // pseudo-random name shuffle
  char r_int1 = 0;
  char r_int2 = 0;
  for (int i = 0; i < 35; i++){
    r_int1 = rand()%n_players;
    while ((r_int2 = rand()%n_players) == r_int1);
    // swap array elements
    (giocatori+r_int1)->player_name = (giocatori+r_int1)->player_name ^ (giocatori+r_int2)->player_name;
    (giocatori+r_int2)->player_name = (giocatori+r_int1)->player_name ^ (giocatori+r_int2)->player_name;
    (giocatori+r_int1)->player_name = (giocatori+r_int1)->player_name ^ (giocatori+r_int2)->player_name;
  }

  // set impostori
  int impostors = 0;
  for (int i = 0; i < ((n_players-1)/3); i++){
    (giocatori+rand()%n_players)->player_state = impostore;
  }

  printf("Giocatori impostati: %d\n", n_players);


  menu_quest();
  while (get_ushort(&quest_da_finire) != 0 || n_players < 4 || n_players > 50){
    printf("Input non consentito, inserisci un numero compreso tra 4 e 50\n");
    menu_quest();
  }

  // inizializzazione stanza_inizio
  stanza_inizio->avanti = NULL;
  stanza_inizio->sinistra = NULL;
  stanza_inizio->destra = NULL;
  stanza_inizio->stanza_precedente = NULL;
  int room_rand = rand()%100;
  if (room_rand < 30)
    stanza_inizio->type = vuota;
  else if (room_rand < 60)
    stanza_inizio->type = quest_semplice;
  else if (room_rand < 75)
    stanza_inizio->type = quest_complicata;
  else
    stanza_inizio->type = botola;

  // scelta stampa giocatori o gioca
  int scelta = 0;
  while (scelta != 2){
    menu_imposta();
    while (get_int(&scelta) != 0){
      printf("Input non consentito, inserisci 1 o 2\n");
      menu_imposta();
    }
    if (scelta == 1)
        stampa_giocatori(n_players);
    else if (scelta != 2)
        printf("Input non consentito, inserisci 1 o 2\n");
  }

  return inizia_gioco();
}


int gioca(){
  printf("gioca\n");
  return 0;
}

int deallocate_memory(){
  printf("dealloca\n");
  return 0;
}
