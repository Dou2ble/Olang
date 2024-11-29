#ifndef LOG_H
#define LOG_H

#include "lexer.h"

typedef enum {
  LEVEL_WARNING,
  LEVEL_ERROR // aka fatal
} LogLevel;

typedef enum { STAGE_LEXING, STAGE_PARSING, STAGE_EVAL } LogStage;

void slog(LogLevel level, LogStage stage, char *message);
void slogLocation(LogLevel level, LogStage stage, const char *source,
                  Location location, char *message);

#endif // !LOG_H
