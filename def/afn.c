#include "afn.h"
typedef enum{false = 0, true} bool;

char * ALPHABET = "&abcdefghijklmnopqrstuvwxyzABCDEFGHIJLMNOPQRSTVWXYZ0123456789";

void afn_init(afn *A, uint nbetat, char * alphabet, ullong init, ullong finals) {
    A->nbetat = nbetat;

    A->alphabet = malloc(strlen(alphabet)+1);
    strcpy(A->alphabet, alphabet);
    A->nbsymb = strlen(alphabet);

    A->init = init;
    A->finals = finals;

    int i;
    uchar symb;

    for (i=0; i<SYMB_NB_MAX; i++)
        A->tsymb[i] = SYMB_NONE;

    for (i=0; i<A->nbsymb; i++){
        if ( (alphabet[i] < SYMB_ASCII_DEB) ||
             (alphabet[i] > SYMB_ASCII_FIN) ){
            fprintf(stderr,"[afd_init] caractère ascii numéro %d invalide\n", alphabet[i]);
            exit(-1);
        }
        symb = (uchar) (alphabet[i] - SYMB_ASCII_DEB);
        if ( A->tsymb[symb] != SYMB_NONE ){
            fprintf(stderr,"[afd_init] caractère <%c> déjà défini (ignorer)\n",alphabet[i]);
        }
        else {
            A->tsymb[symb] = i;
        }
    }
    A->delta = calloc(nbetat, sizeof(ullong *));
    for (i=0; i<nbetat; i++){
        A->delta[i] = calloc(A->nbsymb, sizeof(ullong));
    }
}

/*
  Ajoute la relation (<q1>, <s>, <q2>) à l'AFN <A>
*/
void afn_add_trans(afn *A, uint q1, uint s, uint q2) {
    uchar symb = A->tsymb[s-SYMB_ASCII_DEB];

    if (symb == SYMB_NONE){
        fprintf(stderr, "[add_trans] %u -- %c --> %u symbole inconnu\n", q1,s,q2);
        exit(-1);
    }

    if ( (q1<0) || (q1>=A->nbetat) ){
        fprintf(stderr, "[add_trans] état <%d> non reconnu\n", q1);
        exit(-1);
    }
    if ( (q2<0) || (q2>=A->nbetat) ){
        fprintf(stderr, "[add_trans] état <%d> non reconnu\n", q2);
        exit(-1);
    }

    A->delta[q1][symb] |= INT_ETAT(q2);
}


/*
  Libère la mémoire de l'AFN <A>
*/
void afn_free(afn *A) {
    free(A->alphabet);

    int i;
    for (i=0; i < A->nbetat; i++)
        free(A->delta[i]);
    free(A->delta);
}


/*
  Affiche l'AFN <A>
*/
void afn_print(afn A) {
    uint q,q1,s;
    printf("init:");
    for (q=0; q<A.nbetat; q++)
    {
        if IN(q,A.init)
                 printf(" %d",q);
    }
    printf("\n");
    printf("finals:");
    for (q=0; q<A.nbetat; q++)
    {
        if IN(q,A.finals)
                 printf(" %d",q);
    }
    printf("\n");
    for (q=0; q<A.nbetat; q++){
        for (s=0; s<A.nbsymb; s++){
            if (A.delta[q][s]!=0){
                printf("%d -- %c --> {", q, A.alphabet[s]);
                for (q1=0; q1<A.nbetat; q1++)
                {
                    if (IN(q1, A.delta[q][s]))
                        printf("%d,",q1);
                }
                printf("\b}\n");
            }
        }
    }
}

/*
 * Copie en profondeur de afn src dans afd dest
*/
void afn_copy(afn *dest, afn *src) {
  afn_init(dest, src->nbetat, src->alphabet, src->init, src->finals);
  char* alphabet;
  for (int i = 0; i < src->nbetat; i++) {
      for (int j = 0; j < src->nbetat; j++) {
          alphabet = src->alphabet;
          while (*alphabet != '\0') {
              if (exist_trans(*src, i, *alphabet, j))
                  afn_add_trans(dest, i, *alphabet, j);
              alphabet++;
              }
          }
    }
  }

