#include "compregrex.h"
#include "afn.h"

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
 *       |  Reste_F REP
 *       |  Reste_F
 *
 * Rep   -> {n} REP 
 *       | epsilon 
 *
 * Reste_F -> CHAR
 *         | ( Expr )
 *         | [char *] //chaîne de caractères
*/
uint extract_size_rpn(unilex_t * arr_ul, uint size) {
    uint res = 0;
    for (uint i = 0; i < size; i++) {
        if (arr_ul[i].type != AO && arr_ul[i].type != AF
          && arr_ul[i].type != PO && arr_ul[i].type != PF)
            res++;
    }
    return res;
}

uint i = 0;
uint top = 0;
unilex_t * l;
unilex_t * rpn;
uint n;
unilex_t * parser(unilex_t * arr_ul, uint size, uint * size_rpn) {
    l = arr_ul;
    n = size;
    *size_rpn = extract_size_rpn(arr_ul, size);
    rpn = calloc(*size_rpn, sizeof(unilex_t));

    assert(rpn != NULL);
    if(expr() && i == size) {
        return rpn;
    } else {
        printf("[parser] ERREUR SYNTAXIQUE au caractère %c\n", l[i].val);
        exit(-1);
    }
    return rpn;
}

bool expr() {
    return (terme() && reste_e());
}

bool reste_e() {
    uint old_i = i;
    if (i < n && l[i].val == '+') {
        i++;
        if (terme() && reste_e()) {
            rpn[top++] = l[old_i];
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
    uint old_i = i;
    if (i < n && l[i].val == '.') {
        i++;
        if (fact() && reste_t()) {
            rpn[top++] = l[old_i];
            return true;
        }
        return false;
    }
    return true;
}

bool rep () {
    if (i < n && l[i].type == AO) {
        i++;
        if ((i+1) < n && l[i].type == OP && l[++i].type == AF) {
            rpn[top].val = l[i-1].val;
            rpn[top++].type = OP;
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
            rpn[top++] = l[i];
            i++;
        } else if (rep()) {
        }
        return true;
    }
    return false;
}

bool reste_f() {
    if ( i < n && l[i].type == CHAR) {
        rpn[top++] = l[i++];
        return true;
    } else if (i < n && l[i].type == PO) {
        i++;
        if (expr() && i < n && l[i].type == PF) {
            i++;
            return true;
        }
    } else if (i < n && l[i].type == CO) {
        rpn[top++] = l[i++];
        while (i < n && l[i].type == CHAR) {
            rpn[top++] = l[i];
            i++;
        }
        if (i == n || l[i].type != CF) {
            return false;
        }
        rpn[top++] = l[i];
        i++;
        return true;
    }
    return false;
}

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
    printf("%s\n", *res);
    return size;
}

afd codegen(unilex_t * rpn, uint size) {
    stack_t stack = NULL;
    afn curr_af;
    afn aux_af;
    afn aux_af2;
    char * temp = NULL;
    uint nb_rep;
    uint i = 0;
    print_arr_ul(rpn, size);
    while (i < size) {
        printf("rpn curr char = %c\n", rpn[i].val);
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
        print_stack(stack);
    }
    afd res;
    afn final = pop(&stack);
    afn_determinisation(final, &res);
    afd_print(res);
    afn_free(&final);
    free(rpn);
    return res;
}
