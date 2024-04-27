package main

import (
	"fmt"
	"unicode"
)

type TokenKind uint8

const (
	_type TokenKind = iota
	identifier
	tokenKindString
	keyword
	openBrace
	closeBrace
	openParentheses
	closeParentheses
	semiColon
	equalSign
)

var (
	keywords = [...]string{"var", "defn"}
)

type Token struct {
	kind  TokenKind
	value string
}

func newToken(kind TokenKind, value string) Token {
	return Token{
		kind:  kind,
		value: value,
	}
}

func (t Token) String() string {
	return fmt.Sprintf("Kind: %d, Value: %s", t.kind, string(t.value))
}

func isIdentifiable(r rune) bool {
	if unicode.IsSpace(r) || r == '(' || r == ')' || r == '{' || r == '}' || r == '"' || r == '#' || r == '=' {
		return false
	}

	return true
}

func tokenize(source string) []Token {
	// fmt.Println(isIdentifiable('('))
	var (
		tokens []Token
		kind   TokenKind
		value  string
	)

	for i := 0; i < len(source); i++ {
		value = ""

		// comments
		if source[i] == '#' {
			// continue to the next line
			for ; i < len(source); i++ {
				// a comment should end at a new line
				if source[i] == '\n' {
					break
				}
			}
		} else if source[i] == '{' {
			tokens = append(tokens, newToken(openBrace, "{"))
		} else if source[i] == '}' {
			tokens = append(tokens, newToken(closeBrace, "}"))
		} else if source[i] == '(' {
			tokens = append(tokens, newToken(openParentheses, "("))
		} else if source[i] == ')' {
			tokens = append(tokens, newToken(closeParentheses, ")"))
		} else if source[i] == ';' {
			tokens = append(tokens, newToken(semiColon, ";"))
		} else if source[i] == '=' {
			tokens = append(tokens, newToken(equalSign, "="))
		} else if source[i] == '"' {
			// String

			kind = tokenKindString
			i++
			for ; source[i] != '"'; i++ {
				value = value + string(source[i])
			}
			tokens = append(tokens, newToken(kind, value))
		} else if unicode.IsLetter(rune(source[i])) {
			// Identifiers and some other stuff

			for ; isIdentifiable(rune(source[i])); i++ {
				value = value + string(source[i])
			}

			// check if it is a keyword
			isKeyword := false
			for _, keyword := range keywords {
				if keyword == string(value) {
					isKeyword = true
					break
				}
			}

			if isKeyword {
				kind = keyword
			} else if unicode.IsUpper(rune(value[0])) {
				kind = _type
			} else {
				kind = identifier
			}

			tokens = append(tokens, newToken(kind, value))
			i--
		}

	}

	return tokens
}
