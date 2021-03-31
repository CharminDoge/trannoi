#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

struct Giocatore* giocatori;
unsigned short quest_da_finire;
struct Stanza* stanza_inizio;
//TODO: da utilizzare come giocatori
struct Stanza** lista_stanze;
int n_stanze = 0;

char room[4][17] = {"vuota", "quest_semplice", "quest_complicata", "botola"};
char state[4][13] = {"astronauta", "impostore", "assassinato", "defenestrato"};
char colors[10][7] = {"red", "blue", "green", "yellow", "orange", "black", "white", "purple", "cyan", "brown"};


int n_players = 0;

int myrand(){
  static time_t t;
  static char done = 0;
  if (done == 0){
    srand((unsigned) time(&t));
    done = 1;
  }
  return rand();
}


void randomize_player_intarray(int* player_array, int n){
  for (int i = 0; i < n; i++){
    player_array[i] = i;
  }
  int r_int1 = 0;
  int r_int2 = 0;
  for (int i = 0; i < 100; i++){
    r_int1 = myrand()%n;
    while ((r_int2 = myrand()%n) == r_int1);
    player_array[r_int1] = player_array[r_int1] ^ player_array[r_int2];
    player_array[r_int2] = player_array[r_int1] ^ player_array[r_int2];
    player_array[r_int1] = player_array[r_int1] ^ player_array[r_int2];
  }
}
// da controllare ogni volta che viene eseguita una quest o varia il numero di impostori e astronauti
int check_end_game(){
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
  if (n_impostori > n_astronauti)
    return 3;
  return 0;
}


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

//TODO
int inizia_gioco(){
  printf("Gioco impostato correttamente\n");
  return 0;
}


void print_info(int giocatore){
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
      printf("\nCi sono altri %d impostori nell'astronave\n", n_impostori);
    else
      printf("\nNon ci sono altri impostori nell'astronave\n");
  }
}


int players_in_room(struct Stanza* room){
  int count = 0;
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == room){
      count++;
    }
  }
  return count;
}


int check_killable(struct Stanza* room){
  int count = 0;
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == room && (giocatori+player)->player_state == astronauta){
      return 1;
    }
  }
  return 0;
}


// funzione per stampare le possibili azioni del giocatore
// arr_func serve per trasformare l'input dell'utente (limitato dalle sue possibilità)
// nell'int corrispondente alla rispettiva funzione
// arr_func = [1, 2, 4 ...] mi dice che se l'utente inserisce 3 come input, verrà chiamata la funzione 4 (sabotaggio)
void menu_gioca(int giocatore, int* scelte, int arr_func[]){
  if ((giocatori+giocatore)->player_state == astronauta){
    printf("\nCosa vuoi fare?\n");
    printf("1) Avanza\n");
    arr_func[*scelte] = 1;
    (*scelte)++;
    if ((giocatori+giocatore)->player_room->type == quest_semplice || (giocatori+giocatore)->player_room->type == quest_complicata){
      arr_func[*scelte] = 2;
      (*scelte)++;
      printf("%d) Fai quest\n", *scelte);
    }
    if (players_in_room((giocatori+giocatore)->player_room) > 1){
      arr_func[*scelte] = 3;
      (*scelte)++;
      printf("%d) Chiamata d'emergenza\n", *scelte);
    }
  }
  else if ((giocatori+giocatore)->player_state == impostore){
    printf("Cosa vuoi fare?\n");
    printf("1) Avanza\n");
    arr_func[*scelte] = 1;
    (*scelte)++;
    if (check_killable((giocatori+giocatore)->player_room) == 1){
      arr_func[*scelte] = 2;
      (*scelte)++;
      printf("%d) Uccidi astronauta\n", *scelte);
    }
    arr_func[*scelte] = 3;
    (*scelte)++;
    printf("%d) Usa botola\n", *scelte);
    if ((giocatori+giocatore)->player_room->type == quest_semplice || (giocatori+giocatore)->player_room->type == quest_complicata){
      arr_func[*scelte] = 4;
      (*scelte)++;
      printf("%d) Sabota la nave\n", *scelte);
    }
  }
}


void get_int_azione(int* func, int giocatore){
  int scelte = 0;
  int arr_func[4];
  int choice = 0;
  menu_gioca(giocatore, &scelte, arr_func);
  while (get_int(&choice) != 0 || choice < 1 || choice > scelte){
    printf("Input non consentito, inserisci un numero tra 1 e %d\n", scelte);
    menu_gioca(giocatore, &scelte, arr_func);
  }
  *func = arr_func[choice-1];
}

