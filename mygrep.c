#include "afd.h"
#include "compregrex.h"
#include <assert.h>
int main(int argc, char* argv[]) {
    assert(argc == 3);
    char * regex = argv[1];
    char * str = argv[2];
    unilex_t * arr_ul = scanner(regex);
    print_arr_ul(arr_ul, strlen(regex));
    uint size_rpn;
    unilex_t * rpn_ul = parser(arr_ul, strlen(regex), &size_rpn);
    print_arr_ul(rpn_ul, size_rpn);
    free(arr_ul);
    afd res = codegen(rpn_ul, size_rpn);
    printf("--->%d\n",afd_simul(str, res));
    afd_free(&res);
}
