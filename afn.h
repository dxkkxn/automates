#ifndef AFN_H
#define AFN_H

#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <assert.h>

#include "afd.h"
#include "math.h"



#define INT_ETAT(q) (1ULL<< (q))
#define IN(q,X) ((INT_ETAT(q) & X ) > 0)

typedef unsigned int uint;
typedef unsigned long long int ullong;

typedef struct {
  ullong q;
  char c;
  ullong q2;
}trans_t;

typedef struct{
  uint nbetat, nbsymb;
  char * alphabet;
  uchar tsymb[SYMB_NB_MAX];
  ullong init, finals;
  ullong **delta;
} afn;

void afn_init(afn *A, uint nbetat, char * alphabet, ullong init, ullong finals);
void afn_add_trans(afn *A, uint q1, uint s, uint q2);
void afn_free(afn *A);

void afn_print(afn A);
void afn_finit(afn *A, char *f);

ullong afn_epsilon_fermeture(afn A, ullong R);
void afn_determinisation(afn A, afd *D);

void afn_char(afn *C, char c);
void afn_union(afn *C, afn A, afn B);
void afn_concat(afn *C, afn A, afn B);
void afn_kleene(afn *C, afn A);

void afn_chaine(afn *C, char * str);
void afn_copy(afn *dest, afn* src);

/*
 * HELPER FUNCTONS
*/
int in(ullong* arr, uint size, ullong q);
int in2(uint* arr, uint size, uint q);
void add_ep_trans(afn *C, afn A, uint start);
void add_all_trans(afn *C, afn A, uint start);
ullong simul(afn A, ullong states, char c);
uint indexof(ullong *arr, uint size, ullong target);
ullong exist_trans(afn A, ullong q, char c, ullong p);
void print_array(ullong* array, int n);

#endif