/*
  Initialise l'AFN <A> à partir des données du fichier <nomfichier> au
  format:

  nb_etats alphabet nbr_etats_initiaux nbr_etats_finals
  etat_initial....
  etats_finals....
  etat_i_11 symb_j_1 etat_i_12
  etat_i_21 symb_j_2 etat_i_22
  .
  .
  .
  etat_i_n1 symb_j_n etat_i_n2
*/
void afn_finit(afn *A, char *nomfichier) {
    FILE *f;
    uint nb_states;
    char alphabet[128];
    if ((f = fopen(nomfichier, "r")) == NULL) {
        perror("fopen");
        exit(1);
    }

    uint nb_st_init;
    uint nb_st_fin;
    int r;
    r = fscanf(f, "%u %s %u %u", &nb_states, alphabet, &nb_st_init, &nb_st_fin);
    assert(r == 4);
    
    uint i;
    i = 0;
    ullong st_init = 0;
    ullong curr_st;
    while(i < nb_st_init) {
        fscanf(f, "%llu", &curr_st);
        st_init |= INT_ETAT(curr_st);
        i++;
    }
    

    i = 0;
    ullong st_fin = 0;
    while(i < nb_st_fin) {
        fscanf(f, "%llu", &curr_st);
        st_fin |= INT_ETAT(curr_st);
        i++;
    }
    
    afn_init(A, nb_states, ALPHABET, st_init, st_fin);

    uint st_i; //état_i
    uint st_j; //état_j
    char symb;
    while(fscanf(f, "%d %c %d\n", &st_i, &symb, &st_j) == 3) {
        afn_add_trans(A, st_i, symb, st_j);
    }
}

/*
  Retourne l'epsilon fermeture de l'ensemble d'états <R> par
  l'automate <A>
*/
ullong afn_epsilon_fermeture(afn A, ullong R) {
    ullong stack[A.nbetat];
    int top = -1;
    ullong state = 0;
    while (state < A.nbetat) {
        if (IN(state, R))
            stack[++top] = state;
        state++;
    }
    ullong ferm = R;
    ullong q;
    ullong q2;
    while (top >= 0) {
        q = stack[top--];
        q2 = 0;
        while (q2 < A.nbetat) {
            if(((ferm & INT_ETAT(q2)) == 0) && exist_trans(A, q, '&', q2)) {
                ferm |= INT_ETAT(q2);
                stack[++top] = q2;
            }
            q2++;
        }
    }
    return ferm;
}
  


/*
  Calcule un automate déterministe équivalent à <A> et affecte le
  résultat à <D>.
*/
void afn_determinisation(afn A, afd *D) {
    uint size_states_q  = 64;
    ullong * states_q = calloc(size_states_q, sizeof(ullong));
    uint size_of_trans = 64;
    trans_t * trans = calloc(size_of_trans, sizeof(trans_t));
    assert(trans != NULL);
    assert(states_q != NULL);
    states_q[0] = afn_epsilon_fermeture(A, A.init);
    // Au plus, tous les états sont finaux donc la taille de finals est 64
    uint * finals = calloc(64, sizeof(uint));
    uint nb_finals = 0;

    uint nb_etats = 1;
    uint i = 0;
    ullong ferm;
    uint trans_i = 0;
    ullong temp;
    while(i < nb_etats) {
        if((states_q[i] & A.finals) && !(in2(finals, nb_finals, i)))
            finals[nb_finals++] = i;
        char * sigma = A.alphabet;
        while(*sigma != '\0') {
            if (*sigma != '&') {
                temp = simul(A, states_q[i], *sigma);
                if (temp) {
                    ferm = afn_epsilon_fermeture(A, temp);
                    if(!in(states_q, nb_etats, ferm)) {
                        // verif taille
                        if(nb_etats >= size_states_q) {
                            size_states_q += 64;
                            states_q = realloc(states_q, size_states_q * sizeof(ullong));
                            assert(states_q != NULL);
                        }
                        states_q[nb_etats] = ferm;
                        nb_etats++;
                    }
                    // On garde en memoire les transitions pour les ajouter ulterieurement
                    trans_t new_trans = {i,*sigma, indexof(states_q, nb_etats, ferm)};
                    // Verif de la taille
                    if (trans_i >= size_of_trans) {
                        size_of_trans += 64;
                        trans = realloc(trans, size_of_trans * sizeof(trans_t));
                        assert(trans != NULL);
                    }
                    trans[trans_i] = new_trans;
                    trans_i++;
                }
            }
            sigma++;
        }
        i++;
    }
    afd_init(D, nb_etats, A.alphabet, nb_finals, 0, finals);
    for(i = 0; i< trans_i; i++) {
        afd_add_trans(D, trans[i].q, trans[i].c, trans[i].q2);
    }
    free(states_q);
    free(trans);
    free(finals);
}

