#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

char *readSource() {
  FILE *sourceFile = fopen("source.olang", "r");
  if (sourceFile == NULL) {
    perror("Failed to open source file");
    abort();
  }

  // get the length of the file
  fseek(sourceFile, 0, SEEK_END);
  size_t fileSize = ftell(sourceFile);
  fseek(sourceFile, 0, SEEK_SET);

  char *source = malloc(fileSize + 1); // 1 for NULL pointer

  fread(source, 1, fileSize, sourceFile);
  source[fileSize] = '\0';

  // printf("File contents:\n%s\n", source);

  fclose(sourceFile);

  return source;
}

int main() {
  char *source = readSource();

  Program ast = parse(source);
  printAst(ast);

  return eval(source);
  return 0;
}
