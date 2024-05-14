package main

// Program
type Program struct {
	content   []Statement
	functions []FunctionDeclaration
}

// Interfaces
type Expression interface {
	getLocationOfExpression() (Location, Location)
	getType() Type
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

func (e BooleanLiteralExpression) getType() Type {
	return newType("Bool")
}

type CallExpression struct {
	callee    string
	arguments []Expression
	_type     Type
	start     Location
	end       Location
}

func (e CallExpression) getLocationOfExpression() (Location, Location) {
	return e.start, e.end
}

type EmptyArrayLiteralExpression struct {
	_type    Type
	location Location
}

func (e EmptyArrayLiteralExpression) getLocationOfExpression() (Location, Location) {
	return e.location, e.location
}

type IdentifierExpression struct {
	id    string
	_type Type
	start Location
	end   Location
}

func (e IdentifierExpression) getLocationOfExpression() (Location, Location) {
	return e.start, e.end
}

type IntegerLiteralExpression struct {
	value int64
	_type Type
	start Location
	end   Location
}

func (e IntegerLiteralExpression) getLocationOfExpression() (Location, Location) {
	return e.start, e.end
}

type StringLiteralExpression struct {
	value string
	_type Type
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
	parameters []Parameter
	returnType *string
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
	_type Type
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

// misc
type Parameter struct {
	id    string
	_type Type
	start Location
	end   Location
}

type Type struct {
	id string
}

func newType(id string) Type {
	return Type{
		id: id,
	}
}
