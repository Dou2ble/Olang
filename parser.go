package main

import (
	"errors"
	"strconv"
)

func parseEmptyArrayLiteralExpression(tokens []Token, i *int) (EmptyArrayLiteralExpression, error) {
	return EmptyArrayLiteralExpression{location: tokens[*i].start}, nil
}

func parseStringLiteralExpression(tokens []Token, i *int) (StringLiteralExpression, error) {
	result := StringLiteralExpression{}

	if tokens[*i].kind != tokenKindString {
		return result, errors.New("No string token found in string literal expression")
	}

	result.value = tokens[*i].value
	result.start = tokens[*i].start
	result.end = tokens[*i].end
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

	result.start = tokens[*i].start
	result.end = tokens[*i].end

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
	result.start = tokens[*i].start
	result.end = tokens[*i].end

	return result, nil
}

func parseCallExpression(tokens []Token, i *int) (CallExpression, error) {
	result := CallExpression{}

	if tokens[*i].kind != identifier {
		return result, errors.New("Expected identifier at call expression")
	}
	result.callee = tokens[*i].value
	result.start = tokens[*i].start
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
	result.end = tokens[*i].end

	return result, nil
}

func parseIdentifierExpression(tokens []Token, i *int) (IdentifierExpression, error) {
	result := IdentifierExpression{}

	if tokens[*i].kind != identifier {
		return result, errors.New("Expected identifier token at identifier expression")
	}
	result.id = tokens[*i].value
	result.start = tokens[*i].start
	result.end = tokens[*i].end

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
	result.start, result.end = expression.getLocationOfExpression()
	return result, nil
}

func parseIfStatement(tokens []Token, i *int) (IfStatement, error) {
	result := IfStatement{}

	if tokens[*i].kind != keyword || (tokens[*i].value != "if" && tokens[*i].value != "elif") {
		return result, errors.New("Expected if keyword at if statement")
	}
	result.start = tokens[*i].start
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

	if tokens[*i].kind == keyword && tokens[*i].value == "elif" {
		_else, err := parseIfStatement(tokens, i)
		if err != nil {
			return result, err
		}
		result._else = _else
	} else if tokens[*i].kind == keyword && tokens[*i].value == "else" {
		*i++
		_else, err := parseBlockStatement(tokens, i)
		if err != nil {
			return result, err
		}
		result._else = _else
	} else {
		*i--
		result._else = nil
	}

	result.end = tokens[*i].end
	return result, nil
}

func parseBlockStatement(tokens []Token, i *int) (BlockStatement, error) {
	result := BlockStatement{}

	if tokens[*i].kind != openBrace {
		return result, errors.New("Didn't find opening brace in block")
	}
	result.start = tokens[*i].start
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
	result.end = tokens[*i].end

	return result, nil
}

func parseFunctionDeclarationStatement(tokens []Token, i *int) (FunctionDeclaration, error) {
	result := FunctionDeclaration{}

	if tokens[*i].kind != keyword || string(tokens[*i].value) != "fn" {
		return result, errors.New("Didn't find var keyword")
	}
	result.start = tokens[*i].start
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
	result.end = body.end

	return result, nil
}

func parseAssignmentStatement(tokens []Token, i *int) (AssignmentStatement, error) {
	result := AssignmentStatement{}

	if tokens[*i].kind != identifier {
		return result, errors.New("didn't find any identifier in assignment statement")
	}
	result.id = tokens[*i].value
	result.start = tokens[*i].start

	*i++

	// check for the =
	if tokens[*i].kind != equalSign {
		return result, errors.New("didn't find any = after the variable name")
	}
	*i++

	// now parse an expression
	expression, err := parseExpression(tokens, i)
	if err != nil {
		return result, err
	}
	result.expr = expression
	_, result.end = expression.getLocationOfExpression()

	return result, nil
}

func parseVariableDeclarationStatement(tokens []Token, i *int) (VariableDeclaration, error) {
	result := VariableDeclaration{}

	if tokens[*i].kind != keyword || string(tokens[*i].value) != "var" {
		return result, errors.New("Didn't find var keyword")
	}
	result.start = tokens[*i].start
	*i++

	if tokens[*i].kind != identifier {
		return result, errors.New("didn't find any identifier after the var keyword")
	}
	result.id = tokens[*i].value
	*i++

	// check for the =
	if tokens[*i].kind != equalSign {
		return result, errors.New("didn't find any = after the declaration")
	}
	*i++

	// now parse an expression
	expression, err := parseExpression(tokens, i)
	if err != nil {
		return result, err
	}
	result.expr = expression
	result.end = tokens[*i].end

	return result, nil
}

func parseWhile(tokens []Token, i *int) (Statement, error) {
	result := WhileStatement{}

	if tokens[*i].kind != keyword || tokens[*i].value != "while" {
		return result, errors.New("Expected while keyword at while statement")
	}
	result.start = tokens[*i].start
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
	result.end = tokens[*i].end

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

func parse(source string, path string) (Program, error) {
	tokens := tokenize(source, path)
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
