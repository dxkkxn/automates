#ifndef REGCOMP_H
#define REGCOMP_H

#include <assert.h>

#include "scanner.h"
#include "afd.h"
#include "afn.h"
#include "stack.h"

#include "scanner.h"

afd regcomp(unilex_t * rpn, uint size);

/*
** HELPER FUNCTIONS
*/
uint extract_str_in_brackets(char** res, unilex_t* arr_ul, uint start);
#endif // REGCOMP_H
