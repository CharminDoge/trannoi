enum Stato_giocatore {astronauta, impostore, assassino, defenestrato};
enum Nome_giocatore {red, blue, green, yellow, orange, black, white, purple, cyan, brown};
enum Tipo_stanza {vuota, quest_semplice, quest_complicata, botola};
//enum Tipo_stanza {"cafeteria", "admin", "storage", "electrical", "lowEngine", "security", "reactor", "upEngine", "medbay", "communication", "shields", "navigation", "o2", "weapons"};

struct Stanza{
  struct Stanza* avanti;
  struct Stanza* sinistra;
  struct Stanza* destra;
  struct Stanza* stanza_precedente;
  enum Tipo_stanza type;
  char emergenza_chiamata;
};

struct Giocatore{
  struct Stanza* playerRoom;
  enum Stato_giocatore playerState;
  enum Nome_giocatore playerName;
};

void printHello();

int gioca();
int imposta_gioco();
int deallocate_memory();
