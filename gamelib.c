#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

struct Giocatore* giocatori = NULL;
unsigned short quest_da_finire;
struct Stanza* stanza_inizio;
//TODO: da utilizzare come giocatori
struct Stanza** lista_stanze = NULL;

int n_stanze = 0;
int n_players = 0;

const char room[4][17] = {"vuota", "quest_semplice", "quest_complicata", "botola"};
const char state[4][13] = {"astronauta", "impostore", "assassinato", "defenestrato"};
const char colors[10][7] = {"red", "blue", "green", "yellow", "orange", "black", "white", "purple", "cyan", "brown"};


static int inizia_gioco(){
  printf("\nGioco impostato correttamente\n");
  return 1;
}


static void stampa_giocatori(){
  for (int player = 0; player < n_players; player++){
    printf("%s\t%s\tStanza iniziale -> %s\n", colors[(giocatori+player)->player_name],
              state[(giocatori+player)->player_state],
              room[(giocatori+player)->player_room->type]
              );
  }
}


static void menu_imposta(){
  printf("Cosa vuoi fare:\n");
  printf("1) Vedi informazioni giocatori\n");
  printf("2) Termina impostazione gioco\n");
}


static void menu_quest(){
  printf("Imposta il numero di quest [4-20]: ");
}


static void randomize_intarray(int* player_array, int n){
  int r_int1 = 0;
  int r_int2 = 0;
  for (int i = 0; i < 100; i++){
    r_int1 = rand()%n;
    while ((r_int2 = rand()%n) == r_int1);
    player_array[r_int1] = player_array[r_int1] ^ player_array[r_int2];
    player_array[r_int2] = player_array[r_int1] ^ player_array[r_int2];
    player_array[r_int1] = player_array[r_int1] ^ player_array[r_int2];
  }
}


static void create_room(struct Stanza** new_stanza_ptr, struct Stanza* stanza_precedente){
  // add check to see if malloc succedded
  *new_stanza_ptr = (struct Stanza*) malloc(8*4+sizeof(int)+sizeof(char));
  struct Stanza* new_stanza = *new_stanza_ptr;
  new_stanza->avanti = NULL;
  new_stanza->sinistra = NULL;
  new_stanza->destra = NULL;
  new_stanza->stanza_precedente = stanza_precedente;
  int room_rand = rand()%100;
  new_stanza->type = botola;
  if (room_rand < 75){
    new_stanza->type = quest_complicata;
    if (room_rand < 60){
      new_stanza->type = quest_semplice;
      if (room_rand < 30)
        new_stanza->type = vuota;
    }
  }
  new_stanza->emergenza_chiamata = 0;
  *(lista_stanze+n_stanze) = new_stanza;
  n_stanze++;
}


static void menu_giocatori(){
  printf("Imposta il numero di giocatori [4-10]: ");
}


int imposta_gioco(){


  menu_giocatori();
  while (get_int(&n_players) != 0 || n_players < 4 || n_players > 10){
    printf("Input non consentito, inserisci un numero compreso tra 4 e 10\n");
    menu_giocatori();
  }


  while ((giocatori = (struct Giocatore*) malloc(n_players*(8 + sizeof(int)*2))) == NULL);
  // TODO: check if c'ha senso la size della malloc (non c'ha senso, è troppo piccola) puoi pensare di aggiungere un parametro next alla struct e fare una lista dinamica invece che un array con size fisso
  while ((lista_stanze = (struct Stanza**) malloc(8*n_players*quest_da_finire)) == NULL);
  //TODO: check and fix
  create_room(&stanza_inizio, NULL);


  // pseudo-random name shuffle
  int random_colors[10];
  for (int i = 0; i < 10; i++){
    random_colors[i] = i;
  }
  randomize_intarray(random_colors, 10);

  // player initialization
  for (int player = 0; player < n_players; player++){
    (giocatori+player)->player_room = stanza_inizio;
    (giocatori+player)->player_state = astronauta;
    (giocatori+player)->player_name = random_colors[player];
  }
  // random impostor [1:(n_players-1)/3]
  // 4-6 players -> 1 impostor
  // 7-9 players -> 1-2 impostors
  // 10 players -> 1-3 impostors
  for (int i = 0; i < ((n_players-1)/3); i++){
    (giocatori+rand()%n_players)->player_state = impostore;
  }
  printf("Giocatori impostati: %d\n", n_players);


  menu_quest();
  while (get_ushort(&quest_da_finire) != 0 || quest_da_finire < 4 || quest_da_finire > 20){
    printf("Input non consentito, inserisci un numero compreso tra 4 e 20\n");
    menu_quest();
  }


  int scelta = 0;
  while (scelta != 2){
    menu_imposta();
    while (get_int(&scelta) != 0 || scelta < 1 || scelta > 2){
      printf("Input non consentito, inserisci 1 o 2\n");
      menu_imposta();
    }
    if (scelta == 1)
        stampa_giocatori();
  }

  return inizia_gioco();
}


