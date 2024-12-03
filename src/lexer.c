#include "lexer.h"
#include "slog.h"
// #include "log.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void appendToken(size_t *tokenCount, Token **tokens, const Token newToken) {
  (*tokenCount)++;
  *tokens = realloc(*tokens, sizeof(Token) * (*tokenCount));
  (*tokens)[(*tokenCount) - 1] = newToken;
}

Token *tokenize(size_t *tokenCount, char *source) {
  Token *tokens = NULL;
  *tokenCount = 0;

  // loop over the source string
  size_t c = 0;
  Token token;
  Region region;
  while (c < strlen(source)) {
    region.start = c;

    // skip whitespace
    if (isspace(source[c])) {
      c++;
      continue;
    }

    // check for simple tokens
    TokenType simpleToken = TOKEN_IDENTIFIER;
    switch (source[c]) {
    case '(':
      simpleToken = TOKEN_OPEN_PAREN;
      break;
    case ')':
      simpleToken = TOKEN_CLOSE_PAREN;
      break;
    case '{':
      simpleToken = TOKEN_OPEN_BRACE;
      break;
    case '}':
      simpleToken = TOKEN_CLOSE_BRACE;
      break;
    case '=':
      simpleToken = TOKEN_EQUAL_SIGN;
      if (source[c + 1] == '=') {
        simpleToken = TOKEN_IS_EQUAL;
        c++;
      }
      break;

      // Arithmetic token
    case '+':
      simpleToken = TOKEN_PLUS;
      if (source[c + 1] == '+') {
        simpleToken = TOKEN_INCREMENT;
        c++;
      }
      break;
    case '-':
      simpleToken = TOKEN_MINUS;
      if (source[c + 1] == '-') {
        simpleToken = TOKEN_DECREMENT;
        c++;
      }
      break;
    case '*':
      simpleToken = TOKEN_MULTIPLICATION;
      break;
    case '/':
      simpleToken = TOKEN_DIVISION;
      break;
    case '%':
      simpleToken = TOKEN_MODULUS;
      break;

    // comparitive tokens
    case '!':
      if (source[c + 1] == '=') {
        simpleToken = TOKEN_IS_NOT_EQUAL;
        c++;
      }
      break;
    case '>':
      simpleToken = TOKEN_IS_GREATER_THAN;
      if (source[c + 1] == '=') {
        simpleToken = TOKEN_IS_GREATER_THAN_OR_EQUAL;
        c++;
      }
    case '<':
      simpleToken = TOKEN_IS_LESS_THAN;
      if (source[c + 1] == '=') {
        simpleToken = TOKEN_IS_LESS_THAN_OR_EQUAL;
        c++;
      }
    }
    // if it dose not have it's default value we know that we have found one of
    // our simple tokens so we will need to append it to the token array
    if (simpleToken != TOKEN_IDENTIFIER) {
      region.end = c;
      token.type = simpleToken;
      token.region = region;
      appendToken(tokenCount, &tokens, token);
      c++;
      continue;
    }

    // String token
    if (source[c] == '"') {
      token.type = TOKEN_STRING;
      token.value.string = NULL;

      for (c++; source[c] != '"'; c++) {
        // append to the string
        size_t oldStringLength = 0;
        if (token.value.string != NULL) {
          oldStringLength = strlen(token.value.string);
        }
        char *newString =
            realloc(token.value.string, sizeof(char) * (oldStringLength + 2));
        newString[oldStringLength + 1] = '\0';
        newString[oldStringLength] = source[c];
        token.value.string = newString;
      }

      region.end = c;
      token.region = region;
      appendToken(tokenCount, &tokens, token);
      c++;
      continue;
    }

    // Identifier / Keyword token
    if (isalpha(source[c])) {
      token.type = TOKEN_IDENTIFIER;
      token.value.identifier = NULL;

      for (; isalpha(source[c]) || isdigit(source[c]); c++) {
        // append to the string
        size_t oldStringLength = 0;
        if (token.value.identifier != NULL) {
          oldStringLength = strlen(token.value.identifier);
        }
        char *newString = realloc(token.value.identifier,
                                  sizeof(char) * (oldStringLength + 2));
        newString[oldStringLength + 1] = '\0';
        newString[oldStringLength] = source[c];
        token.value.identifier = newString;
      }

      region.end = c - 1;
      token.region = region;

      if (strcmp(token.value.identifier, "mut") == 0) {
        token.type = TOKEN_KEYWORD_MUT;
      } else if (strcmp(token.value.identifier, "let") == 0) {
        token.type = TOKEN_KEYWORD_LET;
      } else if (strcmp(token.value.identifier, "fun") == 0) {
        token.type = TOKEN_KEYWORD_FUN;
      } else if (strcmp(token.value.identifier, "return") == 0) {
        token.type = TOKEN_KEYWORD_RETUN;
      } else if (strcmp(token.value.identifier, "if") == 0) {
        token.type = TOKEN_KEYWORD_IF;
      } else if (strcmp(token.value.identifier, "for") == 0) {
        token.type = TOKEN_KEYWORD_FOR;
      } else if (strcmp(token.value.identifier, "while") == 0) {
        token.type = TOKEN_KEYWORD_WHILE;
      } else if (strcmp(token.value.identifier, "true") == 0) {
        token.type = TOKEN_BOOL;
        free(token.value.identifier);
        token.value.boolean = true;
        appendToken(tokenCount, &tokens, token);
        continue;
      } else if (strcmp(token.value.identifier, "false") == 0) {
        token.type = TOKEN_BOOL;
        free(token.value.identifier);
        token.value.boolean = false;
        appendToken(tokenCount, &tokens, token);
        continue;
      }

      // if we found a keyword
      if (token.type != TOKEN_IDENTIFIER) {
        free(token.value.identifier);
        token.value.identifier = NULL; // prevent use after free
      }

      appendToken(tokenCount, &tokens, token);
      continue;
    }

    // Int token
    if (isdigit(source[c]) || source[c] == '-') {
      token.type = TOKEN_INT;
      token.value.intVal = 0;

      // check if it is a negative integer
      bool negative = false;
      if (source[c] == '-') {
        negative = true;
        c++;
      }

      for (; isdigit(source[c]) || source[c] == '_'; c++) {
        if (source[c] == '_') {
          continue;
        }
        // convert the char to int64_t
        int64_t charInt = source[c] - '0';

        token.value.intVal = token.value.intVal * 10 + charInt;
      }

      if (negative) {
        token.value.intVal *= -1;
      }

      region.end = c;
      token.region = region;

      appendToken(tokenCount, &tokens, token);
      continue;
    }

    // slogLocation(LEVEL_ERROR, STAGE_LEXING, source, iToLocation(source, c),
    // "skip");
    char buffer[46];
    sprintf(buffer, "Unexpected character found during lexing: %c", source[c]);
    slogLocation(source, c, ERROR, buffer);
    exit(1);
    c++;
  }

  return tokens;
}

