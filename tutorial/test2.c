#include <stdio.h>
#include <stdlib.h>

int main() {
    void *hi = malloc(124);
    printf("hi\n");
    free(hi);
    return 0;
}
