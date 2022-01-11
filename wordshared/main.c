#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "hashtable.h"
#include "holdall.h"

#define COUNT_MAX 1000
#define LIMIT 10
#define PREFIXE 63
#define SIZE 256

//  wordshared : structure contenant le motif de longueur n + 1 fichier lues et le
//    nombre total d'occurrences cptr d'un mot partagé pointé par key.
typedef struct wordshared wordshared;

struct wordshared {
  char *pattern;
  const char *count;
  char *word;
};

char count[COUNT_MAX + 1];

bool same_argument(const char* arg, const char* word);
bool is_contains(const char *arg, const char *subchar);
void display_error(int level);
void display_helps();

//  str_hashfun : l'une des fonctions de pré-hachage conseillées par Kernighan
//    et Pike pour les chaines de caractères.
static size_t str_hashfun(const char *s);

//  scptr_display : Si l'entier pointé par p n'est pas nul
//    affiche sur la sortie standard *count, le caractère
//    tabulation, la chaine de caractères pointée par s et la fin de ligne puis
//    décrémente l'entier pointé par p. Sinon n'affiche rien.
//    Renvoie zéro en cas de succès, une valeur non nulle en cas d'échec.
static int scptr_display(int *p, const wordshared *ws);

//  wifree : libère la zone mémoire pointée par wiptr ainsi que celle des champ
// w et motif de la structure pointée par wiptr et renvoie zéro.
static int word_shared_free(wordshared *ws);

//  comparison_pattern : compare le champ pattern de deux wordshared a et b. Renvoie un nombre
//    positif si a contient plus de "x" que b, 0 si a et b contient autant de "x",
//    un nombre négatif sinon
static int comparison_pattern(wordshared *a, wordshared *b);

//  winfo_hashtable_search : applique la fonction hashtable search au champ w
//    d'une structure winfo
static const void *ws_hashtable_search(hashtable *ht, const wordshared *ws) {
  return hashtable_search(ht, ws->word);
}

// Fonction principale
int main(int argc, char *argv[]) {
  hashtable *ht = hashtable_empty((int (*)(const void *, const void *))strcmp,
      (size_t (*)(const void *))str_hashfun);

  holdall *hl = holdall_empty();
  if (ht == NULL) {
    printf("ERREUR DE CAPACITE\n");
    return EXIT_FAILURE;
  }

  // Valeurs par défaut
  bool show_ponct = true;
  bool display_all_upper = false;
  bool display_not_shared = false;
  int limit = LIMIT;
  unsigned int prefixe = PREFIXE;
  int number_text = 0;

  // Gestion du nombre minimale d'information requis 
  if (argc < 2) {
    printf("LIGNE DE COMMANDE INCORRECTE SAISIR --help POUR PLUS D'INFORMATIONS\n");
    return EXIT_FAILURE;
  }

  // Parcours de la ligne de commande pour savoir ce que l'on va executer après lecture
  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
    if (!is_contains(argv[i], ".txt") && number_text == 0) {
      if (same_argument(argv[i], "--help")) {
        display_helps();
        break;
      } else {
        // Gestion de la limitation d'affichage
        if (same_argument(argv[i], "-t")) {
          limit = atoi(argv[i + 1]);
          i += 1;
        }
        // Gestion de du nombre de préfixe
        if (same_argument(argv[i], "-i")) {
          prefixe = (unsigned int) atoi(argv[i + 1]);
          i += 1;
        }
        // Affichage des termes en majuscules
        if (same_argument(argv[i], "-u")) {
          display_all_upper = true;
        }
        // Non affichage des ponctuations
        if (same_argument(argv[i], "-p")) {
          show_ponct = false;
        }
        // Affichage des mots non encore listés
        if (same_argument(argv[i], "-s")) {
          display_not_shared = true;
        }
      }
    } else {
      // Récupèration d'un fichier ?
      if (is_contains(argv[i], ".txt")) {
        number_text += 1;
      }
    } 
    // printf(" argv[%d] %s\n", i, argv[i]);
  }
  printf("le nombre de fichier recup %d\n", number_text);
  // On parcours chaque fichier
  // int i = argc - number_text;
  for (int files = argc - number_text; files < argc; files++) {
    printf("%d fichier\n", files);
    FILE *f = fopen(argv[files], "r");
    char w[prefixe];
    while (fscanf(f, "%[^\n ] ", w) == 1) {

      if (strlen(w) == prefixe) {
        fprintf(stderr, "*** ATTENTION: Le mot '%s...' est possiblement coupe.\n", w);
        // while (!isspace(fgetc(f))) {}
      }
      // on recherche notre mot car il existe peut être déjà
      wordshared *ws = (wordshared *) hashtable_search(ht, w);
      if (ws != NULL) {
        if ((int) ((ws->count) - count) != COUNT_MAX + 1) {
          ws->count += 1;
          ws->pattern[files - 1] = 'x';
          hashtable_add(ht, w, ws);
        }
      } else {
        // dans le cas ou le mot n'existe pas
        char *s = malloc(strlen(w) + 1);
        if (s == NULL) {
          // return EXIT_FAILURE;
          printf("null\n");
        }
        wordshared *ws = malloc(sizeof *ws);
        if (ws == NULL) {
          free(s);
          // return EXIT_FAILURE;
          printf("null\n");
        }
        char ptrn[SIZE];
        for (int j = (argc - number_text); j < argc; j++) {
          if (j == files) {
            ptrn[j] = 'X';
          } else {
            ptrn[j] = '_';
          }
        }
        ptrn[argc - 1] = '\0';
        char *p = malloc(strlen(ptrn) + 1);
        if (p == NULL) {
          free(s);
          free(ws);
          // return EXIT_FAILURE;
          printf("null\n");
        }
        if (strlen(w) >= prefixe) {
          strncpy(s, w, prefixe);
        } else {
          strcpy(s, w);
        }
        ws->word = s;
        strncpy(p, ptrn, (size_t) number_text + 1);
        // strcpy(p, ptrn);
        ws->pattern = ptrn;
        if (holdall_put(hl, ws) != 0) {
          free(s);
          free(ws);
          printf("ERREUR DE CAPACITE\n");
          // return EXIT_FAILURE;
        }
        ws->count = count + 1;
        if (hashtable_add(ht, s, ws) == NULL) {
          printf("ERREUR DE CAPACITE\n");
        }
      }
    }
    if (!feof(f)) {
      return EXIT_FAILURE;
    }
    if (fclose(f) == EOF) {
      return EXIT_FAILURE;
    }
  }
  // affichage des valeurs par défaut 
  printf("LIMIT %d\n", limit);
  printf("PREFIX %d\n", prefixe);
  printf("PONCT %d\n", (show_ponct ? 1 : 0));
  printf("UPPER %d\n", (display_all_upper ? 1 : 0));
  printf("SHARED %d\n", (display_not_shared ? 1 : 0));

  fprintf(stderr, "--- Info: Number of distinct words: %zu.\n",
      holdall_count(hl));
  int p = limit;
  holdall_sort(hl, (int (*)(const void *, const void *))comparison_pattern);
  if (holdall_apply_context2(hl,
      ht, (void *(*)(void *, void *))ws_hashtable_search, &p,
      (int (*)(void *, void *, void *))scptr_display) != 0) {
    return EXIT_FAILURE;
  }

  // Libère les ressource 
  hashtable_dispose(&ht);
  if (hl != NULL) {
    holdall_apply(hl, (int (*)(void *))word_shared_free);
  }
  holdall_dispose(&hl);
}


