#ifndef LEXER_H
#define LEXER_H

#include "slog.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// arithmetic
// + - * / % ++ --

// assignment
// =
// +=
// -=
// *=
// /=
// %=
// &=
// |=
// ^=
// >>=
// <<=

// comparison
// ==
// !=
// >
// <
// >=
// <=

// logical
// && and
// || or
// ! not

// bitwise
// & and
// | or
// ^ xor
// << left shift
// >> right shift
typedef enum {
  // Arithmetic tokens
  TOKEN_PLUS,           // +
  TOKEN_MINUS,          // -
  TOKEN_MULTIPLICATION, // *
  TOKEN_DIVISION,       // /
  TOKEN_MODULUS,        // %
  TOKEN_INCREMENT,      // ++
  TOKEN_DECREMENT,      // --

  TOKEN_IS_EQUAL,                 // ==
  TOKEN_IS_NOT_EQUAL,             // !=
  TOKEN_IS_GREATER_THAN,          // >
  TOKEN_IS_LESS_THAN,             // <
  TOKEN_IS_GREATER_THAN_OR_EQUAL, // >=
  TOKEN_IS_LESS_THAN_OR_EQUAL,    // <=

  // Keywords
  TOKEN_KEYWORD_RETUN, // return
  TOKEN_KEYWORD_MUT,   // mut (normal variable)
  TOKEN_KEYWORD_LET,   // let (const variable)
  TOKEN_KEYWORD_FUN,   // fun (function literal)
  TOKEN_KEYWORD_IF,    // if (if expression)
  TOKEN_KEYWORD_FOR,   // for (for loop)
  TOKEN_KEYWORD_WHILE, // while (while loop)

  TOKEN_BOOL,        // true or false
  TOKEN_INT,         // 12
  TOKEN_STRING,      // "arst"
  TOKEN_IDENTIFIER,  // main
  TOKEN_OPEN_PAREN,  // (
  TOKEN_CLOSE_PAREN, // )
  TOKEN_OPEN_BRACE,  // {
  TOKEN_CLOSE_BRACE, // }
  TOKEN_EQUAL_SIGN,  // =
} TokenType;

typedef struct {
  Region region;
  TokenType type;
  union {
    int64_t intVal;
    char *string;
    char *identifier;
    bool boolean;
  } value;
} Token;

Token *tokenize(size_t *tokenCount, char *source);
void printTokens(const Token *tokens, const size_t tokenCount);
void printToken(const Token token);

#endif // !LEXER_H
