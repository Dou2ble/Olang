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
	Integer
	pipe
)

var (
	keywords = [...]string{"var", "while", "if", "elif", "else", "fn", "true", "false"}
)

type Location struct {
	path      string
	line      int
	character int
}

type Token struct {
	kind  TokenKind
	value string
	start Location
	end   Location
}

func newToken(kind TokenKind, value string, start Location, end Location) Token {
	return Token{
		kind:  kind,
		value: value,
		start: start,
		end:   end,
	}
}

func (t Token) String() string {
	return fmt.Sprintf("%s", t.value)
}

func isIdentifiable(r rune) bool {
	if unicode.IsSpace(r) || r == '(' || r == ')' || r == '{' || r == '}' || r == '"' || r == '#' || r == '=' {
		return false
	}

	return true
}

func findLocation(source string, path string, index int) Location {
	character := 1
	line := 1

	for i := 0; i < index; i++ {
		character++

		if source[i] == '\n' {
			character = 1
			line++
		}
	}

	return Location{
		line:      line,
		character: character,
		path:      path,
	}

}

func tokenize(path string) []Token {
	// fmt.Println(isIdentifiable('('))
	var (
		tokens []Token
		kind   TokenKind
		value  string
	)

	for i := 0; i < len(source); i++ {
		value = ""
		startLocation := findLocation(source, path, i)

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
			tokens = append(tokens, newToken(openBrace, "{", startLocation, startLocation))
		} else if source[i] == '}' {
			tokens = append(tokens, newToken(closeBrace, "}", startLocation, startLocation))
		} else if source[i] == '(' {
			tokens = append(tokens, newToken(openParentheses, "(", startLocation, startLocation))
		} else if source[i] == ')' {
			tokens = append(tokens, newToken(closeParentheses, ")", startLocation, startLocation))
		} else if source[i] == ';' {
			tokens = append(tokens, newToken(semiColon, ";", startLocation, startLocation))
		} else if source[i] == '=' {
			tokens = append(tokens, newToken(equalSign, "=", startLocation, startLocation))
		} else if source[i] == '|' {
			tokens = append(tokens, newToken(pipe, "|", startLocation, startLocation))
		} else if source[i] == '"' {
			// String

			kind = tokenKindString
			i++
			for ; source[i] != '"'; i++ {
				value = value + string(source[i])
			}
			endLocation := findLocation(source, path, i)
			tokens = append(tokens, newToken(kind, value, startLocation, endLocation))
		} else if unicode.IsDigit(rune(source[i])) {
			kind = Integer
			for ; unicode.IsDigit(rune(source[i])); i++ {
				value = value + string(source[i])
			}
			i--
			endLocation := findLocation(source, path, i)
			tokens = append(tokens, newToken(kind, value, startLocation, endLocation))
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

			i--
			endLocation := findLocation(source, path, i)
			tokens = append(tokens, newToken(kind, value, startLocation, endLocation))
		}

	}

	return tokens
}
