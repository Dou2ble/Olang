package main

import (
	"errors"
)

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
	value string
}

func (e StringLiteralExpression) expression() {}

type CallExpression struct {
	callee string
	// in a function declaration this would be called parameters
	// but in a call expression these are called arguments
	// for some reason :shrug:
	arguments []Expression
}

func (e CallExpression) expression() {}

type ExpressionStatement struct {
	expr Expression
}

func (s ExpressionStatement) statement() {}

type BlockStatement struct {
	content []Statement
}

func (s BlockStatement) statement() {}

type FunctionDeclaration struct {
	id         string
	parameters []string
	body       BlockStatement
}

func (s FunctionDeclaration) statement() {}

type VariableDeclaration struct {
	id    string
	_type string
	expr  Expression
}

func (s VariableDeclaration) statement() {

}

func parseStringLiteralExpression(tokens []Token, i *int) (StringLiteralExpression, error) {
	result := StringLiteralExpression{}

	if tokens[*i].kind != tokenKindString {
		return result, errors.New("No string token found in string literal expression")
	}

	return StringLiteralExpression{
		value: tokens[*i].value,
	}, nil
}

func parseCallExpression(tokens []Token, i *int) (CallExpression, error) {
	result := CallExpression{}

	if tokens[*i].kind != identifier {
		return result, errors.New("Expected identifier at call expression")
	}
	result.callee = tokens[*i].value
	*i++

	if tokens[*i].kind != openParentheses {
		return result, errors.New("Expected identifier at call expression")
	}
	*i++

	var arguments []Expression
	for ; tokens[*i].kind != closeParentheses; *i++ {
		expression, err := parseExpression(tokens, i)
		if err != nil {
			return result, err
		}

		arguments = append(arguments, expression)
	}
	result.arguments = arguments

	return result, nil
}

func parseExpression(tokens []Token, i *int) (Expression, error) {
	if tokens[*i].kind == tokenKindString {
		return parseStringLiteralExpression(tokens, i)
	} else if tokens[*i].kind == identifier {
		// if there is an identifier i think it's fine to assume that the expression
		// is an call expression
		return parseCallExpression(tokens, i)
	}

	return StringLiteralExpression{}, errors.New("Failed to parse expression")
}

func parseExpressionStatement(tokens []Token, i *int) (ExpressionStatement, error) {
	result := ExpressionStatement{}

	expression, err := parseExpression(tokens, i)
	if err != nil {
		return result, err
	}

	result.expr = expression
	return result, nil
}

func parseBlockStatement(tokens []Token, i *int) (BlockStatement, error) {
	result := BlockStatement{}

	if tokens[*i].kind != openBrace {
		return result, errors.New("Didn't find opening brace in block")
	}
	*i++

	var content []Statement
	for ; tokens[*i].kind != closeBrace; *i++ {
		statement, err := parseStatement(tokens, i)
		if err != nil {
			return result, err
		}

		content = append(content, statement)
	}
	result.content = content

	return result, nil
}

func parseFunctionDeclarationStatement(tokens []Token, i *int) (FunctionDeclaration, error) {
	result := FunctionDeclaration{}

	if tokens[*i].kind != keyword || string(tokens[*i].value) != "fn" {
		return result, errors.New("Didn't find var keyword")
	}
	*i++

	if tokens[*i].kind != identifier {
		return result, errors.New("Didn't find function ID")
	}
	result.id = tokens[*i].value
	*i++

	if tokens[*i].kind != openParentheses {
		return result, errors.New("Didn't find function ID")
	}
	*i++

	var parameters []string
	for ; tokens[*i].kind != closeParentheses; *i++ {
		if tokens[*i].kind != identifier {
			return result, errors.New("Parameter is not of token kind identifier")
		}

		parameters = append(parameters, tokens[*i].value)
	}
	*i++
	result.parameters = parameters

	body, err := parseBlockStatement(tokens, i)
	if err != nil {
		return result, err
	}
	result.body = body

	return result, nil
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
		} else if string(tokens[*i].value) == "fn" {
			return parseFunctionDeclarationStatement(tokens, i)
		}
	} else if tokens[*i].kind == openBrace {
		return parseBlockStatement(tokens, i)
	}

	// when no normal statement is found we should try to fall back to a expression statement
	return parseExpressionStatement(tokens, i)
}

func parse(source string) (Program, error) {
	tokens := tokenize(source)
	i := 0

	program := Program{}

	for ; i < len(tokens); i++ {
		statement, err := parseStatement(tokens, &i)
		if err != nil {
			// NOTE: in the future this should result in an error but right
			// now this is fine because not all language features are supported yet
			// return program, err
			// continue

			return program, err
		}

		program.content = append(program.content, statement)
	}

	return program, nil
}
