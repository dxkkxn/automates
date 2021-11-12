#include "compregrex.h"
#include "afn.h"

bool is_char(char c) {
    uint minus = ('a' <= c && c <= 'z');
    uint mayus = ('A' <= c && c <= 'Z');
    return(minus || mayus || c == '&');
}

/* 
 * Retourne vrai si le caractère c est un entier entre 1<=c<=9 
*/
bool is_int(char c) {
    // ascii 1 -> 49
    // ascii 9 -> 57
    return (49<=c && c <=57);
}

void print_arr_ul(unilex_t * arr, uint size) {
    printf("[");
    uint i;
    for (i = 0; i < (size-1); i++) {
        printf("(%d,%c),", arr[i].type, arr[i].val);
    }
    printf("(%d,%c)]\n", arr[i].type, arr[i].val);
}

unilex_t * scanner(char * str) {
    unilex_t * arr_ul = calloc(strlen(str), sizeof(unilex_t));
    assert(arr_ul != NULL);
    uint i = 0;
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
                if (is_int(*str)) {
                    arr_ul[i].type = NB;
                    arr_ul[i].val = *str;
                } else if (is_char(*str)) {
                    arr_ul[i].type = CHAR;
                    arr_ul[i].val =  *str;
                } else {
                    fprintf(stderr, "[scanner] ERREUR LEXICALE au caractère %c\n", *str);
                    exit(-1);
                }
                break;
        }
        str++;
        i++;
    }
    return arr_ul;
}

/* Expr -> Terme Reste_e
 *
 * Reste_e -> + Terme Reste_e
 *         | epsilon
 * Terme -> Fact Reste_t
 *
 * Reste_t -> . Fact Reste_t
 *         | epsilon
 *
 * Fact  -> Reste_F*
 *       |  Reste_F
 *       |  Reste_F REP
 *
 * Rep   -> {n} REP 
 *       | epsilon 
 *
 * Reste_F -> CHAR
 *         | ( Expr )
 *         | [char *] //chaîne de caractères
*/

uint i = 0;
uint top = 0;
char * rpn;
unilex_t * l;
uint n;
char * parser(unilex_t * arr_ul, uint size) {
    l = arr_ul;
    n = size;
    rpn = calloc(size+1, sizeof(char));
    assert(rpn != NULL);
    if(expr() && i == size) {
        return rpn;
    } else {
        printf("[parser] ERREUR SYNTAXIQUE au caractère %c\n", l[i].val);
        exit(-1);
    }
    rpn = NULL;
    return rpn;
}

bool expr() {
    return (terme() && reste_e());
}

bool reste_e() {
    if (i < n && l[i].val == '+') {
        i++;
        if (terme() && reste_e()) {
            rpn[top++] = '+';
            return true;
        }
        return false;
    }
    return true;
}

bool terme() {
    return (fact() && reste_t());
}

bool reste_t() {
    if (i < n && l[i].val == '.') {
        i++;
        if (fact() && reste_t()) {
            rpn[top++] = '.';
            return true;
        }
        return false;
    }
    return true;
}

bool rep () {
    if (i < n && l[i].type == AO) {
        i++;
        if ((i < n && l[i].type == NB) && ((i+1) < n && l[++i].type == AF)) {
            rpn[top++] = l[i-1].val;
            i++;
            rep();
        } else {
          return false;
        }
    }
    return true;
        }
bool fact() {
    if (reste_f()) {
        if (i < n && l[i].val == '*') {
            rpn[top++] = '*';
            i++;
            return true;
        } else if (rep()) {
                return false;
        }
        return true;
    }
    return false;
}

bool reste_f() {
    if ( i < n && l[i].type == CHAR) {
        rpn[top++] = l[i].val;
        i++;
        return true;
    } else if (i < n && l[i].type == PO) {
        i++;
        if (expr() && i < n && l[i].type == PF) {
            i++;
            return true;
        }
    } else if (i < n && l[i].type == CO) {
        rpn[top++] = l[i++].val;
        while (i < n && l[i].type == CHAR) {
            rpn[top++] = l[i].val;
            i++;
        }
        if (i == n || l[i].type != CF) {
            return false;
        }
        rpn[top++] = l[i].val;
        i++;
        return true;
    }
    return false;
}

/* Prend un str commençant par un [ et remplis res avec le str 
   entre ce premier [ et le prochain ] et retourne la taille de res

   (Take a str starting with a [ and fills res with the str between that
   first [ and the next ] and returns the size of res) */

uint extract_str_in_brackets(char** res, char* str) {
    assert(*str== '[');
    str++;
    char * copy = str;
    // calcule la taille du str entre crochets (calculate the size of the str in brackets)
    uint size = 0;
    while(*copy != ']') {
        copy++;
        size++;
    }
    // copie du str
    (*res) = calloc(size+1, sizeof(char));
    uint i = 0;
    while(*str != ']') {
        *(*res + i) = *str;
        i++;
        str++;
    }
    return size;
}

afd codegen(char * rpn_n) {
    printf("here>>>>rpn = %s\n", rpn);
    stack_t stack = NULL;
    afn curr_af;
    afn aux_af;
    afn aux_af2;
    char * temp = NULL;
    uint nb_rep;
    while (*rpn != '\0') {
        printf("*rpn = %c\n", *rpn);
        if (is_char(*rpn)) {
            afn_char(&curr_af, *rpn);
            push(&stack, curr_af);
        } else if (*rpn == '*') {
            aux_af = pop(&stack);
            afn_kleene(&curr_af, aux_af);
            afn_free(&aux_af);
            push(&stack, curr_af);
        } else if (*rpn == '.'){
            aux_af = pop(&stack);
            aux_af2 = pop(&stack);
            afn_concat(&curr_af, aux_af2, aux_af);
            afn_free(&aux_af);
            afn_free(&aux_af2);
            push(&stack, curr_af);
        } else if (*rpn == '[') {
           rpn += extract_str_in_brackets(&temp, rpn);
           rpn++; // Pour être au caractère ] (to be at ] char)
           afn_chaine(&curr_af, temp);
           push(&stack, curr_af);
           free(temp);
        } else if (is_int(*rpn)) {
            nb_rep = *rpn - 48 - 1;
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
            assert(*rpn == '+');
            aux_af = pop(&stack);
            aux_af2 = pop(&stack);
            afn_union(&curr_af, aux_af2, aux_af);
            afn_free(&aux_af);
            afn_free(&aux_af2);
            push(&stack, curr_af);
        }
        rpn++;
        print_stack(stack);
    }
    afd res;
    afn final = pop(&stack);
    afn_determinisation(final, &res);
    afd_print(res);
    afn_free(&final);
    free(rpn_n);
    return res;
}
