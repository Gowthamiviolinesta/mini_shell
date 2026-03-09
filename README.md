# Mini Shell

## Description

Mini Shell is a simplified Unix-like shell implemented in C.
It allows users to execute commands, manage processes, and interact with the operating system through a command-line interface.

## Features Implemented

### Prompt Customization

* The shell prompt can be changed using the `PS1` environment variable.

Example:

```
PS1="MyShell>"
```

### Special Variables

The shell supports some commonly used special variables:

* `echo $$` – Displays the Process ID (PID) of the current shell.
* `echo $?` – Displays the exit status of the last executed command.
* `echo $SHELL` – Displays the path of the current shell.

### Internal Commands

The shell supports basic built-in commands:

* `cd` – Change the current directory.
* `pwd` – Print the current working directory.
* `exit` – Exit from the shell.

### Job Control Commands

The following job control commands are supported:

* `fg` – Bring a background process to the foreground.
* `bg` – Resume a stopped job in the background.
* `jobs` – Display the list of active jobs.

### Pipe Support

* The shell supports **pipe (`|`) operations**, allowing the output of one command to be used as input for another command.

Example:

```
ls | wc
```

## Compilation

Compile the program using:

```
gcc *.c -o minishell
```

## Run

Execute the shell using:

```
./minishell
```

## Author

Gowthami
