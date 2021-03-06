#include "afd.h"
#include "scanner.h"
#include "parser.h"
#include "regcomp.h"
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("ERREUR ARGUMENTS MANQUANTS : Saisissez l'expression regulière et la chaîne à evaluer\n");
        return -1;
    }
    char * regex = argv[1];
    char * str = argv[2];

    unilex_t * arr_ul;
    if ((arr_ul = scanner(regex)) == NULL)
        return -1;

    uint size_rpn;
    unilex_t * rpn_ul;
    if ((rpn_ul = parser(arr_ul, strlen(regex), &size_rpn)) == NULL)
        return -1;

    free(arr_ul);

    afd res = regcomp(rpn_ul, size_rpn);
    if (afd_simul(str, res)) {
        printf("acceptée\n");
    } else {
        printf("rejetée\n");
    }
    free(rpn_ul);
    afd_free(&res);
    return 0;
}