/*
  Calcule l'automate qui reconnaît le caractère <c> dans un alphabet a
  <nbsymb> symboles
*/
void afn_char(afn *C, char c) {
    afn_init(C, 2, ALPHABET, 1, 2);
    afn_add_trans(C, 0, c, 1);
}


/*
  Calcule un automate qui reconnaît l'union de <A> et <B>
*/
void afn_union(afn *C, afn A, afn B) {
    uint nb_states = A.nbetat + B.nbetat;
    ullong fin_states = (A.finals<<1) | (B.finals<<(A.nbetat+1));

    afn_init(C, nb_states + 1, ALPHABET, 1, fin_states);

    // Ajout de toutes les transitions de A dans C
    add_all_trans(C, A, 1);

    // Ajout de toutes les transitions de B dans A commencant a A.nbetat+1
    add_all_trans(C, B, A.nbetat+1);

    // Ajout des epsilon transition depuis 0 (C->init) aux  etats initiaux de A
    add_ep_trans(C, A, 1);

    // Ajout des epsilon transition depuis 0 (C->init) aux  etats initiaux de B
    add_ep_trans(C, B, A.nbetat+1);
}

/*
  Calcule un automate qui reconnat la concaténation de <A> et <B>
*/
void afn_concat(afn *C, afn A, afn B) {
    afn_init(C,A.nbetat+B.nbetat, ALPHABET,A.init, B.finals<<(A.nbetat));
    add_all_trans(C, A, 0);
    add_all_trans(C, B, A.nbetat);
    // Pour chaque état final de A, ajoute un epsilon transition vers chaque état initial de B
    ullong curr_stateb = 1; //binaire
    ullong curr_state = 0; // decimal
    ullong curr_statejb = 1; //binaire
    ullong curr_statej = 0; // decimal
    while (A.finals >= curr_state) {
        if (A.finals & curr_stateb) {
            // curr_state est un état final
            curr_statejb = 1;
            curr_statej = 0;
            while(B.init >= curr_statejb) {
                if (curr_statejb & B.init)
                    // curr_statejb est un état initial de B
                    afn_add_trans(C,curr_state, '&', curr_statej+A.nbetat);
                curr_statejb <<= 1;
                curr_statej++;
            }
        }
        curr_stateb <<= 1;
        curr_state++;
    }
}