void create_room(struct Stanza** new_stanza_ptr, struct Stanza* stanza_precedente){
  // add check to see if malloc succedded
  *new_stanza_ptr = (struct Stanza*) malloc(8*4+sizeof(int)+sizeof(char));
  struct Stanza* new_stanza = *new_stanza_ptr;
  new_stanza->avanti = NULL;
  new_stanza->sinistra = NULL;
  new_stanza->destra = NULL;
  new_stanza->stanza_precedente = stanza_precedente;
  int room_rand = myrand()%100;
  new_stanza->type = botola;
  if (room_rand < 75){
    new_stanza->type = quest_complicata;
    if (room_rand < 60){
      new_stanza->type = quest_semplice;
      if (room_rand < 30)
        new_stanza->type = vuota;
    }
  }
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
    default:
      printf("Input non consentito, inserisci un numero tra 1 e 3\n");
      break;
  }
}

void esegui_quest(struct Stanza* stanza_quest){
  if (stanza_quest->type == 1 ){
    quest_da_finire -= 1;
    printf("hai eseguito una quest quest_semplice\n");
  }
  else if (stanza_quest->type == 2){
    if (quest_da_finire > 1)
      quest_da_finire -= 2;
    else
    quest_da_finire -= 1;
    printf("hai eseguito una quest quest_complicata\n");
  }
  //non posso fare la quest due volte nella stessa stanza
  stanza_quest->type = vuota;
  // or simply quest_da_finire += stanza_quest->type se effettuo prima il controllo sulla stanza per capire se poter fare o meno l'esegui quest
}


void chiamata_emergenza(struct Stanza* stanza_chiamata){
  printf("entered chiamata_emergenza\n");
  // TODO: need to check first if someone has been killed
  int room_astronauti = 0;
  int room_impostori = 0;
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == stanza_chiamata){
      if ((giocatori+player)->player_state == astronauta)
        room_astronauti++;
      else if ((giocatori+player)->player_state == impostore)
        room_impostori++;
    }
  }
  //TODO: ASSOLUTAMENTE da rifare tutto il sistema di probabilità, non fai il controllo su chi è ancora vivo
  int prob_astronauti = (3+(2*(room_impostori-1))-(3*(room_astronauti-1)))*room_astronauti;
  int prob_impostori = (3+(2*(room_astronauti-1))-(3*(room_impostori-1)))*room_impostori;
  int rand_prob = myrand()%(prob_astronauti+prob_impostori);
  if (rand_prob < prob_astronauti){
    printf("L'%s %s è stato defenestrato\n", state[(giocatori+rand_prob/(prob_astronauti/room_astronauti))->player_state], colors[(giocatori+rand_prob/(prob_astronauti/room_astronauti))->player_name]);
    (giocatori+rand_prob/(prob_astronauti/room_astronauti))->player_state = defenestrato;
  }
  else{
    printf("L'%s %s è stato defenestrato\n", state[(giocatori+(rand_prob-prob_astronauti)/(prob_impostori/room_impostori))->player_state], colors[(giocatori+(rand_prob-prob_astronauti)/(prob_impostori/room_impostori))->player_name]);
    (giocatori+(rand_prob-prob_astronauti)/(prob_impostori/room_impostori))->player_state = defenestrato;
  }
  stanza_chiamata->emergenza_chiamata = 1;
}


void uccidi_astronauta(int killer, struct Stanza* stanza_uccisione){
  //il giocatore ucciso non può più giocare (da implementare nel for delle azioni dei giocatori in gioca())
  int room_astronauti = 0;
  int killable_players[n_players];
  for (int player = 0; player < n_players; player++){
    if ((giocatori+player)->player_room == stanza_uccisione && (giocatori+player)->player_state == astronauta){
      static int i = 0;
      room_astronauti++;
      killable_players[i] = player;
      i++;
    }
  }

  if (room_astronauti >= 1){
    printf("Gli astronauti\n");
    for (int player = 0; player < room_astronauti; player++){
      printf("%d) %s\n", player, colors[(giocatori+killable_players[player])->player_name]);
    }
    printf("sono nella stanza con te! Chi vuoi uccidere?\n");
    int player_to_kill = -1;
    int exit = 0;
    while (get_int(&player_to_kill) != 0 || player_to_kill < 0 || player_to_kill > room_astronauti)
      printf("Input non consentito, inserisci un numero tra 0 e %d\n", room_astronauti-1);
    (giocatori+killable_players[player_to_kill])->player_state = assassinato;
    printf("L'astronauta %s è stato ucciso\n", colors[(giocatori+killable_players[player_to_kill])->player_name]);
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
        printf("%s te sei fregato da solo\n", colors[(giocatori+killer)->player_name]);
      }
      else if (impostor_prob > 0){
        int rand_prob = myrand() % 10;
        if (rand_prob > impostor_prob){
          (giocatori+killer)->player_state = defenestrato;
          printf("%s te sei fregato da solo\n", colors[(giocatori+killer)->player_name]);
        }
      }
    }
  }
}