// TODO: da controllare ogni volta che viene eseguita una quest o varia il numero di impostori e astronauti
static int check_end_game(){
  if (quest_da_finire == 0)
    return 1;
  int n_astronauti = 0;
  int n_impostori = 0;
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_state == astronauta)
      n_astronauti++;
    else if ((giocatori+player)->player_state == impostore)
      n_impostori++;
  }
  if (n_impostori == 0)
    return 2;
  if (n_impostori >= n_astronauti)
    return 3;
  return 0;
}


static void sabotaggio(struct Stanza* stanza_sabotaggio){
    stanza_sabotaggio->type = vuota;
}


static void usa_botola(int impostore, struct Stanza* stanza_botola){
  int n_botole = 0;
  int stanze_botole[n_stanze];
  for (int stanza = 0; stanza < n_stanze; stanza++){
    if ((*(lista_stanze+stanza))->type == botola && *(lista_stanze+stanza) != stanza_botola){
      stanze_botole[n_botole] = stanza;
      n_botole++;
    }
  }
  if (n_botole == 0){
    (giocatori+impostore)->player_room = *(lista_stanze+rand()%n_stanze);
    printf("Non ci sono altre botole, sei stato teletrasportato in una stanza casuale\n");
  }
  else{
    printf("Ci sono altre %d stanze con una botola! Dove vuoi andare?\nInserisci un numero tra 1 e %d:\n", n_botole, n_botole);
    int destination_botola = 0;
    while (get_int(&destination_botola) != 0 || destination_botola < 1 || destination_botola > n_botole)
      printf("Input non consentito, inserisci un numero tra 1 e %d\n", n_botole);
    (giocatori+impostore)->player_room = *(lista_stanze+stanze_botole[destination_botola-1]);
    printf("Ti sei teletrasportato nella stanza scelta\n");
  }
}


static void uccidi_astronauta(int killer, struct Stanza* stanza_uccisione){
  int room_astronauti = 0;
  int killable_players[n_players];
  int i = 0;
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == stanza_uccisione && (giocatori+player)->player_state == astronauta){
      room_astronauti++;
      killable_players[i] = player;
      i++;
    }
  }
  //TODO: se non c'è nessuno non dovrebbe stampare nulla
  if (room_astronauti >= 1){
    printf("Gli astronauti\n");
    for (int player = 0; player < room_astronauti; player++){
      printf("%d) %s\n", player+1, colors[(giocatori+killable_players[player])->player_name]);
    }
    printf("sono nella stanza con te! Chi vuoi uccidere?\n");
    int player_to_kill = -1;
    int exit = 0;
    while (get_int(&player_to_kill) != 0 || player_to_kill < 1 || player_to_kill > room_astronauti)
      printf("Input non consentito, inserisci un numero tra 1 e %d\n", room_astronauti);
    (giocatori+killable_players[player_to_kill-1])->player_state = assassinato;
    printf("\nL'astronauta %s è stato ucciso\n", colors[(giocatori+killable_players[player_to_kill-1])->player_name]);
    // se ci sono due astronauti e uno viene ucciso, vince l'impostore
    if (check_end_game() == 0){
      int prevroom_astronauti = 0;
      for (int player = 0; player < n_players; player++){
        if ((giocatori+player)->player_room == (stanza_uccisione)->stanza_precedente)
          prevroom_astronauti++;
      }
      int impostor_prob = 5*(room_astronauti-1)+2*prevroom_astronauti;
      if (impostor_prob > 10){
        (giocatori+killer)->player_state = defenestrato;
        (giocatori+killer)->player_room->emergenza_chiamata = 1;
        printf("%s ti hanno beccato, sei stato defenetrato\n", colors[(giocatori+killer)->player_name]);
      }
      else if (impostor_prob > 0){
        int rand_prob = rand() % 10;
        if (rand_prob > impostor_prob){
          (giocatori+killer)->player_state = defenestrato;
          (giocatori+killer)->player_room->emergenza_chiamata = 1;
          printf("%s ti hanno beccato, sei stato defenetrato\n", colors[(giocatori+killer)->player_name]);
        }
      }
    }
  }
}