/*
  Calcule un automate qui reconnaît la fermeture de Kleene de <A>
*/
void afn_kleene(afn *C, afn A) {
    afn_init(C, A.nbetat+1, A.alphabet, 1, 1);
    add_all_trans(C, A, 1);
    ullong finals = A.finals;
    ullong initials = A.init;
    ullong temp;
    uint state_f;
    uint state_i;
    // Ajout des epsilon transitions depuis chaque etat final vers
    // notre nouveau etat initial 0
    while(finals) {
       temp = finals;
       finals &= finals - 1;
       state_f = log(temp^finals)/log(2);
       afn_add_trans(C, state_f+1, '&', 0);
    }
    // Ajout des epsilon transitions depuis  notre nouveau etat initial 0
    // vers chaque etat initial
    while(initials) {
       temp = initials;
       initials &= initials - 1;
       state_i = log(temp^initials)/log(2);
       afn_add_trans(C, 0, '&', state_i+1);
    }
}

/*
** Prends une chaine de caractères (abcdef..) et construit l'afn
** qui reconnait a ou b ou c ou d ou f ....
*/
void afn_chaine(afn *C, char * str) {
    afn_init(C, 2, ALPHABET, 1, 2);
    while (*str != 0) {
        afn_add_trans(C, 0, *str, 1);
        str++;
    }
}

/*
** HELPER FUNCTIONS
*/

/*
** Renvoie vrai si q est dans arr
*/
int in(ullong* arr, uint size, ullong q) {
    bool is_in = false;
    for(uint i = 0; i < size && is_in == false; i++) {
        is_in = (arr[i] == q);
    }
    return is_in;
}

int in2(uint* arr, uint size, uint q) {
    bool is_in = false;
    for(uint i = 0; i < size && is_in == false; i++) {
        is_in = (arr[i] == q);
    }
    return is_in;
}

/*
** Ajoute dans C les epsilon transitions de 0 à tous les états initiaux de A . (Add & trans in C from 0 to all initial states of A)
** On renomme 0 in A en start (renaming 0 in A to start ...)
*/
void add_ep_trans(afn *C, afn A, uint start){
    ullong curr_st = 1;
    ullong st;
    while(A.init >= curr_st) {
        if (A.init & 1) {
            st = log(curr_st)/log(2);
            afn_add_trans(C, 0, '&', st+start);
        }
        curr_st <<= 1;
    }
}

/*
  Ajoute toutes les transitions de A dans C en commençant à start dans C
  i.e
  l'état 0 en A est l'état start en B
  l'état 1 en A est l'état start+1 en B
*/
void add_all_trans(afn *C, afn A, uint start) {
    for (uint i = 0; A.nbetat > i; i++){
        for (uint j = 0; j < A.nbetat ; j++) {
            for (uint k = 0; k < strlen(ALPHABET); k++) {
                if (exist_trans(A, i, ALPHABET[k], j)) {
                    afn_add_trans(C, i+start, ALPHABET[k], j+start);
                }
            }
        }
    }
}

/* Prend un ensemble d'états et un caractère en paramètre et
   revoie tous les états accessibles à partir de cet unique caractère */
ullong simul(afn A, ullong states, char c) {
    ullong copy;
    ullong res = 0;
    ullong curr_state;
    while(states) {
        copy = states;
        states &= states -1;
        curr_state = log(states^copy)/log(2);

        for(uint i = 0; i < A.nbetat; i++) {
            if(exist_trans(A, curr_state, c, i))
                res |= INT_ETAT(i);
        }
    }
    return res;
}

/* Revoie l'index de target en arr */
uint indexof(ullong *arr, uint size, ullong target) {
    bool found = false;
    uint i;
    for(i = 0; i < size && found == false; i++) {
        if(arr[i] == target)
            found = true;
    }
    assert(arr[i-1]==target);
    return i-1;
}

/*
  Retourne 1 si le triplet (p,c,q) existe
  0 sinon
*/
ullong exist_trans(afn A, ullong q, char c, ullong p) {
    return ((A.delta[q][A.tsymb[c-SYMB_ASCII_DEB]]) & INT_ETAT(p));
}

/*
** Affiche le tableau array
*/
void print_array(ullong* array, int n) {
    printf("[");
    for (int i = 0; i < n - 1; i++)
        printf("%lld,", array[i]);
    printf("%lld]\n", array[n - 1]);
}
