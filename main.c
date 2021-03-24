#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gamelib.h"

#define GOOD 0
#define ERROR 1
#define EMPTY 2
#define TOOSHORT 3
#define TOOLONG 4


void menu(){
  printf("Cosa vuoi fare?\n");
  printf("1) impostare il gioco\n");
  printf("2) giocare\n");
  printf("3) uscire\n");
}

// la funzione prende una stringa di 128 byte in input
int getLine(char* inputStr) {
    int chr, check;

    if (fgets (inputStr, 128, stdin) == NULL){
      return EMPTY;
    }
    // controllo che almeno 1 carattere sia stato inserito in input
    int len = strlen(inputStr);
    if (len < 2){
      return TOOSHORT;
    }
    // controllo a fine linea se trovo \n
    if (inputStr[len - 1] != '\n') {
        check = 0;
        // 
        while (((chr = getchar()) != '\n') && (chr != EOF))
            check = 1;

        if (check==1){
          return TOOLONG;
        }
        else
          return GOOD;
    }
    // termino l'array
    inputStr[len - 1] = '\0';
    return GOOD;
}


int getInt(int* choice){
  char choiceStr[128];
  if (getLine(choiceStr) != GOOD)
    return ERROR;
  char *end;  //TODO: a che cazzo serve?
  *choice = strtol(choiceStr, &end, 10);
  return GOOD;
}


int main(int argc, char** argv){

  char* name = "anonimo"; //TODO: random name generator (aggettivo + nome, in stile docker container for desktop)
  if (argc > 1){
    name = argv[1];
  }

  int choice = 0;
  int exit = 0;
  do{
    menu();
    if (getInt(&choice) != GOOD)
      printf("Input non consentito, inserisci un numero tra quelli proposti\n");
    else{
      switch (choice) {
        case 1:
          imposta_gioco();
          break;
        case 2:
          printHello();
          // TODO
          // non si può giocare se prima non è stato impostato il gioco
          gioca();
          break;
        case 3:
          printf("doing things...\n");
          deallocate_memory(); //TODO
          printf("%s left the chat\n", name);
          exit = 1;
          break;
        default:
          printf("Input non consentito, inserisci un numero tra quelli proposti\n");
          break;
        }
      }
  } while(exit != 1);

  return GOOD;
}