static void chiamata_emergenza(struct Stanza* stanza_chiamata){
  //TODO: rendi il conteggio dei player nella stanza una funzione
  int room_astronauti = 0;
  int astronauti[n_players-1];
  int room_impostori = 0;
  int impostori[3];
  int player_to_defenestrare;
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == stanza_chiamata){
      if ((giocatori+player)->player_state == astronauta){
        astronauti[room_astronauti] = player;
        room_astronauti++;
      }
      else if ((giocatori+player)->player_state == impostore){
        impostori[room_impostori] = player;
        room_impostori++;
      }
    }
  }
  int prob_astronauti = 3+2*room_impostori-3*(room_astronauti-1);
  int prob_impostori = 3+2*room_astronauti-3*(room_impostori-1);
  if (prob_impostori < 0){
    player_to_defenestrare = rand()%room_astronauti;
  }
  else if (prob_astronauti < 0){
    player_to_defenestrare = rand()%room_impostori;
  }
  else if (prob_impostori > 0 && prob_astronauti > 0){
    int rand_prob = rand()%(prob_astronauti*room_astronauti+prob_impostori*room_impostori);
    if (rand_prob < prob_astronauti*room_astronauti){
      player_to_defenestrare = astronauti[rand_prob/prob_astronauti];
    }
    else{
      player_to_defenestrare = impostori[(rand_prob-prob_astronauti*room_astronauti)/prob_impostori];
    }
  }
  printf("L'%s %s è stato defenestrato\n", state[(giocatori+player_to_defenestrare)->player_state], colors[(giocatori+player_to_defenestrare)->player_name]);
  (giocatori+player_to_defenestrare)->player_state = defenestrato;
  stanza_chiamata->emergenza_chiamata = 1;
}


static void esegui_quest(struct Stanza* stanza_quest){
  if (stanza_quest->type == 1 ){
    quest_da_finire -= 1;
    printf("\nHai eseguito una quest semplice, rimangono %d quest\n", quest_da_finire);
  }
  else if (stanza_quest->type == 2){
    if (quest_da_finire > 1)
      quest_da_finire -= 2;
    else
    quest_da_finire -= 1;
    printf("\nHai eseguito una quest complicata, rimangono %d quest\n", quest_da_finire);
  }
  //non posso fare la quest due volte nella stessa stanza
  stanza_quest->type = vuota;
  // or simply quest_da_finire += stanza_quest->type se effettuo prima il controllo sulla stanza per capire se poter fare o meno l'esegui quest
}

static void menu_avanza(){
  printf("Dove vuoi andare?\n");
  printf("1) Avanti\n");
  printf("2) Destra\n");
  printf("3) Sinistra\n");
  printf("4) Resta fermo\n");
}


