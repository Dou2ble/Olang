#include "log.h"
#include "colors.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// void printLineBlank(size_t line) {
//   if (line <= 0) {
//     return;
//   }
//   size_t lineNumberWidth = (size_t)log10(line);

//   for (size_t i = 0; i < lineNumberWidth; i++) {
//     printf(" ");
//   }
//   printf("  | ");
// }

// void printLine(const char *source, size_t line) {
//   // impossible
//   if (line <= 0) {
//     return;
//   }

//   printf("%zu | ", line);

//   // find the line in the source code
//   char *sourceCopy = strdup(source);
//   char *token = strtok(sourceCopy, "\n");
//   for (size_t currentLine = 1; token != NULL; currentLine++) {
//     if (currentLine == line) {
//       printf("%s", token);
//       break;
//     }

//     token = strtok(NULL, "\n");
//     printf("1");
//   }

//   free(sourceCopy);
// }

// structured log
void slog(LogLevel level, LogStage stage, char *message) {
  switch (level) {
  case LEVEL_WARNING:
    printf(ANSI_COLOR_YELLOW);
    break;
  case LEVEL_ERROR:
    printf(ANSI_COLOR_RED);
    break;
  }

  switch (stage) {
  case STAGE_LEXING:
    printf("LEX ");
    break;
  case STAGE_PARSING:
    printf("PARSE ");
    break;
  case STAGE_EVAL:
    printf("EVAL ");
    break;
  }

  switch (level) {
  case LEVEL_WARNING:
    printf("WARNING: ");
    break;
  case LEVEL_ERROR:
    printf("ERROR: ");
    break;
  }

  printf("%s\n" ANSI_RESET, message);
}

void slogLocation(LogLevel level, LogStage stage, const char *source,
                  Location location, char *message) {
  slog(level, stage, message);
  // printLineBlank(location.row);
  printf(ANSI_COLOR_BLUE "row: %zu col: %zu\n" ANSI_RESET, location.row,
         location.col);
  // printLine(source, location.row);
  printf("\n");
  // printLineBlank(location.row);
  // for (size_t i = 0; i < location.col - 1; i++) {
  //   printf(" ");
  // }
  // printf(ANSI_COLOR_RED "^" ANSI_RESET "\n");
  // printf("\n\n");
}
