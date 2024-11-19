#include "utils.h"

#include <stdlib.h>
#include <string.h>

char *repeatString(char *string, int64_t repetitions) {
  if (repetitions <= 0) {
    return NULL; // Return NULL for non-positive repeat counts
  }

  char *result;
  size_t len = strlen(string);
  result = malloc(len * repetitions + 1);

  for (int64_t i = 0; i < repetitions; i++) {
    strcpy(result + i * len, string);
  }

  return result;
}