static void avanza(int giocatore){
  menu_avanza();
  int destination_room = 0;
  while (get_int(&destination_room) != 0 || destination_room < 1 || destination_room > 4){
    printf("Input non consentito, inserisci un numero tra 1 e 4\n");
    menu_avanza();
  }
  // avanti, destra, sinistra
  switch (destination_room){
    case 1:
      if ((giocatori+giocatore)->player_room->avanti == NULL){
        create_room(&((giocatori+giocatore)->player_room->avanti), (giocatori+giocatore)->player_room);
        /*
        for (int player = 0; player < n_players; player++){
          if ((giocatori+player)->player_room == (giocatori+giocatore)->player_room)
          (giocatori+player)->player_room->avanti = (giocatori+giocatore)->player_room->avanti;
        }
        */
      }
      (giocatori+giocatore)->player_room = (giocatori+giocatore)->player_room->avanti;
      break;
    case 2:
      if ((giocatori+giocatore)->player_room->destra == NULL){
        create_room(&((giocatori+giocatore)->player_room->destra), (giocatori+giocatore)->player_room);
      }
      (giocatori+giocatore)->player_room = (giocatori+giocatore)->player_room->destra;
      break;
    case 3:
      if ((giocatori+giocatore)->player_room->sinistra == NULL){
        create_room(&((giocatori+giocatore)->player_room->sinistra), (giocatori+giocatore)->player_room);
      }
      (giocatori+giocatore)->player_room = (giocatori+giocatore)->player_room->sinistra;
      break;
    case 4:
      break;
  }
}


static int check_killable(struct Stanza* room){
  int count = 0;
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == room && (giocatori+player)->player_state == astronauta){
      return 1;
    }
  }
  return 0;
}


static int anyone_killed(struct Stanza* room){
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == room && (giocatori+player)->player_state == assassinato)
      return 1;
  }
  return 0;
}


static int players_in_room(struct Stanza* room){
  int count = 0;
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == room){
      count++;
    }
  }
  return count;
}


// funzione per stampare le possibili azioni del giocatore
// arr_func serve per trasformare l'input dell'utente (limitato dalle sue possibilità)
// nell'int corrispondente alla rispettiva funzione
// arr_func = [1, 2, 4 ...] mi dice che se l'utente inserisce 3 come input, verrà chiamata la funzione 4 (sabotaggio)
static void menu_gioca(int giocatore, int* scelte, int arr_func[]){
  if ((giocatori+giocatore)->player_state == astronauta){
    printf("\nCosa vuoi fare?\n");
    printf("1) Muoviti\n");
    arr_func[*scelte] = 1;
    (*scelte)++;
    if ((giocatori+giocatore)->player_room->type == quest_semplice || (giocatori+giocatore)->player_room->type == quest_complicata){
      arr_func[*scelte] = 2;
      (*scelte)++;
      printf("%d) Fai quest\n", *scelte);
    }
    if ((giocatori+giocatore)->player_room->emergenza_chiamata == 0 && players_in_room((giocatori+giocatore)->player_room) > 1 && anyone_killed((giocatori+giocatore)->player_room) == 1){
      arr_func[*scelte] = 3;
      (*scelte)++;
      printf("%d) Chiamata d'emergenza\n", *scelte);
    }
  }
  else if ((giocatori+giocatore)->player_state == impostore){
    printf("Cosa vuoi fare?\n");
    printf("1) Muoviti\n");
    arr_func[*scelte] = 1;
    (*scelte)++;
    if (check_killable((giocatori+giocatore)->player_room) == 1){
      arr_func[*scelte] = 2;
      (*scelte)++;
      printf("%d) Uccidi astronauta\n", *scelte);
    }
    if ((giocatori+giocatore)->player_room->type == botola && n_stanze > 1){
      arr_func[*scelte] = 3;
      (*scelte)++;
      printf("%d) Usa botola\n", *scelte);
    }
    if ((giocatori+giocatore)->player_room->type == quest_semplice || (giocatori+giocatore)->player_room->type == quest_complicata){
      arr_func[*scelte] = 4;
      (*scelte)++;
      printf("%d) Sabota la nave\n", *scelte);
    }
  }
}


static void get_int_azione(int* func, int giocatore){
  int scelte = 0;
  int arr_func[4];
  int choice = 0;
  menu_gioca(giocatore, &scelte, arr_func);
  while (get_int(&choice) != 0 || choice < 1 || choice > scelte){
    printf("Input non consentito, inserisci un numero tra 1 e %d\n", scelte);
    scelte = 0;
    menu_gioca(giocatore, &scelte, arr_func);
  }
  *func = arr_func[choice-1];
}


