package main

import (
	"strconv"
	"strings"

	"github.com/gookit/color"
)

func logCodeError(start Location, end Location, message string) {
	if start.line == end.line {
		lineString := strconv.Itoa(start.line)
		lineStringLen := len(lineString)

		color.White.Println(strings.Repeat(" ", lineStringLen), "|")
		color.White.Println(lineString, "|")
		color.White.Println(strings.Repeat(" ", lineStringLen), "|")

	}
}
