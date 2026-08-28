#include <cstdio>
#include <joy/jmath.h>
int main(){
    fp_t x = fp_div(fp_from_int(3), fp_from_int(5));
    printf("%d\n", (int)x);
    return 0;
}
