package main

import (
	"errors"
	"fmt"
	"io/ioutil"
	"log/slog"
	"os"
	"strings"
)

func compileStringLiteralExpression(expression StringLiteralExpression) (string, error) {
	return fmt.Sprintf("newPlispVariable(PlispVariableKindString, \"%s\")", expression.value), nil
}

func compileCallExperssion(expression CallExpression) (string, error) {
	var compiledArguments []string
	for _, argument := range expression.arguments {
		compiledArgument, err := compileExpression(argument)
		if err != nil {
			return "", err
		}

		compiledArguments = append(compiledArguments, compiledArgument)
	}

	argumentsString := strings.Join(compiledArguments, ", ")

	return fmt.Sprintf("function_%s(%s)", expression.callee, argumentsString), nil
}

func compileExpression(expression Expression) (string, error) {
	switch expression := expression.(type) {
	case StringLiteralExpression:
		return compileStringLiteralExpression(expression)
	case CallExpression:
		return compileCallExperssion(expression)
	}

	return "", errors.New("Unknown expression type")
}

func compileVariableDeclaration(statement VariableDeclaration) (string, error) {
	compiledExpression, err := compileExpression(statement.expr)
	if err != nil {
		return compiledExpression, err
	}

	return fmt.Sprintf("PlispVariable *variable_%s = %s;\n", statement.id, compiledExpression), nil
}

func compileExpressionStatement(statement ExpressionStatement) (string, error) {
	compiledExpression, err := compileExpression(statement.expr)
	if err != nil {
		return "", err
	}

	return fmt.Sprintf("%s;\n", compiledExpression), nil
}

func compileStatement(statement Statement) (string, error) {
	switch statement := statement.(type) {
	case VariableDeclaration:
		return compileVariableDeclaration(statement)
	case ExpressionStatement:
		return compileExpressionStatement(statement)

	}

	return "", errors.New("Unknown statement type")
}

// import "fmt"
func compile(source string) (string, error) {
	ast, err := parse(source)

	if err != nil {
		return "", err
	}

	// read the header source code
	rawHeader, err := ioutil.ReadFile("header.c")
	if err != nil {
		slog.Error(fmt.Sprint(err))
		os.Exit(1)
	}
	cCode := string(rawHeader)

	// loop over the content of the abstract syntax tree
	for i := 0; i < len(ast.content); i++ {
		val, err := compileStatement(ast.content[i])
		cCode = cCode + val

		if err != nil {
			return cCode, err
		}

	}

	// read the footer source ocde
	rawFooter, err := ioutil.ReadFile("footer.c")
	if err != nil {
		slog.Error(fmt.Sprint(err))
		os.Exit(1)
	}
	cCode = cCode + string(rawFooter)

	return cCode, nil
}
