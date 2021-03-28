#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

struct Giocatore* giocatori;
unsigned short quest_da_finire;
struct Stanza* stanza_inizio;
struct Stanza** lista_stanze;
int n_stanze = 0;

char room[4][17] = {"vuota", "quest_semplice", "quest_complicata", "botola"};
char state[4][13] = {"astronauta", "impostore", "assassino", "defenestrato"};
char colors[10][7] = {"red", "blue", "green", "yellow", "orange", "black", "white", "purple", "cyan", "brown"};

time_t t;
srand((unsigned) time(&t));
int n_players = 0;


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

void stampa_giocatori(){
  for (int player = 0; player < n_players; player++){
    printf("%s\t%s\tStanza iniziale -> %s\n", colors[(giocatori+player)->player_name],
              state[(giocatori+player)->player_state],
              room[(giocatori+player)->player_room->type]
              );
  }
}

int inizia_gioco(){
  return 0;
}


void print_info(int giocatore){
  printf("Stanza: %s\n", myroom = room[(giocatori+giocatore)->player_room]);
  printf("Con te ci sono: \n");
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == myroom && player != giocatore)
      printf("%s ", colors[player]);
  }
}


void menu_gioca(int type, int players_inroom){
    if (type == 0){
      printf("Cosa vuoi fare: \n");
      printf("1) Avanzare\n");
      printf("2) Fare una quest\n");
      printf("3) Fare una chiamata d'emergenza\n");
    }
    else if (type == 1){
      printf("Cosa vuoi fare: \n");
      printf("1) Avanzare\n");
      printf("2) Uccidere un astronauta\n");
      printf("3) Entrare nella botola\n");
      printf("4) Sabotare la nave\n");
    }
}


void create_room(struct Stanza* new_stanza, struct Stanza* stanza_precedente){
  // add check to see if malloc succedded
  new_stanza = (struct Stanza*) malloc(8*4+sizeof(int)+sizeof(char));
  new_stanza->avanti = NULL;
  new_stanza->sinistra = NULL;
  new_stanza->destra = NULL;
  new_stanza->stanza_precedente = stanza_precedente;
  int room_rand = rand()%100;
  if (room_rand < 30)
    stanza_inizio->type = vuota;
  else if (room_rand < 60)
    stanza_inizio->type = quest_semplice;
  else if (room_rand < 75)
    stanza_inizio->type = quest_complicata;
  else
    stanza_inizio->type = botola;
  *(lista_stanze+n_stanze) = new_stanza;
  n_stanze++;
}


void menu_avanza(){
  printf("Dove vuoi andare?\n");
  printf("1) Avanti\n");
  printf("2) Destra\n");
  printf("3) Sinistra\n");
}

void avanza(int giocatore){
  menu_avanza();
  int destination_room = 0;
  while (get_int(&destination_room) != 0);
  // avanti, destra, sinistra
  switch (destination_room){
    case 1:
      if ((giocatori+giocatore)->player_room->avanti == NULL){
        create_room((giocatori+giocatore)->player_room->avanti, (giocatori+giocatore)->player_room);
      }
      (giocatori+giocatore)->player_room = (giocatori+giocatore)->player_room->avanti
      break;
    case 2:
      if ((giocatori+giocatore)->player_room->destra == NULL){
        create_room((giocatori+giocatore)->player_room->destra, (giocatori+giocatore)->player_room);
      }
      (giocatori+giocatore)->player_room = (giocatori+giocatore)->player_room->destra
      break;
    case 3:
      if ((giocatori+giocatore)->player_room->sinistra == NULL){
        create_room((giocatori+giocatore)->player_room->sinistra, (giocatori+giocatore)->player_room);
      }
      (giocatori+giocatore)->player_room = (giocatori+giocatore)->player_room->sinistra
      break;
    default:
      printf("Input non consentito, inserisci un numero tra 1 e 3\n", );
      break;
  }
}

void esegui_quest(struct Stanza* stanza_quest){
  if (stanza_quest->type == 1)
    quest_da_finire += 1;
  else if (stanza_quest->type == 2)
    quest_da_finire += 2;
  // or simply quest_da_finire += stanza_quest->type se effettuo prima il controllo sulla stanza per capire se poter fare o meno l'esegui quest
}


void chiamata_emergenza(struct Stanza* stanza_chiamata){
  // TODO: need to check first if someone has been killed
  int room_astronauti = 0;
  int room_impostori = 0;
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == stanza_chiamata){
      if ((giocatori+player)->player_state == 0)
        room_astronauti++;
      else if ((giocatori+player)->player_state == 1)
        room_impostori++;
    }
  }
  int prob_astronauti = (3+(2*(room_impostori-1))-(3*(room_astronauti-1)))*room_astronauti;
  int prob_impostori = (3+(2*(room_astronauti-1))-(3*(room_impostori-1)))*room_impostori;
  int rand_prob = rand()%(prob_astronauti+prob_impostori);
  if (rand_prob < prob_astronauti)
    (giocatori+rand_prob/(prob_astronauti/room_astronauti))->player_state = defenestrato;
  else
    (giocatori+(rand_prob-prob_astronauti)/(prob_impostori/room_impostori))->player_state = defenestrato;
  stanza_chiamata->emergenza_chiamata = 1;
}


