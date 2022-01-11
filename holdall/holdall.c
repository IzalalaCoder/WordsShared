//  Partie implantation du module holdall.

#include "holdall.h"
#include <stdio.h>
//  struct holdall, holdall : implantation par liste dynamique simplement
//    chainée. L'insertion a lieu en queue si la macroconstante
//    HOLDALL_INSERT_TAIL est définie, en tête sinon. L'activation de
//    l'insertion en queue rend la fonction holdall_sort plus efficace
#define HOLDALL_INSERT_TAIL 1

typedef struct choldall choldall;

struct choldall {
  void *value;
  choldall *next;
};

struct holdall {
  choldall *head;
#ifdef HOLDALL_INSERT_TAIL
  choldall *tail;
#endif
  size_t count;
};

holdall *holdall_empty(void) {
  holdall *ha = malloc(sizeof *ha);
  if (ha == NULL) {
    return NULL;
  }
  ha->head = NULL;
#ifdef HOLDALL_INSERT_TAIL
  ha->tail = NULL;
#endif
  ha->count = 0;
  return ha;
}

int holdall_put(holdall *ha, void *ptr) {
  choldall *p = malloc(sizeof *p);
  if (p == NULL) {
    return -1;
  }
  p->value = ptr;
#ifdef HOLDALL_INSERT_TAIL
  p->next = NULL;
  if (ha->tail == NULL) {
    ha->head = p;
  } else {
    ha->tail->next = p;
  }
  ha->tail = p;
#else
  p->next = ha->head;
  ha->head = p;
#endif
  ha->count += 1;
  return 0;
}

size_t holdall_count(holdall *ha) {
  return ha->count;
}

