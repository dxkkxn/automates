#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include <stdlib.h>

unilex_t * parser(unilex_t * arr_ul, uint size, uint * size_rpn) ;
bool expr();
bool reste_e();
bool terme();
bool reste_t();
bool fact();
bool reste_f();

// HELPER FUNCTIONS
uint extract_size_rpn(unilex_t * arr_ul, uint size);
#endif // PARSER_H
