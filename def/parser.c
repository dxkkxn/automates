#include "parser.h"

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
    bool val_expr;
    if( (val_expr = expr()) && i == size) {
        return rpn;
    } else {
        if (i >= size) {
            printf("[parser] ERREUR SYNTAXIQUE au caractère %c\n", l[size-1].val);
        } else {
            printf("[parser] ERREUR SYNTAXIQUE au caractère %c\n", l[i].val);
        }
        return NULL;
    }
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

/*
** HELPER FUNCTIONS
 */

/*
** Renvoie la future taille du tableau en "notation
** polonaise inversé"
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
