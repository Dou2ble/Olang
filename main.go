package main

import (
	"fmt"
	"io/ioutil"
	"log/slog"
	"os"
)

func main() {
	// file, err := os.Open("source.plisp")

	// if err != nil {
	// 	slog.Error(fmt.Sprint(err))
	// }
	// defer file.Close()

	// var source string
	// scanner := bufio.NewScanner(file)
	// for scanner.Scan() {
	// 	source = scanner.Text()
	// }
	// if err := scanner.Err(); err != nil {
	// 	slog.Error(fmt.Sprint(err))
	// }

	sourcePlisp, err := ioutil.ReadFile("source.plisp")
	if err != nil {
		slog.Error(fmt.Sprint(err))
		os.Exit(1)
	}

	source := []rune(string(sourcePlisp))

	tokens := tokenize(source)

	for _, token := range tokens {
		fmt.Println(token)
		fmt.Println()
	}

	ast, err := parse(source)
	if err != nil {
		slog.Error(fmt.Sprint(err))
	}
	fmt.Println(ast)
}
