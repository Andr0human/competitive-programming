#ifndef SEARCH_H
#define SEARCH_H

#include <type_traits>
#include <map>
#include <iostream>
#include "types.h"
#include "board.h"
#include "tools.h"
#include "perf.h"

using FrequencyMap = std::map<std::vector<int>, uint64_t>;

FrequencyMap frequencyMap;

namespace search {
  // Forward declarations
  // Base case: when currSquare >= endSquare
  template<int currSquare>
  typename std::enable_if<(currSquare >= SudokuConstants::NUM_SQUARES), uint64_t>::type
  countRecursive(SudokuBoard& board);

  // Recursive case: when currSquare < endSquare
  template<int currSquare = 0>
  typename std::enable_if<(currSquare < SudokuConstants::NUM_SQUARES), uint64_t>::type
  countRecursive(SudokuBoard& board);

  namespace optimisations {
    void
    debug(const SudokuBoard& board, int currSquare) {
      board.show();
      int v1 = board.unmarkedValues(currSquare);
      int v2 = board.unmarkedValues(currSquare + 1);
      int v3 = board.unmarkedValues(currSquare + 2);

      int p1 = tools::popCountTable[v1];
      int p2 = tools::popCountTable[v2];
      int p3 = tools::popCountTable[v3];

      std::cout << "p1: " << p1 << " p2: " << p2 << " p3: " << p3 << std::endl;
      std::cout << "v1: " << tools::getBitString(v1) << " v2: " << tools::getBitString(v2) << " v3: " << tools::getBitString(v3) << std::endl;

      std::string input;

      while (true) {
        std::cin >> input;
        SudokuBoard tempBoard = board;
        if (input == "next") {
          break;
        }

        int num1 = std::stoi(std::to_string(input[0] - '0'));
        int num2 = std::stoi(std::to_string(input[1] - '0'));
        int num3 = std::stoi(std::to_string(input[2] - '0'));

        tempBoard.mark(currSquare, num1);
        tempBoard.mark(currSquare + 1, num2);
        tempBoard.mark(currSquare + 2, num3);

        tempBoard.show();

        int temp_v1 = tempBoard.unmarkedValues(currSquare);
        int temp_v2 = tempBoard.unmarkedValues(currSquare + 1);
        int temp_v3 = tempBoard.unmarkedValues(currSquare + 2);

        int temp_p1 = tools::popCountTable[temp_v1];
        int temp_p2 = tools::popCountTable[temp_v2];
        int temp_p3 = tools::popCountTable[temp_v3];

        std::cout << "temp_p1: " << temp_p1 << " temp_p2: " << temp_p2 << " temp_p3: " << temp_p3 << std::endl;
        std::cout << "temp_v1: " << tools::getBitString(temp_v1) << " temp_v2: " << tools::getBitString(temp_v2) << " temp_v3: " << tools::getBitString(temp_v3) << std::endl;
      }
    }

    template<int currSquare>
    inline uint64_t
    blockResult(const SudokuBoard& board, int leftRow = 0) {
      int val1 = board.unmarkedValues(currSquare, leftRow);
      int val2 = board.unmarkedValues(currSquare + 1, leftRow);
      int val3 = board.unmarkedValues(currSquare + 2, leftRow);

      return static_cast<uint64_t>((val1 * val2 * val3) != 0);
    }

    template<int currSquare>
    inline uint64_t
    square69Cut(const SudokuBoard& board) {
      int val1 = board.unmarkedValues(currSquare);
      int val2 = board.unmarkedValues(currSquare + 1);
      int val3 = board.unmarkedValues(currSquare + 2);

      return static_cast<uint64_t>((val1 * val2 * val3) != 0);
    }

    template<int currSquare>
    inline uint64_t
    square66Cut(const SudokuBoard& board) {
      int valLeft  = board.unmarkedValues(currSquare);
      int valMid   = board.unmarkedValues(currSquare + 1);
      int valRight = board.unmarkedValues(currSquare + 2);

      int pLeft  = tools::popCountTable[valLeft];
      int pMid   = tools::popCountTable[valMid];
      int pRight = tools::popCountTable[valRight];

      if (pLeft * pMid * pRight == 1) {
        return 2 * blockResult<currSquare + 3>(board, valLeft | valMid | valRight);
      }

      if (pLeft * pMid * pRight != 2) {
        return 0;
      }

      if (pLeft == 2) {
        int firstVal  = valLeft & (valLeft - 1);
        int secondVal = valLeft ^ firstVal;
        return (
          blockResult<currSquare + 3>(board,  firstVal | valMid | valRight) +
          blockResult<currSquare + 3>(board, secondVal | valMid | valRight)
        ) * 2;
      }

      if (pMid == 2) {
        int firstVal  = valMid & (valMid - 1);
        int secondVal = valMid ^ firstVal;
        return (
          blockResult<currSquare + 3>(board, valLeft |  firstVal | valRight) +
          blockResult<currSquare + 3>(board, valLeft | secondVal | valRight)
        ) * 2;
      }

      if (pRight == 2) {
        int firstVal  = valRight & (valRight - 1);
        int secondVal = valRight ^ firstVal;
        return (
          blockResult<currSquare + 3>(board, valLeft | valMid |  firstVal) +
          blockResult<currSquare + 3>(board, valLeft | valMid | secondVal)
        ) * 2;
      }

      return 0;
    }

