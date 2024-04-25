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
	value []rune
}

func newToken(kind TokenKind, value []rune) Token {
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

func tokenize(source []rune) []Token {
	// fmt.Println(isIdentifiable('('))
	var (
		tokens []Token
		kind   TokenKind
		value  []rune
	)

	for i := 0; i < len(source); i++ {
		value = []rune{}

		// comments
		if source[i] == '#' {
			// continue to the next line
			for ; source[i] != '\n'; i++ {
			}
		}

		// braces
		if source[i] == '{' {
			tokens = append(tokens, newToken(openBrace, []rune{'{'}))
		} else if source[i] == '}' {
			tokens = append(tokens, newToken(closeBrace, []rune{'}'}))
		}

		// Parentheses
		if source[i] == '(' {
			tokens = append(tokens, newToken(openParentheses, []rune{'('}))
		} else if source[i] == ')' {
			tokens = append(tokens, newToken(closeParentheses, []rune{')'}))
		}

		// other single character tokens
		if source[i] == ';' {
			tokens = append(tokens, newToken(semiColon, []rune{';'}))
		} else if source[i] == '=' {
			tokens = append(tokens, newToken(equalSign, []rune{'='}))
		}

		// strings
		if source[i] == '"' {
			kind = tokenKindString
			i++
			for ; source[i] != '"'; i++ {
				value = append(value, source[i])
			}
			tokens = append(tokens, newToken(kind, value))
			i++
		}

		// Types, Keywords and Identifiers
		if unicode.IsLetter(source[i]) {
			for ; isIdentifiable(source[i]); i++ {
				value = append(value, source[i])
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
			} else if unicode.IsUpper(value[0]) {
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
