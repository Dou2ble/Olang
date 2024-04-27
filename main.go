package main

//go:generate go run generate.go

import (
	"fmt"
	"io/ioutil"
	"log/slog"
	"os"
)

func main() {
	// read the source code
	sourceRaw, err := ioutil.ReadFile("source.plisp")
	if err != nil {
		slog.Error(fmt.Sprint(err))
		os.Exit(1)
	}
	source := string(sourceRaw)

	// tokenize
	tokens := tokenize(source)
	for _, token := range tokens {
		fmt.Println(token)
	}

	// parsing
	ast, err := parse(source)
	if err != nil {
		slog.Error(fmt.Sprint(err))
	}
	fmt.Println(ast)

	// compiling
	csource, err := compile(source)
	if err != nil {
		slog.Error(fmt.Sprint(err))
		os.Exit(1)
	}
	fmt.Println()
	fmt.Println("==> C SOURCE CODE")
	fmt.Println(csource)
}
