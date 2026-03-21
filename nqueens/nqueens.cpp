
#include <iostream>
#include "../lib/perf.h"

// nqueen problem

const int MAX_N = 16;

bool col_array[MAX_N];
bool diag1_array[2 * MAX_N - 1];
bool diag2_array[2 * MAX_N - 1];

int nqueens(int row, int n) {
  if (row == n) {
    return 1;
  }

  int count = 0;
  for (int col = 0; col < n; col++) {
    if (col_array[col] || diag1_array[row + col] || diag2_array[row - col + n - 1]) continue;
    col_array[col] = true;
    diag1_array[row + col] = true;
    diag2_array[row - col + n - 1] = true;
    count += nqueens(row + 1, n);
    col_array[col] = false;
    diag1_array[row + col] = false;
    diag2_array[row - col + n - 1] = false;
  }
  return count;
}

void solve_nqueens(int n) {
  perf::Timer t("nqueens_array");
  std::cout << nqueens(0, n) << std::endl;
}

int main() {
  int n;
  std::cin >> n;

  solve_nqueens(n);
}
