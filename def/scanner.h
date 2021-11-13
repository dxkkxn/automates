#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

/*
 * PO - Parenthese Ouvrant
 * PF - Parenthese Fermant
 * OP - Operateur
 * CHAR - Character
 * CO - Crochet Ouvrant
 * CF - Crochet Fermant
 * AO - Accolade Ouvrant
 * AF - Accolade Fermant
*/
typedef enum{PO, PF, OP, CHAR, CO, CF, AO, AF} type_t;

typedef struct  {
    type_t type;
    char val;
} unilex_t;

typedef unsigned int uint;
typedef enum{false = 0, true} bool;

void print_arr_ul(unilex_t * arr, uint size);
unilex_t * scanner(char * str);

// HELPER FUNCTIONS
bool is_char(char c);
bool is_int_accept(char c);
#endif //  SCANNER_H
