#include <array>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <vector>
#include <algorithm>

#include "board.h"
#include "search.h"

/*

board1: 123456789456789123789123456214365897
board2: 123456789456789123789123456214 -- 44078976
board3: 12345678945678912378912345621  -- 176315904

*/

int main(int argc, char **argv) {
  const std::string sudokuString = tools::getArg(argc, argv, "sudoku", "123456789");
  const int threadCount = std::stoi(tools::getArg(argc, argv, "threads", "1"));

  if (tools::hasArg(argc, argv, "count")) {
    search::countBoards(sudokuString);
  } else if (tools::hasArg(argc, argv, "solve")) {
    // search::solve(sudokuString);
  }

  return 0;
}