// Retourne un booléen en fonction de l'égalité de arg avec word
bool same_argument(const char* arg, const char* word) { 
  return strcmp(arg, word) == 0;
}

// Vérifie si subchar est contenue dans arg
bool is_contains(const char *arg, const char *subchar) {
  char *test = strstr(arg, subchar);
  if (test == NULL) {
    return false;
  }
  return true;
}

// Affichera quelque chose en fonction de l'erreur récupérer
void display_error(int level) {
  switch (level) {
    case 1: 
      printf("Vous aviez specifier aucun fichier !\n");
      break;
    case 2:
      printf("Il me faut un second fichier pour comparer !\n");
      break;
    default :
      printf("Je ne sais pas ce que vous aviez fichu debrouillez-vous bordel\n");
      break;
  }
}

void display_helps() {
  printf("_____ LISTE DES COMMANDES ______\n");
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

int scptr_display(int *p, const wordshared *ws) {
  if (*p > 0) {
    (*p)--;
    return printf("%s\t%ld\t\t%s\n", ws->pattern, (ws->count) - count, ws->word) < 0;
  }
  return 0;
}

int word_shared_free(wordshared *ws) {
  free(ws->word);
  free(ws->pattern);
  free(ws);
  return 0;
}

int comparison_pattern(wordshared *a, wordshared *b) {
  size_t number_x_in_a = 0;
  size_t number_x_in_b = 0;
  for (size_t i = 0; a->pattern[i] != '\0'; i++) {
    if (a->pattern[i] == 'x') {
      number_x_in_a++;
    }
    if (b->pattern[i] == 'x') {
      number_x_in_b++;
    }
  }

  if (number_x_in_a > number_x_in_b) {
    return 1;
  } else if (number_x_in_a < number_x_in_b) {
    return -1;
  } else {
    if (a->count - count > b->count - count) {
      return 1;
    } else if (a->count - count < b->count - count) {
      return -1;
    } else {
      return strcmp(b->word, a->word);
    }
  }
}