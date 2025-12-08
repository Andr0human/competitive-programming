#ifndef SUDOKU_BOARD_H
#define SUDOKU_BOARD_H

#include <string>
#include <array>
#include <optional>
#include "types.h"
#include "tools.h"

using MarkedValueArray = std::array<int, SudokuConstants::NUM_BLOCKS>;
using BoardArray = std::array<int, SudokuConstants::NUM_SQUARES>;


class SudokuBoard {
private:
  MarkedValueArray rowArray;
  MarkedValueArray colArray;
  MarkedValueArray blockArray;
  BoardArray boardArray;

  inline void
  reset() noexcept {
    rowArray.fill(0);
    colArray.fill(0);
    blockArray.fill(0);
    boardArray.fill(0);
  }

public:
  SudokuBoard() noexcept {
    reset();
  }

  SudokuBoard(const std::string& sudokuString) {
    if (sudokuString.size() > SudokuConstants::NUM_SQUARES) {
      throw std::invalid_argument("Invalid Sudoku string!!");
    }

    reset();
    for (size_t i = 0; i < sudokuString.size(); i++) {
      if (sudokuString[i] < '1' || sudokuString[i] > '9') {
        continue;
      }
      if (!mark<true>(static_cast<int>(i), static_cast<int>(sudokuString[i] - '0'))) {
        throw std::invalid_argument("Invalid Sudoku string!!");
      }
    } 
  }

  // Factory function that returns optional instead of throwing
  static std::optional<SudokuBoard>
  loadFromString(const std::string& sudokuString, std::string& errorMessage) {
    if (sudokuString.size() > SudokuConstants::NUM_SQUARES) {
      errorMessage = "Invalid Sudoku string: length (" + std::to_string(sudokuString.size()) + 
                     ") exceeds maximum allowed (" + std::to_string(SudokuConstants::NUM_SQUARES) + ")";
      return std::nullopt;
    }

    SudokuBoard board;
    for (size_t i = 0; i < sudokuString.size(); i++) {
      if (sudokuString[i] < '1' || sudokuString[i] > '9') {
        continue;
      }
      if (!board.mark<true>(static_cast<int>(i), static_cast<int>(sudokuString[i] - '0'))) {
        errorMessage = "Invalid Sudoku string: conflict at position " + std::to_string(i) + 
                       " (value " + sudokuString[i] + " violates Sudoku rules)";
        return std::nullopt;
      }
    }
    errorMessage = "";
    return board;
  }

  inline
  ~SudokuBoard() noexcept {
    reset();
  }

  template<bool validCheck = false>
  inline bool
  mark(const int square, const int bit) noexcept {
    const int bitMask = 1 << (bit - 1);
    const auto [row, col, block] = tools::rowColBlockIndexTable[square];

    if (validCheck) {
      if ((rowArray[row] | colArray[col] | blockArray[block]) & bitMask) {
        return false;
      }
    }

    rowArray[row] |= bitMask;
    colArray[col] |= bitMask;
    blockArray[block] |= bitMask;
    boardArray[square] = bit;

    return true;
  }

  inline void
  unmark(const int square, const int bit) noexcept {
    const int bitMask = ~(1 << (bit - 1));
    const auto [row, col, block] = tools::rowColBlockIndexTable[square];

    rowArray[row] &= bitMask;
    colArray[col] &= bitMask;
    blockArray[block] &= bitMask;
    boardArray[square] = 0;
  }

  inline int
  unmarkedValues(const int square) const noexcept {
    const auto [row, col, block] = tools::rowColBlockIndexTable[square];
    return (rowArray[row] | colArray[col] | blockArray[block]) ^ SudokuConstants::NUM_VALUES_MASK;
  }

  inline int
  unmarkedValues(const int square, int leftRow) const noexcept {
    const auto [row, col, block] = tools::rowColBlockIndexTable[square];
    return (rowArray[row] | colArray[col] | blockArray[block] | leftRow) ^ SudokuConstants::NUM_VALUES_MASK;
  }

  inline int
  getRow(const int square) const noexcept {
    const auto row = std::get<0>(tools::rowColBlockIndexTable[square]);
    return rowArray[row];
  }

  inline int
  getColumn(const int square) const noexcept {
    const auto col = std::get<1>(tools::rowColBlockIndexTable[square]);
    return colArray[col];
  }

  inline int
  getBlock(const int square) const noexcept {
    const auto block = std::get<2>(tools::rowColBlockIndexTable[square]);
    return blockArray[block];
  }

  inline bool
  isEmpty(const int square) const noexcept {
    return boardArray[square] == 0;
  }

  void
  show() const noexcept {
    using std::cout, std::string, std::endl;

    const string s = " +-------------+-----------+-------------+\n";
    const string g1 = " || ";
    const string g2 = " | ";

    cout << s;

    for (int row = 0; row < SudokuConstants::NUM_ROWS; ++row){
      cout << g1;
      for (int col = 0; col < SudokuConstants::NUM_COLS; ++col) {
        cout << boardArray[SudokuConstants::NUM_ROWS * row + col];
        cout << ((col != 0 && col % 3 == 2) ? (g1) : (g2));
      }
      cout << '\n' << s;
      if (row != 0 && row != SudokuConstants::NUM_ROWS - 1 && row % 3 == 2) cout << s;
    }
    cout << endl;
  }

  inline std::string
  toString() const noexcept {
    std::string res;
    res.resize(SudokuConstants::NUM_SQUARES);
    for (int i = 0; i < SudokuConstants::NUM_SQUARES; i++) {
      res[i] = static_cast<char>(boardArray[i] + '0');
    }
    return res;
  }

  inline int
  valueAtSquare(const int square) const noexcept {
    return boardArray[square];
  }
};

#endif
