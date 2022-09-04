package main

import (
	"bufio"
	"errors"
	"fmt"
	"io/fs"
	"os"
	"path"
	"strconv"
	"strings"
)

func main() {
	home := os.Getenv("HOME")
	todoDir := path.Join(home, ".todo")
	todoFile := path.Join(todoDir, "todo")
	if len(os.Args) == 1 {
		fmt.Println("Need at least one arg!")
		return
	}
	err := os.Mkdir(todoDir, 0755)
	if err != nil {
		if errors.Is(err, fs.ErrNotExist) {
			fmt.Println(err)
			return
		}
	}

	f, err := os.Open(todoFile)
	if err != nil {
		if errors.Is(err, fs.ErrNotExist) {
			os.Create(todoFile)
		}
	}

	f.Close()

	cmd := os.Args[1]
	switch cmd {
	case "add":
		addTodo(os.Args[2:], todoFile)
	case "del":
		tn, err := strconv.Atoi(os.Args[2])
		if err != nil {
			fmt.Println(err)
			return
		}
		delTodo(tn, todoFile)
	case "update":
		tn, err := strconv.Atoi(os.Args[2])
		if err != nil {
			fmt.Println(err)
			return
		}
		updateTodo(tn, os.Args[3:], todoFile)
	case "list":
		listTodos(todoFile)
	case "done":
		tn, err := strconv.Atoi(os.Args[2])
		if err != nil {
			fmt.Println(err)
			return
		}
		completeTodo(tn, todoFile)
	default:
		fmt.Println("Wrong command")
	}
}

func addTodo(todo []string, fileName string) {
	longTodo := ""
	f, err := os.OpenFile(fileName, os.O_WRONLY|os.O_APPEND, 0755)
	if err != nil {
		fmt.Println(err)
		return
	}

	if len(todo) > 1 {
		for i := 0; i < len(todo); i++ {
			longTodo += todo[i] + " "
		}
	} else {
		longTodo = todo[0]
	}
	longTodo += "\n"

	_, err = f.WriteString(longTodo)
	if err != nil {
		fmt.Println(err)
		return
	}

	f.Close()
}

func completeTodo(todoNum int, fileName string) {
	counter := 1
	data, err := os.ReadFile(fileName)
	if err != nil {
		fmt.Println(err)
		return
	}

	fd, err := os.Create(fileName)
	if err != nil {
		fmt.Println(err)
		return
	}

	reader := strings.NewReader(string(data))
	scanner := bufio.NewScanner(reader)
	for scanner.Scan() {
		if counter == todoNum {
			fd.WriteString(scanner.Text() + "✓" + "\n")
		} else {
			fd.WriteString(scanner.Text() + "\n")
		}
		counter++
	}

	fd.Close()
}

func delTodo(todoNum int, fileName string) {
	counter := 1
	f, err := os.ReadFile(fileName)
	if err != nil {
		fmt.Println(err)
		return
	}

	fd, err := os.Create(fileName)
	if err != nil {
		fmt.Println(err)
		return
	}

	reader := strings.NewReader(string(f))
	scanner := bufio.NewScanner(reader)
	for scanner.Scan() {
		if counter != todoNum {
			fd.WriteString(scanner.Text() + "\n")
		}
		counter++
	}

	fd.Close()
}

func updateTodo(todoNum int, updatedTodo []string, fileName string) {
	counter := 1
	longTodo := ""
	data, err := os.ReadFile(fileName)
	if err != nil {
		fmt.Println(err)
		return
	}

	if len(updatedTodo) > 1 {
		for i := 0; i < len(updatedTodo); i++ {
			longTodo += updatedTodo[i] + " "
		}
	} else {
		longTodo += updatedTodo[0]
	}
	longTodo += "\n"

	fd, err := os.Create(fileName)
	if err != nil {
		fmt.Println(err)
		return
	}

	reader := strings.NewReader(string(data))
	scanner := bufio.NewScanner(reader)
	for scanner.Scan() {
		if counter == todoNum {
			fd.WriteString(longTodo)
		} else {
			fd.WriteString(scanner.Text() + "\n")
		}
		counter++
	}

	fd.Close()
}

func listTodos(fileName string) {
	counter := 1
	data, err := os.ReadFile(fileName)
	reader := strings.NewReader(string(data))
	scanner := bufio.NewScanner(reader)

	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Println("Todo List")
	fmt.Println("---------")
	for scanner.Scan() {
		if strings.Contains(scanner.Text(), "✓") {
			fmt.Printf("\033[32m%d. %s\033[0m\n", counter, scanner.Text())
		} else {
			fmt.Printf("%d. %s\n", counter, scanner.Text())
		}
		counter++
	}
}
