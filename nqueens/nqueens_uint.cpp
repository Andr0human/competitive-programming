#include <cassert>
#include <iostream>
#include "lib/UInt.h"
#include "lib/perf.h"

constexpr int MAX_N = 16;
constexpr int ALLOTED_BITS = 256;

std::array<UInt<ALLOTED_BITS>, MAX_N * MAX_N> computed_masks;

void precompute_masks(const int n) {
  for (int row = 0; row < n; row++) {
    for (int col = 0; col < n; col++) {
      UInt<ALLOTED_BITS> mask(0);

      for (int i = 0; i < n; i++) {
        mask |= UInt<ALLOTED_BITS>(1) << (row * n + i);
      }
      for (int i = row; i < n; i++) {
        mask |= UInt<ALLOTED_BITS>(1) << (i * n + col);
      }
      for (int i = row, j = col; i < n && j < n; i++, j++) {
        mask |= UInt<ALLOTED_BITS>(1) << (i * n + j);
      }
      for (int i = row, j = col; i < n && j >= 0; i++, j--) {
        mask |= UInt<ALLOTED_BITS>(1) << (i * n + j);
      }

      computed_masks[row * n + col] = mask;
    }
  }
}

int nqueens_uint(int row, UInt<ALLOTED_BITS> board, int n) {
  if (row == n) {
    return 1;
  }

  int indices[MAX_N];
  int num_indices = board.get_all_clear_indices(indices, (row + 1) * n);

  int count = 0;

  for (int i = 0; i < num_indices; i++) {
    count += nqueens_uint(row + 1, board | computed_masks[indices[i]], n);
  }

  return count;
}


int main() {
  static_assert(ALLOTED_BITS >= MAX_N * MAX_N, "ALLOTED_BITS must be greater than or equal to MAX_N * MAX_N");
  int n;
  std::cin >> n;
  assert(n <= MAX_N);

  precompute_masks(n);
  
  perf::Timer t("nqueens_uint");
  std::cout << nqueens_uint(0, UInt<ALLOTED_BITS>(0), n) << std::endl;
}
