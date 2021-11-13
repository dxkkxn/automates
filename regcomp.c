#include "regcomp.h"


afd codegen(unilex_t * rpn, uint size) {
    stack_t stack = NULL;
    afn curr_af;
    afn aux_af;
    afn aux_af2;
    char * temp = NULL;
    uint nb_rep;
    uint i = 0;
    while (i < size) {
        if (rpn[i].type == CHAR) {
            afn_char(&curr_af, rpn[i].val);
            push(&stack, curr_af);
        } else if (rpn[i].val == '*') {
            aux_af = pop(&stack);
            afn_kleene(&curr_af, aux_af);
            afn_free(&aux_af);
            push(&stack, curr_af);
        } else if (rpn[i].val == '.'){
            aux_af = pop(&stack);
            aux_af2 = pop(&stack);
            afn_concat(&curr_af, aux_af2, aux_af);
            afn_free(&aux_af);
            afn_free(&aux_af2);
            push(&stack, curr_af);
        } else if (rpn[i].val == '[') {
           i += extract_str_in_brackets(&temp, rpn, i);
           i++; // Pour être au caractère ] (to be at ] char)
           afn_chaine(&curr_af, temp);
           push(&stack, curr_af);
           free(temp);
        } else if (rpn[i].type == OP && is_int_accept(rpn[i].val)) {
            nb_rep = rpn[i].val - 48 - 1;
            aux_af = pop(&stack);
            afn_copy(&curr_af, &aux_af);
            while (nb_rep) {
                afn_concat(&aux_af2, curr_af, aux_af);
                afn_free(&curr_af);
                curr_af = aux_af2;
                nb_rep--;
            }
            push(&stack, curr_af);
            afn_free(&aux_af);
        } else {
            assert(rpn[i].val == '+');
            aux_af = pop(&stack);
            aux_af2 = pop(&stack);
            afn_union(&curr_af, aux_af2, aux_af);
            afn_free(&aux_af);
            afn_free(&aux_af2);
            push(&stack, curr_af);
        }
        i++;
    }
    afd res;
    afn final = pop(&stack);
    afn_determinisation(final, &res);
    afn_free(&final);
    free(rpn);
    return res;
}

/*
** HELPER FUNCTIONS
 */

/* Prend un str commençant par un [ et remplis res avec le str
   entre ce premier [ et le prochain ] et retourne la taille de res

   (Take a str starting with a [ and fills res with the str between that
   first [ and the next ] and returns the size of res) */

uint extract_str_in_brackets(char** res, unilex_t* arr_ul, uint start) {
    assert(arr_ul[start].val == '[');
    // calcule la taille du str entre crochets (calculate the size of str in brackets)
    uint size = 0;
    uint i = start+1;
    while(arr_ul[i].val != ']') {
        i++;
        size++;
    }
    // copie du str
    (*res) = calloc(size+1, sizeof(char));
    i = start + 1;
    uint j = 0;
    while(arr_ul[i].val != ']') {
        *(*res + j) = arr_ul[i].val;
        i++;
        j++;
    }
    return size;
}
