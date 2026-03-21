#ifndef UINT_H
#define UINT_H

#include <cstdint>
#include <array>

template <int N>
class UInt {
 public:
  static constexpr int num_blocks = (N + 63) / 64;

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
    for (int i = 0; i < num_blocks - 1; ++i) r.digits_[i] = ~digits_[i];

    constexpr int top_bits = N & 63;
    constexpr uint64_t top_mask = (top_bits == 0) ? UINT64_MAX : ((1ULL << top_bits) - 1);
    r.digits_[num_blocks - 1] = ~digits_[num_blocks - 1] & top_mask;

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
    const int block_shift = s >> 6;
    const int bit_shift = s & 63;
    const int carry_shift = 64 - bit_shift;

    if (block_shift >= num_blocks) {
      digits_.fill(0);
      return *this;
    }

    if (bit_shift == 0) {
      for (int i = num_blocks - 1; i >= block_shift; --i)
        digits_[i] = digits_[i - block_shift];
    } else {
      for (int i = num_blocks - 1; i > block_shift; --i)
        digits_[i] = (digits_[i - block_shift] << bit_shift) |
                     (digits_[i - block_shift - 1] >> carry_shift);
      digits_[block_shift] = digits_[0] << bit_shift;
    }

    for (int i = 0; i < block_shift; ++i)
      digits_[i] = 0;

    return *this;
  }

  UInt& operator>>=(int s) {
    if (s <= 0) return *this;
    const int block_shift = s >> 6;
    const int bit_shift = s & 63;

    if (block_shift >= num_blocks) {
      digits_.fill(0);
      return *this;
    }

    const int last = num_blocks - 1 - block_shift;
    const int carry_shift = 64 - bit_shift;

    if (bit_shift == 0) {
      for (int i = 0; i <= last; ++i)
        digits_[i] = digits_[i + block_shift];
    } else {
      for (int i = 0; i < last; ++i)
        digits_[i] = (digits_[i + block_shift] >> bit_shift) |
                     (digits_[i + block_shift + 1] << carry_shift);
      digits_[last] = digits_[last + block_shift] >> bit_shift;
    }

    for (int i = last + 1; i < num_blocks; ++i)
      digits_[i] = 0;

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
    for (int i = 0; i < num_blocks - 1; ++i) {
      if (digits_[i] != UINT64_MAX) return i * 64 + __builtin_ctzll(~digits_[i]);
    }
    // mask top block to only consider valid bits
    constexpr int top_bits = N & 63;
    constexpr uint64_t top_mask = (top_bits == 0) ? UINT64_MAX : ((1ULL << top_bits) - 1);
    uint64_t top = ~digits_[num_blocks - 1] & top_mask;
    return top ? (num_blocks - 1) * 64 + __builtin_ctzll(top) : -1;
  }

  /** Fills indicesArray with 0-based indices of all clear (0) bits in [0, max_bit); returns count. */
  int get_all_clear_indices(int indicesArray[], int max_bit = N) const {
    int lastIndex = 0;
    for (int i = 0; i < num_blocks && i * 64 < max_bit; ++i) {
      int block_start = i * 64;
      uint64_t clear_bits = ~digits_[i];
      // clamp to valid bits in top block
      if (i == num_blocks - 1) {
        constexpr int top_bits = N & 63;
        constexpr uint64_t top_mask = (top_bits == 0) ? UINT64_MAX : ((1ULL << top_bits) - 1);
        clear_bits &= top_mask;
      }
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
    uint64_t acc = 0;
    for (int i = 0; i < num_blocks; ++i) acc |= digits_[i];
    return acc != 0;
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
