#include "../include/lexer.h"
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

int isspecial(char c, const char *specials, size_t spc_size) {
  for (size_t i = 0; i < spc_size; i++) {
    if (strchr(specials, c) != NULL) {
      return true;
    }
  }
  return false;
}

Token new_token() {
  Token tok = {
    .type = TOKEN_END,
    .text = NULL,
    .size = 0,
  };
  return tok;
}

Lexer new_lex(char *str, size_t size) {
  Lexer lex = {0};
  lex.content_size = size;
  lex.content = str;
  return lex;
}
int trim(Lexer *lex) {
  while (!token_stream_end(lex) && isspace(at_cursor(lex))) {
    if (at_cursor(lex) == '\n') {
      lex->line++;
      lex->line = lex->cursor;
    }
    lex->cursor += 1;
  }
  // if end 1, else 0
  return token_stream_end(lex) ? true : false;
}
Token next_token(Lexer *lex) {
  int string_mode = false;
  Token tok = new_token();
  if (lex->cursor >= lex->content_size) {
    tok.type = TOKEN_END;
    tok.size = 0;
    tok.text = NULL;
    return tok;
  }
  // trim(lex);
  tok.text = &at_cursor(lex);
  tok.size = 0;
  tok.type = TOKEN_SYMBOL;
  truncate_token(lex, '(', tok, TOKEN_OPEN_PAREN, string_mode);
  truncate_token(lex, ')', tok, TOKEN_CLOSE_PAREN, string_mode);
  truncate_token(lex, '-', tok, TOKEN_SUB, string_mode);
  truncate_token(lex, '+', tok, TOKEN_ADD, string_mode);
  truncate_token(lex, '/', tok, TOKEN_DIV, string_mode);
  truncate_token(lex, '*', tok, TOKEN_MUL, string_mode);
  truncate_token(lex, '[', tok, TOKEN_OPEN_SQUARE_BRACKET, string_mode);
  truncate_token(lex, ']', tok, TOKEN_CLOSE_SQUARE_BRACKET, string_mode);
  truncate_token(lex, ',', tok, TOKEN_COMMA, string_mode);
  truncate_token(lex, '.', tok, TOKEN_DOT, string_mode);
  truncate_token(lex, ':', tok, TOKEN_COLON, string_mode);
  truncate_token(lex, ';', tok, TOKEN_SEMICOLON, string_mode);
  truncate_token(lex, '=', tok, TOKEN_EQUAL_MARK, string_mode);
  truncate_token(lex, '#', tok, TOKEN_HASH, string_mode);
  trim(lex);
  while (!token_stream_end(lex)) {
    if (string_mode == false && isspace(at_cursor(lex))) {
      break;
    }
    if (isspecial(at_cursor(lex), special_chars, special_chars_size) && string_mode == false) {
      if (tok.size != 0) {
	return tok;
      }
    }
    if (isdigit(at_cursor(lex)) && tok.size == 0) {
      tok.type = TOKEN_LITERAL_NUMBER;
    }
    if (at_cursor(lex) == '"' && tok.size == 0) {
      string_mode = true;
      tok.type = TOKEN_LITERAL_STRING;
      lex->cursor += 1;
      tok.size += 1;
      continue;
    }
    if (tok.type == TOKEN_LITERAL_STRING && at_cursor(lex) == '"') {
      lex->cursor += 1;
      tok.size += 2;
      return tok;
    }
    if (tok.type == TOKEN_LITERAL_NUMBER && !isdigit(at_cursor(lex))) {
      return tok;
    }
    tok.size += 1;
    // tok.type = TOKEN_SYMBOL;
    lex->cursor += 1;
  }
  return tok;
}
char *TokenType_to_str(TokenType ty) {
  switch (ty) {
  case TOKEN_OPEN_PAREN:
    return "open paren";
  case TOKEN_CLOSE_PAREN:
    return "close paren";
  case TOKEN_SYMBOL:
    return "symbol";
  case TOKEN_END:
    return "end";
  case TOKEN_LITERAL_NUMBER:
    return "number";
  case TOKEN_LITERAL_STRING:
    return "literal string";
  case TOKEN_SUB:
    return "sub";
  case TOKEN_ADD:
    return "add";
  case TOKEN_DIV:
    return "div";
  case TOKEN_MUL:
    return "mul";
  case TOKEN_OPEN_SQUARE_BRACKET:
    return "open square bracket";
  case TOKEN_CLOSE_SQUARE_BRACKET:
    return "close square bracket";
  case TOKEN_COMMA:
    return "comma";
  case TOKEN_DOT:
    return "dot";
  case TOKEN_COLON:
    return "colon";
  case TOKEN_SEMICOLON:
    return "semicolon";
  case TOKEN_CHAR_LITERAL:
    return "char literal";
  case TOKEN_EQUAL_MARK:
    return "equal mark";
  case TOKEN_HASH:
    return "hash";
  case TOKEN_TAB:
    return "TAB";
  default:
    fprintf(stderr, "ERROR=>TokenType_to_str: invalid token found\n");
    exit(EXIT_FAILURE);
  }
}
