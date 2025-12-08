#ifndef TOOLS_H
#define TOOLS_H

#include <cstdint>
#include <array>
#include <tuple>
#include "types.h"

using PopCountTable = std::array<int, SudokuConstants::NUM_ARRAY_SIZE>;
using CtzTable = std::array<int, SudokuConstants::NUM_ARRAY_SIZE>;
using RowColBlockIndexTable = std::array<std::tuple<int, int, int>, SudokuConstants::NUM_SQUARES>;

namespace tools {
  // Constexpr popcount implementation
  constexpr int
  popcount(unsigned int x) {
    int count = 0;
    while (x) {
      count += x & 1;
      x >>= 1;
    }
    return count;
  }

  // Constexpr count trailing zeros implementation
  constexpr int
  ctz(unsigned int x) {
    if (x == 0) return 32; // or appropriate max for the type
    int count = 0;
    while ((x & 1) == 0) {
      count++;
      x >>= 1;
    }
    return count;
  }

  constexpr PopCountTable popCountTable = []() constexpr {
    PopCountTable table = {0};
    for (int i = 0; i < SudokuConstants::NUM_ARRAY_SIZE; i++) {
      table[i] = popcount(i);
    }
    return table;
  }();

  constexpr CtzTable ctzTable = []() constexpr {
    CtzTable table = {0};
    for (int i = 0; i < SudokuConstants::NUM_ARRAY_SIZE; i++) {
      table[i] = ctz(i);
    }
    return table;
  }();

  constexpr RowColBlockIndexTable rowColBlockIndexTable = []() constexpr {
    RowColBlockIndexTable table = {};
    for (int i = 0; i < SudokuConstants::NUM_SQUARES; i++) {
      table[i] = std::make_tuple(
        i / SudokuConstants::NUM_ROWS,
        i % SudokuConstants::NUM_COLS,
        3 * static_cast<int>(i / (SudokuConstants::NUM_ROWS * 3)) + (i % SudokuConstants::NUM_COLS) / 3
      );
    }
    return table;
  }();

  const int
  nextIndex(int& x) noexcept {
    int res = ctzTable[x] + 1;
    x &= (x - 1);
    return res;
  }

  const std::string
  getArg(int argc, char **argv, const std::string& argName, const std::string defaultValue = "") {
    for (int i = 1; i < argc - 1; i++) {
      if (std::string(argv[i]) == argName) {
        return std::string(argv[i + 1]);
      }
    }
    return defaultValue;
  }

  const bool
  hasArg(int argc, char **argv, const std::string& argName) {
    for (int i = 0; i < argc; i++) {
      if (std::string(argv[i]) == argName) {
        return true;
      }
    }
    return false;
  }

  const std::string
  getBitString(int x) {
    std::string res;
    for (int i = 0; i < SudokuConstants::NUM_BLOCKS; i++) {
      if (x & (1 << i)) {
        res += std::to_string(i + 1) + " ";
      }
    }
    return res;
  }

  const std::string
  humanReadableSpeed(uint64_t boardsFound, double timeSpent) {
    if (timeSpent <= 0.0) {
      return "0 boards / sec";
    }
    
    double speed = static_cast<double>(boardsFound) / timeSpent;
    
    if (speed >= 1e9) {
      double value = speed / 1e9;
      if (value >= 100 || value - static_cast<int>(value) < 0.01) {
        return std::to_string(static_cast<int>(value)) + " B boards / sec";
      } else {
        int rounded = static_cast<int>(value * 10 + 0.5);
        return std::to_string(rounded / 10) + "." + std::to_string(rounded % 10) + " B boards / sec";
      }
    } else if (speed >= 1e6) {
      double value = speed / 1e6;
      if (value >= 100 || value - static_cast<int>(value) < 0.01) {
        return std::to_string(static_cast<int>(value)) + " M boards / sec";
      } else {
        int rounded = static_cast<int>(value * 10 + 0.5);
        return std::to_string(rounded / 10) + "." + std::to_string(rounded % 10) + " M boards / sec";
      }
    } else if (speed >= 1e3) {
      double value = speed / 1e3;
      if (value >= 100 || value - static_cast<int>(value) < 0.01) {
        return std::to_string(static_cast<int>(value)) + " K boards / sec";
      } else {
        int rounded = static_cast<int>(value * 10 + 0.5);
        return std::to_string(rounded / 10) + "." + std::to_string(rounded % 10) + " K boards / sec";
      }
    } else {
      return std::to_string(static_cast<uint64_t>(speed)) + " boards / sec";
    }
  }

  const std::string
  humanReadableBoardCount(uint64_t boardsFound) {
    if (boardsFound >= 1e9) {
      double value = static_cast<double>(boardsFound) / 1e9;
      if (value >= 100 || value - static_cast<int>(value) < 0.01) {
        return std::to_string(static_cast<int>(value)) + " B";
      } else {
        int rounded = static_cast<int>(value * 10 + 0.5);
        return std::to_string(rounded / 10) + "." + std::to_string(rounded % 10) + " B";
      }
    } else if (boardsFound >= 1e6) {
      double value = static_cast<double>(boardsFound) / 1e6;
      if (value >= 100 || value - static_cast<int>(value) < 0.01) {
        return std::to_string(static_cast<int>(value)) + " M";
      } else {
        int rounded = static_cast<int>(value * 10 + 0.5);
        return std::to_string(rounded / 10) + "." + std::to_string(rounded % 10) + " M";
      }
    } else if (boardsFound >= 1e3) {
      double value = static_cast<double>(boardsFound) / 1e3;
      if (value >= 100 || value - static_cast<int>(value) < 0.01) {
        return std::to_string(static_cast<int>(value)) + " K";
      } else {
        int rounded = static_cast<int>(value * 10 + 0.5);
        return std::to_string(rounded / 10) + "." + std::to_string(rounded % 10) + " K";
      }
    }
    return std::to_string(boardsFound);
  }
};

#endif
