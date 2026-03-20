#ifndef UINT_H
#define UINT_H

#include <cstdint>
#include <array>

template <int N>
class UInt {
  static_assert(N > 0 && N % 64 == 0, "N must be a positive multiple of 64");

 public:
  static constexpr int num_blocks = N / 64;

  UInt() : digits_{} {}

  explicit UInt(uint64_t value) : digits_{} {
    digits_[0] = value;
  }

  UInt& operator&=(const UInt& o) {
    for (int i = 0; i < num_blocks; ++i) digits_[i] &= o.digits_[i];
    return *this;
  }

  UInt& operator|=(const UInt& o) {
    for (int i = 0; i < num_blocks; ++i) digits_[i] |= o.digits_[i];
    return *this;
  }

  UInt& operator^=(const UInt& o) {
    for (int i = 0; i < num_blocks; ++i) digits_[i] ^= o.digits_[i];
    return *this;
  }

  UInt& operator&=(uint64_t o) {
    digits_[0] &= o;
    for (int i = 1; i < num_blocks; ++i) digits_[i] = 0;
    return *this;
  }

  UInt& operator|=(uint64_t o) {
    digits_[0] |= o;
    return *this;
  }

  UInt& operator^=(uint64_t o) {
    digits_[0] ^= o;
    return *this;
  }

  UInt operator~() const {
    UInt r;
    for (int i = 0; i < num_blocks; ++i) r.digits_[i] = ~digits_[i];
    return r;
  }

  UInt operator&(const UInt& o) const { UInt r = *this; return r &= o; }
  UInt operator|(const UInt& o) const { UInt r = *this; return r |= o; }
  UInt operator^(const UInt& o) const { UInt r = *this; return r ^= o; }

  UInt operator&(uint64_t o) const { UInt r = *this; return r &= o; }
  UInt operator|(uint64_t o) const { UInt r = *this; return r |= o; }
  UInt operator^(uint64_t o) const { UInt r = *this; return r ^= o; }

  UInt& operator<<=(int s) {
    if (s <= 0) return *this;
    const int block_shift = s / 64;
    const int bit_shift = s % 64;
    for (int i = num_blocks - 1; i >= block_shift; --i) {
      digits_[i] = digits_[i - block_shift] << bit_shift;
    }
    for (int i = block_shift - 1; i >= 0; --i) {
      digits_[i] = 0;
    }
    return *this;
  }

  UInt& operator>>=(int s) {
    if (s <= 0) return *this;
    const int block_shift = s / 64;
    const int bit_shift = s % 64;
    if (block_shift >= num_blocks) {
      digits_.fill(0);
      return *this;
    }
    for (int i = num_blocks - 1; i >= 0; --i) {
      int src = i + block_shift;
      digits_[i] = (src < num_blocks ? digits_[src] >> bit_shift : 0) |
                   (bit_shift != 0 && src > 0
                        ? digits_[src - 1] << (64 - bit_shift)
                        : 0);
    }
    return *this;
  }

  UInt operator<<(int s) const { UInt r = *this; return r <<= s; }
  UInt operator>>(int s) const { UInt r = *this; return r >>= s; }

  /** 0-based index of the least significant set bit, or -1 if zero. */
  int least_bit_index() const {
    for (int i = 0; i < num_blocks; ++i) {
      if (digits_[i] != 0) return i * 64 + __builtin_ctzll(digits_[i]);
    }
    return -1;
  }

  /** 0-based index of the least significant clear (0) bit, or -1 if all bits set. */
  int least_clear_index() const {
    for (int i = 0; i < num_blocks; ++i) {
      if (digits_[i] != UINT64_MAX) return i * 64 + __builtin_ctzll(~digits_[i]);
    }
    return -1;
  }

  /** Fills indicesArray with 0-based indices of all clear (0) bits in [0, max_bit); returns count. */
  int get_all_clear_indices(int indicesArray[], int max_bit = N) const {
    int lastIndex = 0;
    for (int i = 0; i < num_blocks && i * 64 < max_bit; ++i) {
      int block_start = i * 64;
      uint64_t clear_bits = ~digits_[i];
      int max_j = (block_start + 64 <= max_bit) ? 64 : (max_bit - block_start);
      while (clear_bits != 0) {
        int j = __builtin_ctzll(clear_bits);
        if (j >= max_j) break;
        indicesArray[lastIndex++] = block_start + j;
        clear_bits &= clear_bits - 1;
      }
    }
    return lastIndex;
  }

  explicit operator bool() const {
    for (int i = 0; i < num_blocks; ++i)
      if (digits_[i]) return true;
    return false;
  }

  bool operator==(const UInt& o) const {
    for (int i = 0; i < num_blocks; ++i)
      if (digits_[i] != o.digits_[i]) return false;
    return true;
  }
  bool operator!=(const UInt& o) const { return !(*this == o); }

  const std::array<uint64_t, num_blocks>& data() const { return digits_; }
  std::array<uint64_t, num_blocks>& data() { return digits_; }

 private:
  std::array<uint64_t, num_blocks> digits_;
};

#endif
