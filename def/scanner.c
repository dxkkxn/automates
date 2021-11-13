#include "scanner.h"
unilex_t * scanner(char * str) {
    unilex_t * arr_ul = calloc(strlen(str), sizeof(unilex_t));
    assert(arr_ul != NULL);
    int i = 0;
    while(*str != '\0') {
        switch (*str) {
            case  '(':
                arr_ul[i].type = PO;
                arr_ul[i].val =  *str;
                break;
            case  ')':
                arr_ul[i].type = PF;
                arr_ul[i].val =  *str;
                break;
            case  '.': case '*': case '+':
                arr_ul[i].type = OP;
                arr_ul[i].val =  *str;
                break;
            case '[':
                arr_ul[i].type = CO;
                arr_ul[i].val = '[';
                break;
            case ']':
                arr_ul[i].type = CF;
                arr_ul[i].val = ']';
                break;
            case '{':
                arr_ul[i].type = AO;
                arr_ul[i].val = '{';
                break;
            case '}':
                arr_ul[i].type = AF;
                arr_ul[i].val = '}';
                break;
            default:
                if (0 <= (i-1) && arr_ul[i-1].type == AO && is_int_accept(*str)) {
                    arr_ul[i].type = OP;
                    arr_ul[i].val = *str;
                } else if (is_char(*str)) {
                    arr_ul[i].type = CHAR;
                    arr_ul[i].val =  *str;
                } else {
                    fprintf(stderr, "[scanner] ERREUR LEXICALE au caractère %c\n", *str);
                    return NULL;
                }
        }
        str++;
        i++;
    }
    return arr_ul;
}
/*
** Affiche le tableau d'unites lexicales
 */
void print_arr_ul(unilex_t * arr, uint size) {
    char * types[8];
    types[0] = "PO";
    types[1] = "PF";
    types[2] = "OP";
    types[3] = "CHAR";
    types[4] = "CO";
    types[5] = "CF";
    types[6] = "AO";
    types[7] = "AF";
    printf("[");
    uint i;
    for (i = 0; i < (size-1); i++) {
        printf("(%s,%c),", types[arr[i].type], arr[i].val);
    }
    printf("(%s,%c)]\n", types[arr[i].type], arr[i].val);
}

/*
** HELPER FUNCTIONS
*/

/*
** Renvoi vrai si le caractere c est un caractere valide
** selon la convention adopte' en cours
 */
bool is_char(char c) {
    uint num   = ('0' <=c && c <='9');
    uint minus = ('a' <= c && c <= 'z');
    uint mayus = ('A' <= c && c <= 'Z');
    return( num || minus || mayus || c == '&');
}

/*
 * Retourne vrai si le caractère c est un entier entre 1<=c<=9
*/
bool is_int_accept(char c) {
    return ('1'<=c && c <='9');
}