// char *tokenKindToString(TokenKind kind) {
//   switch (kind) {
//   }
// }
//

// void printToken(const Token token) {
//   switch (token.type) {
//   case TOKEN_INT:
//     printf("int");
//     break;
//   case TOKEN_STRING:
//     printf("string");
//     break;
//   case TOKEN_IDENTIFIER:
//     printf("identifier");
//     break;
//   case TOKEN_OPEN_PAREN:
//     printf("open paren (");
//     break;
//   case TOKEN_CLOSE_PAREN:
//     printf("close paren )");
//     break;
//   case TOKEN_OPEN_BRACE:
//     printf("open brace {");
//     break;
//   case TOKEN_CLOSE_BRACE:
//     printf("close brace }");
//     break;
//   case TOKEN_EQUAL_SIGN:
//     printf("equal sign =");
//     break;
//   case TOKEN_KEYWORD_RETUN:
//     printf("return keyword");
//     break;
//   case TOKEN_KEYWORD_LET:
//     printf("let keyword");
//     break;
//   case TOKEN_KEYWORD_MUT:
//     printf("mut keyword");
//     break;
//   case TOKEN_KEYWORD_FUN:
//     printf("fun keyword");
//     break;
//   }

//   printf(" --- ");

//   // to find which kind of value it is we need to check the kind
//   switch (token.type) {
//   case TOKEN_INT:
//     printf("%" PRId64, token.value.intVal);
//     break;
//   case TOKEN_STRING:
//     printf("%s", token.value.string);
//     break;
//   case TOKEN_IDENTIFIER:
//     printf("%s", token.value.identifier);
//     break;
//   default:
//     printf("none");
//   }

//   printf("\n");
// }

// void printTokens(const Token *tokens, const size_t tokenCount) {
//   for (size_t i = 0; i < tokenCount; i++) {
//     printToken(tokens[i]);
//   }
// }
