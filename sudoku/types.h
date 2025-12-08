#ifndef TYPES_H
#define TYPES_H

enum SudokuConstants: int {
  NUM_BLOCKS = 9,
  NUM_ROWS = 9,
  NUM_COLS = 9,
  NUM_SQUARES = 81,
  NUM_VALUES_MASK = (1 << NUM_BLOCKS) - 1,
  NUM_ARRAY_SIZE = 1 << NUM_BLOCKS,
};

#endif