void uccidi_astronauta(int killer, struct Stanza* stanza_uccisione){
  //il giocatore ucciso non può più giocare (da implementare nel for delle azioni dei giocatori in gioca())
  int room_astronauti = 0;
  int killable_players[n_players-1];
  printf("Gli astronauti ");
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == stanza_uccisione){
      if ((giocatori+player)->player_state == astronauta){
        static int i = 0;
        room_astronauti++;
        killable_players[i] = player;
        printf("%s\n", colors[(giocatori+player)->player_name]);
        i++;
      }
    }
  }
  printf("sono nella stanza con te! Chi vuoi uccidere?\n");

  for (int player = 0; player < room_astronauti; player++){
    if (killable_players[player] > 0)
      printf("%d) %s\n", player, colors[(giocatori+player)->player_name]);
  }
  int player_to_kill = -1;
  int exit = 0;
  while (get_int(&player_to_kill) != 0 || player_to_kill < 0 || player_to_kill > room_astronauti)
    printf("Input non consentito, inserisci un numero tra 0 e %d\n", room_astronauti-1);
  (giocatori+killable_players[player_to_kill])->player_state = assassinato;
  // dead XXXXX
  if (room_astronauti > 1){
    int prevroom_astronauti = 0;
    for (int player = 0; player < n_players; player++){
      if ((giocatori+player)->player_room == stanza_uccisione->stanza_precedente)
        prevroom_astronauti++;
    }
    int impostor_prob = 5*(room_astronauti-1)+2*prevroom_astronauti;
    if (impostor_prob > 10)
      (giocatori+killer)->player_state = defenestrato;
    else {
      int rand_prob = rand() % 10;
      if (rand_prob > impostor_prob)
        (giocatori+killer)->player_state = defenestrato;
    }
  }
}


void usa_botola(int impostore, struct Stanza* stanza_botola){
  //TODO: check if room has botola
  int n_botole = 0;
  int stanze_botole[n_stanze];
  for (int stanza = 0; stanza < n_stanze; stanza++){
    if (*(lista_stanze+stanza)->type == botola && *(lista_stanze+stanza) != stanza_botola){
      stanze_botole[i] = stanza;
      n_botole++;
    }
  }
  if (n_botole == 0){
    (giocatori+impostore)->player_room = *(lista_stanze+rand()%n_stanze);
    printf("Non ci sono altre botole, sei stati teletrasportato in una stanza casuale\n");
  }
  else{
    printf("Ci sono %d stanze con una botola! Dove vuoi andare? [0:%d]\n", n_botole, n_botole-1);
    int destination_botola = -1;
    while (get_int(&destination_botola) != 0 || destination_botola < 0 || destination_botola > n_botole)
      printf("Input non consentito, inserisci un numero tra 0 e %d\n", n_botole-1);
    (giocatori+impostore)->player_room = *(lista_stanze+stanze_botole[destination_botola]);
  }
}

void sabotaggio(struct Stanza* stanza_sabotaggio){
    stanza_sabotaggio->type = vuota;
}


int imposta_gioco(){


  menu_giocatori();
  while (get_int(&n_players) != 0 || n_players < 4 || n_players > 10){
    printf("Input non consentito, inserisci un numero compreso tra 4 e 10\n");
    menu_giocatori();
  }
  // pointer to struct * 4 + enum + char + enum * 2 --------NOt NOT NOt
  // pointer + enum*2 == 16

  giocatori = (struct Giocatore*) malloc(n_players*(8 + sizeof(int)*2));
  create_room(stanza_inizio, NULL);


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
  int r_int1 = 0;
  int r_int2 = 0;
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
  while (get_ushort(&quest_da_finire) != 0 || quest_da_finire < 4 || quest_da_finire > 50){
    printf("Input non consentito, inserisci un numero compreso tra 4 e 50\n");
    menu_quest();
  }

  // TODO: check if c'ha senso la size della malloc
  lista_stanze = (struct Stanza**) malloc(8*n_players*quest_da_finire);
  /*
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
    */
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
  int players_order[n_players];
  for (int i = 0; i < n_players; i++){
    players_order[i] = i;
  }
  int r_int1 = 0;
  int r_int2 = 0;
  for (int i = 0; i < 35; i++){
    r_int1 = rand()%n_players;
    while ((r_int2 = rand()%n_players) == r_int1);
    // swap array elements
    players_order[r_int1] = players_order[r_int1] ^ players_order[r_int2];
    players_order[r_int2] = players_order[r_int1] ^ players_order[r_int2];
    players_order[r_int1] = players_order[r_int1] ^ players_order[r_int2];
  }
  for (int player = 0; player < n_players; player++){
    int choiche = 0;
    print_info();
    menu_gioca();
    while (get_int(&choiche) != 0 || choiche < 1 || choiche > 3){
      printf("Input non consentito, inserisci un numero compreso tra 4 e 50\n");
      menu_gioca();
    }
    if ((giocatori+player)->player_state == 0){
      switch (choice){
        case 1:
          avanza();
          break;
        case 2:
          esegui_quest();
          break;
        case 3:
          chiamata_emergenza();
          break;
        default:
          printf("Inserisci un numero tra 1 e 3\n");
          menu_gioca();
          //TODO: gotta scanf again bro
          break;
      }
    }
    else if ((giocatori+player)->player_state == 0){
      switch (choice){
        case 1;
          avanza();
          break;
        case 2:
          uccidi_astronauta();
          break;
        case 3;
          usa_botola();
          break;
        case 4;
          sabotaggio();
          break;
        default:
          printf("Inserisci un numero tra 1 e 3\n");
          menu_gioca();
          //TODO: gotta scanf again bro
          break;
      }
    }

  return 0;
}

int deallocate_memory(){
  printf("dealloca\n");
  return 0;
}