void usa_botola(int impostore, struct Stanza* stanza_botola){
  int n_botole = 0;
  int stanze_botole[n_stanze];
  for (int stanza = 0; stanza < n_stanze; stanza++){
    if ((*(lista_stanze+stanza))->type == botola && *(lista_stanze+stanza) != stanza_botola){
      stanze_botole[n_botole] = stanza;
      n_botole++;
    }
  }
  if (n_botole == 0){
    (giocatori+impostore)->player_room = *(lista_stanze+myrand()%n_stanze);
    printf("Non ci sono altre botole, sei stato teletrasportato in una stanza casuale\n");
  }
  else{
    printf("Ci sono %d stanze con una botola! Dove vuoi andare? [0:%d]\n", n_botole, n_botole-1);
    int destination_botola = -1;
    while (get_int(&destination_botola) != 0 || destination_botola < 0 || destination_botola > n_botole-1)
      printf("Input non consentito, inserisci un numero tra 0 e %d\n", n_botole-1);
    (giocatori+impostore)->player_room = *(lista_stanze+stanze_botole[destination_botola]);
    printf("Ti sei teletrasportato nella stanza scelta\n");
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
  // TODO: check if c'ha senso la size della malloc
  lista_stanze = (struct Stanza**) malloc(8*n_players*quest_da_finire);
  create_room(&stanza_inizio, NULL);

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
  int random_colors[10];
  randomize_player_intarray(random_colors, 10);
  for (int player = 0; player < n_players; player++){
    (giocatori+player)->player_room = stanza_inizio;
    (giocatori+player)->player_state = astronauta;
    (giocatori+player)->player_name = random_colors[player];
  }



  // pseudo-random name shuffle
  int r_int1 = 0;
  int r_int2 = 0;
  for (int i = 0; i < 35; i++){
    r_int1 = myrand()%n_players;
    while ((r_int2 = myrand()%n_players) == r_int1);
    // swap array elements
    (giocatori+r_int1)->player_name = (giocatori+r_int1)->player_name ^ (giocatori+r_int2)->player_name;
    (giocatori+r_int2)->player_name = (giocatori+r_int1)->player_name ^ (giocatori+r_int2)->player_name;
    (giocatori+r_int1)->player_name = (giocatori+r_int1)->player_name ^ (giocatori+r_int2)->player_name;
  }

  // set impostori
  int impostors = 0;
  for (int i = 0; i < ((n_players-1)/3); i++){
    (giocatori+myrand()%n_players)->player_state = impostore;
  }
  printf("Giocatori impostati: %d\n", n_players);


  menu_quest();
  while (get_ushort(&quest_da_finire) != 0 || quest_da_finire < 4 || quest_da_finire > 50){
    printf("Input non consentito, inserisci un numero compreso tra 4 e 50\n");
    menu_quest();
  }


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
        stampa_giocatori();
    else if (scelta != 2)
        printf("Input non consentito, inserisci 1 o 2\n");
  }

  return inizia_gioco();
}


int gioca(){

  // order randomize
  int players_order[n_players];
  randomize_player_intarray(players_order, n_players);


  for (int player = 0; player < n_players; player++){
    printf("%s\n", colors[(giocatori+players_order[player])->player_name]);
  }
  int game = 0;
  int turno = 0;
  while (game == 0){
    for (int player = 0; player < n_players; player++){
      int cycle_player = players_order[player];
      turno++;
      if ((giocatori+cycle_player)->player_state == astronauta){
        int choice = 0;
        printf("\nTurno %d\n", turno);
        print_info(cycle_player);
        /*
        menu_gioca((giocatori+cycle_player)->player_state);
        while (get_int_azione(&choice) != 0 || choice < 1 || choice > 3){
          printf("Input non consentito, inserisci un numero compreso tra 1 e 3\n");
          menu_gioca((giocatori+cycle_player)->player_state);
        }
        */
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
          default:
            printf("Unexpected error\n");
            //menu_gioca((giocatori+cycle_player)->player_state);
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
          default:
            printf("Unexpected error\n");
            //menu_gioca((giocatori+cycle_player)->player_state);
            break;
        }
      }
      else if ((giocatori+cycle_player)->player_state > 2){
        printf("\n%s è un %s, non può giocare\n", colors[(giocatori+cycle_player)->player_name], state[(giocatori+cycle_player)->player_state]);
      }
      if ((game = check_end_game()) > 0){
        switch (game) {
          case 1:
            printf("Le quest sono finite. Gli astronauti vincono il gioco.\n");
            break;
          case 2:
            printf("Gli impostori sono stati scoperti. Gli astronauti vincono il gioco\n");
            break;
          case 3:
            printf("Gli astronauti sono outnumbered. Gli impostori vincono il gioco\n");
            break;
        }
        break;
      }
    }
  }
  return 0;
}

int deallocate_memory(){
  //dealloca memoria quando esci e quando viene chiamata la imposta gioco per la seconda volta
  //resetta variabili globali
  printf("dealloca\n");
  return 0;
}
