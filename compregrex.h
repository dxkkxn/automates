#ifndef COMPREGREX_H
#define COMPREGREX_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "stack.h"
#include "afn.h"

typedef enum{PO, PF, OP, CHAR, CO, CF, AO, AF} type_t;
typedef struct  {
    type_t type;
    char val;
} unilex_t;
typedef unsigned int uint;

void print_arr_ul(unilex_t * arr, uint size);
unilex_t * scanner(char * str);
unilex_t * parser(unilex_t * arr_ul, uint size, uint * size_rpn);
bool expr();
bool reste_e();
bool terme();
bool reste_t();
bool fact();
bool reste_f();
afd codegen();

#endif // COMPREGREX_H
