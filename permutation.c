#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long factorialCache[1000] = {0};
long factorial(int n) {
  if (n == 0) return 1;
  if (n < 1000 && factorialCache[n] != 0) return factorialCache[n];
  return n * factorial(n - 1);
}

void permutationSub(int** resTable, int* resTableCounter, int* perm,
                    char* unused, int total, int N) {
  if (N == 0) {
    int* res = (int*)malloc(sizeof(int) * total);
    memcpy(res, perm, total * sizeof(int));
    resTable[*resTableCounter] = res;
    (*resTableCounter)++;
    return;
  }
  for (int i = 1; i <= total; i++) {
    if (unused[i]) continue;
    unused[i] = 1;
    perm[N - 1] = i;
    permutationSub(resTable, resTableCounter, perm, unused, total, N - 1);
    unused[i] = 0;
    perm[N - 1] = 0;
  }
}
int** createPermutation(int n) {
  long nFactorial = factorial(n);
  int** p = (int**)malloc(sizeof(int*) * nFactorial);
  long pCounter = 0;
  int* tmp = (int*)calloc(n, sizeof(int));
  char* unused = (char*)calloc(n + 1, sizeof(char));
  permutationSub(p, &pCounter, tmp, unused, n, n);
  free(tmp);
  free(unused);
  return p;
}

__attribute__((weak)) int main() {
  int** p = createPermutation(4);
  for (int i = 0; i < factorial(4); i++) {
    for (int j = 0; j < 4; j++) {
      printf("%d ", p[i][j]);
    }
    printf("\n");
  }
}