static void print_info(int giocatore){
  int n_impostori = 0;
  printf("%s sei un %s\n", colors[(giocatori+giocatore)->player_name], state[(giocatori+giocatore)->player_state]);
  printf("Ti trovi nella stanza: %s\n", room[(giocatori+giocatore)->player_room->type]);
  printf("Rimangono %d quest da fare\n", quest_da_finire);
  printf("Con te ci sono: ");
  for (int player = 0; player < n_players; player++){
    if (((giocatori+player)->player_room == (giocatori+giocatore)->player_room) && (giocatori+player)->player_state < 2 && (player != giocatore))
      printf("%s ", colors[(giocatori+player)->player_name]);
    if ((giocatori+player)->player_state == impostore)
      n_impostori++;
  }
  if ((giocatori+giocatore)->player_state == impostore){
    if (n_impostori > 1)
      printf("\nCi sono altri %d impostori nell'astronave\n", n_impostori-1);
    else
      printf("\nNon ci sono altri impostori nell'astronave\n");
  }
}


int gioca(){

  int players_order[n_players];
  for (int i = 0; i < n_players; i++){
    players_order[i] = i;
  }

  int game_on = 0;
  unsigned int turno = 0;
  while (game_on == 0){
    turno++;
    randomize_intarray(players_order, n_players);

    printf("\nPlayers order:\n");
    for (int player = 0; player < n_players; player++){
      if ((giocatori+players_order[player])->player_state > 1)
        printf("%d) %s    \tstato: %s\n", player+1, colors[(giocatori+players_order[player])->player_name], state[(giocatori+players_order[player])->player_state]);
      else
        printf("%d) %s    \tstato: ****\n", player+1, colors[(giocatori+players_order[player])->player_name]);
    }

    // cycle players
    for (int player = 0; player < n_players; player++){
      int cycle_player = players_order[player];
      printf("\nTurno %d\n", turno);
      if ((giocatori+cycle_player)->player_state == astronauta){
        print_info(cycle_player);
        int choice = 0;
        get_int_azione(&choice, cycle_player);
        switch (choice){
          case 1:
            avanza(cycle_player);
            break;
          case 2:
            esegui_quest((giocatori+cycle_player)->player_room);
            break;
          case 3:
            chiamata_emergenza((giocatori+cycle_player)->player_room);
            break;
        }
      }
      else if ((giocatori+cycle_player)->player_state == impostore){
        int choice = 0;
        print_info(cycle_player);
        get_int_azione(&choice, cycle_player);
        switch (choice){
          case 1:
            avanza(cycle_player);
            break;
          case 2:
            uccidi_astronauta(cycle_player, (giocatori+cycle_player)->player_room);
            break;
          case 3:
            usa_botola(cycle_player, (giocatori+cycle_player)->player_room);
            break;
          case 4:
            sabotaggio((giocatori+cycle_player)->player_room);
            break;
        }
      }
      else if ((giocatori+cycle_player)->player_state > 1){
        printf("%s è un %s, non può giocare\n", colors[(giocatori+cycle_player)->player_name], state[(giocatori+cycle_player)->player_state]);
      }
      if ((game_on = check_end_game()) > 0){
        switch (game_on) {
          case 1:
            printf("Le quest sono finite. Gli astronauti vincono il gioco!\n");
            break;
          case 2:
            printf("Gli impostori sono stati scoperti. Gli astronauti vincono il gioco!\n");
            break;
          case 3:
            printf("Gli astronauti sono outnumbered. Gli impostori vincono il gioco!\n");
            break;
        }
        break;
      }
    }
  }
  return 0;
}


//TODO: log degli eveti suddiviso per turni
//Turno 1
//red è stato ucciso da blue
//blue è stato scoperto
//
//Turno 2
//yellow ha fatto una chiamata d'emergenza
//TODO: da chiamare
int deallocate_memory(){
  printf("dealloco la memoria...\n");
  free(giocatori);
  for (int i = 0; i < n_stanze; i++){
    free(*(lista_stanze+i));
    printf("deallocata stanza %d\n", i);
  }
  free(lista_stanze);
  quest_da_finire = 0;
  n_stanze = 0;
  n_players = 0;
  return 0;
}
