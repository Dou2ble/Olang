package main

// Program
type Program struct {
	content []Statement
}

// Interfaces
type Expression interface {
	getLocationOfExpression() (Location, Location)
}

type Statement interface {
	getLocationOfStatement() (Location, Location)
}

// Expressions
type BooleanLiteralExpression struct {
	value bool
	start Location
	end   Location
}

func (e BooleanLiteralExpression) getLocationOfExpression() (Location, Location) {
	return e.start, e.end
}

type CallExpression struct {
	callee    string
	arguments []Expression
	start     Location
	end       Location
}

func (e CallExpression) getLocationOfExpression() (Location, Location) {
	return e.start, e.end
}

type EmptyArrayLiteralExpression struct {
	location Location
}

func (e EmptyArrayLiteralExpression) getLocationOfExpression() (Location, Location) {
	return e.location, e.location
}

type IdentifierExpression struct {
	id    string
	start Location
	end   Location
}

func (e IdentifierExpression) getLocationOfExpression() (Location, Location) {
	return e.start, e.end
}

type IntegerLiteralExpression struct {
	value int64
	start Location
	end   Location
}

func (e IntegerLiteralExpression) getLocationOfExpression() (Location, Location) {
	return e.start, e.end
}

type StringLiteralExpression struct {
	value string
	start Location
	end   Location
}

func (e StringLiteralExpression) getLocationOfExpression() (Location, Location) {
	return e.start, e.end
}

type AssignmentStatement struct {
	id    string
	expr  Expression
	start Location
	end   Location
}

func (s AssignmentStatement) getLocationOfStatement() (Location, Location) {
	return s.start, s.end
}

type BlockStatement struct {
	content []Statement
	start   Location
	end     Location
}

func (s BlockStatement) getLocationOfStatement() (Location, Location) {
	return s.start, s.end
}

// Statements
type ExpressionStatement struct {
	expr  Expression
	start Location
	end   Location
}

func (s ExpressionStatement) getLocationOfStatement() (Location, Location) {
	return s.start, s.end
}

type FunctionDeclaration struct {
	id         string
	parameters []string
	body       BlockStatement
	start      Location
	end        Location
}

func (s FunctionDeclaration) getLocationOfStatement() (Location, Location) {
	return s.start, s.end
}

type IfStatement struct {
	condition Expression
	body      BlockStatement
	_else     Statement
	start     Location
	end       Location
}

func (s IfStatement) getLocationOfStatement() (Location, Location) {
	return s.start, s.end
}

type VariableDeclaration struct {
	id    string
	expr  Expression
	start Location
	end   Location
}

func (s VariableDeclaration) getLocationOfStatement() (Location, Location) {
	return s.start, s.end
}

type WhileStatement struct {
	condition Expression
	body      BlockStatement
	start     Location
	end       Location
}

func (s WhileStatement) getLocationOfStatement() (Location, Location) {
	return s.start, s.end
}
