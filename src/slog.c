// NOTE: this code is very unreadable, i will never work with terminal UX again

#include "slog.h"
#include "colors.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

// get the row and col from location
void getLocationRowCol(size_t location, char *source, size_t *row,
                       size_t *col) {
  *row = 1;
  *col = 1;

  for (size_t c = 0; c < location; c++) {
    if (source[c] == '\n') {
      *col = 1;
      (*row)++;
    } else {
      (*col)++;
    }
  }
};

int measureRowWidth(size_t row) { return (int)log10(row) + 1; }

void printLineHeader(int rowNumberWidth, size_t row) {
  printf(ANSI_RESET);

  int paddingWidth = rowNumberWidth - measureRowWidth(row);
  for (int i = 0; i < paddingWidth; i++) {
    printf(" ");
  }
  printf("%zu | ", row);
}

void printBlankLineHeader(int rowNumberWidth) {
  printf(ANSI_RESET);

  for (int i = 0; i < rowNumberWidth; i++) {
    printf(" ");
  }
  printf(" | ");
}

//////////////////////
// PUBLIC FUNCTIONS //
//////////////////////

void slogLocation(char *source, size_t location, SlogLevel level,
                  char *message) {
  size_t row, col;
  getLocationRowCol(location, source, &row, &col);
  int rowWidth = measureRowWidth(row);

  printf(ANSI_COLOR_BLUE "%zu:%zu ", row, col);
  slog(level, message);

  printBlankLineHeader(rowWidth);
  printf("\n");
  printLineHeader(rowWidth, row);

  // print the row to the screen
  size_t c = 0;
  size_t r = 1;
  while (true) {
    if (r < row) {
      if (source[c] == '\n') {
        r++;
      }
    } else {
      printf("%c", source[c]);
      if (source[c] == '\n') {
        break;
      }
    }
    c++;
  }

  printBlankLineHeader(rowWidth);

  // print the ^ indicator that points to the location
  for (size_t i = 1; i < col; i++) {
    printf(" ");
  }
  printf("^\n\n");
}

void slogRegion(char *source, Region region, SlogLevel level,
                char *message) {
  if (region.start == region.end) {
    slogLocation(source, region.start, level, message);
    return;
  }

  size_t startRow, startCol, endRow, endCol;
  getLocationRowCol(region.start, source, &startRow, &startCol);
  getLocationRowCol(region.end, source, &endRow, &endCol);

  int rowWidth = measureRowWidth(endRow);

  printf(ANSI_COLOR_BLUE "%zu:%zu -> %zu:%zu ", startRow, startCol, endRow,
         endCol);
  slog(level, message);

  // if the start and end are on the same row we can print the error in a
  // similar manner to slogLocation, however if they are one different rows we
  // will need to highlight the error with some kind of red text
  if (startRow == endRow) {
    printBlankLineHeader(rowWidth);
    printf("\n");

    printLineHeader(rowWidth, startRow);

    // print the row to the screen
    size_t c = 0;
    size_t r = 1;
    while (true) {
      if (r < startRow) {
        if (source[c] == '\n') {
          r++;
        }
      } else {
        printf("%c", source[c]);
        if (source[c] == '\n') {
          break;
        }
      }
      c++;
    }

    printBlankLineHeader(rowWidth);

    // print the ^ indicator that points to the location
    for (size_t i = 1; i < startCol; i++) {
      printf(" ");
    }
    // print the appropriate number of ^
    for (size_t i = 0; i < endCol - startCol + 1; i++) {
      printf("^");
    }
    printf("\n\n");
  }
  // TODO: implement error reporting for multi line errors
};

void slog(SlogLevel level, char *message) {
  // print the header
  printf(ANSI_RESET ANSI_FORMAT_BOLD);
  switch (level) {
  case WARNING:
    printf(ANSI_COLOR_YELLOW "WARNING: ");
    break;
  case ERROR:
    printf(ANSI_COLOR_RED "ERROR: ");
    break;
  }

  // print the message
  printf(ANSI_RESET ANSI_FORMAT_BOLD "%s\n" ANSI_RESET, message);
}
