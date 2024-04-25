package main

import "errors"

type Statement interface {
	statement()
}

type Program struct {
	content []Statement
}

type Expression interface {
	expression()
}

type StringLiteralExpression struct {
	value []rune
}

func (e StringLiteralExpression) expression() {}

type MemberExpression struct {
	property []rune
}

func (e MemberExpression) expression() {}

type CallExpression struct {
	callee    MemberExpression
	arguments []Expression
}

func (e CallExpression) expression() {}

type VariableDeclaration struct {
	id    []rune
	_type []rune
	expr  Expression
}

func (e VariableDeclaration) statement() {}

func parseExpression(tokens []Token, i *int) (Expression, error) {
	if tokens[*i].kind == tokenKindString {
		return StringLiteralExpression{
			value: tokens[*i].value,
		}, nil
	}

	return StringLiteralExpression{}, errors.New("Failed to parse expression")
}

func parseVariableDeclarationStatement(tokens []Token, i *int) (VariableDeclaration, error) {
	if tokens[*i].kind != keyword || string(tokens[*i].value) != "var" {
		return VariableDeclaration{}, errors.New("Didn't find var keyword")
	}
	*i++

	if tokens[*i].kind != identifier {
		return VariableDeclaration{}, errors.New("didn't find any identifier after the var keyword")
	}
	id := tokens[*i].value
	*i++

	// check for the type
	if tokens[*i].kind != _type {
		return VariableDeclaration{}, errors.New("didn't find the type for the variable after the variable name")
	}
	_type := tokens[*i].value
	*i++

	// check for the =
	if tokens[*i].kind != equalSign {
		return VariableDeclaration{}, errors.New("didn't find any = after the declaration")
	}
	*i++

	// now parse an expression
	expression, err := parseExpression(tokens, i)
	if err != nil {
		return VariableDeclaration{}, err
	}

	return VariableDeclaration{
		id:    id,
		_type: _type,
		expr:  expression,
	}, nil
}

func parseStatement(tokens []Token, i *int) (Statement, error) {
	if tokens[*i].kind == keyword {
		if string(tokens[*i].value) == "var" {
			return parseVariableDeclarationStatement(tokens, i)
		}
	}

	return VariableDeclaration{}, errors.New("No statement found")
}

func parse(source []rune) (Program, error) {
	tokens := tokenize(source)
	i := 0

	program := Program{}

	for ; i < len(tokens); i++ {
		statement, err := parseStatement(tokens, &i)
		if err != nil {
			return program, err
		}

		program.content = append(program.content, statement)
	}

	return program, nil
}
