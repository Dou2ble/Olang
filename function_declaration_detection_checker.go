package main

var (
	blockLevel uint = 0
	functions  []FunctionDeclaration
)

func checkFunctionDeclaration(statement FunctionDeclaration) error {
	if blockLevel != 0 {
		message := "Function declaration must be a top level statement"
		logCode(LevelError, statement.start, statement.end, message)
		return myError(message)
	}

	functions = append(functions, statement)
	checkBlockStatement(statement.body)

	return nil
}

func checkWhileStatement(statement WhileStatement) error {
	return checkBlockStatement(statement.body)
}

func checkIfStatement(statement IfStatement) error {
	err := checkBlockStatement(statement.body)
	if err != nil {
		return err
	}

	if statement._else != nil {
		// the parser already makes sure that the _else statement of right statement type
		return checkStatement(statement)
	}

	return nil
}

func checkBlockStatement(statement BlockStatement) error {
	blockLevel++

	// loop over the content of the block statement
	for i := 0; i < len(statement.content); i++ {
		err := checkStatement(statement.content[i])
		if err != nil {
			return err
		}
	}

	blockLevel--
	return nil
}

func checkStatement(statement Statement) error {
	switch statement := statement.(type) {
	case FunctionDeclaration:
		return checkFunctionDeclaration(statement)
	case BlockStatement:
		return checkBlockStatement(statement)
	case IfStatement:
		return checkIfStatement(statement)
	case WhileStatement:
		return checkWhileStatement(statement)
	}

	return nil
}

func check(path string) error {
	ast, err := parse(path)
	if err != nil {
		return err
	}

	// loop over the content of the abstract syntax tree
	for i := 0; i < len(ast.content); i++ {
		err := checkStatement(ast.content[i])
		if err != nil {
			return err
		}
	}

	return nil
}
