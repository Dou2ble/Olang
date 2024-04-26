package main

import (
	"errors"
	"fmt"
	"io/ioutil"
	"log/slog"
	"os"
)

func compileStringLiteralExpression(expression StringLiteralExpression) (string, error) {
	return fmt.Sprintf("newPlispVariable(PlispVariableKindString, \"%s\")", expression.value), nil
}

func compileExpression(expression Expression) (string, error) {
	switch expression := expression.(type) {
	case StringLiteralExpression:
		return compileStringLiteralExpression(expression)
	}

	return "", errors.New("Unknown expression type")
}

func compileVariableDeclaration(statement VariableDeclaration) (string, error) {
	compiledExpression, err := compileExpression(statement.expr)
	if err != nil {
		return compiledExpression, err
	}

	return fmt.Sprintf("PlispVariable %s = %s", statement.id, compiledExpression), nil
}

func compileStatement(statement Statement) (string, error) {
	switch statement := statement.(type) {
	case VariableDeclaration:
		return compileVariableDeclaration(statement)
	}

	return "", errors.New("Unknown statement type")
}

// import "fmt"
func compile(source string) (string, error) {
	ast, err := parse(source)

	if err != nil {
		return "", err
	}

	// read the source code
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

	return cCode, nil
}
