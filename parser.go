package main

import (
	"errors"
	"strconv"
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

type EmptyArrayLiteralExpression struct{}

func (e EmptyArrayLiteralExpression) expression() {}

type BooleanLiteralExpression struct {
	value bool
}

func (e BooleanLiteralExpression) expression() {}

type IntegerLiteralExpression struct {
	value int64
}

func (e IntegerLiteralExpression) expression() {}

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

type IfStatement struct {
	condition Expression
	body      BlockStatement
	_else     Statement
}

func (e IfStatement) statement() {}

type WhileStatement struct {
	condition Expression
	body      BlockStatement
}

func (e WhileStatement) statement() {}

type BlockStatement struct {
	content []Statement
}

func (s BlockStatement) statement() {}

type IdentifierExpression struct {
	id string
}

func (e IdentifierExpression) expression() {}

type AssignmentStatement struct {
	id   string
	expr Expression
}

func (s AssignmentStatement) statement() {}

type FunctionDeclaration struct {
	id         string
	parameters []string
	body       BlockStatement
}

func (s FunctionDeclaration) statement() {}

type VariableDeclaration struct {
	id   string
	expr Expression
}

func (s VariableDeclaration) statement() {

}

func parseEmptyArrayLiteralExpression(tokens []Token, i *int) (EmptyArrayLiteralExpression, error) {
	return EmptyArrayLiteralExpression{}, nil
}

func parseStringLiteralExpression(tokens []Token, i *int) (StringLiteralExpression, error) {
	result := StringLiteralExpression{}

	if tokens[*i].kind != tokenKindString {
		return result, errors.New("No string token found in string literal expression")
	}

	result.value = tokens[*i].value
	return result, nil
}

func parseBooleanLiteralExpression(tokens []Token, i *int) (BooleanLiteralExpression, error) {
	result := BooleanLiteralExpression{}

	if tokens[*i].kind != keyword {
		return result, errors.New("No keyword token found in boolean literal expression")
	}

	if tokens[*i].value == "true" {
		result.value = true
	} else if tokens[*i].value == "false" {
		result.value = false
	} else {
		return result, errors.New("Keyword in boolean literal expression is not true or false")
	}

	return result, nil
}

func parseIntegerLiteralExpression(tokens []Token, i *int) (IntegerLiteralExpression, error) {
	result := IntegerLiteralExpression{}

	if tokens[*i].kind != Integer {
		return result, errors.New("No integer token found in integer literal expression")
	}

	num, err := strconv.ParseInt(tokens[*i].value, 10, 64)
	if err != nil {
		return result, err
	}

	result.value = num

	// *i++

	return result, nil
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

func parseIdentifierExpression(tokens []Token, i *int) (IdentifierExpression, error) {
	result := IdentifierExpression{}

	if tokens[*i].kind != identifier {
		return result, errors.New("Expected identifier token at identifier expression")
	}
	result.id = tokens[*i].value

	return result, nil
}

func parseExpression(tokens []Token, i *int) (Expression, error) {
	if tokens[*i].kind == keyword {
		if tokens[*i].value == "true" || tokens[*i].value == "false" {
			return parseBooleanLiteralExpression(tokens, i)
		}
	} else if tokens[*i].kind == pipe {
		return parseEmptyArrayLiteralExpression(tokens, i)
	} else if tokens[*i].kind == tokenKindString {
		return parseStringLiteralExpression(tokens, i)
	} else if tokens[*i].kind == Integer {
		return parseIntegerLiteralExpression(tokens, i)
	} else if tokens[*i].kind == identifier {
		if tokens[*i+1].kind == openParentheses {
			return parseCallExpression(tokens, i)
		}

		return parseIdentifierExpression(tokens, i)
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

func parseIfStatement(tokens []Token, i *int) (IfStatement, error) {
	result := IfStatement{}

	if tokens[*i].kind != keyword || (tokens[*i].value != "if" && tokens[*i].value != "elif") {
		return result, errors.New("Expected if keyword at if statement")
	}
	*i++

	expression, err := parseExpression(tokens, i)
	if err != nil {
		return result, err
	}
	result.condition = expression
	*i++

	// fmt.Println("Expression: ", tokens[*i])

	block, err := parseBlockStatement(tokens, i)
	if err != nil {
		return result, err
	}
	result.body = block
	*i++

	if tokens[*i].kind == keyword {
		if tokens[*i].value == "elif" {
			_else, err := parseIfStatement(tokens, i)
			if err != nil {
				return result, err
			}
			result._else = _else
		} else if tokens[*i].value == "else" {
			*i++
			_else, err := parseBlockStatement(tokens, i)
			if err != nil {
				return result, err
			}
			result._else = _else
		}
	} else {
		*i--
		result._else = nil
	}

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

func parseAssignmentStatement(tokens []Token, i *int) (AssignmentStatement, error) {
	if tokens[*i].kind != identifier {
		return AssignmentStatement{}, errors.New("didn't find any identifier in assignment statement")
	}
	id := tokens[*i].value
	*i++

	// check for the =
	if tokens[*i].kind != equalSign {
		return AssignmentStatement{}, errors.New("didn't find any = after the variable name")
	}
	*i++

	// now parse an expression
	expression, err := parseExpression(tokens, i)
	if err != nil {
		return AssignmentStatement{}, err
	}

	return AssignmentStatement{
		id:   id,
		expr: expression,
	}, nil
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
		id:   id,
		expr: expression,
	}, nil
}

func parseWhile(tokens []Token, i *int) (Statement, error) {
	result := WhileStatement{}

	if tokens[*i].kind != keyword || tokens[*i].value != "while" {
		return result, errors.New("Expected while keyword at while statement")
	}
	*i++

	expression, err := parseExpression(tokens, i)
	if err != nil {
		return result, err
	}
	result.condition = expression
	*i++

	block, err := parseBlockStatement(tokens, i)
	if err != nil {
		return result, err
	}
	result.body = block

	return result, nil
}

func parseStatement(tokens []Token, i *int) (Statement, error) {
	if tokens[*i].kind == keyword {
		if tokens[*i].value == "var" {
			return parseVariableDeclarationStatement(tokens, i)
		} else if tokens[*i].value == "fn" {
			return parseFunctionDeclarationStatement(tokens, i)
		} else if tokens[*i].value == "if" {
			return parseIfStatement(tokens, i)
		} else if tokens[*i].value == "while" {
			return parseWhile(tokens, i)
		}
	} else if tokens[*i].kind == openBrace {
		return parseBlockStatement(tokens, i)
	} else if tokens[*i].kind == identifier && tokens[*i+1].kind == equalSign {
		return parseAssignmentStatement(tokens, i)
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