int holdall_apply(holdall *ha, int (*fun)(void *)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun(p->value);
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context(holdall *ha,
    void *context, void *(*fun1)(void *context, void *ptr),
    int (*fun2)(void *ptr, void *resultfun1)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(p->value, fun1(context, p->value));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context2(holdall *ha,
    void *context1, void *(*fun1)(void *context1, void *ptr),
    void *context2, int (*fun2)(void *context2, void *ptr, void *resultfun1)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(context2, p->value, fun1(context1, p->value));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

void holdall_dispose(holdall **haptr) {
  if (*haptr == NULL) {
    return;
  }
  choldall *p = (*haptr)->head;
  while (p != NULL) {
    choldall *t = p;
    p = p->next;
    free(t);
  }
  free(*haptr);
  *haptr = NULL;
}

//  holdall_length : renvoie la longueur de la liste ha
static size_t holdall_length(holdall *ha) {
  size_t n = 0;
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    n++;
  }
  return n;
}

//  holdall_move_all_head, holdall_move_all_tail : déplace en tête (head) ou en
//    queue (tail) de la liste associée à dest la liste associée à src, de telle
//    sorte que, à la terminaison, la liste associée à src est vide et la liste
//    associée à dest est la concaténation des listes originelles associées à
//    src (head) et dest ou à dest et src (tail). En cas de succès, la fonction
//    renvoie zéro. En cas d'échec, parce que src == dest, renvoie une valeur
//    non nulle.
static int holdall_move_all_tail(holdall *src, holdall *dest) {
  if (src == dest) {
    return -1;
  }
  if (src->head == NULL) {
    return 0;
  }
  if (dest->head == NULL) {
    dest->head = src->head;
    src->head = NULL;
#ifdef HOLDALL_INSERT_TAIL
    dest->tail = src->tail;
    src->tail = NULL;
#endif
  } else {
#ifdef HOLDALL_INSERT_TAIL
    dest->tail->next = src->head;
    dest->tail = src->tail;
    src->head = NULL;
    src->tail = NULL;
#else
    choldall *p = dest->head;
    for (p; p->next != NULL; p = p->next) {
    }
    p->next = src->head;
    src->head = NULL;
#endif
  }
  return 0;
}

static int holdall_move_all_head(holdall *src, holdall *dest) {
  if (src == dest) {
    return -1;
  }
  if (src->head == NULL) {
    return 0;
  }
  if (dest->head == NULL) {
    dest->head = src->head;
    src->head = NULL;
#ifdef HOLDALL_INSERT_TAIL
    dest->tail = src->tail;
    src->tail = NULL;
#endif
  } else {
#ifdef HOLDALL_INSERT_TAIL
    src->tail->next = dest->head;
    dest->head = src->head;
    src->head = NULL;
    src->tail = NULL;
#else
    choldall *p = src->head;
    for (p; p->next != NULL; p = p->next) {
    }
    p->next = dest->head;
    dest->head = src->head;
    src->head = NULL;
#endif
  }
  return 0;
}

//holdall_move_head_tail : déplace l'élément de tête de la
//    liste associée à src vers la queue de
//    la liste associée à dest. Renvoie zéro en cas de succès, une valeur non
//    nulle en cas d'échec pour cause de liste associée à src vide.
static int holdall_move_head_tail(holdall *src, holdall *dest) {
  if (src == dest) {
    return -1;
  }
  if (src->head == NULL) {
    return 0;
  }
  if (dest->head == NULL) {
    dest->head = src->head;
    src->head = src->head->next;
    dest->head->next = NULL;
#ifdef HOLDALL_INSERT_TAIL
    dest->tail = dest->head;
    if (src->head == NULL) {
      src->tail = NULL;
    }
#endif
  } else {
#ifdef HOLDALL_INSERT_TAIL
    dest->tail->next = src->head;
    src->head = src->head->next;
    dest->tail = dest->tail->next;
    dest->tail->next = NULL;
#else
    choldall *p = dest->head;
    for (p; p->next != NULL; p = p->next) {
    }
    p->next = src->head;
    src->head = src->head->next;
    p = p->next;
    p->next = NULL;
#endif
  }
  return 0;
}

//holdall_partition_pivot_tail : déplace respectivement en queue des
// listes associées à halth, haeq et hagth, les éléments de la liste associée à
// ha
// qui sont strictement inférieurs, égaux et strictement supérieurs au
// premier élément de la liste associée à ha au sens de compar. Si la liste
// associée à ha est vide, les quatre listes demeurent inchangées. À la
// terminaison, la liste associée à ha est vide, les éléments des listes
// associées à halth, haeq et hagth sont dans le même ordre que celui avec
// lequel
// ils figuraient originellementdans ha.
static void holdall_partition_pivot_tail(holdall *ha, holdall *halth,
    holdall *haeq,
    holdall *hagth, int (*compar)(const void *, const void *)) {
  if (ha->head == NULL) {
    return;
  }
  void *pivot = ha->head->value;
  while (ha->head != NULL) {
    int r = (*compar)(pivot, ha->head->value);
    if (r > 0) {
      holdall_move_head_tail(ha, hagth);
    } else if (r == 0) {
      holdall_move_head_tail(ha, haeq);
    } else {
      holdall_move_head_tail(ha, halth);
    }
  }
}

int holdall_sort(holdall *ha, int (*compar)(const void *, const void *)) {
  holdall *halth = holdall_empty();
  holdall *haeq = holdall_empty();
  holdall *hagth = holdall_empty();
  holdall *haleft = holdall_empty();
  holdall *haright = holdall_empty();
  while (holdall_length(ha) >= 2) {
    holdall_partition_pivot_tail(ha, halth, haeq, hagth, compar);
    if (holdall_length(halth) < holdall_length(hagth)) {
      holdall_sort(halth, compar);
      holdall_move_all_tail(hagth, ha);
      holdall_move_all_tail(halth, haleft);
      holdall_move_all_tail(haeq, haleft);
    } else {
      holdall_sort(hagth, compar);
      holdall_move_all_head(halth, ha);
      holdall_move_all_head(hagth, haright);
      holdall_move_all_head(haeq, haright);
    }
  }
  holdall_move_all_head(haleft, ha);
  holdall_move_all_tail(haright, ha);
  holdall_dispose(&halth);
  holdall_dispose(&haeq);
  holdall_dispose(&hagth);
  holdall_dispose(&haleft);
  holdall_dispose(&haright);
  return 0;
}
