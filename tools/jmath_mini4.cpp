#include <joy/jmath.h>
static const int  PF_GRID_N      = 20;
static const int  PF_GRID_MIN    = -10;
static const int  PF_GRID_OFFSET = -PF_GRID_MIN;
static const int  PF_CELLS       = PF_GRID_N * PF_GRID_N;
static const fp_t PF_PAD = fp_div(fp_from_int(3), fp_from_int(5));
static const fp_t PF_HALF = fp_div(fp_from_int(1), fp_from_int(2));
int main(){
    printf("PF_GRID_N=%d PF_CELLS=%d\n", PF_GRID_N, PF_CELLS);
    printf("PAD=%lld HALF=%lld\n", (long long)PF_PAD, (long long)PF_HALF);
    return 0;
}
