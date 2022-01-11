//  Interface polymorphe pour la spécification TABLE du TDA Table(T, T') dans le
//    cas d'une table de hachage par chainage séparé.

#ifndef HASHTABLE__H
#define HASHTABLE__H

#include <stdlib.h>

//  struct hashtable, hashtable : structure regroupant les informations
//    permettant de gérer une table de hachage dont les clés et les valeurs sont
//    des objets quelconques. La création de la structure de données associée
//    est confiée à la fonction hashtable_empty dont le paramètre compar précise
//    la fonction de comparaison des clés et le paramètre hashfun, leur fonction
//    de pré-hachage.
//  La structure de données ne stocke pas les objets mais des pointeurs vers ces
//    objets. Les pointeurs sont du type générique const void *.
//  L'utilisateur doit garantir que l'adresse de chaque objet fournie aux
//    diverses fonctions est valide durant toute la durée de la présence de
//    l'objet dans la table de hachage. Si des opérations d'allocation dynamique
//    sont effectuées, elles le sont pour la gestion propre de la structure de
//    données, et en aucun cas pour réaliser de copies ou des destructions
//    d'objets.
typedef struct hashtable hashtable;

//  Les fonctions qui suivent ont un comportement indéterminé si leur paramètre
//    de type hashtable * n'est pas l'adresse d'un objet préalablement renvoyé
//    par hashtable_empty et non révoqué depuis par hashtable_dispose. Cette
//    règle ne souffre que d'une seule exception : hashtable_dispose tolère que
//    la déréférence de son argument ait pour valeur NULL.

//  L'accès à la structure struct hashtable_checkup et aux fonctions
//    hashtable_get_checkup et hashtable_display_checkup requiert la définition
//    de la macroconstante HASHTABLE_CHECKUP.

//  hashtable_empty : crée une structure de données correspondant initialement
//    à la table de hachage vide. La fonction de comparaison des clés est
//    compar et leur fonction de pré-hachage, hashfun. Renvoie NULL en cas de
//    dépassement de capacité. Renvoie sinon un pointeur vers l'objet qui gère
//    la structure de données.
extern hashtable *hashtable_empty(int (*compar)(const void *, const void *),
    size_t (*hashfun)(const void *));

//  hashtable_add : recherche dans la table de hachage associée à ht une clé
//    égale à celle d'adresse keyptr au sens de compar. Si une telle clé existe,
//    remplace l'adresse de la valeur correspondante par valptr. Sinon, le
//    couple (keyptr, valptr) est ajouté à la structure. Renvoie NULL si
//    valptr vaut NULL ou en cas de dépassement de capacité. Renvoie sinon
//    valptr.
extern const void *hashtable_add(hashtable *ht, const void *keyptr,
    const void *valptr);

//  hashtable_remove : recherche dans la table de hachage associée à ht une clé
//    égale à celle d'adresse keyptr au sens de compar. Si un telle clé existe,
//    retire le couple (ekeyptr, evalptr) de la structure, où ekeyptr est
//    l'adresse de la clé trouvée et evalptr l'adresse de la valeur
//    correspondante. Renvoie NULL si aucune clé de la sorte n'existe. Renvoie
//    sinon evalptr.
extern const void *hashtable_remove(hashtable *ht, const void *keyptr);

//  hashtable_search : recherche dans la table de hachage associée à ht une clé
//    égale à celle d'adresse keyptr au sens de compar. Renvoie NULL si aucune
//    clé de la sorte n'existe. Renvoie sinon l'adresse de la valeur
//    correspondant à la clé trouvée.
extern const void *hashtable_search(hashtable *ht, const void *keyptr);

//  hashtable_dispose : si *htptr ne vaut pas NULL, libère les ressources
//    allouées à la structure de données associée à *htptr puis affecte à *htptr
//    la valeur NULL.
extern void hashtable_dispose(hashtable **htptr);

#ifdef HASHTABLE_CHECKUP

#include <stdio.h>

//  struct hashtable_checkup : structure regroupant quelques informations qui
//    constituent un bilan de santé d'une table de hachage.
struct hashtable_checkup {
  size_t nslots;      //  nombre de compartiments
  size_t nentries;    //  nombre de clés
  double ldfactmax;   //  taux de remplissage maximum toléré
  double ldfactcurr;  //  taux de remplissage courant
  size_t maxlen;      //  maximum des longueurs des listes
  double postheo;     //  nombre moyen théorique de comparaisons dans le cas
                      //    d'une recherche positive
  double poscurr;     //  nombre moyen courant de comparaisons dans le cas d'une
                      //    recherche positive
};

//  hashtable_get_checkup : effectue un bilan de santé pour la table de hachage
//    associée à ht et affecte le résultat à *htcuptr.
extern void hashtable_get_checkup(hashtable *ht,
    struct hashtable_checkup *htcuptr);

//  hashtable_display_checkup : effectue un bilan de santé pour la table de
//    hachage associée à ht et écrit le résultat dans le flot texte contrôlé par
//    l'objet pointé par textstream. Renvoie une valeur non nulle si une erreur
//    en écriture survient. Renvoie sinon zéro.
extern int hashtable_display_checkup(hashtable *ht, FILE *textstream);

#endif

#endif