    inline uint64_t
    square63Helper(const SudokuBoard& board, const int leftRow) {
      const int square = 66;

      int valLeft  = board.unmarkedValues(square, leftRow);
      int valMid   = board.unmarkedValues(square + 1, leftRow);
      int valRight = board.unmarkedValues(square + 2, leftRow);

      int pLeft  = tools::popCountTable[valLeft];
      int pMid   = tools::popCountTable[valMid];
      int pRight = tools::popCountTable[valRight];

      if (pLeft * pMid * pRight == 1) {
        return 2 * blockResult<square + 3>(board, leftRow | valLeft | valMid | valRight);
      }

      if (pLeft * pMid * pRight != 2) {
        return 0;
      }

      if (pLeft == 2) {
        int firstVal  = valLeft & (valLeft - 1);
        int secondVal = valLeft ^ firstVal;
        return (
          blockResult<square + 3>(board, leftRow |  firstVal | valMid | valRight) +
          blockResult<square + 3>(board, leftRow | secondVal | valMid | valRight)
        ) * 2;
      }

      if (pMid == 2) {
        int firstVal  = valMid & (valMid - 1);
        int secondVal = valMid ^ firstVal;
        return (
          blockResult<square + 3>(board, leftRow | valLeft |  firstVal | valRight) +
          blockResult<square + 3>(board, leftRow | valLeft | secondVal | valRight)
        ) * 2;
      }

      if (pRight == 2) {
        int firstVal  = valRight & (valRight - 1);
        int secondVal = valRight ^ firstVal;
        return (
          blockResult<square + 3>(board, leftRow | valLeft | valMid |  firstVal) +
          blockResult<square + 3>(board, leftRow | valLeft | valMid | secondVal)
        ) * 2;
      }

      return 0;
    }

    template<int currSquare>
    inline uint64_t
    square63Cut(SudokuBoard& board) {
      int val1 = board.unmarkedValues(currSquare);
      int val2 = board.unmarkedValues(currSquare + 1);
      int val3 = board.unmarkedValues(currSquare + 2);

      int x1 = val1 & (val1 - 1);
      int x2 = val1 ^ x1;
      int y1 = val2 & (val2 - 1);
      int y2 = val2 ^ y1;
      int z1 = val3 & (val3 - 1);
      int z2 = val3 ^ z1;

      return square63Helper(board, x1 | y1 | z1) + square63Helper(board, x1 | y1 | z2)
           + square63Helper(board, x1 | y2 | z1) + square63Helper(board, x1 | y2 | z2)
           + square63Helper(board, x2 | y1 | z1) + square63Helper(board, x2 | y1 | z2)
           + square63Helper(board, x2 | y2 | z1) + square63Helper(board, x2 | y2 | z2);
    }

    template<int currSquare>
    inline uint64_t
    square60Cut(SudokuBoard& board) {
      int v1 = board.unmarkedValues(currSquare);
      int v2 = board.unmarkedValues(currSquare + 1);
      int v3 = board.unmarkedValues(currSquare + 2);

      int p1 = tools::popCountTable[v1];
      int p2 = tools::popCountTable[v2];
      int p3 = tools::popCountTable[v3];

      if (p1 * p2 * p3 != 1) {
        return 0;
      }

      board.mark(currSquare, tools::ctzTable[v1] + 1);
      board.mark(currSquare + 1, tools::ctzTable[v2] + 1);
      board.mark(currSquare + 2, tools::ctzTable[v3] + 1);

      uint64_t boardsFound = countRecursive<currSquare + 3>(board);

      board.unmark(currSquare, tools::ctzTable[v1] + 1);
      board.unmark(currSquare + 1, tools::ctzTable[v2] + 1);
      board.unmark(currSquare + 2, tools::ctzTable[v3] + 1);

      return boardsFound;
    }
  }

  // Base case: when currSquare >= endSquare
  template<int currSquare>
  typename std::enable_if<(currSquare >= SudokuConstants::NUM_SQUARES), uint64_t>::type
  countRecursive(SudokuBoard& board) {
    return 1;
  }

  // Recursive case: when currSquare < endSquare
  template<int currSquare>
  typename std::enable_if<(currSquare < SudokuConstants::NUM_SQUARES), uint64_t>::type
  countRecursive(SudokuBoard& board) {
    if (!board.isEmpty(currSquare)) {
      return countRecursive<currSquare + 1>(board);
    }

    if (currSquare == 69) {
      return optimisations::square69Cut<currSquare>(board);
    }

    if (currSquare == 66) {
      return optimisations::square66Cut<currSquare>(board);
    }

    if (currSquare == 63) {
      return optimisations::square63Cut<currSquare>(board);
    }

    if (currSquare == 60) {
      return optimisations::square60Cut<currSquare>(board);
    }


    uint64_t boardsFound = 0;
    int leftValues = board.unmarkedValues(currSquare);

    while (leftValues > 0) {
      const int num = tools::nextIndex(leftValues);
      board.mark(currSquare, num);
      boardsFound += countRecursive<currSquare + 1>(board);
      board.unmark(currSquare, num);
    }

    return boardsFound;
  }

  void
  countBoards(const std::string& sudokuString) {
    std::string errorMessage;
    std::optional<SudokuBoard> boardOpt = SudokuBoard::loadFromString(sudokuString, errorMessage);
    
    if (!boardOpt.has_value()) {
      std::cerr << "Error: " << errorMessage << std::endl;
      std::cerr << "Please provide a valid Sudoku string." << std::endl;
      return;
    }

    SudokuBoard board = boardOpt.value();
    board.show();

    Perf perf;
    perf.start();
    const uint64_t boardsFound = countRecursive(board);
    const double timeSpent = perf.timeSpent();
    std::cout << "Boards Found: " << boardsFound << " (" << tools::humanReadableBoardCount(boardsFound) << ")" << std::endl;
    std::cout << "Time Spent: " << timeSpent << " seconds" << std::endl;
    std::cout << "Speed: " << tools::humanReadableSpeed(boardsFound, timeSpent) << "\n" << std::endl;
  }
}

#endif
