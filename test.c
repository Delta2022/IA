#include "main.h"
// TODO make print functions

int main(/*@unused@*/ int argc, /*@unused@*/ char *argv[])
{
    int foutput = 0;

    struct campaign my_campaign;
    foutput = init_campaign(&my_campaign);

    debug_campaign(&my_campaign, stdout);

    if (foutput != 0) {
        log_err("function run fail");
        exit(EXIT_FAILURE);
    }
    return 0;
}
