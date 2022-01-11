//  Partie interface du module holdall.

//  Le module holdall permet de gérer un fourretout de pointeurs du type
//    générique void *.

#ifndef HOLDALL__H
#define HOLDALL__H

#include <stdlib.h>

//  struct holdall, holdall : structure regroupant les informations permettant
//    de gérer un fourretout de pointeurs du type générique void *. La création
//    de la structure de données associée est confiée à la fonction
//    holdall_empty.
//  L'utilisateur est le seul responsable de la valeur des adresses fournies à
//    la fonction holdall_put et des fonctions fournies aux fonctions
//    holdall_apply et holdall_apply_context. Si des opérations d'allocation
//    dynamique sont effectuées, elles le sont pour la gestion propre de la
//    structure de données, et alors en aucun cas pour réaliser de copies ou
//    des destructions d'objets éventuellement pointés par les adresses
//    fournies, ou par l'utilisation des fonctions fournies.
typedef struct holdall holdall;

//  Les fonctions qui suivent ont un comportement indéterminé si leur paramètre
//    de type holdall * n'est pas l'adresse d'un objet préalablement renvoyé par
//    holdall_empty et non révoqué depuis par holdall_dispose. Cette règle ne
//    souffre que d'une seule exception : holdall_dispose tolère que la
//    déréférence de son argument ait pour valeur NULL.

//  holdall_empty : crée une structure de données correspondant initialement à
//    un fourretout vide. Renvoie NULL en cas de dépassement de capacité.
//    Renvoie un pointeur vers l'objet qui gère la structure de données sinon.
extern holdall *holdall_empty(void);

//  holdall_put : ajoute ptr au fourretout associé à ha. Renvoie une valeur non
//    nulle en cas de dépassement de capacité. Renvoie zéro sinon.
extern int holdall_put(holdall *ha, void *ptr);

//  holdall_count : renvoie le nombre d'ajouts effectués au fourretout associé à
//     ha depuis sa création.
extern size_t holdall_count(holdall *ha);

//  holdall_apply : exécute fun sur les adresses ajoutées au fourretout associé
//    à ha. Si, pour une adresse, fun renvoie une valeur non nulle, l'exécution
//    prend fin et holdall_apply renvoie cette valeur. Sinon, fun est exécutée
//    sur toutes les adresses et holdall_apply renvoie zéro.
extern int holdall_apply(holdall *ha, int (*fun)(void *));

//  holdall_apply_context : exécute fun2(ptr, fun1(context, ptr)) sur les
//    adresses ptr ajoutées au fourretout associé à ha. Si, pour une adresse
//    ptr, fun2 renvoie une valeur non nulle, l'exécution prend fin et
//    holdall_apply_context renvoie cette valeur. Sinon, fun est exécutée sur
//    toutes les adresses et holdall_apply_context renvoie zéro.
int holdall_apply_context(holdall *ha,
    void *context, void *(*fun1)(void *context, void *ptr),
    int (*fun2)(void *ptr, void *resultfun1));

//  holdall_apply_context2 : exécute fun2(context2, ptr, fun1(context1, ptr))
//    sur les adresses ptr ajoutées au fourretout associé à ha. Si, pour une
//    adresse ptr, fun2 renvoie une valeur non nulle, l'exécution prend fin et
//    holdall_apply_context2 renvoie cette valeur. Sinon, fun2 est exécutée sur
//    toutes les adresses et holdall_apply_context2 renvoie zéro.
extern int holdall_apply_context2(holdall *ha,
    void *context1, void *(*fun1)(void *context1, void *ptr),
    void *context2, int (*fun2)(void *context2, void *ptr, void *resultfun1));

//  holdall_sort : trie le fourretout associé à ha selon la fonction compar
//    appliquée aux adresses qui lui ont été ajoutées. Renvoie une valeur non
//    nulle en cas de dépassement de capacité. Renvoie sinon zéro.
extern int holdall_sort(holdall *ha, int (*compar)(const void *, const void *));

//  holdall_dispose : si *haptr ne vaut pas NULL, libère les ressources allouées
//    à la structure de données associée à *haptr puis affecte à *haptr la
//    valeur NULL.
extern void holdall_dispose(holdall **haptr);

#endif
