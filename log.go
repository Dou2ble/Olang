package main

import (
	"errors"
	"strconv"
	"strings"

	"github.com/gookit/color"
)

type Level uint8

const (
	LevelInfo Level = iota
	LevelWarning
	LevelError
)

func myError(text string) error {
	return errors.New(strings.ToLower(text))
}

func log(level Level, message string) {
	var (
		title string
		style color.Style
	)

	switch level {
	case LevelInfo:
		title = "Info"
		style = color.Style{color.Blue, color.Bold}
	case LevelWarning:
		title = "Warning"
		style = color.Style{color.Yellow, color.Bold}
	case LevelError:
		title = "Error"
		style = color.Style{color.Red, color.Bold}
	}

	style.Print(title)
	color.Bold.Println(":", message)
}

func logCode(level Level, start Location, end Location, message string) {
	log(level, message)

	color.Cyan.Print("--> ")
	color.Bold.Println(start.path)

	var style color.Style

	switch level {
	case LevelInfo:
		style = color.Style{color.Blue, color.Bold}
	case LevelWarning:
		style = color.Style{color.Yellow, color.Bold}
	case LevelError:
		style = color.Style{color.Red, color.Bold}
	}

	lineString := strconv.Itoa(end.line)
	lineStringLen := len(lineString)

	// if the error is on a single line
	if start.line == end.line {
		color.Cyan.Println(strings.Repeat(" ", lineStringLen), "|")
		color.Cyan.Print(lineString, " | ")
		color.White.Println(strings.Split(source, "\n")[start.line-1])
		color.Cyan.Print(strings.Repeat(" ", lineStringLen), " | ")

		color.White.Print(strings.Repeat(" ", start.character-1))

		style.Println(strings.Repeat("^", end.character-start.character+1))

	} else {
		// multi line error
		color.Cyan.Println(strings.Repeat(" ", lineStringLen), "|")

		lines := strings.Split(source, "\n")

		// the first line of the error
		color.Cyan.Print(strconv.Itoa(start.line), " | ")
		for c := 0; c < len(lines[start.line-1]); c++ {
			if c < start.character-1 {
				color.White.Print(string(lines[start.line-1][c]))
			} else {
				style.Print(string(lines[start.line-1][c]))
			}
		}
		style.Println()

		// the "body" lines of the error
		for l := start.line; l < end.line-1; l++ {
			color.Cyan.Print(strconv.Itoa(l+1), " | ")
			style.Println(lines[l])
		}

		// the last line of the error
		color.Cyan.Print(strconv.Itoa(end.line), " | ")
		for c := 0; c < len(lines[end.line-1]); c++ {
			if c < end.character {
				style.Print(string(lines[end.line-1][c]))
			} else {
				color.White.Print(string(lines[end.line-1][c]))
			}
		}
		style.Println()

		color.Cyan.Println(strings.Repeat(" ", lineStringLen), "|")
	}
}
