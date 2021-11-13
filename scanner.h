#ifndef SCANNER_H
#define SCANNER_H

/*
 * PO - Parenthese Ouvrant
 * PF - Parenthese Fermant
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

void print_arr_ul(unilex_t * arr, uint size);
unilex_t * scanner(char * str);

#endif //  SCANNER_H
