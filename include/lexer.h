#ifndef _LEXER_H
#define _LEXER_H
#include "defs_utils.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  TOKEN_END = 0,
  TOKEN_OPEN_PAREN,
  TOKEN_CLOSE_PAREN,
  TOKEN_SYMBOL,
  TOKEN_LITERAL_STRING,
  TOKEN_LITERAL_NUMBER,
  TOKEN_SUB,
  TOKEN_ADD,
  TOKEN_DIV,
  TOKEN_MUL,
  TOKEN_OPEN_SQUARE_BRACKET,
  TOKEN_CLOSE_SQUARE_BRACKET,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_COLON,
  TOKEN_SEMICOLON,
  TOKEN_CHAR_LITERAL,
  TOKEN_EQUAL_MARK,
  TOKEN_HASH,
  TOKEN_TAB
} TokenType;

typedef struct {
  TokenType type;
  const char *text;
  size_t size;
} Token;

#define COL(lex) lex->line_start - lex->cursor
#define at_cursor(lex) lex->content[lex->cursor]
#define token_stream_end(lex) (lex->cursor >= lex->content_size)

#define truncate_token(lex, c, tok, ty, string_mode)			\
  if (at_cursor(lex) == c && string_mode == false) {			\
    if (tok.size != 0) {						\
      return tok;							\
    }									\
    tok.type = ty;							\
    tok.size = 1;							\
    tok.text = &at_cursor(lex);						\
    lex->cursor += 1;							\
    return tok;								\
  }

typedef struct {
  const char *content;
  size_t content_size, line, line_start, cursor;
} Lexer;

Token new_token();
Lexer new_lex(char *str, size_t size);
Token next_token(Lexer *lex);
int trim(Lexer *lex);
char *TokenType_to_str(TokenType ty);

int isspecial(char s, const char *spc, size_t spc_size);
static const char special_chars[] = "()-+/*[],.:;=";
static const size_t special_chars_size = sizeof(special_chars);
#endif //_LEXER_H
