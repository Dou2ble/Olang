package main

var (
	variables             []VariableDeclaration
	typeCheckerBlockLevel uint
)

func typeCheckExpression(expression *Expression) error {
	switch expression := (*expression).(type) {
	case BooleanLiteralExpression:
		expression._type = newType("Bool")
	case IntegerLiteralExpression:
		expression._type = newType("Int")
	case StringLiteralExpression:
		expression._type = newType("Str")
	}

	start, end := (*expression).getLocationOfExpression()
	message := "Unknown expression type"
	logCode(LevelError, start, end, message)
	return myError(message)
}

// func typeCheckAssignmentStatement(statement *AssignmentStatement) error {
// 	typeCheckExpression(&statement.expr)
//
// 	statement.
// }

func typeCheckVariableDeclaration(statement *VariableDeclaration) error {
	typeCheckExpression(&statement.expr)
	statement._type
}

func typeCheckStatement(statement *Statement) error {
	switch statement := (*statement).(type) {
	// Complex cases
	case AssignmentStatement:
		return typeCheckAssignmentStatement(&statement)
	case VariableDeclaration:
		return typeCheckVariableDeclaration(&statement)
	case FunctionDeclaration:
		return typeCheckFunctionDeclaration(&statement)

	// Easy cases
	case ExpressionStatement:
		return typeCheckExpression(&statement.expr)
	case IfStatement:
		return typeCheckBlockStatement(&statement.body)
	case WhileStatement:
		return typeCheckBlockStatement(&statement.body)
	}

	// not all types of statements need type checking so it is fine if we reach the end
	return nil
}

func typeCheck(path string) (Program, error) {
	ast, err := functionCheck(path)
	if err != nil {
		return ast, err
	}

	variables = []VariableDeclaration{}
	typeCheckerBlockLevel = 0

	// loop over the content of the abstract syntax tree
	for i := 0; i < len(ast.content); i++ {
		err := typeCheckStatement(&ast.content[i])
		if err != nil {
			return ast, err
		}
	}

	return ast, nil
}
