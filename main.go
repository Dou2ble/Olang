package main

//go:generate go run generate.go

import (
	"fmt"
	"log/slog"
	"os"
	"os/exec"
)

var source string

func main() {
	// read the source code
	sourceRaw, err := os.ReadFile("source.oc")
	if err != nil {
		slog.Error(fmt.Sprint(err))
		os.Exit(1)
	}
	source = string(sourceRaw)

	// tokenize
	tokens := tokenize("source.oc")
	for _, token := range tokens {
		fmt.Println(token)
	}

	fmt.Println(tokens[0].start, tokens[0].end)

	// parsing
	ast, err := parse("source.oc")
	if err != nil {
		slog.Error(fmt.Sprint(err))
	}
	fmt.Println(ast)

	// compiling
	cSource, err := compile("source.oc")
	if err != nil {
		slog.Error(fmt.Sprint(err))
		os.Exit(1)
	}

	// writing c file
	cSourceFile, err := os.Create("out.c")
	if err != nil {
		slog.Error(fmt.Sprint(err))
		os.Exit(1)
	}
	defer cSourceFile.Close()

	cSourceFile.Write([]byte(cSource))

	cmd := exec.Command("gcc", "-lgc", "-o", "a.out", "out.c")
	err = cmd.Run()
	if err != nil {
		slog.Error(fmt.Sprint(err))
	}
}
