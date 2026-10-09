## Project Scope and Roadmap

This project follows the CodeCrafters “Build Your Own Shell” challenge,
progressing from a basic interactive shell to more advanced command-line
features.

The areas below describe the intended development roadmap, not a list
of completed features. Implementation is ongoing, and the scope may
expand as I progress through the challenge.

### Core Shell Functionality

| Area | Intended functionality |
|---|---|
| Interactive shell | Display a prompt and run a read–evaluate–print loop (REPL) |
| Command parsing | Identify command names and separate their arguments |
| Command handling | Recognise supported commands and report unknown commands |
| Built-in commands | Implement `exit`, `echo`, and `type` |
| Executable discovery | Search the `PATH` environment variable for external commands |
| Program execution | Launch external programs and manage their completion |

### Extended Shell Features

| Area | Intended functionality |
|---|---|
| Directory navigation | Implement `pwd` and `cd`, including relative paths and home-directory navigation |
| Quoting and escaping | Interpret single quotes, double quotes, and backslash escapes |
| Output redirection | Redirect or append standard output and standard error to files |
| Command completion | Complete built-in and external command names interactively |
| Filename completion | Complete file and directory paths, including nested paths and multiple matches |
| Programmable completion | Register and use custom completion behaviour |
| Pipelines | Connect commands so that one command’s output becomes another command’s input |
| Background jobs | Start commands in the background, list jobs, and collect completed processes |
| Command history | Record previous commands and support history listing and navigation |
| History persistence | Read, write, and append command history using files |
| Parameter expansion | Explore shell variables and variable expansion |

### Engineering Priorities

Alongside functionality, development focuses on:

- Bounded input handling and validation.
- Clear memory ownership and resource cleanup.
- Separation of parsing, command lookup, dispatch, and execution.
- Error handling for input, filesystem, and process operations.
- Incremental testing of normal behaviour and edge cases.
- Documentation of implementation decisions and lessons learned.

This is an educational project under active development. It does not currently claim full POSIX compatibility or production readiness.

# License

This project is licensed under the MIT License. See the LICENSE file
for details.

Third-party material remains subject to its applicable licenses.
